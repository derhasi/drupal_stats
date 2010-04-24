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
  function execute();
  function load($reload);
  function calculate();
  function write();
  function add_subject($subject, $addition);
  function add_all();
}
