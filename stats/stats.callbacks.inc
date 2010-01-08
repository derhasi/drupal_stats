<?php

/**
 * @file
 *   Default callback functions for Stats API
 */

/**
 * Default callback function for loading stats values.
 * @param $type
 *   type array
 * @param $subject
 *   (optional) will retrive data only for the given subject
 * @param $new_value
 *   (optional) - not needed for the default load implementation
 * @return
 *   array of stats value arrays
 */
function stats_callback_storage_load($type, $subject = NULL, $new_value = NULL) {

  $table = $type['storage']['table'];
  $col_subj = $type['storage']['subject field'];
  $col_val = $type['storage']['value field'];

  $query = "SELECT * FROM {$table}";
  $args = array();

  if (isset($subject)) {
    $cast_subject = '%d'; // TODO: retrieve type conversion from schema
    $query .= " WHERE $col_subj = $cast_subject";
    $args = array($subject);
  }

  $res = db_query($query, $args);
  // Return FALSE for invalid queries.
  if ($res === FALSE) return FALSE;

  // Return an array for valid queries.
  $return = array();
  while ($line = db_fetch_array($res)) {
    if ($line[$type['storage']['type field']] == $type['type']) {
      $return[$line[$col_subj]] = array(
        'subject' => $line[$col_subj],
        'type' => $type['type'],
        'value' => $line[$col_val],
        'data' => $line,
      );
    }
  }
  return $return;

}

/**
 * Default callback function for writing stats values.
 * @param $type
 *   type array
 * @param $values,
 *   data to write
 * @param $subject
 *   (optional) will write data only for the given subject
 * @param $new_value
 *   (optional) - not needed for the default write implementation
 * @return
 *   count of succesful writes
 */
function stats_callback_storage_write($type, $values, $subject = NULL, $new_value = NULL) {

  if (isset($subject) && isset($values[$subject]) && $values[$subject]['type'] == $type['type']) {
    $write = array($subject => $values[$subject]);
  }
  elseif (isset($subject)) {
    return FALSE;
  }
  else {
    //only write defined types data
    foreach ($values as $key => $val) {
      if ($val['type'] == $type['type']) {
        $write[$val['subject']] = $val;
      }
    }
  }

  $table = $type['storage']['table'];
  $col_subj = $type['storage']['subject field'];
  $col_val = $type['storage']['value field'];

  $return = 0;

  // retrieve update or insert for each value
  foreach ($write as $subject => $val) {

    if ($val['update']) {
      // TODO, maybe retrieve extra col
      $update = array($col_subj);
    }
    else {
      $update = array();
    }

    //Get line-object
    $data = $val['data'];
    $data[$col_subj] = $subject;
    $data[$col_val] = $val['value'];

    $ret = drupal_write_record($table, $record, $update);
    $return += $ret;
  }
  return $return;
}

/**
 * Default callback function for batch rebuild
 * @param $type
 *   type array, as defined in hook_stats_info()
 * @param $old_values
 *   array of former values
 * @return
 *   array of derived values
 */
function stats_callback_derivation_rebuild_all($type, $old_values = NULL) {

  $mode = strtoupper($type['derivation']['mode']);
  $table = $type['source']['table'];
  $c_subj = $type['source']['subject field'];
  $c_val = $type['source']['value field'];

  switch ($mode) {
    case 'COUNT ROWS':
    case 'COUNT_ROWS':
      $query = "SELECT $c_subj AS subject, COUNT(*) AS result FROM {$table} GROUP BY $c_subj";
      $args = array();
      break;
    default:
      $query = "SELECT $c_subj AS subject, $mode($c_val) AS result FROM {$table} GROUP BY $c_subj";
      $args = array();
      break;
  }

  $res = db_query($query, $args);
  // Return FALSE for invalid queries.
  if ($res === FALSE) return FALSE;

  // Return an array for valid queries.
  $return = array();
  while ($line = db_fetch_object($res)) {
    $return[$line->subject] = array(
      'subject' => $line->subject,
      'type' => $type['type'],
      'value' => $line->result,
    );
    // add old line data
    if (isset($old_values[$line->subject])) {
      $return[$line->subject]['data'] = $old_values[$line->subject]['data'];
      $return[$line->subject]['update'] = TRUE;
    }
  }
  return $return;
}

