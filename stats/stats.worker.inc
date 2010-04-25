<?php
// $Id$

/**
 * @file
 * Defines the stats worker interface.
 */

/**
 * Interface for stats worker classes that may work for stats.module.
 */
interface stats_worker {
  function __construct($info);
  /**
   * Method to run calculation and storage in one step.
   */
  function execute();
  /**
   * Load stats values.
   */
  function load($reload);
  /**
   * Calculate and return new values.
   */
  function calculate();
  /**
   * Write new values to storage facility (database).
   */
  function write();
  /**
   * Adds subjects to calculation/load queue.
   */
  function add_subject($subject, $addition);
  /**
   * Sets all available subjects to get calculated/loaded.
   */
  function add_all();
}
