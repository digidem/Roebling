<?php
// $Id: views.inc,v 1.1.2.2 2009/12/07 21:23:49 jbrown Exp $

// Copyright 2007-2009 Jonathan Brown


function _bd_video_views_data($field) {
  $data = content_views_field_views_data($field);

  $data['bd_video_data_' . $field['field_name']] = array(
    'table' => array(
      'group' => 'Video',
      'join' => array(
        'node' => array(
          'left_table' => 'node_data_' . $field['field_name'],
          'left_field' => $field['field_name'] . '_video_id',
          'table' => 'bd_video',
          'field' => 'video_id'
        )
      )
    ),
    
    'duration' => array(
      'title' => 'Duration - (' . $field['field_name'] . ')',
      'help' => 'Duration of the video.',

      'field' => array(
        'handler' => 'bd_video_views_handler_duration',
        'click sortable' => TRUE
      ),

      'sort' => array(
        'handler' => 'views_handler_sort',
      )
    ),

    'preview_image' => array(
      'title' => 'Preview image - (' . $field['field_name'] . ')',
      'help' => 'The video\'s preview image with configurable size.',

      'field' => array(
        'field' => 'video_id',
        'handler' => 'bd_video_views_handler_preview_image'
      )
    )
  );
  
  $data['bd_video_data_' . $field['field_name'] . '_source_file'] = array(
    'table' => array(
      'group' => 'Video',
      'join' => array(
        'node' => array(
          'left_table' => 'bd_video_data_'  . $field['field_name'],
          'left_field' => 'source_file_id',
          'table' => 'storage_file',
          'field' => 'file_id'
        )
      )
    ),
    
    'servings' => array(
      'title' => 'Total downloads - (' . $field['field_name'] . ')',
      'help' => 'Total downloads of the source video.',
      
      'field' => array(
        'click sortable' => TRUE
      ),
      
      'sort' => array(
        'handler' => 'views_handler_sort',
      )
    )
  );

  $data['bd_video_data_' . $field['field_name'] . '_source_object'] = array(
    'table' => array(
      'group' => 'Video',
      'join' => array(
        'node' => array(
          'left_table' => 'bd_video_data_' . $field['field_name'] . '_source_file',
          'left_field' => 'object_id',
          'table' => 'storage_object',
          'field' => 'object_id'
        )
      )
    ),
    
    'size' => array(
      'title' => 'Filesize - (' . $field['field_name'] . ')',
      'help' => 'Filesize of the source video.',
      
      'field' => array(
        'handler' => 'bd_video_views_handler_filesize',
        'click sortable' => TRUE
      ),
      
      'sort' => array(
        'handler' => 'views_handler_sort',
      )
    )
  );

  $data['bd_video_data_' . $field['field_name'] . '_flv_file'] = array(
    'table' => array(
      'group' => 'Video',
      'join' => array(
        'node' => array(
          'left_table' => 'bd_video_data_'  . $field['field_name'],
          'left_field' => 'flv_file_id',
          'table' => 'storage_file',
          'field' => 'file_id'
        )
      )
    ),
    
    'servings' => array(
      'title' => 'Total views - (' . $field['field_name'] . ')',
      'help' => 'Total views of the video in-browser.',
      'field' => array(
        'click sortable' => TRUE
      ),
      
      'sort' => array(
        'handler' => 'views_handler_sort',
      )
    )
  );

  return $data;
}


/**
 * Implement hook_views_handlers() .
 */
function bd_video_views_handlers() {

  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'bd_video') . '/includes/modules/views'
    ),
    'handlers' => array(
      'bd_video_views_handler_filesize' => array(
        'parent' => 'views_handler_field',
        'file' => 'handlers.inc'
      ),
      'bd_video_views_handler_duration' => array(
        'parent' => 'views_handler_field',
        'file' => 'handlers.inc'
      ),
      'bd_video_views_handler_preview_image' => array(
        'parent' => 'views_handler_field',
        'file' => 'handlers.inc'
      )
    )
  );
}

