<?php

/**
 * @file
 *   functions that are needed to define, process, display the annotation
 *   form element.
 */

/**
 *  Theme an annotation element.
 */
function theme_annotation($element) {
  dpm($element);
  return '<div'. drupal_attributes($element['#attributes']) .'>'. ($element['#title'] ? '<label>'. $element['#title'] .'</label>' : '') . (isset($element['#description']) && $element['#description'] ? '<div class="description">'. $element['#description'] .'</div>' : '') . (!empty($element['#children']) ? $element['#children'] : '') . "</div>\n";
}

/**
 * Processing of annotation form element.
 */
function annotation_element_process($element) {
  dpm($element);
  $annotation = (object)(isset($element['#value'])) ? $element['#value'] : $element['#default_value'];
  $info = annotation_api_info();
  $annotation_callback = $info[$annotation->type]['process'];
  $content_callback = $info[$annotation->content_type]['process'];
  if ($content_callback) $element = $content_callback($element);
  if ($annotation_callback) $element = $annotation_callback($element);
  return $element;
}

/**
 * Validate the annotation value
 */
function annotation_element_validate($form) {
  $annotation = (object)$form['#value'];
  $info = annotation_api_info();
  $annotation_callback = $info[$annotation->type]['validate'];
  $content_callback = $info[$annotation->content_type]['validate'];
  if ($content_callback) $element = $content_callback($form);
  if ($annotation_callback) $element = $annotation_callback($form);
}
