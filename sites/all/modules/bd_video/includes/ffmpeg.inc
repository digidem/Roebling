<?php
// $Id: ffmpeg.inc,v 1.1.2.15 2009/12/17 07:15:03 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


define("BD_VIDEO_DEFAULT_FFMPEG_BINARY", 'ffmpeg');
define("BD_VIDEO_DEFAULT_QT_FASTSTART_BINARY", 'qt-faststart');


function _bd_video_ffmpeg_exec($params, &$output, &$result, $mode = 0) {

  $ffmpeg = variable_get('bd_video_ffmpeg_binary', BD_VIDEO_DEFAULT_FFMPEG_BINARY);
  $extra_params = variable_get('bd_video_ffmpeg_extra_params', '');

  $command = array($ffmpeg, $extra_params, $params, '2>&1');

  _bd_video_exec($command, $output, $result, $mode);
}


function _bd_video_qt_faststart_exec($params, &$output, &$result, $mode = 0) {

  $qt_faststart = variable_get('bd_video_qt_faststart_binary', BD_VIDEO_DEFAULT_QT_FASTSTART_BINARY);

  $command = array($qt_faststart, $params, '2>&1');
  
  _bd_video_exec($command, $output, $result, $mode);
}


function _bd_video_ffmpeg_version_info_fieldset($output) {
  array_shift($output);
  
  if($output[0] == 'malloc: using debugging hooks')
    array_shift($output);

  array_pop($output);

  foreach($output as &$line) {
    $line = str_replace('  ', '&nbsp;&nbsp;', $line);
  }
  
  $output = implode("<br />\n", $output);
  
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Version info'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $fieldset[] = array(
    '#value' => '<code>' . $output . '</code>'
  );
  
  return $fieldset;
}


function _bd_video_ffmpeg_dot() {

  $url = base_path() . 'misc/menu-leaf.png';

  return array(
    'data' => '<img src="' . $url . '" />', 
    'style' => 'text-align: center;'
  );
}


function _bd_video_ffmpeg_formats_fieldset(&$output) {
  $dot = _bd_video_ffmpeg_dot();

  while(($line = array_shift($output))) {
    if($line == 'File formats:')
      break;
  }
  
  if(!$line)
    return;

  while(($line = array_shift($output)) != '') {
  
    // fixup for latest ffmpeg builds, while maintaining compatibility with 0.5
    if($line == ' --') {
      unset($rows);
      continue;
    }
      
  
    preg_match('/ (.)(.) ([^ ]*) *(.*)/', $line, $matches);
    
    $important = in_array($matches[3], array('flv', 'mp4'));

    $rows[] = array(
      'data' => array(
        $matches[3],
        $matches[4],
        $matches[1] == 'D' ? $dot : '',
        $matches[2] == 'E' ? $dot : ''
      ),
      'style' => $important ? 'background-color: #425F95; color: white;' : NULL      
    );
  }

  $header = array(
    t('Name'), 
    t('Description'),
    array(
      'data' => t('Decode'), 
      'style' => 'text-align: center;'
    ),
    array(
      'data' => t('Encode'), 
      'style' => 'text-align: center;'
    ),
  );

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('File formats'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $fieldset[] = array(
    '#value' => theme('table', $header, $rows)
  );
  
  $note  = 'Note, the names of decoders and encoders do not always match, so there may be cases where the above table ';
  $note .= 'shows decode only or encode only entries even though both decoding and encoding are supported, ';
  $note .= 'e.g. mov / mp4 / 3gp.';

  $fieldset[] = array(
    '#value' => '<p>' . t($note) . '</p>'
  );
  
  return $fieldset;
}


function _bd_video_ffmpeg_codecs_fieldset(&$output, &$codecs) {
  $dot = _bd_video_ffmpeg_dot();

  while(($line = array_shift($output))) {
    if($line == 'Codecs:')
      break;
  }

  if(!$line)
    return;
    
  $codecs = array(
    'flv' => FALSE,
    'libmp3lame' => FALSE,
    'libx264' => FALSE,
    'libfaac' => FALSE,
    'png' => FALSE
  );

  while(($line = array_shift($output)) != '') {
  
    // fixup for latest ffmpeg builds, while maintaining compatibility with 0.5
    if($line == ' ------') {
      unset($audio);
      unset($video);
      continue;
    }


    preg_match('/ (.)(.)(.)... ([^ ]*) *(.*)/', $line, $matches);

    if(array_key_exists($matches[4], $codecs))
      $codecs[$matches[4]] = TRUE;
    
    $codec = array(
      '',
      $matches[4],
      $matches[5],
      $matches[1] == 'D' ? $dot : '',
      $matches[2] == 'E' ? $dot : ''
    );
    
    switch($matches[3]) {
    
      case 'A':
        $audio[] = array(
          'data' => $codec,
          'style' => $codecs[$matches[4]] ? 'background-color: #425F95; color: white;' : NULL
        );
        break;
        
      case 'V':
        $video[] = array(
          'data' => $codec,
          'style' => $codecs[$matches[4]] ? 'background-color: #425F95; color: white;' : NULL
        );
        break;
    }
  }
  
  $header = array(
    '',
    t('Name'), 
    t('Description'), 
    array(
      'data' => t('Decode'), 
      'style' => 'text-align: center;'
    ),
    array(
      'data' => t('Encode'), 
      'style' => 'text-align: center;'
    ),
  );

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Codecs'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  array_unshift($audio, array(array(
    'data' => '<b>' . t('Audio codecs') . '</b>', 
    'colspan' => '5'
  )));

  array_unshift($video, array(array(
    'data' => '<b>' . t('Video codecs') . '</b>', 
    'colspan' => '5'
  )));

  $fieldset[] = array(
    '#value' => theme('table', $header, array_merge($audio, $video))
  );

  $note  = 'Note, the names of decoders and encoders do not always match, so there may be cases where the above table ';
  $note .= 'shows decode only or encode only entries even though both decoding and encoding are supported, ';
  $note .= 'e.g. h264 / libx264.';

  $fieldset[] = array(
    '#value' => '<p>' . t($note) . '</p>'
  );

  return $fieldset;
}


