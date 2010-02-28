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