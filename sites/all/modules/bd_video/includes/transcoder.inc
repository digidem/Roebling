<?php
// $Id: transcoder.inc,v 1.1.2.5 2009/12/28 18:23:28 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


define("BD_VIDEO_FORMAT_FLV", 0);
define("BD_VIDEO_FORMAT_MP4", 1);

define("BD_VIDEO_DEFAULT_FORMAT", BD_VIDEO_FORMAT_MP4);
define("BD_VIDEO_DEFAULT_WIDTH", 640);
define("BD_VIDEO_DEFAULT_HEIGHT", 360);
define("BD_VIDEO_DEFAULT_FRAME_RATE", 24);
define("BD_VIDEO_DEFAULT_KEYINT", 2);
define("BD_VIDEO_DEFAULT_VBITRATE", 320);
define("BD_VIDEO_DEFAULT_CHANNELS", 2);
define("BD_VIDEO_DEFAULT_SAMPLE_RATE", 22050);
define("BD_VIDEO_DEFAULT_ABITRATE", 64);
define("BD_VIDEO_DEFAULT_MAX_DURATION", 0);
define("BD_VIDEO_DEFAULT_KEEP_ORIGINAL", 1);

define("BD_VIDEO_MIN_DIMENSION", 32);
define("BD_VIDEO_MAX_DIMENSION", 4096);
define("BD_VIDEO_MAX_FRAME_RATE", 200);
define("BD_VIDEO_MAX_KEYINT", 600);
define("BD_VIDEO_MIN_VBITRATE", 32);
define("BD_VIDEO_MAX_VBITRATE", 4096);
define("BD_VIDEO_MIN_ABITRATE", 16);
define("BD_VIDEO_MAX_ABITRATE", 512);

_bd_video_require('ffmpeg.inc');
_bd_video_require('bluedroplet.com.inc');


function _bd_video_transcoding_params() {

  return array(
    'format',
    'width',
    'frame_rate',
    'key_interval',
    'video_bitrate',
    'audio_channels',
    'audio_sample_rate',
    'audio_bitrate',
    'max_duration',
    'keep_source_file',
  );
}


function _bd_video_transcoding_params_defaults() {

  return array(
    'format' => BD_VIDEO_DEFAULT_FORMAT,
    'width' => BD_VIDEO_DEFAULT_WIDTH,
    'height' => BD_VIDEO_DEFAULT_HEIGHT,
    'frame_rate' => BD_VIDEO_DEFAULT_FRAME_RATE,
    'key_interval' => BD_VIDEO_DEFAULT_KEYINT,
    'video_bitrate' => BD_VIDEO_DEFAULT_VBITRATE,
    'audio_channels' => BD_VIDEO_DEFAULT_CHANNELS,
    'audio_sample_rate' => BD_VIDEO_DEFAULT_SAMPLE_RATE,
    'audio_bitrate' => BD_VIDEO_DEFAULT_ABITRATE,
    'max_duration' => BD_VIDEO_DEFAULT_MAX_DURATION,
    'keep_source_file' => BD_VIDEO_DEFAULT_KEEP_ORIGINAL
  );
}


function _bd_video_transcoding_params_load($params_id) {
  
  return db_fetch_array(db_query("
    SELECT *
    FROM {bd_video_params}
    WHERE params_id = %d
  ",
    $params_id
  ));
}


function _bd_video_transcoding_params_insert($params = NULL) {
  if(!$params)
    $params = _bd_video_transcoding_params_defaults();
  
  drupal_write_record('bd_video_params', $params);
  return $params['params_id'];
}


function _bd_video_transcoding_params_update($params_id, $params) {
  $params['params_id'] = $params_id;

  drupal_write_record('bd_video_params', $params, 'params_id');
}


function _bd_video_transcoding_params_delete($params_id) {
  
  db_query("
    DELETE FROM {bd_video_params}
    WHERE params_id = %d
  ",
    $params_id
  );
}


