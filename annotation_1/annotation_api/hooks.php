<?php
// $Id$

/**
 * @file
 *   hook implementations for annotation_api
 */

/**
 * Collect information for annotation module components.
 *
 * You can define three different types of hanlders for annotation_api.
 * * Annotation Type (Connect handler)
 *   this is the method you can assign values to a specific element, for example
 *   the default point module, assigns values via x/y-coordinate at a specific
 *   part (defined by selector) on the element
 * * Scope type (scope handler)
 *   this is the type of element you assign values to, for example you can set
 *   a mark on a node (in view mode) or a user profile
 * * Content Type (content handler)
 *   that's the element that will be assigned to a scope, for example you can
 *   link a user (that will be the content) on a node
 *
 * @param $type
 *   type of plugin/handler, either 'annotatio', 'scope' or 'content'
 * @return
 *   an array of plugin definitions for the specific type
 *
 *   * type: annotation
 *      - title - Human readable name for the plugin
 *      - view - the function that is used to view the annotation
 *      - settings_form - form function to add additional settings
 *      - use hook_form_annotation_form_alter() to alter the input form
 *      - fit
 *   * type: scope
 *      - title - Human readable name for the plugin
 */
function hook_annotation_info() {
  return array(
    'scope' => array(
      'node' => array(
        'title' => t('Node'),
        'help' => t('Provides nodes as scope for an annotation.'),
        'relation' => array(
          'table' => 'node',
          'field' => 'nid',
        ),
        'settings_form' => '_annotation_node_options',
        'fit' => '', //Function that defines, if this scope fits to the current call or not
      ),
    ),
    'content' => array(
      'example' => array(
        'title' => t('Example'),
        'help' => t('Connects example to annotations'),
        'relation' => array(
          'table' => 'example',
          'field' => 'eid',
        ),
        'load' => 'example_load',
        'view' => 'example_content_view',
        'form' => 'annotation_example_content_form', // ($annotation)provides the form element for content_id
        'fit' => '', // function that indicates if the content may be i used in the current call.
        'default object' => '', // function that will build default values for new $annotation objects
      ),
    ),
    'annotation' => array(
      'point' => array(
        'title' => t('Point'),
        'view' => 'annotation_example_annotation_view',// ($annotation, $content)
        //'form_alter' => 'annotation_example_annotation_formalter', // (&$form, $annotation)is the first function to alter the default form.
        'fit' => '', // callback function that indicates whether this annotation type may be used on the current context.
      ),
    ),
  );
}

/**
 * Load function for example object.
 *
 * @param $id
 *  the object id to load
 * @return
 *  an object representing the content/scope.
 */
function example_load($id) {
  return db_fetch_object(db_query('SELECT * FROM {example} WHERE eid = %d', $id));
}