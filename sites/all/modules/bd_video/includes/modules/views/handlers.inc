<?php
// $Id: handlers.inc,v 1.1.2.1 2009/04/23 02:30:33 jbrown Exp $

// Copyright 2009 Jonathan Brown


class bd_video_views_handler_preview_image extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['width'] = array('default' => 160);
    $options['height'] = array('default' => 120);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['width'] = array(
      '#title' => t('Width'),
      '#type' => 'textfield',
      '#size' => 3,
      '#maxlength' => 3,
      '#default_value' => $this->options['width']
    );

    $form['height'] = array(
      '#title' => t('Height'),
      '#type' => 'textfield',
      '#size' => 3,
      '#maxlength' => 3,
      '#default_value' => $this->options['height']
    );
  }

  function render($values) {

    if($values->{$this->field_alias}) {
      $video = _bd_video_load($values->{$this->field_alias});
      return _bd_video_render_preview_image_link($video, $this->options['width'], $this->options['height']);
    }
  }
}


class bd_video_views_handler_filesize extends views_handler_field {

  function render($values) {

    if($values->{$this->field_alias}) {
      return storage_api_format_size($values->{$this->field_alias});
    }
  }
}


class bd_video_views_handler_duration extends views_handler_field {

  function render($values) {

    if($values->{$this->field_alias}) {
      return format_interval($values->{$this->field_alias});
    }
  }
}

