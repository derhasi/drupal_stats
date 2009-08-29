<?php
function test_annotation_marker_form(&$form_state) {
  $form = array();
  $form['div'] = array('#value' => '<div style="width: 400px; height: 200px;" id="a66666"> </div>');
  $form['toogler'] = array('#value' => theme('annotation_toggle', '#edit-am:not(.input)', 'Show', 'Marker'));
  $form['toogler_add'] = array('#value' => theme('annotation_toggle', '#edit-am.input', 'Add', 'Undo'));
  $form['am'] = array(
    '#input' => TRUE,
    '#draggable' => TRUE,
    '#resizable' => TRUE,
    '#title' => '',
    '#attributes' => array('style' => 'width: 50px; height: 50px;'),
    '#type' => 'annotation_marker',
    '#default_value' => array('container selector' => '#a66666'),
    'form' => array('#type' => 'textfield',
                    '#default_value' => 9000,
                    ),
    'submit' => array('#type' => 'submit', '#submit' => array('test_subm_func'), '#value' => 'haha'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('submit'),
  );
    $schema = drupal_get_schema('annotation');
  dpm($schema);
  return $form;
}

function test_annotation_marker_form_submit($form, &$form_state) {
  dpm($form_state);
}

function baumform(&$form_state) {
  return array(
    'posx' => array('#type' => 'textfield', '#default_value' => 'baum'),
  );
}

function test_subm_func($form, &$form_state) {
  dpm(" Jetzt");
  dpm($form_state);
}