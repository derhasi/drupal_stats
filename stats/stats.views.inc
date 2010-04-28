<?php

/**
 * @file
 *   Views implementation for stats.module
 */

/**
 * Implementation of hook_views_data().
 */
function stats_views_data() {
  $stats_types = stats_info();
  $views_data = array();
  foreach ($stats_types as $key => $type) {
    if (!empty($type['views data']) && is_array($type['views data'])) {
      $views_data = array_merge($views_data, $type['views data']);
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
  $stats_types = stats_info();
  foreach ($stats_types as $key => $type) {
    $type_id = $type['type'];
    if (!empty($type['views data alter']) && is_array($type['views data alter'])) {
      $data = array_merge_recursive($data, $type['views data alter']);
    }
  }
}