function _bd_video_ffmpeg_settings_fieldset($active) {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Local FFmpeg installation'),
    '#collapsible' => TRUE,
    '#collapsed' => !$active
  );

  $fieldset['bd_video_ffmpeg_binary'] = array(
    '#type' => 'textfield',
    '#title' => t('FFmpeg binary'),
    '#description' => t('If the binary is not in the default path, you should enter the absolute path to it.'),
    '#default_value' => variable_get('bd_video_ffmpeg_binary', BD_VIDEO_DEFAULT_FFMPEG_BINARY),
  );
  
  $fieldset['bd_video_ffmpeg_extra_params'] = array(
    '#type' => 'textfield',
    '#title' => t('Extra parameters (advanced)'),
    '#description' => t('Any extra <a href="http://ffmpeg.org/ffmpeg-doc.html">parameters</a> you wish to have appended onto the FFmpeg command.'),
    '#default_value' => variable_get('bd_video_ffmpeg_extra_params', ''),
  );
  
  $output = array();
  _bd_video_ffmpeg_exec('-version', $output, $result);
  
  if($result == 0) {
    $fieldset[] = _bd_video_ffmpeg_version_info_fieldset($output);

    $output = array();
    _bd_video_ffmpeg_exec('-formats', $output, $result);

    $fieldset[] = _bd_video_ffmpeg_formats_fieldset($output);
    
    $codecs_fieldset = _bd_video_ffmpeg_codecs_fieldset($output, $codecs);
    
    if(!$codecs_fieldset) {
      _bd_video_ffmpeg_exec('-codecs', $output, $result);
      $codecs_fieldset = _bd_video_ffmpeg_codecs_fieldset($output, $codecs);
    }
    
    $fieldset[] = $codecs_fieldset;
    
    if(!$codecs['libmp3lame']) {
      $fieldset[] = array(
        '#value' => '<p class="error">' . t('libmp3lame codec not found - FLV encoding will fail.') . '</p>'
      );
    }
    
    if(!$codecs['libx264']) {
      $fieldset[] = array(
        '#value' => '<p class="error">' . t('libx264 codec not found - MP4 encoding will fail.') . '</p>'
      );
    }
    
    if(!$codecs['libfaac']) {
      $fieldset[] = array(
        '#value' => '<p class="error">' . t('libfaac codec not found - MP4 encoding will fail.') . '</p>'
      );
    }

    if(!$codecs['png']) {
      $fieldset[] = array(
        '#value' => '<p class="error">' . t('png codec not found - preview image generation will fail.') . '</p>'
      );
    }
  }
  else {
    $fieldset[] = array(
      '#value' => '<p class="error">' . t('FFmpeg not available - transcoding will fail.') . '</p>'
    );
  }
  
  $fieldset['bd_video_qt_faststart_binary'] = array(
    '#type' => 'textfield',
    '#title' => t('qt-faststart binary'),
    '#description' => t('If the binary is not in the default path, you should enter the absolute path to it.'),
    '#default_value' => variable_get('bd_video_qt_faststart_binary', BD_VIDEO_DEFAULT_QT_FASTSTART_BINARY),
  );
  
  $output = array();
  _bd_video_qt_faststart_exec(NULL, $output, $result);
  
  if($result != 0) {
    $fieldset[] = array(
      '#value' => '<p class="error">' . t('qt-faststart not available - MP4 encoding will fail.') . '</p>'
    );
  }

  return $fieldset;
}


