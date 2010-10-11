<?php

/**
 * Implementation of hook_content_default_fields().
 */
function spaces_video_gallery_content_default_fields() {
  module_load_include('inc', 'spaces_video_gallery', 'spaces_video_gallery.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_video_gallery_content_default_fields', $args);
}

/**
 * Implementation of hook_context_default_contexts().
 */
function spaces_video_gallery_context_default_contexts() {
  module_load_include('inc', 'spaces_video_gallery', 'spaces_video_gallery.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_video_gallery_context_default_contexts', $args);
}

/**
 * Implementation of hook_menu_default_items().
 */
function spaces_video_gallery_menu_default_items() {
  module_load_include('inc', 'spaces_video_gallery', 'spaces_video_gallery.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_video_gallery_menu_default_items', $args);
}

/**
 * Implementation of hook_node_info().
 */
function spaces_video_gallery_node_info() {
  module_load_include('inc', 'spaces_video_gallery', 'spaces_video_gallery.features.node');
  $args = func_get_args();
  return call_user_func_array('_spaces_video_gallery_node_info', $args);
}

/**
 * Implementation of hook_user_default_permissions().
 */
function spaces_video_gallery_user_default_permissions() {
  module_load_include('inc', 'spaces_video_gallery', 'spaces_video_gallery.defaults');
  $args = func_get_args();
  return call_user_func_array('_spaces_video_gallery_user_default_permissions', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function spaces_video_gallery_views_default_views() {
  module_load_include('inc', 'spaces_video_gallery', 'spaces_video_gallery.features.views');
  $args = func_get_args();
  return call_user_func_array('_spaces_video_gallery_views_default_views', $args);
}
