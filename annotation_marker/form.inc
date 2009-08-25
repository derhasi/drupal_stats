<?php
function test_annotation_marker_form(&$form_state) {
  $form = array();
  $form['div'] = array('#value' => '<div style="width: 400px; height: 200px;" id="a66666"> </div>');
  $form['am'] = array(
    '#input' => TRUE,
    '#draggable' => TRUE,
    '#resizable' => TRUE,
    '#title' => '',
    '#type' => 'annotation_marker',
    '#default_value' => array('container selector' => '#a66666'),
    'form' => array('#type' => 'form',
                    'text' => array('#type' => 'textfield'),
                    ),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('submit'),
  );
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