function _bd_video_ffmpeg_get_info($source_file) {
  
  $params = '-i ' . escapeshellarg($source_file['filepath']);
  $output = array();
  _bd_video_ffmpeg_exec($params, $output, $result);
  array_shift($output);
  $output = implode("\n", $output);
  
  if(preg_match('/Duration: *(..):(..):(..\..*), /', $output, $matches))
    $info['duration'] = ($matches[1] * 60 + $matches[2]) * 60 + $matches[3];
  else
    return NULL;
    
  if(preg_match('/Audio: (.*)/', $output, $matches)) {
    $parts = explode(', ', $matches[1]);
    
    $info['audio_codec'] = strtok(array_shift($parts), ' ');
    $info['audio_sample_rate'] = strtok(array_shift($parts), ' ');
    
    switch(array_shift($parts)) {
      
      case 'mono':
      case '1 channels':
        $info['audio_channels'] = 1;
        break;

      case 'stereo':
      case '2 channels':
        $info['audio_channels'] = 2;
        break;
    }
    
    if(count($parts) > 1)
      array_shift($parts);
      
    $info['audio_bitrate'] = strtok(array_shift($parts), ' ');
  }
  else {
    $info['audio_channels'] = 0;
  }

  if(preg_match('/Video: (.*)/', $output, $matches)) {
  
    $parts = explode(', ', $matches[1]);
    
    $info['video_codec'] = array_shift($parts);
    array_shift($parts);
    $dims = explode('x', array_shift($parts));
    $info['width'] = $dims[0];
    $info['height'] = strtok($dims[1], ' ');  // PAR / DAR info is sometimes after the height
    
    // latest FFmpegs have three frame rates, optionally preceeded by the bitrate
    // older FFmpegs have one frame rate, optionally preceeded by the bitrate
    
    if(count($parts) == 4 || count($parts) == 2)
      $info['video_bitrate'] = strtok(array_shift($parts), ' ');
    else
      $info['video_bitrate'] = $source_file['size'] * 8 / 1000 / $info['duration'] - $info['audio_bitrate'];
      
    $info['frame_rate'] = strtok(array_shift($parts), ' ');
  }

  $info['source_file'] = $source_file;
  
  return $info;
}


