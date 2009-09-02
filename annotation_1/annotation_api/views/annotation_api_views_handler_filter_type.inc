<?php
// $Id$

/**
 * Filter by annotation type
 */
class annotation_api_views_handler_filter_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Annotation type');
      $types = annotation_api_info('annotation');
      foreach ($types as $type => $info) {
        $options[$type] = $info['title'];
      }
      $this->value_options = $options;
    }
  }
}