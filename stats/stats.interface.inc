<?php
// $Id$
/**
 * @file
 *
 * Defines the basic handler interface.
 */

interface stats_storage_handler {
  public function load_single($type, $subject);
  public function load_all($type);
  public function write_single($type, $value);
  public function write_all($type, $values);
}

interface stats_calculation_handler {
  public function calculate();
}

interface stats_calculate_and_write_handler {
  public function calculate_and_write();
}

/**
 * Structure of info array().
  * type                  (string)
  * name                  (string)
  * description           (string)
  * handler               (string)
  * callback              (string)
  OR callback             (array)
      * calculate (add, rebuild single, rebuild all)
      * storage (load, write)
      * calculate & storage (add, rebuild single, rebuild all)

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
  */

class stats_type {
  var $type;
  var $name;
  var $description;
  var $settings = array();

  var $loaded_subjects = array();
  var $loaded_values = array();

  /**
   * Default constructor to implement the stats type info.
   *
   */
  function __construct($info) {
    $this->type = $info['type'];
    $this->name = $info['name'];
    $this->description = $info['description'];
    // Remove default keys (see above) and handler (selector for class)
    unset($info['type'], $info['name'], $info['description'], $info['handler']);
    // @TODO: add error handling for missing values.


  }

  /**
   * Load
   *
   *
   * The mode for calculation and storage is dependent on the way data is
   * loaded.
   */
  function load($subject = NULL) {

    $this->initialize_subject_mode($subject);

  }

  function write($values = array(), $subject = NULL) {

  }

  function calculate($subject = NULL, $new_value = NULL) {

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