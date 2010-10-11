<?php
// $Id: bluedroplet.com.inc,v 1.1.2.6 2009/11/29 20:53:09 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


define("BD_VIDEO_BLUEDROPLET_URL", 'http://bluedroplet.com');

define("BD_VIDEO_DEFAULT_BLUEDROPLET_RETURN_METHOD", 'poll');
define("BD_VIDEO_BLUEDROPLET_PROTO_VERSION", 5);

define("HTTP_CONNECT_TIMEOUT", 600);
define("HTTP_LOW_SPEED_LIMIT", 256);
define("HTTP_LOW_SPEED_TIMEOUT", 600);


function _bd_video_bluedroplet_settings_fieldset($active) {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Remote transcoding on bluedroplet.com (paid service)'),
    '#collapsible' => TRUE,
    '#collapsed' => !$active
  );

  $fieldset['bd_video_bluedroplet_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#size' => 15,
    '#maxlength' => USERNAME_MAX_LENGTH,
    '#default_value' => variable_get('bd_video_bluedroplet_name', '')
  );

  $fieldset['bd_video_bluedroplet_hash'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#maxlength' => 60,
    '#size' => 15
  );

  $description  = 'Direct return is the optimal setting, but will not work if your site is not visible on the ';
  $description .= 'Internet.<br />If you are not running cron.php through your web server, ensure that it is running ';
  $description .= 'as the same user as your web server and that you have defined $base_url in settings.php.';
  
  $fieldset['bd_video_bluedroplet_return_method'] = array(
    '#type' => 'radios',
    '#title' => t('Return method'),
    '#options' => array(
      'direct' => 'Direct return',
      'poll' => 'Periodic check'
    ),
    '#default_value' => 
      variable_get('bd_video_bluedroplet_return_method', BD_VIDEO_DEFAULT_BLUEDROPLET_RETURN_METHOD),
    '#description' => t($description)
  );

  return $fieldset;
}


