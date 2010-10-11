<?php
// $Id: content.inc,v 1.1.2.25 2009/12/29 10:14:35 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


define("BD_VIDEO_DEFAULT_ACQUIRE_FROM_URL", TRUE);
define("BD_VIDEO_DEFAULT_ALLOW_REPLACE", FALSE);
define("BD_VIDEO_DEFAULT_ALLOW_EMBED", FALSE);


/*
      FIELD
*/


/**
 * Implement hook_field_info() .
 */
function bd_video_field_info() {

  return array(
    'bd_video' => array(
      'label' => 'Blue Droplet Video',
      'description' => 'Video'
    )
  );
}


function _bd_video_get_params_id($field) {
  $params_ids = variable_get('bd_video_field_params_ids', array());
  return $params_ids[$field['field_name']];
}


function _bd_video_field_load_params($field) {
  $params_ids = variable_get('bd_video_field_params_ids', array());
  $params = _bd_video_transcoding_params_load($params_ids[$field['field_name']]);

  if(!$params) {
    $params_ids[$field['field_name']] = _bd_video_transcoding_params_insert();
    variable_set('bd_video_field_params_ids', $params_ids);
    $params = _bd_video_transcoding_params_load($params_ids[$field['field_name']]);
  }
  
  return $params;
}


function _bd_video_field_retranscode_all($field) {

  $table = 'content_type_' . $field['type_name'];
  $video_id_field = $field['field_name'] . '_video_id';

  $result = db_query("
    SELECT {bd_video}.video_id
    FROM {{$table}}
    INNER JOIN {bd_video}
      ON {{$table}}.$video_id_field = {bd_video}.video_id
    WHERE {bd_video}.source_file_id IS NOT NULL
  ");

  while($video = db_fetch_array($result)) {
    _bd_video_retranscode_video($video['video_id']);
  }

  $msg = t('Retranscoding all field %label videos.', array('%label' => $field['label']));
  drupal_set_message($msg);
  watchdog('bd_video', $msg, NULL);
}


function _bd_video_field_store_params($field) {
  $params_id = _bd_video_get_params_id($field);
  _bd_video_transcoding_params_update($params_id, $field);
  
  if($field['retranscode_all']) {
    _bd_video_field_retranscode_all($field);
  }
}


function _bd_video_get_selector_ids($field_name) {

  $selector_ids = variable_get('bd_video_field_selector_ids', array());

  foreach(array('source', 'feed_source', 'transcoded', 'cache') as $selector) {

    if($selector_ids[$field_name][$selector]) {
    
      // this will fix a situation where storage_api was uninstalled, but bd_video was not
    
      $new = !db_result(db_query("
        SELECT COUNT(*)
        FROM {storage_selector}
        WHERE selector_id = %d
      ",
        $selector_ids[$field_name][$selector]
      ));
    }
    else {    
      $new = TRUE;
    }
    
    if($new) {
      $selector_ids[$field_name][$selector] = storage_api_new_selector_id();
      $update = TRUE;
    }
  }
  
  if($update)
    variable_set('bd_video_field_selector_ids', $selector_ids);
  
  return $selector_ids[$field_name];
}


function _bd_video_selectors_fieldset($field_name) {

  $selector_ids = _bd_video_get_selector_ids($field_name);
  
  $fieldset = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Storage classes'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $fieldset['source'] = storage_api_selector_item($selector_ids['source'], t('Source class'),
    t('Source video and custom preview images.'));
    
  if(module_exists('feedapi_mapper')) {
  
    $description  = 'Source video from video feeds.<br />';
    $description .= 'Typically, this should point to a class with no containers and serve source url enabled.';
  
    $fieldset['feed_source'] = storage_api_selector_item($selector_ids['feed_source'], t('Feed source class'),
      t($description));
  }
    
  $fieldset['transcoded'] = storage_api_selector_item($selector_ids['transcoded'], t('Transcoded class'),
    t('Transcoded video and captured preview images.'));
    
  $fieldset['cache'] = storage_api_selector_item($selector_ids['cache'], t('Cache class'),
    t('Cached scaled images.'));
      
  return $fieldset;
}


