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
      ),
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
      'help' => t("The User ID of the annotator."),
      'relationship' => array(
        'base' => 'users',
        'field' => 'uid',
        'handler' => 'views_handler_relationship',
        'label' => t('User'),
      ),
    ),
  );
}