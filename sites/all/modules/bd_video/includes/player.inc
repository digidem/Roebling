<?php
// $Id: player.inc,v 1.1.2.9 2009/12/28 18:25:26 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


define("BD_VIDEO_DEFAULT_VIDEO_WIDTH", 640);
define("BD_VIDEO_DEFAULT_VIDEO_HEIGHT", 360);
define("BD_VIDEO_DEFAULT_CONTROLBAR_HEIGHT", 24);
define("BD_VIDEO_DEFAULT_CONTROLBAR_AUTO_HIDE", 'never');
define("BD_VIDEO_DEFAULT_BUFFER_LENGTH", 10);
define("BD_VIDEO_DEFAULT_AUTO_START", FALSE);
define("BD_VIDEO_DEFAULT_PLAYLOG", FALSE);

define("BD_VIDEO_MAX_BUFFER_LENGTH", 600);


function _bd_video_player_settings_fields() {

  return array(
    'video_player_settings_saved',
    'video_width',
    'video_height',
    'controlbar_height',
    'controlbar_auto_hide',
    'buffer_length',
    'auto_start',
    'playlog',
  );
}


function _bd_video_player_settings_fieldset($settings = NULL) {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Video player'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $fieldset['video_player_settings_saved'] = array(
    '#type' => 'value',
    '#value' => TRUE
  );

  $fieldset['video_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Video width'),
    '#default_value' => $settings['video_player_settings_saved'] ? 
      $settings['video_width'] : BD_VIDEO_DEFAULT_VIDEO_WIDTH,
    '#description' => t('pixels'),
    '#size' => 6,
    '#maxlength' => 4,
    '#required' => TRUE
  );

  $fieldset['video_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Video height'),
    '#default_value' => $settings['video_player_settings_saved'] ? 
      $settings['video_height'] : BD_VIDEO_DEFAULT_VIDEO_HEIGHT,
    '#description' => t('pixels'),
    '#size' => 6,
    '#maxlength' => 4,
    '#required' => TRUE
  );

  $fieldset['controlbar_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Controlbar height'),
    '#description' => t('pixels'),
    '#size' => 6,
    '#maxlength' => 6,
    '#default_value' => $settings['video_player_settings_saved'] ? 
      $settings['controlbar_height'] : BD_VIDEO_DEFAULT_CONTROLBAR_HEIGHT,
  );

  $fieldset['controlbar_auto_hide'] = array(
    '#type' => 'select',
    '#title' => t('Controlbar auto hide'),
    '#options' => array(
      'never' => t('Never'),
      'always' => t('Always'),
      'fullscreen' => t('Fullscreen only')
    ),
    '#default_value' => $settings['video_player_settings_saved'] ? 
      $settings['controlbar_auto_hide'] : BD_VIDEO_DEFAULT_CONTROLBAR_AUTO_HIDE,
  );

  $fieldset['buffer_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Buffer length'),
    '#default_value' => $settings['video_player_settings_saved'] ? 
      $settings['buffer_length'] : BD_VIDEO_DEFAULT_BUFFER_LENGTH,
    '#size' => 6,
    '#maxlength' => 3, 
    '#attributes' => array('autocomplete' => 'off'),
    '#required' => TRUE,
    '#description' => t('This sets the number of seconds a video should be buffered ahead.'),
  );

  $fieldset['auto_start'] = array(
    '#type' => 'checkbox',
    '#title' => t('Start immediately'),
    '#default_value' => $settings['video_player_settings_saved'] ? 
      $settings['auto_start'] : BD_VIDEO_DEFAULT_AUTO_START,
    '#description' => 
      t("Start playing the video when the page loads (only applies when the node is being viewed as a page).")
  );

  $fieldset['playlog'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log detailed play statistics'),
    '#default_value' => $settings['video_player_settings_saved'] ? 
      $settings['playlog'] : BD_VIDEO_DEFAULT_PLAYLOG,
    '#description' => 
      t("Ajax calls are made when seeking and when play stops to log which segments of video were watched."),
  );

  return $fieldset;
}