/**
 * Implement hook_field_settings() .
 */
function bd_video_field_settings($op, $field) {

  switch ($op) {

    case 'form':
/*    
      $description = t("Allow the video the be embedded on other websites.") . '<br />';
      $description .= t("Users with <em>administer video</em> permission can always do this.");

      $form['allow_embed'] = array(
        '#type' => 'checkbox',
        '#title' => 'Allow embedding',
        '#default_value' => isset($field['allow_embed']) ? $field['allow_embed'] : BD_VIDEO_DEFAULT_ALLOW_EMBED,
        '#description' => $description
      );
*/    
      $form['preview_image_settings'] = _bd_video_preview_image_settings_fieldset($field);
      $form['player_settings'] = _bd_video_player_settings_fieldset($field);
      
      $params = _bd_video_field_load_params($field);
      $form['transcoding_params'] = _bd_video_transcoding_params_fieldset($params);
      
      $form['selectors'] = _bd_video_selectors_fieldset($field['field_name']);
      
      if(module_exists('bd_video_ads'))
        $form['bd_video_ads'] = bd_video_ads_field_settings_fieldset($field);

      return $form;


    case 'validate':
    
      _bd_video_preview_image_settings_validate($field);
      _bd_video_player_settings_validate($field);
      _bd_video_transcoding_params_validate($field);
      
      if(!form_get_errors()) {    // $form data is not passed to 'save' op below
        _bd_video_field_store_params($field);
        storage_api_selector_submit($field['selectors']['source']);
        storage_api_selector_submit($field['selectors']['feed_source']);
        storage_api_selector_submit($field['selectors']['transcoded']);
        storage_api_selector_submit($field['selectors']['cache']);
      }
      return;


    case 'save':
    
      $fields = _bd_video_player_settings_fields();
      $fields = array_merge($fields, _bd_video_preview_image_settings_fields());
      
      if(module_exists('bd_video_ads'))
        $fields = array_merge($fields, bd_video_ads_field_settings_fields());
        
      return $fields;


    case 'database columns':

      return array(
        'video_id' => array('type' => 'int', 'unsigned' => TRUE, 'not null' => FALSE)
      );

      
    case 'views data':
    
      return _bd_video_views_data($field);
  }
}


function _bd_video_field_item_presave(&$item, $selector_ids, $nid) {

  $options = array(
    'module' => 'bd_video',
    'nid' => $nid
  );
  
  if($item['video_id'])
    $video = _bd_video_load($item['video_id']);

  if($item['source_file']['flash_ready']) {
    $options['type'] = 'transcoded video';
  }
  else {
    $options['type'] = 'source video';
    $options['force_download'] = TRUE;
  }

  $file = $item['source_file']['uploaded']['file'];
  
  if($file) {
    $options['filename'] = $file->filename;
    $options['filepath'] = $file->filepath;

    $file_id = storage_api_file_add($selector_ids['source'], $options);
  }
  else if($item['source_file']['acquire_url']) {
    $options['get_filename'] = TRUE;
    $options['source_url'] = $item['source_file']['acquire_url'];

    $file_id = storage_api_file_add($selector_ids['source'], $options);
  }
  else if($item['source_file']['filepath']) {
    $options['get_filename'] = TRUE;
    $options['filepath'] = $item['source_file']['filepath'];

    $file_id = storage_api_file_add($selector_ids['source'], $options);
  }
  
  if($file_id) {
  
    if($item['source_file']['flash_ready'])
      $item['flv_file_id'] = $file_id;
    else
      $item['source_file_id'] = $file_id;
  }
    

  if($video || $file_id) {
  
    $options = array(
      'force_download' => TRUE,
      'module' => 'bd_video',
      'type' => 'custom preview image',
      'nid' => $nid
    );

    $file = $item['preview_image']['source_file']['uploaded']['file'];

    if($file) {
      $options['filename'] = $file->filename;
      $options['filepath'] = $file->filepath;
        
      $item['custom_image_file_id'] = storage_api_file_add($selector_ids['source'], $options);
    }
    else if($item['preview_image']['source_file']['acquire_url']) {
      $options['get_filename'] = TRUE;
      $options['source_url'] = $item['preview_image']['source_file']['acquire_url'];

      $item['custom_image_file_id'] = storage_api_file_add($selector_ids['source'], $options);
    }
    else if($item['preview_image']['source_file']['filepath']) {
      $options['get_filename'] = TRUE;
      $options['filepath'] = $item['preview_image']['source_file']['filepath'];

      $item['custom_image_file_id'] = storage_api_file_add($selector_ids['source'], $options);
    }
  }
}


