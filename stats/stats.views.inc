<?php

/**
 * @file
 *   Views implementation for stats.module
 */

/**
 * Implementation of hook_views_data().
 *
 * - Uses 'views_data' method from worker objects.
 */
function stats_views_data() {
  $stats_types = stats_info();
  $views_data = array();
  foreach ($stats_types as $type => $info) {
    $worker = stats_get_worker($type);
    if (method_exists($worker, 'views_data')) {
      $add = (array) $worker->views_data();
      $views_data = array_merge($views_data, $add);
    }
  }
  return $views_data;
}


/**
 * Implementation of hook_views_data_alter().
 *
 * - Uses 'views_data_alter' method from worker objects.
 */
function stats_views_data_alter(&$data) {
  $stats_types = stats_info();
  foreach ($stats_types as $type => $info) {
    $worker = stats_get_worker($type);
    if (method_exists($worker, 'views_data_alter')) {
      $worker->views_data_alter(&$data);
    }
  }
}