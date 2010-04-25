<?php
// $Id$

/**
 * @file
 * Defines the default stats worker.
 */

/**
 * Default stats worker class for stats.module.
 *
 * It's the basic implementation for calculating and storing simple statistics.
 *
 * The stats worker itself can be controlled by the info array - defined in
 * hook_stats_info().
 *
 * Structure of info array():
 * General information
 * - type (string) - stats type
 * - name (string) - Human readable name of stats type
 * - description (string) - description for Stats type
 *
 * Calculation
 * - calculation mode (string) - the way stat is calculated (default: count),
 *   stats core implementations are 'count', 'count_rows', 'min', 'max'
 *
 * Source information (where the data comes from)
 * - source table (string)
 * - source subject field (string)
 * - source value field (string)
 *
 * Storage information (where stats are stored to)
 * - storage table (string)
 * - storage type field (optional string)
 * - storage subject field (string)
 * - storage value field (string)
 *
 * Callbacks
 * - callback storage (string) - @see stats_callback_default_storage().
 * - callback calculation (string) - @see stats_callback_default_calculation().
 * - @TODO callback storing calculation (string) - @see stats_callback_default_storing_calculation().
 *
 * @TODO - Not implemented yet!
 * - source and storage extra
 * - views relevant data, like:
 *   - subject table         (string)
 *   - subject field         (string)
 *   - subject extra         (array)
 *   - views data            (array)
 *   - views data alter      (array)
 *
 * @TODO: FEATURES:
 * - locked subjects
 * - ...
 */
class stats_default_worker {

  /**
   * Basic variables for this class.
   */
  var $type; // stats type
  var $name; // Human readable name
  var $description; // Human readable description
  var $settings = array(); // Additional settings, e.g for storage and calc.

  var $_reload = TRUE; // Indicator for if loading is necessary

  var $callbacks = array(  // will get filled in ->initialize_callbacks
    'storage' => '',
    'calculation' => '',
    'storing calculation' => '',
  );

  /**
   * Subjects to be calculated.
   */
  var $subjects = array(); // key subject, val subject

  /**
   * Additions that shall be considered for the specific subject for calculation.
   * Each subject has its own keyed array:
   * @code
   * 'subject97' => array( 'addition1', 'addition2'),
   * 'subject220' => array(...),
   * @endcode
   */
  var $additions = array(); // key subject, val array of additions

  /**
   * Defines subjects for that values should be recalculated.
   */
  var $recalculate_subjects = array(); // key subject, val TRUE
  /**
   * Defines that all values for the defined stats type should be recalculated.
   */
  var $recalculate_all = FALSE;

  //var $subjects_loaded = array();
  //var $subjects_locked = array();
  //var $loaded_values = array();

  /**
   * Values that have been calculated by ->calculate().
   */
  var $new_values = array();

  /**
   * Existing values loaded via ->load().
   */
  var $current_values = array();

  private $updated_values = array();

  /**
   * Methods are listed below.
   */

  /**
   * Default constructor to implement the stats type info.
   * @param $info
   *   type string or info array returned by stats_get_type()/stats_types().
   */
  function __construct($info) {
    // Load info array
    if (!is_array($info)) {$info = stats_get_type($info);}

    $this->type = $info['type'];
    $this->name = $info['name'];
    $this->description = $info['description'];
    // // Remove default keys (see above) and handler (selector for class)
    // unset($info['type'], $info['name'], $info['description'], $info['handler']);
    // @TODO: add error handling for missing values.
    $this->settings = $info;
    $this->initialize_callbacks();
    $this->load_files();
  }

  /**
   * Build the callback implementations for storage, calculation and storing
   * calculation.
   */
  private function initialize_callbacks () {
    $storage = $this->settings['callback storage'];
    $calculation = $this->settings['callback calculation'];
    $storing_calculation = $this->settings['storing calculation'];
    // Set defaults
    if (!$storage && !$calculation) {
      // With specific storing calculation
      if ($storing_calculation) {
        $this->callbacks = array(
          'storage' => 'stats_callback_default_storage',
          'calculation' => 'stats_callback_default_calculation',
          'storing calculation' => $storing_calculation,
        );
      }
      else {
        $this->callbacks = array(
          'storage' => 'stats_callback_default_storage',
          'calculation' => 'stats_callback_default_calculation',
          //'storing calculation' => 'stats_callback_default_storing_calculation', - not yet implemented
        );
      }
    }
    // No storing calculation
    elseif (!$storing_calculation) {
      $this->callbacks = array(
        'storage' => $storage,
        'calculation' => $calculation,
      );
    }
    else {
      $this->callbacks = array(
        'storage' => $storage,
        'calculation' => $calculation,
        'storing calculation' => $storing_calculation,
      );
    }
  }

  /**
   * Load additional files needed for callbacks.
   */
  private function load_files() {
    if (isset($this->settings['files']) && $this->settings['files']) {
      if (!is_array($this->settings['files']) && is_file('./'. $this->settings['files'])) {
        require_once './'. $this->settings['files'];
      }
      else {
        foreach ($this->settings['files'] as $file) {
          if (is_file('./'. $file)) {
            require_once('./'. $file);
          }
        }
      }
    }
    if (isset($this->settings['file']) && $this->settings['file']) {
      if (is_file('./'. $this->settings['file'])) {
        require_once './'. $this->settings['file'];
      };
    }
    // Default file
    module_load_include('inc', 'stats', 'stats_default_worker.callbacks');
  }