function _bd_video_field_item_update(&$item, $field) {

  $video = _bd_video_load($item['video_id']);

  if($item['source_file_id']) {
    _bd_video_update_source_file($video, $item['source_file_id']);
    drupal_set_message('The new video will be transcoded.');
  }
  else if($item['flv_file_id']) {
    _bd_video_update_flv_file($video, $item['flv_file_id']);
  }
  else if($item['retranscode']) {
    _bd_video_retranscode_video($item['video_id']);
    drupal_set_message('The video will be retranscoded.');
  }
  
  if($item['custom_image_file_id']) {
    _bd_video_update_custom_image($video, $item['custom_image_file_id']);
  }
  else if($item['preview_image']['source_file']['delete']) {
  
    _bd_video_update_custom_image($video, 0);
    
    if($item['preview_image']['preview_image'] == 'custom')
      $item['preview_image']['preview_image'] = $field['preview_image_image'];
  }

  if($item['preview_image']['preview_image'] && 
    ($item['preview_image']['preview_image'] != $video['preview_image'])) {
    _bd_video_set_preview_image($item['video_id'], $item['preview_image']['preview_image']);
  }
}


function _bd_video_field_item_insert(&$item, $field, $nid) {

  if(!$item['source_file_id'] && !$item['flv_file_id'])
    return;

  $video = $item;
  $video['dirty_params_id'] = _bd_video_get_params_id($field);
  $video['preview_image'] = $item['preview_image']['preview_image'];
  $video['field'] = $field['field_name'];
  $video['nid'] = $nid;

  $item['video_id'] = _bd_video_create($video);
}


/**
 * Implement hook_field() .
 */
function bd_video_field($op, &$node, $field, &$items, $teaser, $page) {

  foreach($items as &$item) {

    if($item['custom_image_file_id'])
      $item['preview_image']['preview_image'] = 'custom';
  }
  
  switch ($op) {
  
    case 'validate':

      foreach($items as $delta => &$item) {
      
        if(!$item['source_file']['uploaded']['file'] &&
          !$item['source_file']['acquire_url'] &&
          $item['source_file']['filepath'])
        {
          if(!is_file($item['source_file']['filepath'])) {
            form_set_error($field['field_name'] . '][' . $delta . '][source_file][filepath', t('File not found.'));
          }
        }

        if($field['required'] && !$item['video_id'] &&
          !$item['source_file']['uploaded']['file'] &&
          !$item['source_file']['acquire_url'] &&
          !$item['source_file']['filepath'])
        {
          form_set_error($field['field_name'], t('You need to add a video.'));
        }

        if(!$item['preview_image']['source_file']['uploaded']['file'] &&
          !$item['preview_image']['source_file']['acquire_url'] &&
          $item['preview_image']['source_file']['filepath'])
        {
          if(!is_file($item['preview_image']['source_file']['filepath'])) {
            form_set_error($field['field_name'] . '][' . $delta . '][preview_image][source_file][filepath', t('File not found.'));
          }
        }
      }

      break;
      
      
    case 'presave':
    
      $selector_ids = _bd_video_get_selector_ids($field['field_name']);

      foreach($items as &$item) {
        _bd_video_field_item_presave($item, $selector_ids, $node->nid);
      }

      break;
      

    case 'update':
      
      foreach($items as &$item) {
      
        if($item['video_id']) {
          _bd_video_field_item_update(&$item, $field);
        }
        else {
          _bd_video_field_item_insert(&$item, $field, $node->nid);
        }
      }
      
      break;
      

    case 'insert':
    
      foreach($items as &$item) {

        _bd_video_field_item_insert(&$item, $field, $node->nid);
      }
                  
      break;
  
      
    case 'delete':
    
      foreach($items as &$item) {
      
        if($item['video_id'])
          _bd_video_delete(_bd_video_load($item['video_id']));
      }
        
      break;
  }
}


