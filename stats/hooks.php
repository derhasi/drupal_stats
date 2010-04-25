<?php

/**
 * @file
 *  example hook implemantions for stats.module
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Define Stats types.
 *
 * @see DEV.txt
 * Return some stats info types
 */
function hook_stats_info() {
  $return = array(
  );
  return $return;
}

/**
 * Alter Stats type info.
 */
function hook_stats_info_alter(&$info) {

}

/**
 * Specifiy stats worker classes, and the files and parents to load.
 */
function hook_stats_worker_info() {
  return array(
    'stats_default_worker' => array(
      'file' => drupal_get_path('moduel', 'stats') .'/stats.worker.inc',
      //'extends' => 'stats_default_worker',
    ),
  );
}