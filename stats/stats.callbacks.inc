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
 *  with $settings['calculation mode'] the way values are calculated will be
 *  specified. In recalculate and reculalculate an SQL-Function with this name
 *  will be called, like SUM or COUNT.
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

  // copy current values for return
  $return = $current_values;

  // Only process add mode, if callback is available for calc mode
  $addfunc = str_replace(' ', '_', '_stats_calc_add_'. strtolower($calc_mode));
  if ($op == 'add' && !function_exists($addfunc)) {
    // switch to recalculate
    watchdog('stats', 'Calculation switched from add to recalculate, due to missing add function for %calc_mode', array('%calc_mode' => $calc_mode), WATCHDOG_WARNING);
    $op = 'recalculate';
  }

  $args = array();
  // @TODO
  switch ($op) {
    case 'add':
      foreach ($subjects as $subject) {
        $return[$subject] = call_user_func($addfunc, $current_values[$subject], $additions[$subject]);
      }
      return $return;
    case 'recalculate':
      // no Subjects no calculation, return uncalulated current values
      if (!count($subjects)) {
        return $return;
      }
      $cast_subject = _stats_column_type_conversion_helper($table, $subject_field);
      $where = implode(' OR ', array_fill(0, count($subjects), "$subject_field = $cast_subject"));
      $where = "WHERE ($where)";
      $args = $subjects;
      // no break, because query of recalculate will be altered with $where
    case 'recalculate all':
      $recalcfunc = str_replace(' ', '_', '_stats_calc_recalculate_'. strtolower($calc_mode));

      // Use custom recalcfunc
      if (function_exists($recalcfunc)) {
        $vars = array(
          'table' => $table,
          'subject field' => $subject_field,
          'value field' => $value_field,
          'calc mode' => $calc_mode,
          'settings' => $settings,
          'op' => $op,
          'subjects' => $subjects,
          'additions' => $additions,
          'current_values' => $current_values,
          'query_where' => $where,
          'query_args' => $args,
        );
        $res = call_user_func($recalcfunc, $vars);
        // Func could return the concrete
        if (is_array($res)) {
          return $res;
        }
      }
      else {
        $query = "SELECT $subject_field AS subject, $calc_mode($value_field) AS result FROM {$table} $where GROUP BY $subject_field";
        $res = db_query($query, $args);
      }

      $res = db_query($query, $args);
      while ($item = db_fetch_object($res)) {
        $return[$item->subject] = $item->result;
      }
      return $return;

      break;
  }
}

/**
 * Recalculate functions.
 *
 * @param $vars
 * @return
 *  May return a query result handler that contains the columns subject and result,
 *  or a completely calculated array of $subject => $value
 */
function _stats_calc_recalculate_count_rows($vars) {
  $where = $vars['query_where'];
  $args = $vars['query_args'];
  $subject_field = $vars['subject field'];
  $table = $vars['table'];
  $query = "SELECT $subject_field AS subject, COUNT(*) AS result FROM {$table} $where GROUP BY $subject_field";
  return db_query($query, $args);
}

/**
 * Add calc functions
 */
function _stats_calc_add_count($old_value, $additions) {
  return $old_value + count($additions);
}
function _stats_calc_add_count_rows($old_value, $additions) {
  return $old_value + count($additions);
}
function _stats_calc_add_sum($old_value, $additions) {
  return $old_value + sum($additions);
}
function _stats_calc_add_min($old_value, $additions) {
  return min($old_value, min($additions));
}
function _stats_calc_add_max($old_value, $additions) {
  return max($old_value, max($additions));
}
