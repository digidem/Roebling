<?php
// $Id: services.inc,v 1.1.2.1 2009/12/07 21:23:49 jbrown Exp $


/**
 * Implement hook_service() .
 */
function bd_video_service() {
  return array(
    // user.login
    array(
      '#method'   => 'video.load',
      '#callback' => 'bd_video_service_load',
      '#args'     => array(
        array(
          '#name' => 'vid',
          '#type' => 'int',
          '#description' => t('The video ID to load.'),
        ), 
      ),
      '#return'   => 'struct',
      '#help'     => t('Loads information about the given video.')
    ),
  );
}


/**
 * Loads video information
 */
function bd_video_service_load($vid) {
  $video = _bd_video_load($vid);
  if (!$video) {
    return services_error(t('Wrong video id.'));
  }
  else {
    return $video;
  }
}
