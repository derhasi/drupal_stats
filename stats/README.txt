// $ID$

Stats is a module to provide an API for implementing statistics creation,
calculation and storage.

Through hook_stats_info() there can be defined several types of stats. A contrib
module can use these definitions to kick statistic derivations for its data.

It's recommended to use stats_queue() for initializing stats derivation via Drupal
Queue (drupal.org/project/drupal_queue). Instant derivation could be called
directly with calling stats_run().

There are three types of statistic creation/rebuilds:
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
- stats specification
  * array that holds parameters and callbacks for a stats type
- subject
  * entity stat is assigned to
- value
  * a value (can be of any type, any type combination, also an entity) that is
    stored for a given type
- addition
  * a new variable/value that initiates the calcualtion of a new stat value

Elements:
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