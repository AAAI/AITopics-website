<?php
// $Id$
/**
 * @file
 * Provide user interface for changing workflow state.
 */

/**
 * Menu callback. Display workflow summary of a node.
 */
function workflow_tab_page($node = NULL) {
  drupal_set_title($node->title);
  $workflow = workflow_get_workflow_type_map_by_type($node->type);
  foreach (workflow_get_workflow_states() as $data) {
    $states[$data->sid] = check_plain(t($data->state));
  }
  $deleted_states = array();
  $options = array('status' => 0);
  foreach (workflow_get_workflow_states($options) as $data) {
    $deleted_states[$data->sid] = check_plain(t($data->state));
  }
  $current = workflow_node_current_state($node);
  // theme_workflow_current_state() must run state through check_plain().
  $current_state = theme('workflow_current_state', array('state_name' => $states[$current]));
  $output = t('Current state: !state', array('!state' => $current_state));
  $output .= drupal_render(drupal_get_form('workflow_tab_form', $node, $workflow->wid, $states, $current));
  $rows = array();
  foreach (workflow_get_workflow_node_history_by_nid($node->nid) as $history) {
    if ($history->sid == $current && !isset($deleted_states[$history->sid]) && !isset($current_themed)) {
      // Theme the current state differently so it stands out.
      $state_name = theme('workflow_current_state',  array('state_name' => $states[$history->sid]));
      // Make a note that we have themed the current state; other times in the history
      // of this node where the node was in this state do not need to be specially themed.
      $current_themed = TRUE;
    }
    elseif (isset($deleted_states[$history->sid])) {
      // The state has been deleted, but we include it in the history.
      $state_name = theme('workflow_deleted_state', array('deleted_state' => $deleted_states[$history->sid]));
      $footer_needed = TRUE;
    }
    else {
      // Regular state.
      $state_name = check_plain(t($states[$history->sid]));
    }
    if (isset($deleted_states[$history->old_sid])) {
      $old_state_name = theme('workflow_deleted_state', array('deleted_state' => $deleted_states[$history->old_sid]));
      $footer_needed = TRUE;
    }
    elseif (isset($states[$history->old_sid])) {
      $old_state_name = check_plain(t($states[$history->old_sid]));
    }
    else {
      $old_state_name = '*';
    }
    $rows[] = theme('workflow_history_table_row', array( 'history' => $history, 'old_state_name' => $old_state_name, 'state_name' => $state_name ));
  }
  $output .= theme('workflow_history_table', array('rows' => $rows, 'footer' => !empty($footer_needed)));
  $output .= theme('pager', array('tags' => variable_get('workflow_states_per_page', 20)));
  return $output;
}

/*
 * Theme one workflow history table row.
 *
 * $old_state_name and $state_name must be run through check_plain(t()) prior
 * to calling this theme function.
 */
function theme_workflow_history_table_row($variables) {
  $old_state_name = $variables['old_state_name'];
  $state_name = $variables['state_name'];
  $history = $variables['history'];
  return array(
    format_date($history->stamp),
    $old_state_name,
    $state_name,
    theme('username', array('account' => $history)),
    filter_xss($history->comment, array('a', 'em', 'strong')),
  );
}

/*
 * Theme entire workflow history table.
 */
function theme_workflow_history_table($variables) {
  $rows = $variables['rows'];
  $footer = $variables['footer'];
  $headers = array(t('Date'), t('Old State'), t('New State'), t('By'), t('Comment'));
  $output = theme('table', array('header' => $headers, 'rows' => $rows, 'caption' => t('Workflow History')));
  if ($footer) {
    $output .= t('*State is no longer available.');
  }
  return $output;
}

/**
 * Theme the current state in the workflow history table.
 */
function theme_workflow_current_state($variables) {
  return '<strong>' . check_plain(t($variables['state_name'])) . '</strong>';
}

/**
 * Theme a deleted state in the workflow history table.
 */
function theme_workflow_deleted_state($variables) {
  return check_plain(t($variables['state_name'])) . '*';
}

/**
 * Form builder. Allow workflow state change and scheduling from workflow tab.
 *
 * @param $node
 *   Node for which workflow information will be displayed.
 * @param $wid
 *   ID of workflow to display.
 * @param $states
 *   Array of states for the workflow.
 * @param $current
 *   Current workflow state of this node.
 * @return
 *   Form definition array.
 */
function workflow_tab_form($form_id, $form_state, $node, $wid, $states, $current) {
  $form['#tab'] = TRUE;
  $choices = workflow_field_choices($node);
  $min = $states[$current] == t('(creation)') ? 1 : 2;
  // Only build form if user has possible target state(s).
  if (count($choices) >= $min) {
    $workflow = workflow_get_workflow_type_map_by_type($node->type);
    $workflow = workflow_get_workflows_by_wid($workflow->wid);
    $form['#wf'] = $workflow;
    $name = check_plain((t($workflow->name)));
    $timestamp = '';
    $comment = '';
    // See if scheduling information is present.
    if (!empty($node->workflow_scheduled_timestamp) && !empty($node->workflow_scheduled_sid)) {
      global $user;
      if (variable_get('configurable_timezones', 1) && $user->uid && drupal_strlen($user->timezone)) {
        $timezone = $user->timezone;
      }
      else {
        $timezone = variable_get('date_default_timezone', 0);
      }
      // The default value should be the upcoming sid.
      $current = $node->workflow_scheduled_sid;
      $timestamp = $node->workflow_scheduled_timestamp;
      $comment = $node->workflow_scheduled_comment;
    }
    // Include the same form elements here that are included on a
    // regular node editing page. $form is modified by reference.
    workflow_node_form($form, $form_state, t('Change @name state', array('@name' => $name)), $name, $current, $choices, $timestamp, $comment);
    $form['node'] = array(
      '#type' => 'value',
      '#value' => $node,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update workflow'),
    );
  }
  return $form;
}

/**
 * Submit handler for the form on the workflow tab.
 *
 * @see workflow_tab_form()
 */
function workflow_tab_form_submit($form, &$form_state) {
  // The entire node object was stashed in the form.
  $node = $form_state['values']['node'];
  if (isset($form_state['values']['workflow'])) {
    $node->workflow = $form_state['values']['workflow'];
    $node->workflow_comment = $form_state['values']['workflow_comment'];
    if (!empty($form_state['values']['workflow_scheduled'])) {
      $node->workflow_scheduled = $form_state['values']['workflow_scheduled'];
    }
    if (!empty($form_state['values']['workflow_scheduled_date'])) {
      $node->workflow_scheduled_date = $form_state['values']['workflow_scheduled_date'];
    }
    if (!empty($form_state['values']['workflow_scheduled_hour'])) {
      $node->workflow_scheduled_hour = $form_state['values']['workflow_scheduled_hour'];
    }
  }
  // Call node_save() to make sure any handlers that use the
  // new workflow values will see them.
  node_save($node);
  $form_state['redirect'] = 'node/' . $node->nid;
}