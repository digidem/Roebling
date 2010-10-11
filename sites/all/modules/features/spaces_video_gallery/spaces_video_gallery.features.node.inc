<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _spaces_video_gallery_node_info() {
  $items = array(
    'video' => array(
      'name' => t('Video'),
      'module' => 'features',
      'description' => t('For posting videos to your group'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '0',
      'body_label' => t(''),
    ),
    'video_gallery' => array(
      'name' => t('Video Gallery'),
      'module' => 'features',
      'description' => t('Put your videos here after creating them'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '1',
      'body_label' => t('Body'),
    ),
  );
  return $items;
}
