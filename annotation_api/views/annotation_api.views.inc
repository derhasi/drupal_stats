<?php
// $Id$

/**
 * @file
 *   views functionality for annotation_api
 *
 *  like comment integration
 */

/**
 * Implemenatation of hook_views_data().
 */
function annotation_api_views_data() {
  $data['annotation'] = array(
    'table' => array(
      'group' => t('Annotation dH'),
      'base' => array(
        'field' => 'aid',
        'title' => t('Annotation'),
        'help' => t('Annotations are elements that are assigned to other elements. Elements could be nodes, users, comments....'),
      ),/**
      'join' => array(
        'users' => array(
          'left_field' => 'uid',
          'field' => 'uid',
          'type' => 'INNER', // all nodes have an author.
        ),
      ),*/
    ),
    // ----------------------------------------------------------------
    // Fields

    // aid
    'aid' => array(
      'title' => t('ID'),
      'help' => t('The annotation ID of the field'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument',
      ),
    ),
    'timestamp' => array(
      'title' => t('Post date'),
      'help' => t('Date and time of when the annotation was posted.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
    ),
    'uid' => array(
      'title' => t('User'),
      'help' => t('The User ID of the annotator.'),
      'relationship' => array(
        'base' => 'users',
        'field' => 'uid',
        'handler' => 'views_handler_relationship',
        'label' => t('User'),
      ),
    ),
    // Node type
    'type' => array(
      'title' => t('Type'),
      'help' => t('The type of an annotation (for example, "grid" etc).'),
      'field' => array(
        'handler' => 'views_handler_field_annotation_type',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'annotation_api_views_handler_filter_type',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument',
      ),
    ),

  );


  $info = annotation_api_info('scope');

  foreach ($info as $key => $item) {
    if (isset($item['relation'])) {
      $data['annotation']["scope_$key"] = array(
        'real field' => 'scope_id',
        'title' => t('Scope: @scope', array('@scope' => $item['title'])),
        'help' => $item['help'],
        'relationship' => array(
          'base' => $item['relation']['table'],
          'field' => $item['relation']['field'],
          'relationship field' => 'scope_id',
          'extra' => array(
            array(
              'table' => 'annotation',
              'field' => 'scope_type',
              'value' => $key,
            ),
          ),
          'handler' => 'views_handler_relationship',
          'label' => $item['title'],
        ),
      );
    }
  }
  return $data;
}

/**
 * Implementation of hook_views_data_alter().
 */
function annotation_api_views_data_alter(&$data) {

  $data['users']['table']['join']['annotation'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
    'type' => 'INNER', // all nodes have an author.
  );

  $info = annotation_api_info('scope');
  foreach ($info as $key => $item) {
    if (isset($item['relation'])) {
      $data[$item['relation']['table']]["annotation_scope_$key"] = array(
        'real field' => $item['relation']['field'],
        'title' => t('Annotation: Scope'),
        'help' => $item['help'],
        'relationship' => array(
          'base' => 'annotation',
          'field' => 'scope_id',
          'left field' => $item['relation']['field'],
          'handler' => 'views_handler_relationship',
          'label' => t('Annotation: scope'),
          'extra' => array(
            array(
              'field' => 'scope_type',
              'value' => $key,
            ),
          ),
        ),
      );
    }
  }
}

/**
 * Implementation of hook_views_handler().
 */
function annotation_api_views_handlers(){
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'annotation_api') . '/views',
    ),
    'handlers' => array(
      // filters
      'annotation_api_views_handler_filter_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}