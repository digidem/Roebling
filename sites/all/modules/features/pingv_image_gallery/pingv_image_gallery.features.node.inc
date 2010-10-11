<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _pingv_image_gallery_node_info() {
  $items = array(
    'pingv_gallery' => array(
      'name' => t('Gallery'),
      'module' => 'node',
      'description' => t('A gallery with a node reference field (backreferenced) to the associated image nodes.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '0',
      'body_label' => t(''),
    ),
    'pingv_image' => array(
      'name' => t('Image'),
      'module' => 'node',
      'description' => t('An image with a node reference (backreferenced) field to select the associated gallery.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '1',
      'body_label' => t('Body'),
    ),
  );
  return $items;
}
