<?php
function test_annotation_marker_form(&$form_state) {
  $form = array();

  $form['am'] = array(
    '#input' => TRUE,
    '#title' => 'markerinhof',
    '#type' => 'annotation_marker',
    '#default_value' => array('value'),
  );
  return $form;
}