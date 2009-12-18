<?php

/**
 * @file
 *  example hook implemantions for stats.module
 */

/**
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
          'remove' => 'stats_callback_derivation_remove',
          'remove_batch' => 'stats_callback_derivation_remove_batch',
          'rebuild_single' => 'stats_callback_derivation_rebuild_single',
          'rebuild_batch' => 'stats_callback_derivation_rebuild_batch',
        ),
        'mode' => 'count',
      ),
      'storage' => array(
        'callback' => 'stats_callback_storage',
        'table' => 'stats_int_int',
        'subject field' => 'subject',
        'value field' => 'value',
        //'extra' => ,
      ),
      //'derivation & storage' => array(),
    )
  );
  return $return;
}