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
      $info['node'] = array(
        'title' => t('Node'),
        'scope' => TRUE,
        'content' => TRUE,
        'help' => t('Connects nodes with annotation, as scope or content.'),
        'relation' => array(
          'table' => 'node',
          'field' => 'nid',
        ),
        'load' => 'node_load', // additional load function to provide an object
        'view' => 'node_view', // ($content, $annotation)function to view content in annotation
        // 'process' =>
        // 'after_build' =>
        // 'submit'
      );
      break;
    case 'annotation':
      $info['point'] = array(
        'annotation' => 'TRUE',
        'title' => t('Point'),
        'view' => 'annotation_point',// ($annotation)
      );
      break;
  }
  return $info;
}
