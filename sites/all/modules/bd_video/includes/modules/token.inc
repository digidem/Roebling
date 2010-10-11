<?php
// $Id: token.inc,v 1.1.2.2 2009/12/07 21:23:49 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


/**
 * Implement hook_token_values() .
 */
function bd_video_token_values($type, $object = NULL) {
  $values = array();

  if($type == 'field' || $type == 'all') {
    $video = _bd_video_load($object[0]['video_id']);
    
    if($video) {

      $values['duration'] = $video['duration'] ? 
        format_interval($video['duration']) : '';
    
      $values['duration-raw'] = $video['duration'] ? 
        $video['duration'] : '';
    
      $values['filename'] = $video['source_file']['filename'];
      $values['filemime'] = $video['source_file']['mimetype'];
      $values['filesize'] = storage_api_format_size($video['source_file']['size']);
      $values['filesize-raw'] = $video['source_file']['size'];

      $values['views'] = (int)$video['flv_file']['servings'] . ' ' . 
        format_plural($video['flv_file']['servings'], 'view', 'views');
      $values['views-raw'] = (int)$video['flv_file']['servings'];
      
      $values['downloads'] = (int)$video['source_file']['servings'] . ' ' . 
        format_plural($video['source_file']['servings'], 'download', 'downloads');
      $values['downloads-raw'] = (int)$video['source_file']['servings'];

      $values['download-url'] = storage_api_serve_url($video['source_file'], TRUE);
      $values['download-link'] = l('Download', $values['download-url']);
    }
  }
  
  return $values;
}


/**
 * Implement hook_token_list() .
 */
function bd_video_token_list($type = 'all') {
  $list = array();

  if($type == 'field' || $type == 'all') {
  
    $list['Video']['duration'] = t("Video duration (formatted)");
    $list['Video']['duration-raw'] = t("Video duration (raw value - seconds)");
    
    $list['Video']['filename'] = t("Filename of the source video file");
    $list['Video']['filemime'] = t("MIME type of the source video file");
    $list['Video']['filesize'] = t("Size of the source video file (formatted)");
    $list['Video']['filesize-raw'] = t("Size of the source video file (raw value - bytes)");
    
    $list['Video']['views'] = t("Views of the video in-browser (formatted)");
    $list['Video']['views-raw'] = t("Views of the video in-browser (raw value)");
    $list['Video']['downloads'] = t("Downloads of the source video file (formatted)");
    $list['Video']['downloads-raw'] = t("Downloads of the source video file (raw value)");
    
    $list['Video']['download-link'] = t("Download link for the source video file (html)");
    $list['Video']['download-url'] = t("Download url for the source video file");
  }

  return $list;
}