/**
 * Implement hook_content_is_empty() .
 */
function bd_video_content_is_empty($item, $field) {

  if(!$item['video_id'] && !$item['source_file_id'] && !$item['flv_file_id'])
    return TRUE;
  else
    return FALSE;
}



/*
      WIDGET
*/



/**
 * Implement hook_widget_info() .
 */
function bd_video_widget_info() {

  return array(
    'bd_video' => array(
      'label' => t('Upload / Acquire from URL / Filepath'),
      'field types' => array('bd_video'),
    )
  );
}


/**
 * Implement hook_widget_settings() .
 */
function bd_video_widget_settings($op, $widget) {

  switch ($op) {

    case 'form':
    
      $description = t("Instead of uploading a video, a URL can be specified for the video to be acquired from.") . 
        "<br />\n";
      $description .= t("Users with <em>administer video</em> permission can always do this.");

      $form['acquire_from_url'] = array(
        '#type' => 'checkbox',
        '#title' => 'Acquire from URL',
        '#default_value' => isset($widget['acquire_from_url']) ? 
          $widget['acquire_from_url'] : BD_VIDEO_DEFAULT_ACQUIRE_FROM_URL,
        '#description' => $description
      );
      

      $description = t("Allow the video the be replaced / deleted after the node has been created.") . "<br />\n";
      $description .= t("Users with <em>administer video</em> permission can always do this.");

      $form['allow_replace'] = array(
        '#type' => 'checkbox',
        '#title' => 'Allow replacement',
        '#default_value' => isset($widget['allow_replace']) ? $widget['allow_replace'] : BD_VIDEO_DEFAULT_ALLOW_REPLACE,
        '#description' => $description
      );
      
      return $form;

    case 'save':
      return array('acquire_from_url', 'allow_replace');

    case 'callbacks':
      return array(
        'view' => CONTENT_CALLBACK_CUSTOM,
      );
  }
}


function _bd_video_widget_upload_form_item($text, $field, $state, &$storage, $existing, $required = FALSE) {

  if($existing) {
  
    if(!$required) {
      $item['delete'] = array(
        '#type' => 'checkbox',
        '#title' => t('Delete this') . ' ' . t($text),
        '#default_value' => $state['delete']    // for preview
      );
    }
    
    $item[] = array(
      '#value' => '<div style="clear: both;"></div>'
    );
  }
  
  $file = $storage[$field['field_name']];

  if($file) {
    $info = 'Filename: ' . $file->filename . '<br />';
    $info .= 'Size: ' . storage_api_format_size($file->filesize);
    $item['uploaded'] = array(
      '#type' => 'fieldset',
      '#title' => t('Uploaded') . ' ' . t($text)
    );
    $item['uploaded'][] = array(
      '#value' => $info
    );
    $item['uploaded']['ignore'] = array(
      '#type' => 'checkbox',
      '#title' => t('Ignore')
    );
    $item['uploaded']['file'] = array(
      '#type' => 'value',
      '#value' => $file
    );
  }

  $item[$field['field_name']] = array(
    '#type' => 'file', 
    '#name' => 'files[' . $field['field_name'] . ']',
    '#description' => 'Max size: ' . storage_api_format_size(file_upload_max_size())
  );

  if(!$existing && !$file)
    $item[$field['field_name']]['#title'] = t('Upload') . ' ' . t($text);
  else
    $item[$field['field_name']]['#title'] = t('Upload new') . ' ' . t($text);

  if($field['widget']['acquire_from_url'] || user_access('administer video')) {

    $item['acquire_url'] = array(
      '#type' => 'textfield',
      '#title' => 'Acquire from URL',
      '#description' => t('Must point to an actual file (not a web page).'),
      '#default_value' => $state['acquire_url']   // for preview
    );
  }
  
  if(user_access('administer video')) {
    $item['filepath'] = array(
      '#type' => 'textfield',
      '#title' => 'Acquire from filepath',
      '#description' => t('Can either be absolute or relative to the root of the Drupal installation.'),
      '#default_value' => $state['filepath'],     // for preview
      '#autocomplete_path' => BD_VIDEO_FILEPATH_AUTOCOMPLETE_PATH
    );
  }
  
  return $item;
}