function _bd_video_transcoding_params_fieldset($params) {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Transcoding parameters'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  
  $description = 'MP4 uses the higher performance H.264 (video) and AAC (audio) codecs, and requires Flash 9.0.115.0.';

  $fieldset['format'] = array(
    '#type' => 'select',
    '#title' => t('Format'),
    '#default_value' => $params['format'],
    '#options' => array(
      BD_VIDEO_FORMAT_FLV => 'FLV',
      BD_VIDEO_FORMAT_MP4 => 'MP4'
    ),
    '#description' => t($description)
  );

  $fieldset['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum video width'),
    '#default_value' => $params['width'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#attributes' => array('autocomplete' => 'off'),
    '#description' => t("pixels")
  );

  $fieldset['height'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum video height'),
    '#default_value' => $params['height'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#attributes' => array('autocomplete' => 'off'),
    '#description' => t("pixels")
  );

  $fieldset['frame_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum frame rate'),
    '#default_value' => $params['frame_rate'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 3,
    '#attributes' => array('autocomplete' => 'off'),
    '#description' => t("Maximum frames per second. If the rate of the source video is less, that rate is used.")
  );

  $fieldset['key_interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Key frame interval'),
    '#default_value' => $params['key_interval'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 5,
    '#attributes' => array('autocomplete' => 'off'),
    '#description' => t("Maximum interval between seekable frames (seconds).")
  );

  $fieldset['video_bitrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Video bitrate'),
    '#default_value' => $params['video_bitrate'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#attributes' => array('autocomplete' => 'off'),
    '#description' => t("kbits, 1 kbit = 1000 bits")
  );

  $fieldset['audio_channels'] = array(
    '#type' => 'radios',
    '#title' => t('Audio channels'),
    '#default_value' => $params['audio_channels'],
    '#options' => array(
      '0' => 'Mute', 
      '1' => 'Mono', 
      '2' => 'Stereo'
    ),
    '#required' => TRUE,
  );

  $fieldset['audio_sample_rate'] = array(
    '#type' => 'radios',
    '#title' => t('Audio sample rate'),
    '#default_value' => $params['audio_sample_rate'],
    '#options' => array(
      '11025' => '11,025 Hz', 
      '22050' => '22,050 Hz', 
      '44100' => '44,100 Hz'
    ),
    '#required' => TRUE,
  );

  $fieldset['audio_bitrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Audio bitrate'),
    '#default_value' => $params['audio_bitrate'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 3,
    '#attributes' => array('autocomplete' => 'off'),
    '#description' => t("kbits, 1 kbit = 1000 bits")
  );
  
  $fieldset['max_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Max duration'),
    '#default_value' => ($params['max_duration'] != 0) ? $params['max_duration'] : '',
    '#size' => 6,
    '#maxlength' => 5,
    '#attributes' => array('autocomplete' => 'off'),
    '#description' => t("Maximum duration of transcoded video (seconds).")
  );
    
  $description = 'Should uploaded video files be kept once they have been transcoded to Flash?<br />';
  $description .= 'This is necessary for videos to be available for download and ';
  $description .= 'also for retranscoding if transcoding settings are changed.';

  $fieldset['keep_source_file'] = array(
    '#type' => 'checkbox',
    '#title' => t('Keep uploaded files?'),
    '#default_value' => $params['keep_source_file'],
    '#description' => t($description)
  );

  $fieldset['break'] = array(
    '#value' => '<hr />'
  );

  $fieldset['retranscode_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Retranscode all videos?'),
    '#description' => t("This will retranscode all the videos that these parameters apply to that have their source video stored.")
  );

  return $fieldset;
}