function _bd_video_reattempt_invalid_credentials() {

  db_query("
    UPDATE {bd_video}
    SET  status = 'dirty',
      error = 'none',
      upload_failed = 0
    WHERE error = 'invalid credentials'
  ");

  if(db_affected_rows()) {
    $msg = 'Reattempting uploads that failed due to invalid credentials.';
    drupal_set_message($msg);
    watchdog('bd_video', $msg, NULL);
  }
}


function _bd_video_bluedroplet_settings_form_submit(&$form_state) {

  if($form_state['clicked_button']['#value'] != t('Save configuration'))
    return;

  $form_state['values']['bd_video_bluedroplet_hash'] = trim($form_state['values']['bd_video_bluedroplet_hash']);

  if($form_state['values']['bd_video_transcoder'] == 'bluedroplet.com' &&
    $form_state['values']['bd_video_bluedroplet_hash'] != '')
  {
    $form_state['values']['bd_video_bluedroplet_hash'] = md5($form_state['values']['bd_video_bluedroplet_hash']);
    _bd_video_reattempt_invalid_credentials();
  }
  else {
    unset($form_state['values']['bd_video_bluedroplet_hash']);
  }
}


function _bd_video_curl_post($url, $fields, &$output) {

  $options = array(
    CURLOPT_POST => TRUE,
    CURLOPT_POSTFIELDS => $fields,
    CURLOPT_CONNECTTIMEOUT => HTTP_CONNECT_TIMEOUT,
    CURLOPT_LOW_SPEED_LIMIT => HTTP_LOW_SPEED_LIMIT,
    CURLOPT_LOW_SPEED_TIME => HTTP_LOW_SPEED_TIMEOUT,
    CURLOPT_RETURNTRANSFER => TRUE
  );
  
  $ch = curl_init($url);
  curl_setopt_array($ch, $options);
  $output = curl_exec($ch);
  $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
  curl_close($ch);
  
  return $http_code;
}


function _bd_video_upload_video($video) {
  
  $fields['proto_ver'] = BD_VIDEO_BLUEDROPLET_PROTO_VERSION;

  $fields['name'] = variable_get('bd_video_bluedroplet_name', '');
  $fields['hash'] = variable_get('bd_video_bluedroplet_hash', '');

  $fields['site_url'] = url('', array('absolute' => TRUE));
  $fields['return_method'] = variable_get('bd_video_bluedroplet_return_method', 
    BD_VIDEO_DEFAULT_BLUEDROPLET_RETURN_METHOD);
  $fields['video_id'] = $video['video_id'];
  $fields['secret'] = $video['secret'];

  unset($video['dirty_params']['keep_source_file']);
  $fields['params'] = serialize($video['dirty_params']);

  $filepath = storage_api_get_local_filepath($video['source_file'], $keep_me);
  $fields['video'] = '@' . $filepath;
  $fields['filename'] = $video['source_file']['filename'];

  $url = variable_get('bd_video_bluedroplet_url', BD_VIDEO_BLUEDROPLET_URL) . '/incoming';
  
  $http_code = _bd_video_curl_post($url, $fields, $output);
  
  if(!$keep_me)
    @unlink($filepath);
  
  return $http_code;
}


function _bd_video_transcode_bluedroplet($video) {

/*
  if($video['upload_failed'] && 
    $video['upload_failed'] > time() - 10 * 60)
  {
    return;
  }
*/

  $http_code = _bd_video_upload_video($video);
  
  if(!$http_code) {

    $update = array(
      'video_id' => $video['video_id'],
      'status' => 'idle',
      'error' => 'file not found',
    );
    
    drupal_write_record('bd_video', $update, 'video_id');
  }
  else {
    switch($http_code) {

      case 200:
        $update = array(
          'video_id' => $video['video_id'],
          'status' => 'uploaded',
          'error' => 'none',
          'uploaded' => time(),
        );
        
        drupal_write_record('bd_video', $update, 'video_id');
        watchdog('bd_video', 'Video successfully uploaded. video_id: ' . $video['video_id'], NULL);
        break;

      case 402:
        $update = array(
          'video_id' => $video['video_id'],
          'status' => 'uploaded',
          'error' => 'insufficient credit',
          'uploaded' => time(),
        );
        
        drupal_write_record('bd_video', $update, 'video_id');
        watchdog('bd_video', 'Video uploaded. Insufficient credit. video_id: ' . $video['video_id'], NULL, WATCHDOG_WARNING);
        break;

      case 403:
        $update = array(
          'video_id' => $video['video_id'],
          'status' => 'error',
          'error' => 'invalid credentials',
        );
        
        drupal_write_record('bd_video', $update, 'video_id');
        watchdog('bd_video', 'Video upload failed. Invalid credentials. video_id: ' . $video['video_id'], NULL, WATCHDOG_WARNING);
        break;

      case 406:
        _bd_video_set_transcoding_failed($video['video_id']);
//        watchdog('bd_video', 'Video upload failed. Invalid credentials. video_id: ' . $video['video_id'], NULL, WATCHDOG_WARNING);
        break;

      default:
        $update = array(
          'video_id' => $video['video_id'],
          'status' => 'dirty',
          'error' => 'connection error',
          'upload_failed' => time(),
        );
        
        drupal_write_record('bd_video', $update, 'video_id');
        watchdog('bd_video', 'Video upload failed - will reattempt in 10 min. Connection error. video_id: ' . $video['video_id'], NULL, WATCHDOG_WARNING);
        break;
    }
  }
}


function _bd_video_curl_post_get_file($url, $fields, $filepath) {

  $fp = fopen($filepath, "w");

  $options = array(
    CURLOPT_POST => TRUE,
    CURLOPT_POSTFIELDS => $fields,
    CURLOPT_CONNECTTIMEOUT => HTTP_CONNECT_TIMEOUT,
    CURLOPT_LOW_SPEED_LIMIT => HTTP_LOW_SPEED_LIMIT,
    CURLOPT_LOW_SPEED_TIME => HTTP_LOW_SPEED_TIMEOUT,
    CURLOPT_FILE => $fp
  );
  
  $ch = curl_init($url);
  curl_setopt_array($ch, $options);
  curl_exec($ch);
  fclose ($fp);
  $result = curl_getinfo($ch, CURLINFO_HTTP_CODE);
  curl_close($ch);
  
  return $result;
}


function _bd_video_bluedroplet_get_file($video, $file) {
  $url = variable_get('bd_video_bluedroplet_url', BD_VIDEO_BLUEDROPLET_URL) . '/get';

  $fields['name'] = variable_get('bd_video_bluedroplet_name', '');
  $fields['hash'] = variable_get('bd_video_bluedroplet_hash', '');
  $fields['site_url'] = url('', array('absolute' => TRUE));
  $fields['video_id'] = $video['video_id'];
  $fields['file'] = $file;

  $selector_ids = _bd_video_get_selector_ids($video['field']);

  $filepath = tempnam(file_directory_temp(), '');
  
  $result = _bd_video_curl_post_get_file($url, $fields, $filepath);
  
  if($result == 200) {
  
    $options = array(
      'module' => 'bd_video',
      'nid' => $video['source_file']['nid'],
      'filepath' => $filepath
    );
    
    switch($file) {
      
      case 'flv':
        $options['type'] = 'transcoded video';
        break;
  
      case 'raw_image_1':
      case 'raw_image_2':
        $options['type'] = 'preview image';
        break;
    }
        
    return storage_api_file_add($selector_ids['transcoded'], $options);
  }
  else {
    @unlink($filepath);
    return -$result;
  }
}


function _bd_video_bluedroplet_get_info($video) {
  $url = variable_get('bd_video_bluedroplet_url', BD_VIDEO_BLUEDROPLET_URL) . '/get';

  $fields['name'] = variable_get('bd_video_bluedroplet_name', '');
  $fields['hash'] = variable_get('bd_video_bluedroplet_hash', '');
  $fields['site_url'] = url('', array('absolute' => TRUE));
  $fields['video_id'] = $video['video_id'];
  $fields['info'] = TRUE;

  _bd_video_curl_post($url, $fields, $output);
  
  return unserialize($output);
}


function _bd_video_retrieve_videos() {

  $result = db_query("
    SELECT video_id
    FROM {bd_video}
    WHERE status = 'uploaded'
    ORDER BY video_id
  ");

  while($video = db_fetch_array($result)) {
    $video = _bd_video_load($video['video_id']);
  
    $flv_file_id = _bd_video_bluedroplet_get_file($video, 'flv');
    
    if($flv_file_id > 0) {
      $raw_image_1_file_id = _bd_video_bluedroplet_get_file($video, 'raw_image_1');
      $raw_image_2_file_id = _bd_video_bluedroplet_get_file($video, 'raw_image_2');
      $info = _bd_video_bluedroplet_get_info($video);

      $params_id = _bd_video_transcoding_params_insert($info['params']);
      
      $update = array(
        'video_id' => $video['video_id'],
        'duration' => $info['duration'],
        'flv_file_id' => $flv_file_id,
        'start_image_file_id' => $raw_image_1_file_id,
        'third_image_file_id' => $raw_image_2_file_id,
        'current_params_id' => $params_id,
        'status' => 'idle',
        'error' => 'none',
      );
      
      drupal_write_record('bd_video', $update, 'video_id');
      
      storage_api_inherit_servings($flv_file_id, $video['flv_file_id']);
      
      storage_api_file_remove($video['flv_file_id']);
      storage_api_file_remove($video['start_image_file_id']);
      storage_api_file_remove($video['third_image_file_id']);

      _bd_video_transcoding_completed($video['video_id']);
      watchdog('bd_video', 'Video returned. video_id: ' . $video['video_id'], NULL);
    }
    else {
    
      switch(-$flv_file_id) {

        case 204:    // not transcoded yet
          break;
      
        case 403:
          $update = array(
            'video_id' => $video['video_id'],
            'error' => 'invalid credentials'
          );
          
          drupal_write_record('bd_video', $update, 'video_id');
          break;
      
        case 406:
          _bd_video_set_transcoding_failed($video['video_id']);
          break;
      }
    }
  }
}


function bd_video_incoming() {

  watchdog('bd_video', var_export($_POST, TRUE), NULL);

  $video_id = (int)$_POST['video_id'];

  $video = db_fetch_array(db_query("
    SELECT *
    FROM {bd_video}
    WHERE video_id = %d AND
      secret = '%s'
  ",
    $video_id,
    $_POST['secret']
  ));

  if(!$video) {
    watchdog('bd_video', 'Returned video invalid. $_POST: ' . var_export($_POST, TRUE), NULL, WATCHDOG_WARNING);
    drupal_set_header('HTTP/1.0 200 OK');
    exit();
  }

  if($_POST['error']) {
    _bd_video_set_transcoding_failed($video_id);
    watchdog('bd_video', 'Error returned. video_id: ' . $video_id, NULL);
    drupal_set_header('HTTP/1.0 200 OK');
    exit();
  }

  $params = unserialize($_POST['params']);
  $params_id = _bd_video_transcoding_params_insert($params);

  foreach($_FILES as $source_key => $source_value)
    foreach($source_value as $key => $value)
      $_FILES['files'][$key][$source_key] = $value;

  $selector_ids = _bd_video_get_selector_ids($video['field']);
  
  $options = array(
    'module' => 'bd_video',
    'type' => 'preview image',
    'nid' => $video['source_file']['nid']
  );
  
  $file = file_save_upload('raw_image_1');
  $options['filepath'] = $file->filepath;
  $start_image_file_id = storage_api_file_add($selector_ids['transcoded'], $options);
  
  $file = file_save_upload('raw_image_2');
  $options['filepath'] = $file->filepath;
  $third_image_file_id = storage_api_file_add($selector_ids['transcoded'], $options);
  
  $options['type'] = 'transcoded video';

  $file = file_save_upload('flv');
  $options['filepath'] = $file->filepath;
  $flv_file_id = storage_api_file_add($selector_ids['transcoded'], $options);
  
  $update = array(
    'video_id' => $video['video_id'],
    'duration' => $_POST['duration'],
    'flv_file_id' => $flv_file_id,
    'start_image_file_id' => $start_image_file_id,
    'third_image_file_id' => $third_image_file_id,
    'current_params_id' => $params_id,
    'status' => 'idle',
    'error' => 'none',
  );
  
  drupal_write_record('bd_video', $update, 'video_id');

  storage_api_inherit_servings($flv_file_id, $video['flv_file_id']);

  storage_api_file_remove($video['flv_file_id']);
  storage_api_file_remove($video['start_image_file_id']);
  storage_api_file_remove($video['third_image_file_id']);

  _bd_video_transcoding_completed($video['video_id']);
  watchdog('bd_video', 'Video returned. video_id: ' . $video_id, NULL);
  drupal_set_header('HTTP/1.0 200 OK');
  exit();
}