function _bd_video_widget_preview_image($field, $item, &$storage) {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Preview image'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  if($item['video_id']) {
    $video = _bd_video_load($item['video_id']);
  }
  
  if($video['start_image_file_id'])
    $options['start'] = _bd_video_render_preview_image($video, 80, 60, NULL, 'start') . ' - start of video';
  else
    $options['start'] = 'Start of video';
  
  if($video['third_image_file_id'])
    $options['third'] = _bd_video_render_preview_image($video, 80, 60, NULL, 'third') . ' - @ 1/3 of video duration';
  else
    $options['third'] = '@ 1/3 of video duration';
    
  $existing = $video['custom_image_file_id'] != 0;
  
  if($existing)
    $options['custom'] = _bd_video_render_preview_image($video, 80, 60, NULL, 'custom') . ' - custom image';
    
  $fieldset['preview_image'] = array(
    '#type' => 'radios',
    '#default_value' => isset($video['preview_image']) ? $video['preview_image'] : 
      (isset($field['preview_image_image']) ? $field['preview_image_image'] : BD_VIDEO_DEFAULT_PREVIEW_IMAGE_IMAGE),
    '#options' => $options
  );

  $field['field_name'] .= '_image';
  $fieldset['source_file'] = _bd_video_widget_upload_form_item('custom image', $field, 
    $item['preview_image']['source_file'], $storage, $existing);

  return $fieldset;
}


/**
 * Implement hook_widget() .
 */