function _bd_video_ffmpeg_create_png($video_id, $info, $i, $selector_id, &$output) {
  
  $params[] = '-v 0 -y -i ' . escapeshellarg($info['source_file']['filepath']);

  $out_file = tempnam(file_directory_temp(), '');

  if($i == 2)
    $params[] = '-ss ' . $info['duration'] / 3;

  $params[] = '-vcodec png';
  $params[] = '-vframes 1';
  $params[] = '-f rawvideo';
  $params[] = '-s ' . ((int)($info['width'] / 2)) * 2 . 'x' . ((int)($info['height'] / 2)) * 2;
  $params[] = '-an';
  $params[] = $out_file;

  _bd_video_ffmpeg_exec($params, $output, $result, BD_VIDEO_EXEC_MODE_ECHO);

  if($result == 0) {
  
    $options = array(
      'module' => 'bd_video',
      'type' => 'preview image',
      'nid' => $info['source_file']['nid'],
      'filepath' => $out_file
    );
  
    return storage_api_file_add($selector_id, $options);
  }
  else {
    @unlink($out_file);
    return FALSE;
  }
}


function _bd_video_ffmpeg_create_flv($video_id, $info, $dirty_params, $selector_id, &$output) {

  $params[] = '-v 0 -y -i ' . escapeshellarg($info['source_file']['filepath']);

  if($dirty_params['max_duration'] != 0 && 
    $info['duration'] > $dirty_params['max_duration'])
  {
    $params[] = '-t ' . $dirty_params['max_duration'];
  }

  $params[] = '-f flv';

  if($info['video_codec'] == 'flv' &&
    $info['video_bitrate'] <= $dirty_params['video_bitrate'] &&
    $info['width'] <= $dirty_params['width'] &&
    $info['height'] <= $dirty_params['height'] &&
    $info['frame_rate'] <= $dirty_params['frame_rate'])
  {
    $params[] = '-vcodec copy';
    $passthrough = 1;
  }
  else {
    $params[] = '-vcodec flv';
    $params[] = '-b ' . $dirty_params['video_bitrate'] . 'k';

    if($info['frame_rate'] > $dirty_params['frame_rate']) {
      $frame_rate = $dirty_params['frame_rate'];
      $params[] = '-r ' . $frame_rate;
    }
    else {
      $frame_rate = $info['frame_rate'];
    }
    
    $params[] = '-g ' . (int)round($frame_rate * $dirty_params['key_interval']);

    if($info['width'] <= $dirty_params['width'] &&
      $info['height'] <= $dirty_params['height'])
    {
      $width = $info['width'];
      $height = $info['height'];
    }
    else {
      if($dirty_params['width'] / $dirty_params['height'] > $info['width'] / $info['height']) {
      
        $height = $dirty_params['height'];
        $width = (int)round(($info['width'] * $dirty_params['height']) / $info['height']);
      }
      else {
        $width = $dirty_params['width'];
        $height = (int)round(($info['height'] * $dirty_params['width']) / $info['width']);
      }
    }
    
    $params[] = '-s ' . ((int)($width / 2)) * 2 . 'x' . ((int)($height / 2)) * 2;
  }
  
  $channels = min($info['audio_channels'], $dirty_params['audio_channels']);

  if($channels == 0) {
    $params[] = '-an';
  }
  else if($info['audio_codec'] == 'mp3' &&
    in_array($info['audio_sample_rate'], array(11025, 22050, 44100)) &&
    $info['audio_bitrate'] <= $dirty_params['audio_bitrate'] &&
    $info['audio_channels'] <= $dirty_params['audio_channels'] &&
    $info['audio_sample_rate'] <= $dirty_params['audio_sample_rate'])
  {
    $params[] = '-acodec copy';
  }
  else {
    $params[] = '-acodec libmp3lame';
    $params[] = '-ab ' . $dirty_params['audio_bitrate'] . 'k';
    $params[] = '-ac ' . $channels;
    $params[] = '-ar ' . $dirty_params['audio_sample_rate'];
  }
  
  $out_file = tempnam(file_directory_temp(), '');

  if($passthrough) {
    _bd_video_ffmpeg_exec(array($params, $out_file), $output, $result, BD_VIDEO_EXEC_MODE_ECHO);
  }
  else {
    $log_file = tempnam(file_directory_temp(), '');

    _bd_video_ffmpeg_exec(array($params, '-pass 1 -passlogfile ' . $log_file, $out_file), 
      $output, $result, BD_VIDEO_EXEC_MODE_ECHO);

    if($result == 0) {
      _bd_video_ffmpeg_exec(array($params, '-pass 2 -passlogfile ' . $log_file, $out_file), 
        $output, $result, BD_VIDEO_EXEC_MODE_ECHO);

      if($result != 0) {  // dual pass failed - try single pass
        _bd_video_ffmpeg_exec(array($params, $out_file), 
          $output, $result, BD_VIDEO_EXEC_MODE_ECHO);
      }
    }

    @unlink($log_file . '-0.log');
  }
  
  if($result == 0) {
  
    $options = array(
      'module' => 'bd_video',
      'type' => 'transcoded video',
      'nid' => $info['source_file']['nid'],
      'filepath' => $out_file
    );
  
    return storage_api_file_add($selector_id, $options);
  }
  else {
    @unlink($out_file);
    return FALSE;
  }
}


