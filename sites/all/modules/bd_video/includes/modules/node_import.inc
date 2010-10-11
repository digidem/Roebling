<?php
// $Id: node_import.inc,v 1.1.2.1 2009/12/07 21:23:49 jbrown Exp $

// Copyright 2007-2008 Jonathan Brown


/**
 * Implement hook_node_import_fields() .
 */
function bd_video_node_import_fields($type) {

  $types = content_types();
  
  foreach($types[$type]['fields'] as $field) {
    if($field['type'] == 'bd_video') {

      $fields['_bd_video_' . $field['field_name'] . '_source_filepath'] = 
        $field['widget']['label'] . t(': Source video filepath');
      $fields['_bd_video_' . $field['field_name'] . '_preview_image'] = 
        $field['widget']['label'] . t(': Preview image');
      $fields['_bd_video_' . $field['field_name'] . '_custom_image_filepath'] = 
        $field['widget']['label'] . t(': Custom preview image filepath');
      $fields['_bd_video_' . $field['field_name'] . '_flv_filepath'] = 
        $field['widget']['label'] . t(': Flash-ready video filepath');
      $fields['_bd_video_' . $field['field_name'] . '_duration'] = 
        $field['widget']['label'] . t(': Duration');
    }
  }
  
  return $fields;
}


/**
 * Implement hook_node_import_prepare() .
 */
function bd_video_node_import_prepare(&$node, $preview = FALSE) {
  if($preview)
    return;
    
  $filepath_prefix = $node->node_import_bd_video['filepath_prefix'];
  
  $types = content_types();
  
  foreach($types[$node->type]['fields'] as $field) {
    if($field['type'] == 'bd_video') {
    
      $source_filepath = $node->{'_bd_video_' . $field['field_name'] . '_source_filepath'};
      
      if($source_filepath) {
        $node->{$field['field_name']}[0]['source_file_id'] = _bd_video_insert_filepath($filepath_prefix . '/' . $source_filepath, TRUE);
      }

      $preview_image = $node->{'_bd_video_' . $field['field_name'] . '_preview_image'};
      
      if($preview_image)
        $node->{$field['field_name']}[0]['$preview_image']['preview_image'] = $preview_image;
      else
        $node->{$field['field_name']}[0]['$preview_image']['preview_image'] = $node->node_import_bd_video['preview_image'];

      $custom_image_filepath = $node->{'_bd_video_' . $field['field_name'] . '_custom_image_filepath'};
      
      if($custom_image_filepath) {
        $node->{$field['field_name']}[0]['custom_image_file_id'] = _bd_video_insert_filepath($filepath_prefix . '/' . $custom_image_filepath, TRUE);
      }

      $flv_filepath = $node->{'_bd_video_' . $field['field_name'] . '_flv_filepath'};
      
      if($flv_filepath) {
        $node->{$field['field_name']}[0]['flv_file_id'] = _bd_video_insert_filepath($filepath_prefix . '/' . $flv_filepath, TRUE);
      }
    
      $node->{$field['field_name']}[0]['duration'] = $node->{'_bd_video_' . $field['field_name'] . '_duration'};
    }
  }
}


/**
 * Implement hook_node_import_global() .
 */
function bd_video_node_import_global($type, $global_values) {
  $types = content_types();
  
  foreach($types[$type]['fields'] as $field) {
    if($field['type'] == 'bd_video') {
    
        $form['node_import_bd_video'] = array(
          '#type' => 'fieldset',
          '#title' => t('Video options'),
          '#tree' => TRUE
        );
        
        $default = $global_values['node_import_bd_video']['preview_image'] ? 
          $global_values['node_import_bd_video']['preview_image'] : $field['preview_image_image'];
        
        $form['node_import_bd_video']['preview_image'] = array(
          '#type' => 'radios',
          '#title' => t('Default preview image'),
          '#default_value' => $default,
          '#options' => array(
            'start' => t('Start of video'),
            'third' => t('@ 1/3 of video duration'),
            'custom' => t('Custom image'),
          )
        );
        
        $form['node_import_bd_video']['filepath_prefix'] = array(
          '#type' => 'textfield',
          '#title' => t('Filepath prefix'),
          '#default_value' => $global_values['node_import_bd_video']['filepath_prefix']
        );

        return $form;
    }
  }
}

