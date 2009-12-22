<?php

/**
 * @file
 *   Views implementation for stats.module
 */

/**
 * Implementation of hook_views_data().
 */
function stats_views_data() {
  $stats_types = stats_types();
  $views_data = array();
  foreach ($stats_types as $key => $type) {
    if (!empty($type['views data'])) {
      $views_data = array_merge($views_data, $type['view data']);
    }
  }
  return $views_data;
}


/**
 * Implementation of hook_views_data_alter().
 *
 * Adds relations from base to storage tables.
 */
function stats_views_data_alter(&$data) {
  $stats_types = stats_types();
  foreach ($stats_types as $key => $type) {
    $type_id = $type['type'];
    if (!empty($type['views data alter'])) {
      $data = array_merge_recursive($data, $type['views data alter']);
    }
  }
}