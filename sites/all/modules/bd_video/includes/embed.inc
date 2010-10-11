<?php
// $Id: embed.inc,v 1.1 2009/04/01 14:48:32 jbrown Exp $

// Copyright 2007-2008 Jonathan Brown


function _bd_video_generate_embed_code($video_id) {

  $video = _bd_video_load($video_id);
  $url = url('bd_video/' . $video_id . '/embed', array('absolute' => TRUE));
  $code = '<iframe src="' . $url . '" width="400" height="300" frameborder="0" scrolling="no"></iframe>';
  return htmlspecialchars($code);
}


function _bd_video_embed_tab($node, $video_id) {

  drupal_set_title(check_plain($node->title));

  $output .= "<code>\n";
  $output .= _bd_video_generate_embed_code($video_id);
  $output .= "</code>\n";

  return $output;
}


function _bd_video_embed($video_id) {
  
  $video = _bd_video_load($video_id);
  
  $settings['player_width'] = '400';
  $settings['player_height'] = '300';

  $settings['preview_image_user_selectable'] = TRUE;

  $settings['controlbar'] = FALSE;

  $settings['showicons'] = TRUE;
  $settings['showdigits'] = TRUE;
  $settings['showfsbutton'] = TRUE;
  $settings['repeat'] = FALSE;
  $settings['showvolume'] = TRUE;
  $settings['autostart'] = 'false';
  $settings['showdownload'] = FALSE;

  _bd_video_set_render_settings($settings);

  $player = _bd_video_render_inline($video, 0);
  
  $ufo_path = base_path() . drupal_get_path('module', 'bd_video') . '/ufo.js';
  
  $language = $GLOBALS['locale'];

  $output  = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";
  $output .= "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"$language\" xml:lang=\"$language\">\n";
  $output .= "<head>\n";
  $output .= "<script type=\"text/javascript\" src=\"" . $ufo_path . "\"></script>\n";
  $output .= "</head>";
  $output .= "<body>\n";
  $output .= $player;
  $output .= " </body>\n";
  $output .= "</html>\n";
  
  echo $output;
}

