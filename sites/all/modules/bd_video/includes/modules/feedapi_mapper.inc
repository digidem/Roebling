<?php
// $Id: feedapi_mapper.inc,v 1.1.2.5 2009/12/07 21:23:49 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


/**
 * Implement hook_feedapi_mapper() .
 */
function bd_video_feedapi_mapper($op, $node, $feed_element = array(), $field_name = '', $sub_field = '') {

  switch($op) {
  
    case 'describe':
      $info = 'Map options->enclosures->video to a bd_video field to pull in video content.<br />';
      $info .= 'Note that Common syndication parser does not parse enclosures - use SimplePie parser instead.';
      return t($info);
      
    case 'list':
    
      $options = FALSE;
    
      $fields = content_fields(NULL, $node->type);
      
      foreach($fields as $field) {
      
        if($field['type'] == 'bd_video')
          $options[$field['field_name']] = $field['widget']['label'];
      }
     
      return $options;

      
    case 'map';
    
      if(!is_array($feed_element))
        return $node;
    
      $format = array_shift($feed_element);
      $video = $format[0];
    
      $field = $node->$field_name;

      $selector_ids = _bd_video_get_selector_ids($field_name);
      
      $options = array(
        'module' => 'bd_video',
        'type' => 'source video',
        'get_filename' => TRUE,
        'force_download' => TRUE,
        'source_url' => $video['link']
      );
      
      $field[0]['source_file_id'] = storage_api_file_add($selector_ids['feed_source'], $options);
      
      $options = array(
        'module' => 'bd_video',
        'type' => 'custom preview image',
        'source_url' => $video['thumbnails'][0]
      );
      
      $field[0]['custom_image_file_id'] = storage_api_file_add($selector_ids['source'], $options);
        
      $node->$field_name = $field;
      return $node;
  }
}