function bd_video_widget(&$form, &$form_state, $field, $items, $delta = 0) {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => $field['widget']['label'],
    '#description' => content_filter_xss($field['widget']['description'])
  );
  
  $existing = $items[$delta]['video_id'] != 0;
  
  if($existing) {
  
    $fieldset['video_id'] = array(
      '#type' => 'value',
      '#value' => $items[$delta]['video_id']
    );

    $video = _bd_video_load($items[$delta]['video_id']);
    $filepath = $video['source_file']['filepath'];
    
    $info = _bd_video_render_preview_image($video, 240, 180, 
      array('style="float: left; margin-right: 10px; margin-bottom: 10px;"'));

    if($video['source_file']) {
      $filename = $video['source_file']['filename'];
    }
    else {
      $filename = $video['flv_file']['filename'];
    }
    
    if($filename)
      $info .= 'Filename: ' . $filename . '<br />';

    if($video['source_file']['size']) {
      $info .= 'Size: ' . storage_api_format_size($video['source_file']['size']) . '<br />';
    }
    
    if($video['flv_file']['size']) {
      $info .= 'Transcoded size: ' . storage_api_format_size($video['flv_file']['size']) . '<br />';
    }

    if($video['duration'])
      $info .= 'Duration: ' . format_interval($video['duration']) . '<br />';
    
    $fieldset[] = array(
      '#value' => '<p>' . $info . '</p>'
    );

    if(user_access('administer video') && $video['source_file_id']) {
      
      $fieldset['retranscode'] = array(
        '#type' => 'checkbox', 
        '#title' => t('Retranscode this video')
      );
    }
  }
  
  $field['field_name'] .= '_' . $delta;

  if(!$existing || $field['widget']['allow_replace'] || user_access('administer video')) {
  
    $fieldset['source_file'] = 
      _bd_video_widget_upload_form_item('video', $field, $items[$delta]['source_file'], $form_state['storage'], 
        $existing, $field['required']);
        
    if(user_access('administer video')) {
          
      $fieldset['source_file']['flash_ready'] = array(
        '#type' => 'checkbox',
        '#title' => t('Video is Flash-ready'),
        '#description' => t('The submitted video does not need to be transcoded and should played directly in-browser.<br />Preview images will not be captured.'),
        '#default_value' => $items[$delta]['source_file']['flash_ready']
      );
    }
  }
  else {
    $fieldset[] = array(
      '#value' => '<div style="clear: both;"></div>'
    );
  }

  if(isset($field['preview_image_user_selectable']) ? $field['preview_image_user_selectable'] :
    BD_VIDEO_DEFAULT_PREVIEW_IMAGE_USER_SELECTABLE) // || user_access('administer video'))
  {
    $fieldset['preview_image'] = _bd_video_widget_preview_image($field, $items[$delta], $form_state['storage']);
  }


  // we need our validate function to kick in before 'node_form_validate'

  if(is_array($form['#validate'])) {
  
    if(!in_array('bd_video_node_form_validate', $form['#validate']))
      array_unshift($form['#validate'], 'bd_video_node_form_validate');
  }
  else {
    $form['#validate'] = array('bd_video_node_form_validate');
  }

  if(is_array($form['#submit'])) {
  
    if(!in_array('bd_video_node_form_submit', $form['#submit']))
      array_unshift($form['#submit'], 'bd_video_node_form_submit');
  }
  else {
    $form['#submit'] = array('bd_video_node_form_submit');
  }

  $form['#attributes']['enctype'] = "multipart/form-data";
  
  return $fieldset;
}


function bd_video_node_form_validate($form, &$form_state) {
  static $fields_processed = array();
  
  foreach($form['#field_info'] as $field) {
    
    if($field['type'] != 'bd_video')
      continue;
      
    // ensure we only process each field once, or it will malfunction

    if($fields_processed[$field['field_name']])
      continue;
      
    $fields_processed[$field['field_name']] = TRUE;

    foreach($form_state['values'][$field['field_name']] as $delta => &$values) {

      if(!is_numeric($delta))  // ignore field_video_add_more
        continue;
    
      $file = file_save_upload($field['field_name'] . '_' . $delta);
      
      
      // has a file been uploaded on a previous form submit?

      if($form_state['storage'][$field['field_name'] . '_' . $delta]) {
      
        // do we have a new upload, or has the user chosen to ignore the previous one?

        if($file || $values['source_file']['uploaded']['ignore']) {
        
          // delete the previous file and remove it from form storage and values

          file_delete($form_state['storage'][$field['field_name'] . '_' . $delta]->filepath);
          $form_state['storage'][$field['field_name'] . '_' . $delta] = NULL;  // unset() does not work for some reason
          unset($values['source_file']['uploaded']['file']);
        }
      }

      
      // if a file was uploaded, then store its details
      
      if($file) {
        $form_state['storage'][$field['field_name'] . '_' . $delta] = $file;
        $values['source_file']['uploaded']['file'] = $file;
      }


      // do the same for preview images

      $file = file_save_upload($field['field_name'] . '_' . $delta . '_image');
      
      if($form_state['storage'][$field['field_name'] . '_' . $delta . '_image']) {
      
        if($file || $values['preview_image']['source_file']['uploaded']['ignore']) {
        
          file_delete($form_state['storage'][$field['field_name'] . '_' . $delta . '_image']->filepath);
          $form_state['storage'][$field['field_name'] . '_' . $delta . '_image'] = NULL;  // unset() does not work for some reason
          unset($values['preview_image']['source_file']['uploaded']['file']);
        }
      }

      if($file) {
        $form_state['storage'][$field['field_name'] . '_' . $delta . '_image'] = $file;
        $values['preview_image']['source_file']['uploaded']['file'] = $file;
      }
    }
  }
}


