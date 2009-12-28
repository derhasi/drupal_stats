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
 * Implementation of hook_stats_info().
 *
 * Return some stats info types
 */
function hook_stats_info() {
  $return = array(
    'term_nodecount' => array(
      'type' => 'term_nodecount',
      'active' => TRUE,
      'name' => t('Term - Node count'),
      'description' => t('Counts the number of nodes assigned to term'),
      'source' => array(
        'table' => 'term_node',
        'subject field' => 'tid',
        'value field' => 'nid',
        // 'extra' =>,
      ),
      'subject' => array(
        'table' => 'term_data',
        'field' => 'tid',
        // 'extra' =>,
      ),
      'derivation' => array(
        'callback' => array(
          'add' => 'stats_callback_derivation_add',
          //'remove' => 'stats_callback_derivation_remove',
          //'remove_batch' => 'stats_callback_derivation_remove_batch',
          'rebuild single' => 'stats_callback_derivation_rebuild_single',
          'rebuild all' => 'stats_callback_derivation_rebuild_all',
          'skip load' => array('rebuild_single', 'rebuild_all'),
        ),
        'mode' => 'count',
      ),
      'storage' => array(
        'callback' => array(
          'load' => 'stats_callback_storage_load',
          'write' => 'stats_callback_storage_write',
        ),
        'table' => 'stats_int_int',
        'subject field' => 'subject',
        'value field' => 'value',
        'type field' => 'type',
        //'extra' => ,
        'views table declared' => TRUE, // FALSE default, TRUE for allready declared as table for views
        'views field declared' => FALSE, // FALSE default,
      ),
      //'derivation & storage' => array(),
      // views data will be built automatically and/or can be altered via
      // hook_stats_info_alter().
      'views data' => array(
        // ...
      ),
      'views data alter' => array(
        // ...
      ),
    )
  );
  return $return;
}

/**
 * Implementation of hook_stats_info_alter().
 */
function hook_stats_info_alter(&$info) {

}