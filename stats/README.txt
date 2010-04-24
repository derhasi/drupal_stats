// $ID$

Stats is a module to provide an API for implementing statistics creation,
calculation and storage.

Through hook_stats_info() there can be defined several types of stats. A contrib
module can use these definitions to kick statistic derivations for its data.

It's recommended to use stats_queue() for initializing stats derivation via Drupal
Queue (drupal.org/project/drupal_queue). Instant derivation could be called
directly with calling stats_run().

This module is designed in a way that allows you to seperate the collection of
data that should be calculated, the actual calculation, and the storage of
calculated values.

There are three modes of statistic creation/rebuilds:
* add
* rebuild single
* rebuild all
@TODO: description



Wording:
- stat:
  * triple of a "value" assigned to an entity (=subject), specified by a
    predefined type (stats type)
    example "5 views of node #12" => triple:(node #12, hadViews, 5)
- stats:
  * name of the module
  * collection of stat
- stats type:
  * predefined way of storing, calculation and assignation of stats
  * string identifier for type
- stats specification:
  * array that holds parameters and callbacks for a stats type
- subject:
  * entity stat is assigned to
- value
  * a value (can be of any type, any type combination, also an entity) that is
    stored for a given type
- addition:
  * a new variable/value that initiates the calcualtion of a new stat value
- new values:
  * most times an array of newly calculated values
    it shall never be used as alias for 'addition'

- stats_worker:
  * a handler like class that defines the way statistics will work. This glues
    the different levels storage and calculation. For "storing calculation"
    there specialized callbacks are needed
- specification array:
  * this array is defined in hook_stats_info() and can define parameters and
    callbacks and the stats_worker class for a stat type
- callback:
  * function that has a specific task to work on. In the default stats_worker
    there are used different callbacks:
    *


Default stats_worker
====================
The default stats_worker can be overwritten by settings 'handler' in the
specification array.


Default callbacks for stats worker
==================================
stats_worker implements three different callbacks:
* callback storage
* callback calculation
* callback storing calculation

The default callbacks for that are:
* stats_callback_default_storage
* stats_callback_default_calculation
* @TODO: stats_callback_default_storing_calculation

The can be overwritten by setting 'callback storage', 'callback calculation' or
'callback storing calculation' in the specifications array.


stats_callback_default_calculation
----------------------------------
In stats_info array you can specifiy the 'calculation mode'. This defines the
way new values ar calculated.
- add
- recalculate

fall back on recalculate if no 'add' function exists.