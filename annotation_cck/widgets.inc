<?php

function _annotation_cck_widget_hidden(&$form, &$form_state, $field, $items, $delta = 0) {
	$element = array('#type' => 'fieldset', '#title' => $field['widget']['label'], '#attributes' => array('class' => 'hallobaum'));
  $element['posx']              = array('#type' => 'textfield', '#title' => 'x', '#default_value' =>  $items[$delta]['posx']);
  $element['posy']              = array('#type' => 'textfield', '#title' => 'y', '#default_value' =>  $items[$delta]['posy']);
  $element['width']             = array('#type' => 'textfield', '#title' => 'w', '#default_value' =>  $items[$delta]['width']);
  $element['height']            = array('#type' => 'textfield', '#title' => 'h', '#default_value' =>  $items[$delta]['height']);
  $element['container']         = array('#type' => 'textfield', '#title' => 'c', '#default_value' =>  $items[$delta]['container']);
  $element['container_width']   = array('#type' => 'textfield', '#title' => 'cw', '#default_value' =>  $items[$delta]['container_width']);
  $element['container_height']  = array('#type' => 'textfield', '#title' => 'ch', '#default_value' =>  $items[$delta]['container_height']);
  $element['source']            = array('#type' => 'textfield', '#title' => 's', '#default_value' =>  $items[$delta]['source']);
  $element['marker'] = array('#type' => 'annotation_marker', '#selector' => '.hallobaum', '#draggable' => TRUE);
  return $element;
}
