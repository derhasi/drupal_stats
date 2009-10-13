<?php
// $Id$

/**
 * @file
 * Contains the annotated content filter handler.
 */

/**
 * Handler to filter for content that has not been annotated.
 *
 * copied from flag.module
 *
 * @ingroup views
 */
class annotation_api_handler_filter_annotated extends views_handler_filter_boolean_operator {
  function options(&$options) {
    parent::options($options);
    $options['value'] = 1;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['value']['#type'] = 'radios';
    $form['value']['#title'] = t('Status');
    $form['value']['#options'] = array(1 => t('Annotated'), 0 => t('Not annotated'));
    $form['value']['#default_value'] = empty($this->options['value']) ? 0 : $this->options['value'];
    $form['value']['#description'] = '<p>' . t('This filter is only needed if the relationship used has the "Include only flagged content" option <strong>unchecked</strong>. Otherwise, this filter is useless, because all records are already limited to flagged content.') . '</p><p>' . t('By choosing <em>Not flagged</em>, it is possible to create a list of content <a href="@unflagged-url">that is specifically not flagged</a>.', array('@unflagged-url' => 'http://drupal.org/node/299335')) . '</p>';
  }

  function query() {
    $operator = $this->value ? 'IS NOT' : 'IS';
    $this->query->add_where($this->options['group'], $this->relationship .'.uid '. $operator .' NULL');
  }
}