  /**
   * Add a new subject to the 'to work queue'. This subject will be calculated
   * when $this->calculate() is called.
   *
   * Additions are queued in a additions array for passing it later to the
   * calculation function.
   * Without setting the $addition the stat will be recalculated for the subject
   * therefore additions for the subject would be ignored.
   *
   * @param $subject
   *   ID of the subject
   * @param $addition
   *   (optional) the addition that initiates the calculation
   */
  public function add_subject($subject, $addition = NULL) {
    if (!in_array($subject, $this->subjects)) {
      // Reload is needed for new subjects
      $this->_reload = TRUE;
    }
    // Add to subjects array
    $this->subjects[$subject] = $subject;
    // Add to new additions array
    if (isset($addition)) {
      $this->additions[$subject][] = $addition;
    }
    // rebuild for the given subject
    else {
      $this->recalculate_subjects[$subject] = TRUE;
    }
  }

  /**
   * Sets all subjects to get recalculated/loaded.
   */
  public function add_all() {
    $this->recalculate_all = TRUE;
    return TRUE;
  }

  /**
   * Calculate and store new values for the defined subjects.
   *
   * Will consider ->additions, ->recalculate_subjects and ->recalculate_all
   * for calculating and
   */
  public function execute() {

    // If no storing calculation is available, serial processing is needed
    // first calculate then store
    if (!$this->callbacks['storing calculation']) {
      $this->calculate();
      return $this->write();
    }
    // @TODO: implement storing calculation callback
    else {
      if ($this->recalculate_all == TRUE) {
      }
      else {
      }
    }
  }

  /**
   * Load existing values for the defined subjects.
   * @param $reload
   *   force load of existing values with TRUE
   */
  public function load($reload = FALSE) {
    if ($this->_reload || $reload) {
      $callback = $this->callbacks['storage'];

      // Load all existing
      if ($this->recalculate_all) {
        $current_values = call_user_func($callback, 'load', $this->settings);
      }
      else {
        $current_values = call_user_func($callback, 'load', $this->settings, $this->subjects);
      }

      // @TODO: maybe set $this->current_values_raw = $current_values;

      // Reset current values
      $this->current_values = array();
      // Transform return array to simple key => value
      foreach($current_values as $key => $item) {
        if (is_array($item) && isset($item['subject'])) {
          $subj = $item['subject'];
          $val = $item['value'];
          $this->current_values[$subj] = $val;
        }
        else {
          $this->current_values[$key] = $item;
        }
      }
    }
    $this->_reload = FALSE;
    return $this->current_values;
  }

  /**
   * Calculate new values without storing them.
   */
  public function calculate() {

    $callback = $this->callbacks['calculation'];
    $return = array();
    // Recalculate all
    if ($this->recalculate_all) {
      $return = call_user_func($callback, 'recalculate all', $this->settings, $this->subjects, $this->additions);
    }
    // Else recalculate defined subjects
    else {
      if (count($this->recalculate_subjects)) {
        $return = call_user_func($callback, 'recalculate', $this->settings, $this->subjects, $this->additions);
      }
      // Process Additions for subjects that are not recalculated
      if (count($this->additions)) {
        $additions = array_diff_key($this->additions, $this->recalculate_subjects);
        // less subjects to work on
        $subjects = drupal_map_assoc(array_keys($additions));

        // Addition needs current values
        $this->load();

        $return_additions = call_user_func($callback, 'add', $this->settings, $subjects, $this->additions, $this->current_values);
        // Merge addition return with recalculate return - no array_merge(), because it renumbers keys
        $return = $return + $return_additions;
      }
    }
    // Set new_values
    $this->new_values += $return;

    // Define inserted and updated values
    // This has to be done at this place and not in ->write(), because there may
    // be a delay between calculate and write, that adds or removes subjects,
    // so ->load would return out-of-date current_values.
    $this->_updated_values();

    return $return;
  }

  /**
   * Retrieves the status (new = 0, or updated = 1) for stats that shall be
   * written to database.
   * This will use ->current_values and ->new_values to retrieve the status.
   */
  private function _updated_values() {
    $this->load(); // Ensure current values to be loaded.
    $this->updated_values = array();
    $existing = array_intersect_key($this->new_values, $this->current_values);
    $this->updated_values = $existing;
  }

  /**
   * Store actual calculation.
   */
  public function write() {
    $callback = $this->callbacks['storage'];
    // Get updated values (set in ->calculate / ->updated_values)
    $cur_updated = $this->updated_values;

    $updated = array_intersect_key($this->new_values, $cur_updated);
    $new = array_diff_key($this->new_values, $cur_updated);
    $subjects_updated = drupal_map_assoc(array_keys($updated));
    $subjects_new = drupal_map_assoc(array_keys($new));

    $success = TRUE;
    // updated values
    if (count($subjects_updated)) {
      $success = call_user_func($callback, 'update',  $this->settings, $subjects_updated, $updated);
    }
    // new values
    if (count($subjects_new)) {
      $ret2 = call_user_func($callback, 'insert',  $this->settings, $subjects_new, $new);
      $success = ($success) ? $ret2 : FALSE;
    }

    // Set new_values to empty, after write
    $this->new_values = array();

    // return TRUE if all values have been saved.
    return $success;
  }

}
