<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _pingv_image_gallery_content_default_fields() {
  $fields = array();

  // Exported field: field_pingv_ref_images
  $fields[] = array(
    'field_name' => 'field_pingv_ref_images',
    'type_name' => 'pingv_gallery',
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
        'format' => 'default',
        'exclude' => 0,
      ),
      '5' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '2' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '3' => array(
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
      'admin_message' => 0,
      'audio' => 0,
      'blog' => 0,
      'book' => 0,
      'casetracker_basic_case' => 0,
      'event' => 0,
      'feed' => 0,
      'forum' => 0,
      'pingv_gallery' => 0,
      'group' => 0,
      'pingv_image' => 0,
      'profile' => 0,
      'casetracker_basic_project' => 0,
      'shoutbox' => 0,
      'feed_ical_item' => 0,
      'feed_ical' => 0,
    ),
    'advanced_view' => 'pingv_image_gallery_ref',
    'advanced_view_args' => 'pingv_image',
    'widget' => array(
      'autocomplete_match' => 'contains',
      'size' => 60,
      'default_value' => array(
        '0' => array(
          'nid' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Images',
      'weight' => '31',
      'description' => '',
      'type' => 'nodereference_autocomplete',
      'module' => 'nodereference',
    ),
  );

  // Exported field: field_pingv_image
  $fields[] = array(
    'field_name' => 'field_pingv_image',
    'type_name' => 'pingv_image',
    'display_settings' => array(
      'weight' => '-3',
      'parent' => '',
      'label' => array(
        'format' => 'hidden',
      ),
      'teaser' => array(
        'format' => 'pingv_image-gallery_linked',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'pingv_image-main_default',
        'exclude' => 0,
      ),
      '5' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
      '2' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
      '3' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'filefield',
    'required' => '1',
    'multiple' => '0',
    'module' => 'filefield',
    'active' => '1',
    'list_field' => '0',
    'list_default' => 1,
    'description_field' => '0',
    'widget' => array(
      'file_extensions' => 'png gif jpg jpeg',
      'file_path' => '',
      'progress_indicator' => 'bar',
      'max_filesize_per_file' => '',
      'max_filesize_per_node' => '',
      'max_resolution' => '0',
      'min_resolution' => '0',
      'alt' => '',
      'custom_alt' => 0,
      'title' => '',
      'custom_title' => 0,
      'title_type' => 'textfield',
      'default_image' => NULL,
      'use_default_image' => 0,
      'label' => 'Image',
      'weight' => '-3',
      'description' => '',
      'type' => 'imagefield_widget',
      'module' => 'imagefield',
    ),
  );

  // Exported field: field_pingv_ref_gallery
  $fields[] = array(
    'field_name' => 'field_pingv_ref_gallery',
    'type_name' => 'pingv_image',
    'display_settings' => array(
      'weight' => '-2',
      'parent' => '',
      'label' => array(
        'format' => 'hidden',
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 1,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 1,
      ),
      '5' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '2' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '3' => array(
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
      'admin_message' => 0,
      'audio' => 0,
      'blog' => 0,
      'book' => 0,
      'casetracker_basic_case' => 0,
      'event' => 0,
      'feed' => 0,
      'forum' => 0,
      'pingv_gallery' => 0,
      'group' => 0,
      'pingv_image' => 0,
      'profile' => 0,
      'casetracker_basic_project' => 0,
      'shoutbox' => 0,
      'feed_ical_item' => 0,
      'feed_ical' => 0,
    ),
    'advanced_view' => 'pingv_image_gallery_ref',
    'advanced_view_args' => 'pingv_gallery',
    'widget' => array(
      'autocomplete_match' => 'contains',
      'size' => 60,
      'default_value' => array(
        '0' => array(
          'nid' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Gallery',
      'weight' => '-2',
      'description' => '',
      'type' => 'nodereference_select',
      'module' => 'nodereference',
    ),
  );

  // Translatables
  array(
    t('Gallery'),
    t('Image'),
    t('Images'),
  );

  return $fields;
}

/**
 * Helper to implementation of hook_context_default_contexts().
 */
function _pingv_image_gallery_context_default_contexts() {
  $items = array();

  $items[] = array(
    'namespace' => 'spaces',
    'attribute' => 'feature',
    'value' => 'pingv_image_gallery',
    'description' => '',
    'node' => array(
      '0' => 'pingv_gallery',
      '1' => 'pingv_image',
    ),
    'views' => array(
      '0' => 'pingv_image_gallery',
      '1' => 'pingv_image_gallery_ref',
    ),
    'menu' => 'gallery',
    'block' => array(
      'views_pingv_image_gallery-block_1' => array(
        'module' => 'views',
        'delta' => 'pingv_image_gallery-block_1',
        'weight' => 20,
        'region' => 'right',
        'status' => '0',
        'label' => 'pingv_image_gallery: Block',
        'type' => 'context_ui',
      ),
    ),
  );
  return $items;
}

/**
 * Helper to implementation of hook_imagecache_default_presets().
 */
function _pingv_image_gallery_imagecache_default_presets() {
  $items = array(
    'pingv_image-gallery' => array(
      'presetname' => 'pingv_image-gallery',
      'actions' => array(
        '0' => array(
          'actionid' => '2',
          'presetid' => '2',
          'weight' => '0',
          'module' => 'imagecache',
          'action' => 'imagecache_scale_and_crop',
          'data' => array(
            'width' => '168px',
            'height' => '168px',
          ),
        ),
      ),
    ),
    'pingv_image-main' => array(
      'presetname' => 'pingv_image-main',
      'actions' => array(
        '0' => array(
          'actionid' => '3',
          'presetid' => '3',
          'weight' => '0',
          'module' => 'imagecache',
          'action' => 'imagecache_scale',
          'data' => array(
            'width' => '610px',
            'height' => '',
            'upscale' => 0,
          ),
        ),
      ),
    ),
    'pingv_image-thumb' => array(
      'presetname' => 'pingv_image-thumb',
      'actions' => array(
        '0' => array(
          'actionid' => '1',
          'presetid' => '1',
          'weight' => '0',
          'module' => 'imagecache',
          'action' => 'imagecache_scale_and_crop',
          'data' => array(
            'width' => '80px',
            'height' => '80px',
          ),
        ),
      ),
    ),
  );
  return $items;
}

/**
 * Helper to implementation of hook_menu_default_items().
 */
function _pingv_image_gallery_menu_default_items() {
  $items = array();

  $items[] = array(
    'title' => 'Image gallery',
    'path' => 'gallery',
    'weight' => '0',
  );
  // Translatables
  array(
    t('Image gallery'),
  );


  return $items;
}

/**
 * Helper to implementation of hook_user_default_permissions().
 */
function _pingv_image_gallery_user_default_permissions() {
  $permissions = array();

  // Exported permission: create pingv_gallery content
  $permissions[] = array(
    'name' => 'create pingv_gallery content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: create pingv_image content
  $permissions[] = array(
    'name' => 'create pingv_image content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: delete any pingv_gallery content
  $permissions[] = array(
    'name' => 'delete any pingv_gallery content',
    'roles' => array(
      '0' => 'admin',
      '1' => 'manager',
    ),
  );

  // Exported permission: delete any pingv_image content
  $permissions[] = array(
    'name' => 'delete any pingv_image content',
    'roles' => array(
      '0' => 'admin',
      '1' => 'manager',
    ),
  );

  // Exported permission: delete own pingv_gallery content
  $permissions[] = array(
    'name' => 'delete own pingv_gallery content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: delete own pingv_image content
  $permissions[] = array(
    'name' => 'delete own pingv_image content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: edit any pingv_gallery content
  $permissions[] = array(
    'name' => 'edit any pingv_gallery content',
    'roles' => array(
      '0' => 'admin',
      '1' => 'manager',
    ),
  );

  // Exported permission: edit any pingv_image content
  $permissions[] = array(
    'name' => 'edit any pingv_image content',
    'roles' => array(
      '0' => 'admin',
      '1' => 'manager',
    ),
  );

  // Exported permission: edit own pingv_gallery content
  $permissions[] = array(
    'name' => 'edit own pingv_gallery content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: edit own pingv_image content
  $permissions[] = array(
    'name' => 'edit own pingv_image content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: view imagecache pingv_image-gallery
  $permissions[] = array(
    'name' => 'view imagecache pingv_image-gallery',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: view imagecache pingv_image-main
  $permissions[] = array(
    'name' => 'view imagecache pingv_image-main',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  // Exported permission: view imagecache pingv_image-thumb
  $permissions[] = array(
    'name' => 'view imagecache pingv_image-thumb',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'admin',
      '2' => 'manager',
    ),
  );

  return $permissions;
}
