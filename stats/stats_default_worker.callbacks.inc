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
  $value_field = $settings['storage value field'];
  //$extra = $settings['storage extra']; @TODO
  // Ensure subjects to be in array
  if (isset($subjects) && !is_array($subjects)) {
    $subjects = array($subjects);
  }

  switch ($op) {
    case 'load':
      $query = 'SELECT * FROM {'. $table .'}';
      $args = array();

      // Restrict load on specified subjects
      if (isset($subjects)) {
        $cast_subject = _stats_column_type_conversion_helper($table, $subject_field);
        $where = implode(' OR ', array_fill(0, count($subjects), "$subject_field = $cast_subject"));
        $where = "($where)";
        $args = array_merge($args, $subjects);
      }

      // Add type column to check in query
      if ($type_field) {
        if ($where) {
          $where .= ' AND ';
        }
        $where .= "$type_field = ". _stats_column_type_conversion_helper($table, $type_field);
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
        else {
          $obj = (object) array(
            $subject_field => $subject,
            $value_field => $new_values[$subject],
          );
        }
        $return += drupal_write_record($table, $obj) / SAVED_NEW;
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
        $return += drupal_write_record($table, $obj, $update) / SAVED_UPDATED;
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
  $calc_mode = ($settings['calculation mode']) ? strtoupper($settings['calculation mode']) : 'COUNT';

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

  switch ($op) {
    case 'add':
      foreach ($subjects as $subject) {
        if (isset($additions[$subject])) {
          if (isset($current_values[$subject])) {
            $return[$subject] = call_user_func($addfunc, $additions[$subject], $current_values[$subject]);
          }
          else {
            $return[$subject] = call_user_func($addfunc, $additions[$subject]);
          }
        }
      }
      return $return;
    case 'recalculate':
      // no Subjects no calculation, return uncalculated current values
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
          'calculation mode' => $calc_mode,
          'settings' => $settings,
          'op' => $op,
          'subjects' => $subjects,
          'additions' => $additions,
          'current_values' => $current_values,
          'query_where' => $where,
          'query_args' => $args,
        );
        $res = call_user_func($recalcfunc, $vars);
        // Func could return the concrete return array
        if (is_array($res)) {
          return $res;
        }
      }
      // Execute default query
      else {
        $query = "SELECT $subject_field AS subject, $calc_mode($value_field) AS result FROM {$table} $where GROUP BY $subject_field";
        $res = db_query($query, $args);
      }

      // Evaluate result resource (either from default query or custom recalcfunc)
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
function _stats_calc_add_count($additions, $old_value = 0) {
  return $old_value + count($additions);
}
function _stats_calc_add_count_rows($additions, $old_value = 0) {
  return $old_value + count($additions);
}
function _stats_calc_add_sum($additions, $old_value = 0) {
  return $old_value + sum($additions);
}
function _stats_calc_add_min($additions, $old_value = NULL) {
  if (!isset($old_value)) {
    return min($additions);
  }
  return min($old_value, min($additions));
}
function _stats_calc_add_max($additions, $old_value = NULL) {
  if (!isset($old_value)) {
    return max($additions);
  }
  return max($old_value, max($additions));
}

/**
 * Stats default worker views implementation.
 */
function stats_callback_default_views_data_implementation($settings) {

  $type = $settings['type'];

  $storage_table = $settings['storage table'];
  $storage_subject_field = $settings['storage subject field'];
  $storage_type_field = $settings['storage type field'];
  $storage_value_field = $settings['storage value field'];

  //PREPARE settings
  $prep = _stats_callback_default_prepaore_for_views_data($settings);
  $storage_views_relations = (array) $prep['relations'];
  $storage_views_fields = (array) $prep['fields'];

  //Set data array
  $data['stats_'. $type .'_'. $storage_table]['table']['group'] = t('Stats');
  $data['stats_'. $type .'_'. $storage_table]['table']['title'] = "field: $type - $storage_table";
  $data['stats_'. $type .'_'. $storage_table]['table']['help'] = "help: $type - $storage_table";

  foreach ($storage_views_fields as $field => $field_type) {
    $data['stats_'. $type .'_'. $storage_table][$field] = array(
      'title' => t('@type - @field', array('@type' => $type, '@field' =>$field)),
      'help' => "help: $type - $field",
      'field' => array(
        'handler' => 'views_handler_field_'. $field_type,
        'click sortable' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_'. $field_type,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_'. $field_type,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
  }

  // Add joins and relations.
  foreach ($storage_views_relations as $field => $rel) {
    // Only add if there are relation tables and field.
    if ($rel['table'] && $rel['field']) {
      // Add join
      $data['stats_'. $type .'_'. $storage_table]['table']['join'][$rel['table']] = array(
        'table' => $storage_table,
        'field' => $field,
        'left_field' => $rel['field'],
      );
      if ($rel['extra']) {
        $data['stats_'. $type .'_'. $storage_table]['table']['join'][$rel['table']]['extra'] = $rel['extra'];
      }

      // Add relations only if there are more than 1, because back relation for only one relation is useless.
      if (count($storage_views_relations) > 1) {
        if (!isset($data['stats_'. $type .'_'. $storage_table][$field])) {
          $data['stats_'. $type .'_'. $storage_table][$field] = array(
            'title' => t('Stats: @type - @field', array('@type' => $type, '@field' =>$field)),
            'help' => "help: $type - $field",
          );
        }
        $data['stats_'. $type .'_'. $storage_table][$field]['relationship'] = array(
          'base' => $rel['table'],
          'field' => $rel['field'],
          'left field' => $field,
          'handler' => 'views_handler_relationship',
          'label' => t('@type:@field:@table', array('@type' => $type, '@field' => $field, '@table' => $rel['table'])),
        );
        if ($rel['extra']) {
          $data['stats_'. $type .'_'. $storage_table][$field]['relationship']['extra'] = $rel['extra'];
        }
      }
    }
  }
  return $data;
}

/**
 * Stats default worker views data alter implementation.
 *
 * - does nothing, only for callback presentation
 */
function stats_callback_default_views_data_alter_implementation(&$data, $settings) {
  // nothing because most storage tables are non-base-tables.
}

/**
 * Helper functions to prepare settings for views data (alter) implementation.
 */
function _stats_callback_default_prepaore_for_views_data($settings) {

  $type = $settings['type'];

  $storage_table = $settings['storage table'];
  $storage_subject_field = $settings['storage subject field'];
  $storage_type_field = $settings['storage type field'];
  $storage_value_field = $settings['storage value field'];

  $storage_views_relations = (array) $settings['storage views relations'];
  $storage_views_fields = (array) $settings['storage views relations'];

  // Set default field definition for value field.
  if (!isset($storage_views_fields) || !isset($storage_views_fields[$storage_value_field])) {
    $cast = _stats_column_type_conversion_helper($storage_table, $storage_value_field);
    $storage_views_fields[$storage_value_field] = ($cast == "%d") ? 'numeric' : 'string';
  }

  // Set default relations for
  if (!isset($storage_views_relations) || !isset($storage_views_relations[$storage_subject_field])) {
    // Extract information from subject table setting
    if (isset($settings['subject table'])) {
      $subject_table = $settings['subject table'];
      if (isset($settings['subject field'])) {
        $subject_field = $settings['subject field'];
      }
      // Get primary key or serial field
      else {
        $schema = drupal_get_schema($subject_table);
        $subject_field = _stats_get_idfield($subject_table);
      }
      if ($subject_table && $subject_field) {
        $storage_views_relations[$storage_subject_field] = array(
          'table' => $subject_table,
          'field' => $subject_field,
        );
      }
    }
  }

  // Convert to consistent format
  foreach ($storage_views_relations as $field => $spec) {
    if (!is_array($spec) && $spec == $storage_table) {
      unset($storage_views_relations[$field]);
    }
    elseif (!is_array($spec)) {
      $storage_views_relations[$field] = array(
        'table' => $spec,
        'field' => _stats_get_idfield($spec),
      );
    }
    // no or invalid table => remove relation entry
    elseif (!isset($spec['table']) || $spec['table'] == $storage_table) {
      unset($storage_views_relations[$field]);
    }
    elseif (!isset($spec['field'])) {
      $storage_views_relations[$field] = array(
        'table' => $spec['table'],
        'field' => _stats_get_idfield($rel_field),
        'extra' => $spec['extra'],
      );
    }
  }

  return array('fields' => $storage_views_fields, 'relations' => $storage_views_relations);
}

/**
 * Helper function to get ID field from table definition.
 */
function _stats_get_idfield($table) {
  $schema = drupal_get_schema($table);
  if (count($schema['primary key']) == 1) {
    return current($schema['primary key']);
  }
  else {
    foreach ($schema['fields'] as $fieldname => $field) {
      if ($field['type'] == 'serial') {
        return $fieldname;
      }
    }
  }
  return FALSE;
}