function _bd_video_player_settings_validate($form_values) {

  if(!is_numeric($form_values['buffer_length'])) {
    form_set_error('buffer_length', t('Buffer length field must be numeric.'));
  }
  else if($form_values['buffer_length'] < 1) {
    form_set_error('buffer_length', t('Buffer length field must be greater than 0.'));
  }
  else if($form_values['buffer_length'] > BD_VIDEO_MAX_BUFFER_LENGTH) {
    form_set_error('buffer_length', 
      t('Buffer length field must be less than or equal to ') . BD_VIDEO_MAX_BUFFER_LENGTH . '.');
  }
}


function _bd_video_player_render($video) {
  static $player_id;

  $output = '<div id="flowplayer' . $player_id . '" class="flowplayer"';

  $settings_saved = _bd_video_get_render_setting('video_player_settings_saved');
  
  $video_width = $settings_saved ? _bd_video_get_render_setting('video_width') : BD_VIDEO_DEFAULT_VIDEO_WIDTH;
  $video_height = $settings_saved ? _bd_video_get_render_setting('video_height') : BD_VIDEO_DEFAULT_VIDEO_HEIGHT;

  $controlbar_height = $settings_saved ? _bd_video_get_render_setting('controlbar_height') :
    BD_VIDEO_DEFAULT_CONTROLBAR_HEIGHT;
  $controlbar_auto_hide = $settings_saved ? _bd_video_get_render_setting('controlbar_auto_hide') :
    BD_VIDEO_DEFAULT_CONTROLBAR_AUTO_HIDE;

  $player_height = $video_height;

  if($controlbar_auto_hide != 'always')
    $player_height += $controlbar_height;

  $output .= 'style="width: ' . $video_width . 'px; height: ' . $player_height . 'px;">';

  $auto_start = $settings_saved ? _bd_video_get_render_setting('auto_start') : BD_VIDEO_DEFAULT_AUTO_START;

  if(!$auto_start) {
    $output .= _bd_video_render_preview_image($video, $video_width, $video_height, array('style="cursor: pointer;"'));
  }
  
  $output .= '</div>';
  
  $config = array(
    'plugins' => array(
      'controls' => array(
        'autoHide' => $controlbar_auto_hide,
        'height' => $controlbar_height,
      )
    ),
    'clip' => array(
      'scaling' => 'fit',
      'bufferLength' => _bd_video_get_render_setting('buffer_length', BD_VIDEO_DEFAULT_BUFFER_LENGTH),
    ),
  );
  
  $playlog = $settings_saved ? _bd_video_get_render_setting('playlog') : BD_VIDEO_DEFAULT_PLAYLOG;
  
  if($playlog) {

    $config['clip'] += array(
      'playlog' => TRUE,
      'onBeforeSeek' => 'bdVideoPlayLogSeek',
      'onStop' => 'bdVideoPlayLogStop',
    );

    drupal_add_js(drupal_get_path('module', 'bd_video') .'/playlog.js');
  }
  
  if(!$video['advert']['flv_file']) {

    $config['clip']['url'] = rawurlencode(storage_api_serve_url($video['flv_file']));
    $config['clip']['video_id'] = $video['video_id'];
  }
  else {
    $config['plugins']['controls']['display'] = 'none';
    
    $config['playlist'][] = array(
      'url' => rawurlencode(storage_api_serve_url($video['advert']['flv_file'])),
      'video_id' => $video['advert']['video_id'],
      'onStart' => 'new Function("this.unmute(); this.setVolume(100);")',
      'onBeforePause' => 'new Function("return false;")',
    );

    $config['playlist'][] = array(
      'url' => rawurlencode(storage_api_serve_url($video['flv_file'])),
      'video_id' => $video['video_id'],
      'onStart' => 'new Function("this.getControls().show();")',
      'onFinish' => 'new Function("this.getControls().hide();")',
    );
  }
  
  flowplayer_add('#flowplayer' . $player_id, $config);
  
  $player_id++;
  return $output;
}

