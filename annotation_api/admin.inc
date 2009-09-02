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
    'title' => array('#type' => 'textfield', '#default_value' => $set['title'], '#title' => t('Title')),
  );
  $c = 0;
  $form['settings']['#tree'] = TRUE;
  $validates = array();
  $submits = array();
  foreach (array('scope', 'annotation', 'content') as $type) {
    if ($infos[$type][$set[$type]]['settings_form']) {
      $func = $infos[$type][$set[$type]]['settings_form'];
      $form['settings'][$type] = $func($set['settings'][$type]);
      // Set title and type, if they haven't been set by the handler module.
      if (!isset($form['settings'][$type]['#type'])) $form['settings'][$type]['#type'] = 'fieldset';
      if (!isset($form['settings'][$type]['#title'])) $form['settings'][$type]['#title'] = t("$type : @type_title", array('@type_title' => $infos[$type][$set[$type]]['title']));
      // Fetch validation functions defined in the handler module.
      if (isset($form['settings'][$type]['#validate'])) {
        $validates += $form['settings'][$type]['#validate'];
        unset($form['settings'][$type]['#validate']);
      }
      // Fetch submit functions.
      if (isset($form['settings'][$type]['#submit'])) {
        $validates += $form['settings'][$type]['#submit'];
        unset($form['settings'][$type]['#submit']);
      }
      $c++;
    }
  }
  $form['#validate'] = $validates;
  $form['#submit'] = $submits;
  // Show a message and go back button for no setting forms available.
  if (!$c) {
    $form['message'] = array('#value' => t('- No settings are necessary. -  <br/>!backlink', array('!backlink' => l(t('Back to list'),'admin/settings/annotation_api'))));
  }
  // Else show the submit button and Cancel link.
  else {
    $form['submit'] = array('#value' => t('Submit'), '#type' => 'submit', '#submit' => array('annotation_api_admin_edit_form_submit'));
    $form['cancel'] = array('#value' => l(t('Cancel'), 'admin/settings/annotation_api'));
  }
  return $form;
}

/**
 * Submission for a specific annotation setting.
 */
function annotation_api_admin_edit_form_submit($form, &$form_state) {
  $set = variable_get('annotation_api', array());
  $id = $form_state['values']['id'];
  $set[$id] = array(
    'scope' => $form_state['values']['scope'],
    'content' => $form_state['values']['content'],
    'annotation' => $form_state['values']['annotation'],
    'title' => check_plain($form_state['values']['title']),
    'settings' => $form_state['values']['settings'],
  );
  variable_set('annotation_api', $set);
  drupal_set_message(t('Settings for annotation %id : %title have been saved.', array('%id' => $id, '%title' => check_plain($form_state['values']['title']))));
  $form_state['redirect'] = 'admin/settings/annotation_api';
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