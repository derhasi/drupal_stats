<?php
// $Id$
/**
 * @file
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
      single
      batch
      all

    CALLBACK -  $op, $stats_worker, $subjects, $values, $update
    op: load, insert, update, delete

  * callback calculation
    variables per subject (old stats value, addition)
    - all - no subject
    - batch - 1 to n subjects
    - single

    CALLBACK -  $op, $stats_worker, $subjects, $new_values, $old_values
    op: add, recalculate, recalculate all

  * callback storing calculation

      * calculate (add, rebuild single, rebuild all)
      * storage (load, write)
      * storing calculation (add, rebuild single, rebuild all)

  * source table          (string)
  * source subject field  (string)
  * source value field    (string)
  * source extra          (array) - not implemented yet!
  * calculation mode      (string)
  * storage table         (string)
  * storage subject field (string)
  * storage value field   (string)
  * storage extra         (array) - not implemented yet!
  for building automated views integration
  * subject table         (string)
  * subject field         (string)
  * subject extra         (array)  - not implemented yet!
  for custom views integration
  * views data            (array)
  * views data alter      (array)
  *
  *
  *
  *
  *
  * @TODO: FEATURES:
  * - locked subjects
  * -
  */
class stats_worker {
  /**
   * Basic variables for this class.
   */
  var $type; // stats type
  var $name; // Human readable name
  var $description; // Human readable description
  var $settings = array(); // Additional settings, e.g for storage and calc.

  var $callbacks = array(
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
  }

  /**
   * Build the callback implementations for storage, calculation and storing
   * calculation.
   */
  function initialize_callbacks (&$callbacks) {

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
   * Recalculate all values for the defined stats type.
   */
  public function recalculate_all() {
    $this->recalculate_all = TRUE;
    return $this->calculate();
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
    if (!$this->settings['callback storing calculation']) {
      $this->calculate();
      $this->write();
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
   */
  private function load() {

  }

  /**
   * Calculate new values without storing them.
   */
  public function calculate() {

  }

  /**
   * Store actual calculation.
   */
  public function write() {

  }

}