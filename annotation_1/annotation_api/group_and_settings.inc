<?php

/**
 * These functions are part of an administratable UI for combining annotation
 * types with different scopes and content.
 *
 * They are not implemented yet.
 */

/**
 * Implementation of hook_perm().
 */
function annotation_api_perm() {
  return array('administer annotations');
}

/**
 * Implementation of hook_menu().
 */
function annotation_api_menu() {
  $items = array(
    'admin/settings/annotation_api' => array(
      'title' => 'Annotation API Settings',
      'description' => 'Settings for annotation api',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('annotation_api_admin_form'),
      'access arguments' => array('administer annotations'),
      'file' => 'admin.inc',
    ),
    'admin/settings/annotation_api/%/edit' => array(
      'title' => 'Edit Annotation Setting',
      'page arguments' => array('annotation_api_admin_edit_form', 3),
      'access arguments' => array('administer annotations'),
    ),
    'admin/settings/annotation_api/%/delete' => array(
      'title' => 'Delete Annotation Setting',
      //'page callback' => 'drupal_get_form',
      'page arguments' => array('annotation_api_admin_delete_form', 3),
      'access arguments' => array('administer annotations'),
    ),
  );
  return $items;
}

/**
 * Check if the setting fits to the current call.
 *
 * This function calls the fit function that can be defined in each module. This
 * will decide if the current page view or other display is meant to work with
 * this annotation setting.
 *
 * @param $set
 *   the setting that shall be checked
 * @return (bool)
 *   returns TRUE, if it fits, FALSE if not so
 */
function _annotation_api_setting_fit($annotation){
  $infos = annotation_api_info();
  // Check each fit function of the setting.
  foreach (array('scope', 'annotation', 'content') as $type) {
    $func = $info[$type][$set[$type]];
    if ($func && !$func($set)) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Returns all available forms to ad annotations to the current form.
 *
 * @params $scope_type
 *   the tye of scope to get forms for
 * @param $scope_id
 *   the scope ID
 * @return
 *   an array of rendered forms
 */
function annotation_api_get_add_forms($scope_type, $scope_id) {
  $sets = variable_get('annotation_api', array());
  $forms = array();
  $annotations = annotation_api_get_annotations($scope_type, $scope_id, TRUE);
  foreach ($annotations as $id => $annotation) {
    $forms[$id] = drupal_get_form('annotation_form', $annotation);
  }
  return $forms;
}

/**
 * Retrieve valid annotations for the current scope.
 *
 * If new is set to TRUE, the function will return the default object for each
 * valid setting.
 * @param $scope_type
 * @param $scope_id
 * @param $new
 * @return
 *   array of annotation objects
 */
function annotation_api_get_annotations($scope_type, $scope_id, $new = FALSE) {
  $sets = variable_get('annotation_api', array());
  $annotations = array();
  if ($new) {
    foreach ($sets as $id => $set) {
      if ($set['scope'] != $scope_type) continue;
      $annotation = (object) array(
        'scope_type' => $scope_type,
        'scope_id' => $scope_id,
        'is_new' => TRUE,
        'default_built' => FALSE,
        'set_id' => $id,
        'settings' => $set,
      );
      if (!_annotation_api_setting_fit($annotation)) continue;
      // Build default
      annotation_api_build_default($annotation);
      $annotations[$id] = $annotation;
    }
  }
  else {
    $annotations = annotation_load(array('scope_type' => $scope_type, 'scope_id' => $scope_id));
    foreach ($annotations as $annotation) {
      //...
    }
  }
}


/**
 * Get an annotation form with default values.
 */
function annotation_api_build_default(&$annotation) {
  // Only build non-built objects
  if (isset($annotation->default_built) && $annotation->default_built) return FALSE;

  // If no types are existant, setting will set the type
  if ($annotation->settings) {
    if (!$annotation->scope_type) {
      $annotation->scope_type = $annotation->settings['scope'];
    }
    if (!$annotation->type) {
      $annotation->type = $annotation->settings['annotation'];
    }
    if (!$annotation->content_type) {
      $annotation->content_type = $annotation->settings['content'];
    }
  }

  // Scope ID should get set by scope module on call
  $handlers = array('scope', 'annotation', 'content');

  // Call default object handler for each component.
  foreach ($handlers as $handler) {
    $func = annotation_api_info($handler, $annotation->settings[$handler], 'default object');
    if ($func) {
      $func($annotation);
    }
  }
  // Set default_built indicator to true.
  $annotation->default_built = TRUE;
  return $annotation;
}