function _bd_video_transcoding_params_validate($form_values) {

  settype($form_values['width'], 'int');
  settype($form_values['height'], 'int');
  settype($form_values['frame_rate'], 'int');
  $form_values['key_interval'] = number_format($form_values['key_interval'], 1, '.', '');
  settype($form_values['video_bitrate'], 'int');
  settype($form_values['audio_bitrate'], 'int');

  if($form_values['width'] < BD_VIDEO_MIN_DIMENSION) {
    form_set_error('width', t('Video width field must be greater than or equal to ') . BD_VIDEO_MIN_DIMENSION . '.');
  }
  else if($form_values['width'] > BD_VIDEO_MAX_DIMENSION) {
    form_set_error('width', t('Video width field must be less than or equal to ') . BD_VIDEO_MAX_DIMENSION . '.');
  }

  if($form_values['height'] < BD_VIDEO_MIN_DIMENSION) {
    form_set_error('height', t('Video height field must be greater than or equal to ') . BD_VIDEO_MIN_DIMENSION . '.');
  }
  else if($form_values['height'] > BD_VIDEO_MAX_DIMENSION) {
    form_set_error('height', t('Video height field must be less than or equal to ') . BD_VIDEO_MAX_DIMENSION . '.');
  }

  if($form_values['frame_rate'] <= 0) {
    form_set_error('frame_rate', t('Video frame rate field must be greater than 0.'));
  }
  else if($form_values['frame_rate'] > BD_VIDEO_MAX_FRAME_RATE) {
    form_set_error('frame_rate', 
      t('Video frame rate field must be less than or equal to ') . BD_VIDEO_MAX_FRAME_RATE . '.');
  }

  if($form_values['key_interval'] <= 0) {
    form_set_error('key_interval', t('Key frame interval field must be greater than 0.'));
  }
  else if($form_values['key_interval'] > BD_VIDEO_MAX_KEYINT) {
    form_set_error('key_interval', 
      t('Key frame interval field must be less than or equal to ') . BD_VIDEO_MAX_KEYINT . '.');
  }

  if($form_values['video_bitrate'] < BD_VIDEO_MIN_VBITRATE) {
    form_set_error('video_bitrate', 
      t('Video bitrate field must be greater than or equal to ') . BD_VIDEO_MIN_VBITRATE . '.');
  }
  else if($form_values['video_bitrate'] > BD_VIDEO_MAX_VBITRATE) {
    form_set_error('video_bitrate', 
      t('Video bitrate field must be less than or equal to ') . BD_VIDEO_MAX_VBITRATE . '.');
  }

  if($form_values['audio_bitrate'] < BD_VIDEO_MIN_ABITRATE) {
    form_set_error('audio_bitrate', 
      t('Audio bitrate field must be greater than or equal to ') . BD_VIDEO_MIN_ABITRATE . '.');
  }
  else if($form_values['audio_bitrate'] > BD_VIDEO_MAX_ABITRATE) {
    form_set_error('audio_bitrate', 
      t('Audio bitrate field must be less than or equal to ') . BD_VIDEO_MAX_ABITRATE . '.');
  }
}


function _bd_video_set_transcoding_failed($video_id) {

  $update = array(
    'video_id' => $video_id,
    'status' => 'idle',
    'error' => 'transcoding failed'
  );
  
  drupal_write_record('bd_video', $update, 'video_id');
}


function _bd_video_transcode_videos() {

  $transcoder = variable_get('bd_video_transcoder', BD_VIDEO_DEFAULT_TRANSCODER);
  
  $result = db_query("
    SELECT *
    FROM {bd_video}
    WHERE status = 'dirty'
    ORDER BY flv_file_id IS NOT NULL, video_id
    LIMIT 1
  ");

  while($video = db_fetch_array($result)) {
  
    if($transcoder == 'bluedroplet.com') {
    
      $queued = db_result(db_query("
        SELECT COUNT(*)
        FROM {bd_video}
        WHERE status = 'uploaded'
      "));
      
      if($queued >= 2) {
        watchdog('bd_video', 'Too many videos queued on bluedroplet.com to upload any more.', NULL);
        return;
      }
    }

/*
    if(!$video['filepath']) {    // aquire from url failed
      _bd_video_set_transcoding_failed($video['video_id']);
      continue;
    }
*/
  
    $video['source_file'] = storage_api_file_load($video['source_file_id']);
    $video['dirty_params'] = _bd_video_transcoding_params_load($video['dirty_params_id']);
    
    switch($transcoder) {

      case 'ffmpeg':
        _bd_video_transcode_ffmpeg($video);
        break;

      case 'bluedroplet.com':
        _bd_video_transcode_bluedroplet($video);
        break;
    }
  }
}


function _bd_video_transcoding_completed($video_id) {

  $video = _bd_video_load($video_id);
  
  if(!$video['dirty_params']['keep_source_file']) {
    storage_api_file_remove($video['source_file_id']);
    
    db_query("
      UPDATE {bd_video}
      SET source_file_id = NULL
      WHERE video_id = %d
    ",
      $video['video_id']
    );
  }
  
  bd_video_flush_image_cache(array('video_id' => $video['video_id']));
  _bd_video_do_transcoding_actions($video['video_id']);
}


function _bd_video_retranscode_video($video_id) {

  $update = array(
    'video_id' => $video_id,
    'secret' => md5(user_password()),
    'status' => 'dirty',
    'upload_failed' => 0,
    'uploaded' => 0,
  );
  
  drupal_write_record('bd_video', $update, 'video_id');
}

