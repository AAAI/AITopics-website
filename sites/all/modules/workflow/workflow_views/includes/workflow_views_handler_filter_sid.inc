<?php
// $Id$
/**
 * @file
 * Provide views filter handler for workflow.module.
 */

/**
 * Filter by state.
 */
class workflow_views_handler_filter_sid extends views_handler_filter_in_operator {
  var $value_form_type = 'select';

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Workflow state');
      foreach (workflow_get_workflows() as $data) {
        $workflows[$data->wid] = check_plain(t($data->name));
      }
      $options = array('status' => 1);
      if (count($workflows) > 1) {
        $states = array('' => t('No state'));
        foreach ($workflows as $wid => $name) {
          foreach (workflow_get_workflow_states_by_wid($wid, $options) as $state) {
            $states[$name][$state->sid] = check_plain($state->state);
          }
        }
      }
      else {
        foreach (workflow_get_workflow_states($options) as $state) {
          $states[$state->sid] = check_plain($state->state);
        }
      }
      $this->value_options = $states;
    }
  }

  function query() {
    if (empty($this->value)) {
      return;
    }
    $this->ensure_my_table();
    $placeholder = !empty($this->definition['numeric']) ? '%d' : "'%s'";
    if (count($this->value) == 1) {
      $this->operator = ($this->operator == 'in') ? '= ' : '!= ';
      $in = !empty($this->definition['numeric']) ? '%d' : "'%s'";
    }
    else {
      $replace = array_fill(0, sizeof($this->value), $placeholder);
      $in = ' (' . implode(", ", $replace) . ')';
    }
    $this->query->add_where($this->options['group'], $this->table_alias . '.' . $this->real_field, $this->value,  $this->operator);
  }
}
