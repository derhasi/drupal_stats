<?php
// $Id$

/**
 * @file
 *   Rules implementation of annotation api
 */

/**
 * Implementation of hook_rules_event_info().
 */
function annotation_api_rules_event_info() {

  $scopes = annotation_api_info('scope');
  $contents = annotation_api_info('content');

  $events = array();

  foreach ($scopes as $scope_type => $scope) {
    foreach ($contents as $content_type => $content) {
      $events["annotation_add:$scope_type:$content_type"] = array(
        // Create Annotation
        'label' => t('Annotation added - Scope: @scope - Content: @content', array('@scope' => $scope['title'], '@content' => $content['title'])),
        'module' => 'Annotation',
        'arguments' => array(
          'scope' => array('type' => $scope_type, 'label' => t('Scope of the annotation')),
          'content' => array('type' => $content_type, 'label' => t('Content of the annotation')),
          'user' => array('type' => 'user', 'label' => t('User who annotated')),
          'annotation' => array('type' => 'annotation', 'label' => t('Annotation object')),
        ),
      );

      // Delete Annotation
      $events["annotation_deleted:$scope_type:$content_type"] = array(
        'label' => t('Annotation deleted - Scope: @scope - Content: @content', array('@scope' => $scope['title'], '@content' => $content['title'])),
        'module' => 'Annotation',
        'arguments' => array(
          'scope' => array('type' => $scope_type, 'label' => t('Scope of the annotation')),
          'content' => array('type' => $content_type, 'label' => t('Content of the annotation')),
          'user' => array('type' => 'user', 'label' => t('User who annotated')),
          'annotation' => array('type' => 'annotation', 'label' => t('Annotation object')),
        ),
      );
    }
  }

  return $events;
}