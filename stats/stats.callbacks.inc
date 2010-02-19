<?php

/**
 * @file
 *   Default callback functions for Stats API
 */

/**
 * Default callback to provide storage via database.
 *
 * @param $op
 *  the operator can either be:
 *  - 'load' to load data from the databse
 *  - 'insert' to write new data to the database
 *  - 'update' to write exisiting data to the database
 *  - 'delete' data from the database
 * @param $settings
 *  settings array from the type definition (like defined in hook_stats_info())
 * @param $subjects
 *  array of subjects that should by affected with write, load or delete
 *  (optional) for 'load' and 'delete' - if not specified it will delete or load
 *  all subjects
 * @param $new_values
 *  only needed for 'insert' and 'update', new values that will be written to the
 *  database
 *
 * @return
 *  - insert, update, delete: TRUE on successful operation, else FALSE
 *  - load: array of values
 *    @code
 *    array(
 *      0 => array(
 *      - 'subject' => subject_id,
 *      - 'value' => value of the value column,
 *      - 'line' => the whole line,
 *      ),
 *      1 => ...
 *    )
 *    @endcode
 */
function stats_callback_default_storage($op, $settings, $subjects = NULL, $new_values = NULL) {

  $table = $settings['storage table'];
  $subject_field = $settings['storage subject field'];
  $type_field = $settings['storage type field'];
  $value_field = $type['storage value field'];
  $extra =  $type['storage extra'];
  if (isset($subjects) && !is_array($subjects)) {
    $subjects = array($subjects);
  }

  switch ($op) {
    case 'load':
      $query = "SELECT * FROM {$table}";
      $args = array();

      // Restrict load on specified subjects
      if (isset($subjects)) {
        $cast_subject = _stats_column_type_conversion_helper($table, $subject_field);
        $where = implode(' OR ', array_fill(0, count($subjects), "$subject_field = $cast_subject"));
        $where = "($where)";
        $args = $subjects;
      }

      // Add type column to check in query
      if ($type_field) {
        if ($where) {
          $where .= ' AND ';
        }
        $where .= "$type_field = ". _stats_column_type_conversion_helper($table, $type_field);// @TODO: retrieve type conversion from schema
        $args[] = $settings['type'];
      }

      if ($where) { $query .= ' WHERE '. $where;}

      $res = db_query($query, $args);
      // Return FALSE for invalid queries.
      if ($res === FALSE) return FALSE;

      // Return an array for valid values.
      $return = array();
      while ($line = db_fetch_array($res)) {
        $return[] = array(
          'subject' => $line[$subject_field],
          //'type' => $type['type'],
          'value' => $line[$value_field],
          'line' => $line,
          );
      }
      return $return;
    case 'insert':
      // Insert every subject via drupal_write_record
      // @TODO: may be low performant on many inserts, so add alternative
      $return = FALSE;
      foreach ($subjects as $subject) {
        if ($type_field) {
          $obj = (object) array(
            $subject_field => $subject,
            $type_field => $settings['type'],
            $value_field => $new_values[$subject],
          );
        }
        $return += drupal_write_record($table,$obj) / SAVED_NEW;
      }
      return (count($subjects) == $return);
    case 'update':
      // Insert every subject via drupal_write_record
      // @TODO: may be low performant on many inserts, so add alternative
      $return = FALSE;
      // Specifiy update columns
      $update = array($subject_field);
      if ($type_field) { $update[] = $type_field;}

      foreach ($subjects as $subject) {
        if ($type_field) {
          $obj = (object) array(
            $subject_field => $subject,
            $type_field => $settings['type'],
            $value_field => $new_values[$subject],
          );
        }
        // divide by constant to retrieve succesfull calls, as SAVED_UPDATED = 2
        $return += drupal_write_record($table,$obj, $update) / SAVED_UPDATED;
      }
      return (count($subjects) == $return);
    case 'delete':
      $query = "DELETE FROM {$table}";
      $args = array();

      // Restrict load on specified subjects
      if (isset($subjects)) {
        $cast_subject = _stats_column_type_conversion_helper($table, $subject_field);
        $where = implode(' OR ', array_fill(0, count($subjects), "$subject_field = $cast_subject"));
        $where = "($where)";
        $args = $subjects;
      }

      // Add type column to check in query
      if ($type_field) {
        if ($where) {
          $where .= ' AND ';
        }
        $where .= "$type_field = ". _stats_column_type_conversion_helper($table, $type_field);// @TODO: retrieve type conversion from schema
        $args[] = $settings['type'];
      }

      if ($where) { $query .= ' WHERE '. $where;}

      $res = db_query($query, $args);
      return $res;
  }
  return ;
}

/**
 * Default callback for general calculations modes.
 *
* @param $op
 *  the operator can either be:
 *  - 'recalculate all'
 *  - 'recalculate'
 *  - 'add'
 * @param $settings
 *  settings array from the type definition (like defined in hook_stats_info())
 * @param $subjects
 *  array of subjects that should by calculated
 *  (optional) for 'load' and 'delete' - if not specified it will delete or load
 *  all subjects
 * @param $additions
 * @param $current_values
 * @return
 */
function stats_callback_default_calculation($op, $settings, $subjects = NULL, $additions = array(), $current_values = array()) {
  $table = $settings['source table'];
  $subject_field = $settings['source subject field'];
  $value_field = $settings['source value field'];
  $calc_mode = strtoupper($settings['calculation mode']);

  // @TODO
  switch ($op) {
    case 'add':

    case 'recalculate all':
    case 'recalculate':
  }
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
 * @param $addition
 *   new value that will or was added to the system, and so may be compared to
 *   existing values
 * @return
 *   array of derived values
 */
function stats_callback_derivation_add($type, $old_values, $subject, $addition) {

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
            'value' => max($val['value'], $addition),
          );
          break;
        case 'MIN':
          $return = array(
            'subject' => $subject,
            'type' => $type['type'],
            'value' => min($val['value'], $addition),
          );
          break;
        case 'SUM':
          $return = array(
            'subject' => $subject,
            'type' => $type['type'],
            'value' => $val['value'] + $addition,
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
