<?php
// $Id$

/**
 * @file
 *   annotation management for node
 */

/**
 *
 */
function annotation_cs_node_form(&$form_state, $node) {

  if (isset($node->annotation) && count($node->annotation) > 0) {

    GLOBAL $user;
    $any = user_access('manage any annotation_cs');
    $own_user = user_access('manage own annotation_cs');
    $own_scope = user_access('manage annotation_cs from own node');
    $own_content = user_access('manage annotation_cs to own profile');

    foreach ($node->annotation as $annotation) {
      if ($any
          || ($own_user && $user->uid == $annotation->uid)
          || ($own_scope && $user->uid == $node->uid)
          || ($own_content && $user->uid == $node->content_id)
          ) {
        $c = user_load($annotation->content_id);
        $u = user_load($annotation->uid);
        $options[$annotation->aid] = t('<strong>!content</strong> <em>by !user - @time</em>', array(
            '!content' => l($c->name, 'user/'. $u->uid),
            '!user' => l($u->name, 'user/'. $u->uid),
            '@time' => format_date($annotation->timestamp, 'small'),
          )
        );
      }
    }

    $form['annotations'] = array(
      '#type' => 'checkboxes',
      '#description' => t('Select annotations to remove'),
      '#options' => $options,
      '#multiple' => TRUE,
      '#default_value' => array(),
    );
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('annotation_cs_node_form_submit_delete'),
    );
  }
  else {
    $form['text'] = array(
      '#type' => 'markup',
      '#value' => t('No annotations available. !link', array('!link' => l('Back to node', 'node/'. $node->nid))),
    );
  }
  return $form;
}

/**
 * Submit delete function for annotation_cs_node_form().
 */
function annotation_cs_node_form_submit_delete($form, &$form_state) {
  $annotations = array_filter($form_state['values']['annotations']);
  foreach ($annotations as $aid) {
    annotation_delete($aid);
    drupal_set_message(t('Annotation %aid deleted', array('%aid' => $aid)));
  }
}