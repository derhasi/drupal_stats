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
