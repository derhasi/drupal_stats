<?php
// $Id$

/**
 * @file
 *   hook implementations for annotation_api
 */

/**
 * Collect information for annotation module components.
 */
function hook_annotation_info($type) {
  $info = array();
  switch ($type) {
    case 'scope':

    case 'content':
      $info['example'] = array(
        'title' => t('Example'),
        'scope' => TRUE,
        'content' => TRUE,
        'help' => t('Connects example to annotations'),
        'relation' => array(
          'table' => 'example',
          'field' => 'eid',
        ),
        'load' => 'example_load',
        'view' => 'example_content_view',
        'form' => 'annotation_example_content_form', // ($annotation)provides the form element for content_id
      );
      break;
    case 'annotation':
      $info['point'] = array(
        'annotation' => 'TRUE',
        'title' => t('Point'),
        'view' => 'annotation_example_annotation_view',// ($annotation, $content)
        'form_alter' => 'annotation_example_annotation_formalter', // (&$form, $annotation)is the first function to alter the default form.
      );
      break;
  }
  return $info;
}

/**
 * Load function for example object.
 *
 * @param $id
 *  the object id to load
 * @return
 *  an object representing the content/scope.
 */
function example_load($id) {
  return db_fetch_object(db_query('SELECT * FROM {example} WHERE eid = %d', $id));
}