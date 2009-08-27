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
      );
      break;
    case 'annotation':
      $info['point'] = array(
        'annotation' => 'TRUE',
        'title' => t('Point'),
      );
      break;
  }
  return $info;
}