function _bd_video_ffmpeg_create_mp4($video_id, $info, $dirty_params, $selector_id, &$output) {
  $original_dir = getcwd();
  
  $params[] = '-v 0 -y -i ' . escapeshellarg(realpath($info['source_file']['filepath']));

  if($dirty_params['max_duration'] != 0 && 
    $info['duration'] > $dirty_params['max_duration'])
  {
    $params[] = '-t ' . $dirty_params['max_duration'];
  }
  
  $params[] = '-f mp4';

  if($info['video_codec'] == 'h264' && 
    $info['video_bitrate'] <= $dirty_params['video_bitrate'] &&
    $info['width'] <= $dirty_params['width'] &&
    $info['height'] <= $dirty_params['height'] &&
    $info['frame_rate'] <= $dirty_params['frame_rate'])
  {
    $params[] = '-vcodec copy';
    $passthrough = 1;
  }
  else {
    $params[] = '-vcodec libx264';
    $params[] = '-b ' . $dirty_params['video_bitrate'] . 'k';
    $params[] = '-bt ' . $dirty_params['video_bitrate'] . 'k';

    if($info['frame_rate'] > $dirty_params['frame_rate']) {
      $frame_rate = $dirty_params['frame_rate'];
      $params[] = '-r ' . $frame_rate;
    }
    else {
      $frame_rate = $info['frame_rate'];
    }
    
    $params[] = '-g ' . (int)round($frame_rate * $dirty_params['key_interval']);

    if($info['width'] <= $dirty_params['width'] &&
      $info['height'] <= $dirty_params['height'])
    {
      $width = $info['width'];
      $height = $info['height'];
    }
    else {
      if($dirty_params['width'] / $dirty_params['height'] > $info['width'] / $info['height']) {
      
        $height = $dirty_params['height'];
        $width = (int)round(($info['width'] * $dirty_params['height']) / $info['height']);
      }
      else {
        $width = $dirty_params['width'];
        $height = (int)round(($info['height'] * $dirty_params['width']) / $info['width']);
      }
    }
    
    $params[] = '-s ' . ((int)($width / 2)) * 2 . 'x' . ((int)($height / 2)) * 2;
  }
  
  $channels = min($info['audio_channels'], $dirty_params['audio_channels']);

  if($channels == 0) {
    $audio_params[] = '-an';
  }
  else if($info['audio_codec'] == 'aac' &&
    $info['audio_bitrate'] <= $dirty_params['audio_bitrate'] &&
    $info['audio_channels'] <= $dirty_params['audio_channels'] &&
    $info['audio_sample_rate'] <= $dirty_params['audio_sample_rate'])
  {
    $audio_params[] = '-acodec copy';
  }
  else {
    $audio_params[] = '-acodec libfaac';
    $audio_params[] = '-ab ' . $dirty_params['audio_bitrate'] . 'k';
    $audio_params[] = '-ac ' . $channels;
    $audio_params[] = '-ar ' . $dirty_params['audio_sample_rate'];
  }
  
  $pre_file = tempnam(file_directory_temp(), '');

  if($passthrough) {
    _bd_video_ffmpeg_exec(array($params, $audio_params, $pre_file), $output, $result, BD_VIDEO_EXEC_MODE_ECHO);
  }
  else {
    chdir(file_directory_temp());   // this is necessary because libx264 dumps its log file in the current directory,
                                    // which usually isn't writable

    $log_file = '2pass-' . $video_id;
    
    _bd_video_ffmpeg_exec(array($params, '-vpre libx264-fastfirstpass -an -pass 1 -passlogfile ' . $log_file, $pre_file), 
      $output, $result, BD_VIDEO_EXEC_MODE_ECHO);

    if($result == 0) {
      _bd_video_ffmpeg_exec(array($params, '-vpre libx264-hq', $audio_params, '-pass 2 -passlogfile ' . $log_file, $pre_file), 
        $output, $result, BD_VIDEO_EXEC_MODE_ECHO);

      if($result != 0) {  // dual pass failed - try single pass
        _bd_video_ffmpeg_exec(array($params, '-vpre libx264-hq', $audio_params, $pre_file), 
          $output, $result, BD_VIDEO_EXEC_MODE_ECHO);
      }
    }

    @unlink($log_file . '-0.log');
    @unlink('x264_2pass.log');

    chdir($original_dir);
  }
  
  if($result != 0) {
    @unlink($pre_file);
    return FALSE;
  }

  $out_file = tempnam(file_directory_temp(), '');
  
  _bd_video_qt_faststart_exec(array($pre_file, $out_file), 
    $output, $result, BD_VIDEO_EXEC_MODE_ECHO);

  @unlink($pre_file);
  
  if($result == 0) {

    $options = array(
      'module' => 'bd_video',
      'type' => 'transcoded video',
      'nid' => $info['source_file']['nid'],
      'filepath' => $out_file
    );
  
    return storage_api_file_add($selector_id, $options);
  }
  else {
    @unlink($out_file);
    return FALSE;
  }
}


