<?php
// $Id$

/**
 * @file
 *   Forms for annotation API adminsitration.
 */

/**
 * Form for overview of annotations.
 */
function annotation_api_admin_form(&$form_state) {
  $infos = annotation_api_info();
  $set = variable_get('annotation_api', array());
  $form = array();

  $op_s = array();
  foreach ($infos['scope'] as $scope => $info) {
    $op_s[$scope] = $info['title'];
  }
  $op_a = array();
  foreach ($infos['annotation'] as $annotation => $info) {
    $op_a[$annotation] = $info['title'];
  }
  $op_c = array();
  foreach ($infos['content'] as $content => $info) {
    $op_c[$content] = $info['title'];
  }

  $form['add'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new setting'),
    '#description' => t('By selecting a new combination of scope, annotation type
                        and content type, you can add annotations of different
                        types with different settings.'),
    '#collapsible' => TRUE,
    '#collapsed' => count($set),
    'scope' => array('#type' => 'select', '#title' => t('Scope type'), '#options' => $op_s, '#required' => TRUE),
    'annotation' => array('#type' => 'select', '#title' => t('Annotation type'), '#options' => $op_a, '#required' => TRUE),
    'content' => array('#type' => 'select', '#title' => t('Content type'), '#options' => $op_c, '#required' => TRUE),
    'title' => array('#type' => 'textfield', '#required' => TRUE, '#title' => t('Title')),
    'add' => array('#type' => 'submit', '#value' => t('Add'), '#submit' => array('annotation_api_admin_form_add')),
  );

  $header = array(t('Title'), t('Scope type'), t('Annotation type'), t('Content type'), t('Edit'), t('Delete'));
  $rows = array();
  foreach ($set as $id => $s) {
    $rows[] = array($s['title'], $s['scope'], $s['annotation'], $s['content'], l(t('Edit'), 'admin/settings/annotation_api/'. $id .'/edit'), l(t('Delete'), 'admin/settings/annotation_api/'. $id .'/delete'));
  }
  if (count($rows)) {
    $form['list'] = array('#value' => theme('table', $header, $rows));
  }
  else {
    $form['list'] = array('#value' => t('- No annotation settings availbale at the moment. You can add a new one above.'));
  }
  return $form;
}

/**
 * Submit function for annotation_api_admin_form().
 */
function annotation_api_admin_form_add($form, &$form_state) {
  if ($form_state['submitted']) {
    $set = variable_get('annotation_api', array());
    $set[] = array(
      'title' => $form_state['values']['title'],
      'scope' => $form_state['values']['scope'],
      'annotation' => $form_state['values']['annotation'],
      'content' => $form_state['values']['content'],
    );
    variable_set('annotation_api', $set);
  }
}

/**
 * Edit settings for an annotation package.
 */
function annotation_api_admin_edit_form(&$form_state, $id) {
  $infos = annotation_api_info();
  $sets = variable_get('annotation_api', array());
  $set = $sets[$id];
  drupal_set_title(t('Annotation settings for %title', array('%title' => $set['title'])));
  $form = array(
    'id' => array('#type' => 'value', '#value' => $id),
    'scope' => array('#type' => 'value', '#value' => $set['scope']),
    'annotation' => array('#type' => 'value', '#value' => $set['annotation']),
    'content' => array('#type' => 'value', '#value' => $set['content']),
  );
  $c = 0;
  $form['settings']['#tree'] = TRUE;
  foreach (array('scope', 'annotation', 'content') as $type)
  if ($infos[$type][$set[$type]]['settings_form']) {
    $func = $infos[$type][$set[$type]]['settings_form'];
    $form['settings'][$type] = $func($set['settings'][$type]);
    $c++;
  }
  if (!$c) {
    $form['message'] = array('#value' => t('- No settings are necessary. -  <br/>!backlink', array('!backlink' => l(t('Back to list'),'admin/settings/annotation_api'))));
  }
  else {
    $form['submit'] = array('#value' => t('Submit'), '#type' => 'submit', '#submit' => array('annotation_api_admin_edit_form_submit'));
    $form['cancel'] = array('#value' => l(t('Cancel'), 'admin/settings/annotation_api'));
  }
  return $form;
}

function annotation_api_admin_edit_form_submit($form, &$form_state) {
  dpm($form_state);
}


/**
 * Confirmation form for deleting annotation settings.
 */
function annotation_api_admin_delete_form(&$form_state, $id) {
  $form['id'] = array('#type' => 'value', '#value' => $id);
  $form['#submit'] = array('annotation_api_admin_delete_form_submit');
  return confirm_form($form,
                      t('Dou you really want to delete annotation with id %id',array('%id' => $id)),
                      'admin/settings/annotation_api',
                      NULL, t('Delete'), t('Cancel'));
}

/**
 * Submission for deleting annotation settings.
 */
function annotation_api_admin_delete_form_submit($form, &$form_state) {
  $id = $form_state['values']['id'];
  $set = variable_get('annotation_api', array());
  unset($set[$id]);
  variable_set('annotation_api', $set);
  drupal_set_message(t('Annotation settings with id %id was deleted.', array('%id' => $id)));
  $form_state['redirect'] = 'admin/settings/annotation_api';
}