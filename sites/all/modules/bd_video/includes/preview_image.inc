<?php
// $Id: preview_image.inc,v 1.1.2.5 2009/11/20 10:35:25 jbrown Exp $

// Copyright 2007-2008 Jonathan Brown

define("BD_VIDEO_DEFAULT_PREVIEW_IMAGE_WIDTH", 640);
define("BD_VIDEO_DEFAULT_PREVIEW_IMAGE_HEIGHT", 360);
define("BD_VIDEO_DEFAULT_PREVIEW_IMAGE_IMAGE", 'third');
define("BD_VIDEO_DEFAULT_PREVIEW_IMAGE_USER_SELECTABLE", 1);


function _bd_video_preview_image_settings_fields() {

  return array(
    'preview_image_width',
    'preview_image_height',
    'preview_image_image',
    'preview_image_user_selectable'
  );
}


function _bd_video_preview_image_settings_fieldset($settings = NULL) {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Preview image'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  
  $fieldset['preview_image_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Default width'),
    '#default_value' => $settings['preview_image_width'] ? 
      $settings['preview_image_width'] : BD_VIDEO_DEFAULT_PREVIEW_IMAGE_WIDTH,
    '#description' => t('pixels'),
    '#size' => 6,
    '#maxlength' => 4,
    '#required' => TRUE
  );

  $fieldset['preview_image_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Default height'),
    '#default_value' => $settings['preview_image_height'] ? 
      $settings['preview_image_height'] : BD_VIDEO_DEFAULT_PREVIEW_IMAGE_HEIGHT,
    '#description' => t('pixels'),
    '#size' => 6,
    '#maxlength' => 4,
    '#required' => TRUE
  );
  
  $fieldset['preview_image_image'] = array(
    '#type' => 'radios',
    '#title' => t('Image'),
    '#default_value' => $settings['preview_image_image'] ? 
      $settings['preview_image_image'] : BD_VIDEO_DEFAULT_PREVIEW_IMAGE_IMAGE,
    '#options' => array(
      'start' => 'Start of video',
      'third' => '@ 1/3 of video duration'
    )
  );

  $fieldset['preview_image_user_selectable'] = array(
    '#type' => 'checkbox',
    '#title' => 'User selectable',
//    '#default_value' => $settings['preview_image_user_selectable'] ? 
//      $settings['preview_image_user_selectable'] : BD_VIDEO_DEFAULT_PREVIEW_IMAGE_USER_SELECTABLE
    '#default_value' => $settings['preview_image_user_selectable']
  );
    
  return $fieldset;
}


function _bd_video_preview_image_settings_validate($form_values) {
  ;
}


function _bd_video_preview_image_settings_submit(&$form_values) {
  ;
}


function _bd_video_set_preview_image($video_id, $preview_image) {
  
  $update = array(
    'video_id' => $video_id,
    'preview_image' => $preview_image,
  );
  
  drupal_write_record('bd_video', $update, 'video_id');
}


function _bd_video_get_preview_image($video) {
  if(_bd_video_get_render_setting('preview_image_user_selectable', BD_VIDEO_DEFAULT_PREVIEW_IMAGE_USER_SELECTABLE)) {
    return $video['preview_image'];
  }
  else {
    return _bd_video_get_render_setting('preview_image_image', BD_VIDEO_DEFAULT_PREVIEW_IMAGE_IMAGE);
  }
}


function _bd_video_render_preview_image($video, $width = NULL, $height = NULL, $extra = NULL, $image = NULL) {

  if(!$width)
    $width = _bd_video_get_render_setting('preview_image_width', BD_VIDEO_DEFAULT_PREVIEW_IMAGE_WIDTH);
    
  if(!$height)
    $height = _bd_video_get_render_setting('preview_image_height', BD_VIDEO_DEFAULT_PREVIEW_IMAGE_HEIGHT);
  
  if(!$image)
    $image = _bd_video_get_preview_image($video);
  
  $url = _bd_video_render_image($video, $image, $width, $height);

  if(!$url)
    return;

  $parts[] = '<img';
  $parts[] = 'src="' . $url . '"';
  $parts[] = 'width="' . $width . '"';
  $parts[] = 'height="' . $height . '"';
  $parts[] = 'alt="Video preview image"';
  
  if(is_array($extra))
    $parts = array_merge($parts, $extra);

  $parts[] = '/>';

  return implode(' ', $parts); 
}


function _bd_video_render_preview_image_link($video, $width = NULL, $height = NULL) {
  $thumb = _bd_video_render_preview_image($video, $width, $height);
  return l($thumb, 'node/' . $video['nid'], array('html' => TRUE));
}


function _bd_video_scaled_image_cache_settings_fieldset() {

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Scaled image cache')
  );

  $results = db_fetch_array(db_query("
    SELECT COUNT(*) as count, SUM({storage_object}.size) as size
    FROM {bd_video_image_cache}
    INNER JOIN {storage_file}
      ON {bd_video_image_cache}.file_id = {storage_file}.file_id
    INNER JOIN {storage_object}
      ON {storage_file}.object_id = {storage_object}.object_id
  "));
  
  $info = t('Count: ') . $results['count'] . "<br />";
  $info .= t('Size: ') . storage_api_format_size($results['size']);
  
  $fieldset[] = array(
    '#value' => $info
  );
  
  $intervals = array(3600, 10800, 21600, 32400, 43200, 86400, 172800, 
    259200, 604800, 1209600, 2419200, 4838400, 9676800);
  
  $options = drupal_map_assoc($intervals, 'format_interval');
  
  $fieldset['bd_video_scaled_image_cache_flush_interval'] = array(
    '#type' => 'select',
    '#title' => t('Cache flush interval'),
    '#default_value' => variable_get('bd_video_scaled_image_cache_flush_interval', 43200),
    '#options' => $options,
    '#description' => t('How often the cache should be flushed of stale items.')
  );
  
  $fieldset['bd_video_scaled_image_cache_timeout'] = array(
    '#type' => 'select',
    '#title' => t('Cache timeout'),
    '#default_value' => variable_get('bd_video_scaled_image_cache_timeout', 1209600),
    '#options' => $options,
    '#description' => t('Scaled images that have not been served within this time limit are considered stale and will be flushed.<br />They are generated instantly as required.')
  );
  
  $fieldset['flush_all'] = array(
    '#type' => 'submit',
    '#value' => t('Flush all'),
    '#submit' => array('_bd_video_preview_image_flush_all')
  );

  return $fieldset;
}


function _bd_video_preview_image_flush_all() {

  bd_video_flush_image_cache();
  cache_clear_all();

  $message = 'Flushed the scaled image cache.';
  
  drupal_set_message(t($message));
  watchdog('bd_video', $message);
}


function _bd_video_preview_image_flush_cache() {

  if($_SERVER['REQUEST_TIME'] < variable_get('bd_video_scaled_image_cache_last_flushed', 0) + 
    variable_get('bd_video_scaled_image_cache_flush_interval', 43200)) {
    
    return;
  }

  variable_set('bd_video_scaled_image_cache_last_flushed', $_SERVER['REQUEST_TIME']);
  
  bd_video_flush_image_cache(
    array('last_served' => $_SERVER['REQUEST_TIME'] - variable_get('bd_video_scaled_image_cache_timeout', 1209600))
  );
  
  cache_clear_all();

  watchdog('bd_video', "Flushed the scaled image cache.");
}

