<?php
// $Id: image_cache.inc,v 1.1.2.10 2009/12/07 21:23:49 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


/**
 * Implement hook_storage_file_generate() .
 */
function bd_video_storage_file_generate($file) {

  if($file['type'] != 'scaled preview image')
    return FALSE;

  $source_file_id = $file['data']['source_file_id'];
  $width = $file['data']['width'];
  $height = $file['data']['height'];

  $source_file = storage_api_file_load($source_file_id);
  $in_file = storage_api_get_local_filepath($source_file, $keep_me);

  $info = image_get_info($in_file);
  
  if(!$info) {
    if(!$keep_me)
      @unlink($in_file);
    return FALSE;
  }
  
  if($width / $height > $info['width'] / $info['height']) {
  
    $scale_height = $height;
    $scale_width = (int)round(($info['width'] * $height) / $info['height']);
    $left = (int)round(($width - $scale_width) / 2);
  }
  else {
    $scale_width = $width;
    $scale_height = (int)round(($info['height'] * $width) / $info['width']);
    $top = (int)round(($height - $scale_height) / 2);
  }
  
  
  include_once 'includes/image.gd.inc';
  $src = image_gd_open($in_file, $info['extension']);
  
  if(!$keep_me)
    @unlink($in_file);

  if(!$src)
    return FALSE;
  
  $dest = imagecreatetruecolor($width, $height);
  
  if(!$dest) {
    imagedestroy($src);
    return FALSE;
  }
  
  $success = imagecopyresampled($dest, $src, $left, $top, 0, 0, 
    $scale_width, $scale_height, $info['width'], $info['height']);

  imagedestroy($src);
  
  if(!$success) {
    imagedestroy($dest);
    return FALSE;
  }
  
  $out_file = tempnam(file_directory_temp(), '');
  imageinterlace($dest, TRUE);
  $success = image_gd_close($dest, $out_file, 'jpeg');
  imagedestroy($dest);
  
  if(!$success) {
    @unlink($out_file);
    return FALSE;
  }
  
  return $out_file;
}


function _bd_video_render_image($video, $image, $width, $height) {

  $file_id = db_result(db_query("
    SELECT file_id
    FROM {bd_video_image_cache}
    WHERE video_id = %d AND 
      image = '%s' AND
      width = %d AND
      height = %d
  ",
    $video['video_id'], $image, $width, $height
  ));
  
  if($file_id)
    return storage_api_serve_url(storage_api_file_load($file_id));

  $source_file_id = $video[$image . '_image_file']['file_id'];

  if(!$source_file_id)
    return FALSE;

  $selector_ids = _bd_video_get_selector_ids($video['field']);

  $options = array(
    'module' => 'bd_video',
    'type' => 'scaled preview image',
    'nid' => $video[$image . '_image_file']['nid'],
    'data' => array(
      'source_file_id' => $source_file_id,
      'width' => $width,
      'height' => $height
    )
  );

  $file_id = storage_api_file_add($selector_ids['cache'], $options);
  
  if(!$file_id)
    return FALSE;
    
  $image_cache = array(
    'video_id' => $video['video_id'],
    'image' => $image,
    'width' => $width,
    'height' => $height,
    'file_id' => $file_id,
  );

  drupal_write_record('bd_video_image_cache', $image_cache);
  
  return storage_api_serve_url(storage_api_file_load($file_id));
}


/**
 * Flushes scaled images from the cache.
 *
 * @param $conditions
 *   An associative array of conditions. Scaled images matching all conditions will be flushed.
 *     - 'video_id'
 *       video_id of video the scaled images belong to.
 *     - 'image'
 *       'first', 'third' or 'custom' scaled images.
 *     - 'last_touched'
 *       A unix timestamp. Scaled images touched before this time.
 */
function bd_video_flush_image_cache($conditions = array()) {

  if($conditions['video_id']) {
  
    $where[] = "{bd_video_image_cache}.video_id = %d";
    $values[] = $conditions['video_id'];
  }
    
  if($conditions['image']) {
  
    $where[] = "{bd_video_image_cache}.image = '%s'";
    $values[] = $conditions['image'];
  }

  if($conditions['last_touched']) {
  
    $where[] = "{storage_file}.obtained < %d";
    $values[] = $conditions['last_touched'];
    
    $where[] = "{storage_file}.last_touched < %d";
    $values[] = $conditions['last_touched'];
  }

  if(count($where))
    $where = 'WHERE ' . implode(' AND ', $where);

  $result = db_query("
    SELECT {bd_video_image_cache}.file_id AS file_id
    FROM {bd_video_image_cache}
    INNER JOIN {storage_file}
      ON {bd_video_image_cache}.file_id = {storage_file}.file_id
    $where
  ",
    $values
  );
  
  while($file = db_fetch_array($result)) {
    storage_api_file_remove($file['file_id']);

    db_query("
      DELETE FROM {bd_video_image_cache}
      WHERE file_id = %d
    ",
      $file['file_id']
    );
  }
}

