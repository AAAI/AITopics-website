<?php
// $Id$
/**
 * @file
 * Provide views field handler for workflow.module.
 */

/**
 * Field handler to provide simple renderer that allows linking to a node.
 */
class workflow_views_handler_field_sid extends views_handler_field {
  function render($values) {
    if (empty($values->{$this->field_alias})) {
      return t('No state');
    }
    static $states;
    if (!isset($states)) {
      foreach (workflow_get_workflow_states() as $state) {
        $states[$state->sid] = check_plain($state->state);
      }
    }
    $output = $states[$values->{$this->field_alias}];
    if (empty($output)) {
      $output = t('Unknown state');
    }
    return check_plain($output);
  }
}
