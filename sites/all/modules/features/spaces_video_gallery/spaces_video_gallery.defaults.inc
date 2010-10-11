<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _spaces_video_gallery_content_default_fields() {
  $fields = array();

  // Exported field: field_video
  $fields[] = array(
    'field_name' => 'field_video',
    'type_name' => 'video',
    'display_settings' => array(
      'weight' => '31',
      'parent' => '',
      'label' => array(
        'format' => 'hidden',
      ),
      'teaser' => array(
        'format' => 'video_nodelink',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'video_plain',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'video_plain',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'video_plain',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'filefield',
    'required' => '0',
    'multiple' => '0',
    'module' => 'filefield',
    'active' => '1',
    'list_field' => '0',
    'list_default' => 1,
    'description_field' => '1',
    'widget' => array(
      'file_extensions' => 'mp4 mpeg avi mpg wmv flv mov',
      'file_path' => '',
      'progress_indicator' => 'bar',
      'max_filesize_per_file' => '',
      'max_filesize_per_node' => '',
      'default_video_thumb' => NULL,
      'default_width' => '640',
      'default_resolution' => '16:9',
      'autoconversion' => 1,
      'autothumbnail' => 1,
      'label' => 'Video',
      'weight' => '31',
      'description' => '',
      'type' => 'uploadfield_widget',
      'module' => 'uploadfield',
    ),
  );

  // Exported field: field_video_gallery_ref
  $fields[] = array(
    'field_name' => 'field_video_gallery_ref',
    'type_name' => 'video',
    'display_settings' => array(
      'weight' => '33',
      'parent' => '',
      'label' => array(
        'format' => 'inline',
      ),
      'teaser' => array(
        'format' => 'hidden',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'nodereference',
    'required' => '0',
    'multiple' => '0',
    'module' => 'nodereference',
    'active' => '1',
    'referenceable_types' => array(
      'gallery' => 0,
      'group' => 0,
      'image' => 0,
      'page' => 0,
      'story' => 0,
      'video' => 0,
      'video_gallery' => 0,
    ),
    'advanced_view' => 'video_gallery_ref',
    'advanced_view_args' => 'video_gallery',
    'widget' => array(
      'autocomplete_match' => 'contains',
      'size' => 60,
      'default_value' => array(
        '0' => array(
          'nid' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Video Gallery',
      'weight' => '33',
      'description' => '',
      'type' => 'nodereference_select',
      'module' => 'nodereference',
    ),
  );

  // Exported field: field_video_ref
  $fields[] = array(
    'field_name' => 'field_video_ref',
    'type_name' => 'video_gallery',
    'display_settings' => array(
      'weight' => '31',
      'parent' => '',
      'label' => array(
        'format' => 'hidden',
      ),
      'teaser' => array(
        'format' => 'teaser',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'teaser',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'nodereference',
    'required' => '0',
    'multiple' => '1',
    'module' => 'nodereference',
    'active' => '1',
    'referenceable_types' => array(
      'gallery' => 0,
      'group' => 0,
      'image' => 0,
      'page' => 0,
      'story' => 0,
      'video' => 0,
      'video_gallery' => 0,
    ),
    'advanced_view' => 'video_gallery_ref',
    'advanced_view_args' => 'video',
    'widget' => array(
      'autocomplete_match' => 'contains',
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'nid' => NULL,
          '_error_element' => 'default_value_widget][field_video_ref][0][nid][nid',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Videos',
      'weight' => '31',
      'description' => '',
      'type' => 'nodereference_autocomplete',
      'module' => 'nodereference',
    ),
  );

  // Translatables
  array(
    t('Video'),
    t('Video Gallery'),
    t('Videos'),
  );

  return $fields;
}

/**
 * Helper to implementation of hook_context_default_contexts().
 */
function _spaces_video_gallery_context_default_contexts() {
  $items = array();

  $items[] = array(
    'namespace' => 'spaces',
    'attribute' => 'feature',
    'value' => 'video_gallery',
    'description' => 'The context for video galleries',
    'node' => array(
      '0' => 'video',
      '1' => 'video_gallery',
    ),
    'views' => array(
      '0' => 'video_gallery',
      '1' => 'video_gallery_ref',
    ),
    'menu' => 'video-gallery',
    'block' => array(
      'views_video_gallery-block_1' => array(
        'module' => 'views',
        'delta' => 'video_gallery-block_1',
        'weight' => 12,
        'region' => 'right',
        'status' => '0',
        'label' => 'video_gallery: Block',
        'type' => 'context_ui',
      ),
    ),
  );
  return $items;
}

/**
 * Helper to implementation of hook_menu_default_items().
 */
function _spaces_video_gallery_menu_default_items() {
  $items = array();

  $items[] = array(
    'title' => 'Video Gallery',
    'path' => 'video-gallery',
    'weight' => '0',
    'description' => 'For the video gallery module',
  );
  // Translatables
  array(
    t('For the video gallery module'),
    t('Video Gallery'),
  );


  return $items;
}

/**
 * Helper to implementation of hook_user_default_permissions().
 */
function _spaces_video_gallery_user_default_permissions() {
  $permissions = array();

  // Exported permission: create video content
  $permissions[] = array(
    'name' => 'create video content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
      '2' => 'admin',
    ),
  );

  // Exported permission: create video_gallery content
  $permissions[] = array(
    'name' => 'create video_gallery content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
      '2' => 'admin',
    ),
  );

  // Exported permission: delete any video content
  $permissions[] = array(
    'name' => 'delete any video content',
    'roles' => array(
      '0' => 'manager',
      '1' => 'admin',
    ),
  );

  // Exported permission: delete any video_gallery content
  $permissions[] = array(
    'name' => 'delete any video_gallery content',
    'roles' => array(
      '0' => 'manager',
      '1' => 'admin',
    ),
  );

  // Exported permission: delete own video content
  $permissions[] = array(
    'name' => 'delete own video content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
      '2' => 'admin',
    ),
  );

  // Exported permission: delete own video_gallery content
  $permissions[] = array(
    'name' => 'delete own video_gallery content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
      '2' => 'admin',
    ),
  );

  // Exported permission: edit any video content
  $permissions[] = array(
    'name' => 'edit any video content',
    'roles' => array(
      '0' => 'manager',
      '1' => 'admin',
    ),
  );

  // Exported permission: edit any video_gallery content
  $permissions[] = array(
    'name' => 'edit any video_gallery content',
    'roles' => array(
      '0' => 'manager',
      '1' => 'admin',
    ),
  );

  // Exported permission: edit own video content
  $permissions[] = array(
    'name' => 'edit own video content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
      '2' => 'admin',
    ),
  );

  // Exported permission: edit own video_gallery content
  $permissions[] = array(
    'name' => 'edit own video_gallery content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
      '2' => 'admin',
    ),
  );

  return $permissions;
}
