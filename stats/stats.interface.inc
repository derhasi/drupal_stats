<?php
// $Id$
/**
 * @file
 *
 * Defines the basic handler interface.
 */



/**
 * Structure of info array().
  * type                  (string)
  * name                  (string)
  * description           (string)
  * handler               (string) => class name


  * callback storage
    - load:
      single: subject is no array or count 1
      batch: subject is array with count > 1
      all: no subject is given or array with count 0

      => return ( subject, type, value)

      callback storage load single  - (int|str) , (stats_type)
      callback storage load batch   - (array)   , (stats_type)
      callback storage load all     - NULL      , (stats_type)

      callback storage load ($stats_type, $subjects)
      OR callback storage (op=load , $stats_type, $subjects)

    - write:
      callback storage write
      insert single new
      update single existing
      update batch existing
      insert batch new
      write batch new and updated
      all


    - delete



  * callback calculation
    variables per subject (old stats value, new item)
    - all - no subject
    - batch - 1 to n subjects
    - single

  * callback storage

  * callback storing calculation

      * calculate (add, rebuild single, rebuild all)
      * storage (load, write)
      * storing calculation (add, rebuild single, rebuild all)

  * source table          (string)
  * source subject field  (string)
  * source value field    (string)
  * source extra          (array)
  * calculation mode      (string)
  * storage table         (string)
  * storage subject field (string)
  * storage value field   (string)
  * storage extra         (array)
  for building automated views integration
  * subject table         (string)
  * subject field         (string)
  * subject extra         (array)
  for custom views integration
  * views data            (array)
  * views data alter      (array)
  *
  *
  *
  *
  *
  * FEATURES:
  * - locked subjects
  * -



  */

class stats_worker {
  var $type; // stats type
  var $name; // Human readable name
  var $description; // Human readable description
  var $settings = array(); // Additional settings, e.g for storage and calc.

  var $callbacks = array();

  var $subjects = array(); // key subject, val subject

  var $new_items = array(); // key subject, val array of items
  var $recalculate_subjects = array(); // key subject, val TRUE
  var $recalculate_all = FALSE;

  //var $subjects_loaded = array();
  //var $subjects_locked = array();
  //var $loaded_values = array();
  var $values = array();


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
    // Remove default keys (see above) and handler (selector for class)
    unset($info['type'], $info['name'], $info['description'], $info['handler']);
    // @TODO: add error handling for missing values.
    $this->settings = $info;
    $this->initialize_callbacks();
  }

  /**
   * Add a new subject to the 'to work queue'. This subject will be calculated
   * when $this->calculate() is called.
   *
   * New items are queued in a new_items array for passing it later to the
   * calculation function.
   * Without setting the $new_item the stat will be recalculated for the subject
   * therefore new items for the subject would be ignored.
   *
   * @param $subject
   *   ID of the subject
   * @param $new_item
   *   (optional) the new item that initiates the calculation
   */
  public function add_subject($subject, $new_item = NULL) {
    // Add to subjects array
    $this->subjects[$subject] = $subject;
    // Add to new items array
    if (isset($new_item)) {
      $this->new_items[$subject][] = $new_item;
    }
    // rebuild for the given subject
    else {
      $this->recalculate_subjects[$subject] = TRUE;
    }
  }

  public function recalculate() {
    $this->recalculate_all = TRUE;
    return $this->calculate();
  }

  /**
   * Build the callback implementations for storage, calculation and storing
   * calculation.
   */
  function initialize_callbacks (&$callbacks) {

  }

  /**
   * Load
   *
   * The mode for calculation and storage is dependent on the way data is
   * loaded.
   *
   */
  private function load($subject = NULL) {

  }

  /**
   * Calculate
   */
  public function calculate() {

  }

  public function write() {

  }

  /**
   * Determines the subject range to work on. May queue locked subjects for
   * further calculation.
   * Subjects could be locked through other processes dealing with the specific
   * subjects. ...
   */
  function initialize_subject_mode($subject = NULL) {



    if (!isset($subject)) {

    }
  }

}