/**
 * Default callback function for a single rebuild.
 * @param $type
 *   type array, as defined in hook_stats_info()
 * @param $old_values
 *   array of former values
 * @param $subject
 *   id for the subject to rebuild for
 * @return
 *   array of derived values
 */
function stats_callback_derivation_rebuild_single($type, $old_values, $subject) {

  $mode = strtoupper($type['derivation']['mode']);
  $table = $type['source']['table'];
  $col_subj = $type['source']['subject field'];
  $col_val = $type['source']['value field'];

  $cast_subject = '%d'; // TODO: retrieve type conversion from schema

  switch ($mode) {
    case 'COUNT ROWS':
    case 'COUNT_ROWS':
      $query = "SELECT $col_subj AS subject, COUNT(*) AS result FROM {$table} WHERE $col_subj = $cast_subject GROUP BY $col_subj";
      $args = array($subject);
      break;
    default:
      $query = "SELECT $col_subj AS subject, $mode($col_val) AS result FROM {$table} WHERE $col_subj = $cast_subject GROUP BY $col_subj";
      $args = array($subject);
      break;
  }

  $res = db_query($query, $args);
  // Return FALSE for invalid queries.
  if ($res === FALSE) return FALSE;

  // Return an array for valid queries.
  $return = array();
  while ($line = db_fetch_object($res)) {
    $return[$line->subject] = array(
      'subject' => $line->subject,
      'type' => $type['type'],
      'value' => $line->result,
    );
    // add old new line data
    if (isset($old_values[$line->subject])) {
      $return[$line->subject]['data'] = $old_values[$line->subject]['data'];
      $return[$line->subject]['update'] = TRUE;
    }

  }
  return $return;
}

/**
 * Default callback function for a single rebuild.
 * @param $type
 *   type array, as defined in hook_stats_info()
 * @param $old_values
 *   array of former values
 * @param $subject
 *   id for the subject to rebuild for
 * @param $new_value
 *   new value that will or was added to the system, and so may be compared to
 *   existing values
 * @return
 *   array of derived values
 */
function stats_callback_derivation_add($type, $old_values, $subject, $new_value) {

  $mode = strtoupper($type['derivation']['mode']);
  $table = $type['source']['table'];
  $col_subj = $type['source']['subject field'];
  $col_val = $type['source']['value field'];

  $cast_subject = '%d'; // TODO: retrieve type conversion from schema

  // Work with old_values
  if (isset($old_values[$subject])) {
    $val = $old_values[$subject];
    if ($val['subject'] == $subject) {
      switch ($mode) {
        case 'COUNT':
          $return = array(
            'subject' => $subject,
            'type' => $type['type'],
            'value' => $val['value'] + 1,
          );
          break;
        case 'MAX':
          $return = array(
            'subject' => $subject,
            'type' => $type['type'],
            'value' => max($val['value'], $new_value),
          );
          break;
        case 'MIN':
          $return = array(
            'subject' => $subject,
            'type' => $type['type'],
            'value' => min($val['value'], $new_value),
          );
          break;
        case 'SUM':
          $return = array(
            'subject' => $subject,
            'type' => $type['type'],
            'value' => $val['value'] + $new_value,
          );
          break;
      }
      // Add data
      if (isset($return)) {
        $return['data'] = $val['data'];
        $return['update'] = TRUE; // set update key for storage
      }

    }
  }
  // new subjects -> else {}

  // Fall back on single rebuild
  return stats_callback_derivation_rebuild_single($type, $old_values, $subject);
}