function bd_video_node_form_submit($form, &$form_state) {

  if($form_state['clicked_button']['#value'] != t('Save'))
    return;
    
  foreach($form['#field_info'] as $field) {
    
    if($field['type'] != 'bd_video')
      continue;
      
    foreach($form_state['values'][$field['field_name']] as &$item) {
    
      if(!is_array($item))  // ignore field_video_add_more
        continue;
          
      if($item['source_file']['delete']) {
        _bd_video_delete(_bd_video_load($item['video_id']));
        unset($item['video_id']);
      }
    }
  }
  
  unset($form_state['storage']);    // make sure we redirect: http://drupal.org/node/144132#form-state
}



/*
      FORMATTERS
*/



/**
 * Implement hook_field_formatter_info() .
 */
function bd_video_field_formatter_info() {

  return array(
    'default' => array(
      'label' => 'Inline player',
      'field types' => array('bd_video'),
    ),
    'preview_image_link' => array(
      'label' => 'Preview image - link to node',
      'field types' => array('bd_video'),
    )
  );
}


function theme_bd_video_formatter_default($element) {

  $field = content_fields($element['#field_name'], $element['#type_name']);

  $video = _bd_video_load($element['#item']['video_id']);

  if(module_exists('bd_video_ads'))
    $video['advert'] = bd_video_ads_select_ad($field);

  _bd_video_set_render_settings($field);
  
  return _bd_video_render_video($video, 'default');
}


function theme_bd_video_formatter_preview_image_link($element) {

  $video = _bd_video_load($element['#item']['video_id']);
  _bd_video_set_render_settings(content_fields($element['#field_name'], $element['#type_name']));
  
  return _bd_video_render_video($video, 'preview-image-link', $element['#node']->nid);
}


function _bd_video_get_first_video_field($node) {
  $content_type_info = _content_type_info();
  
  foreach((array)$node as $field => $value) {
    if($content_type_info['fields'][$field]['type'] == 'bd_video') {
      return $field;
    }
  }
}


function _bd_video_get_video_fields($node) {
  $content_type_info = _content_type_info();
  $fields = array();
  
  foreach((array)$node as $field => $value) {
    if($content_type_info['fields'][$field]['type'] == 'bd_video') {
      $fields[] = $field;
    }
  }
  
  return $fields;
}


function _bd_video_get_first_video($node) {

  $field = _bd_video_get_first_video_field($node);
  
  if($field)
     return _bd_video_load($node->{$field}[0]['video_id']);
}


function bd_video_get_node_types() {
  $node_types = array();
  $content_type_info = _content_type_info();
  
  foreach($content_type_info['content types'] as $node_type) {

    foreach($node_type['fields'] as $field) {
     
      if($field['type'] == 'bd_video') {
        $node_types[$node_type['type']] = $node_type['name'];
        break;
      }
    }
  }
  
  return $node_types;
}


/**
 * Implement hook_nodeapi() .
 */
function bd_video_nodeapi($node, $op, $teaser, $page) {

  if($op != 'rss item')
    return;
    
  $video = _bd_video_get_first_video($node);

  if($video['source_file']) {

    $rss_items[] = array(
      'key' => 'enclosure',
      'attributes' => array(
        'url' => storage_api_serve_url($video['source_file'], TRUE),
        'length' => $video['source_file']['size'],
        'type' => $video['source_file']['mimetype']
      )
    );
    
    return $rss_items;
  }
}