function _bd_video_transcode_ffmpeg($video) {

  $file = storage_api_file_load($video['source_file_id']);
  $video['source_file']['filepath'] = storage_api_get_local_filepath($file, $keep_me);

  $info = _bd_video_ffmpeg_get_info($video['source_file']);

  $selector_ids = _bd_video_get_selector_ids($video['field']);
  
  if($info) {
    switch($video['dirty_params']['format']) {
    
      case BD_VIDEO_FORMAT_FLV:
        $flv_file_id = _bd_video_ffmpeg_create_flv($video['video_id'], $info, $video['dirty_params'], 
          $selector_ids['transcoded'], $output);
        break;
    
      case BD_VIDEO_FORMAT_MP4:
        $flv_file_id = _bd_video_ffmpeg_create_mp4($video['video_id'], $info, $video['dirty_params'], 
          $selector_ids['transcoded'], $output);
        break;
    }
  }
  
  if($flv_file_id) {
    $start_image_file_id = 
      _bd_video_ffmpeg_create_png($video['video_id'], $info, 1, $selector_ids['transcoded'], $output);
    $third_image_file_id = 
      _bd_video_ffmpeg_create_png($video['video_id'], $info, 2, $selector_ids['transcoded'], $output);

    $current_params_id = _bd_video_transcoding_params_insert($video['dirty_params']);

    $update = array(
      'video_id' => $video['video_id'],
      'duration' => (int)ceil($info['duration']),
      'flv_file_id' => $flv_file_id,
      'start_image_file_id' => $start_image_file_id,
      'third_image_file_id' => $third_image_file_id,
      'current_params_id' => $current_params_id,
      'status' => 'idle',
      'error' => 'none',
    );
    
    drupal_write_record('bd_video', $update, 'video_id');

    storage_api_inherit_servings($flv_file_id, $video['flv_file_id']);
    
    storage_api_file_remove($video['flv_file_id']);
    storage_api_file_remove($video['start_image_file_id']);
    storage_api_file_remove($video['third_image_file_id']);

    _bd_video_transcoding_completed($video['video_id']);
  }
  else {
    _bd_video_set_transcoding_failed($video['video_id']);
  }
  
  if(!$keep_me)
    @unlink($video['source_file']['filepath']);
}

