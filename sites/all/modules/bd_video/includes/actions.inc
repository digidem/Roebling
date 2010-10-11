<?php
// $Id: actions.inc,v 1.1.2.1 2009/04/01 17:37:18 jbrown Exp $

// Copyright 2008-2009 Jonathan Brown


function _bd_video_append_actions_fieldset(&$form) {
  $content_type_info = _content_type_info();
  
  $fields = $content_type_info['content types'][$form['#node_type']->type]['fields'];

  if(is_array($fields)) {
    foreach($fields as $field) {
      if($field['type'] == 'bd_video') {
        $append = TRUE;
        break;
      }
    }
  }
  
  if(!$append)
    return;
  
  actions_synchronize();
  
  $result = db_query("
    SELECT aid, description
    FROM {actions}
    WHERE type = 'node'
  ");
  
  while($action = db_fetch_array($result)) {
  
    if($action['aid'] == 'node_save_action')  // we save automatically
      continue;
  
    $node_actions[$action['aid']] = $action['description'];
  }
  
  $form['bd_video'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Blue Droplet Video'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
   );
   
  $actions = variable_get('bd_video_once_transcoded_actions', array());
  $description  = 'The selected actions will be fired once all videos in the node have been ';
  $description .= 'successfully transcoded.';
  
   $form['bd_video']['once_transcoded_actions'] = array(
    '#type' => 'select',
    '#title' => t("'Once transcoded' actions"),
    '#options' => $node_actions,
    '#multiple' => TRUE,
    '#default_value' => $actions[$form['#node_type']->type],
    '#description' => t($description)
  );
  
  $form['#submit'][] = '_bd_video_node_type_form_form_submit';
}


function _bd_video_node_type_form_form_submit($form, &$form_state) {

  if($form_state['clicked_button']['#value'] != 'Save content type')
    return;
  
  $actions = variable_get('bd_video_once_transcoded_actions', array());
  $actions[$form_state['values']['type']] = $form_state['values']['bd_video']['once_transcoded_actions'];
  variable_set('bd_video_once_transcoded_actions', $actions);
}


function _bd_video_do_transcoding_actions($video_id) {

  $nid = db_result(db_query("
    SELECT nid
    FROM {bd_video}
    WHERE video_id = %d
  ",
    $video_id
  ));

  if(db_result(db_query("
    SELECT COUNT(*)
    FROM {bd_video}
    WHERE {bd_video}.nid = %d
      AND {bd_video}.flv_file_id IS NULL
  ",
    $nid
  ))) {
    return;
  }
  
  $node = node_load($nid);
  
  if(!$node)
    return;
  
  $actions = variable_get('bd_video_once_transcoded_actions', array());
  $actions = $actions[$node->type];

  if(count($actions))  {
    actions_do($actions, $node);
    actions_do('node_save_action', $node);
  }
}

