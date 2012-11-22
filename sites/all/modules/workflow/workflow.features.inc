<?php
// $Id$

/**
 * Workflows are a **faux-exportable** component.
 */

/**
 * Implements hook_features_export().
 */
function workflow_features_export($data, &$export, $module_name = '') {
  // fontyourface_default_fonts integration is provided by Features.
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['workflow'] = 'workflow';
  foreach (workflow_get_workflows() as $workflow) {
    if (in_array($workflow->name, $data)) {
      $export['features']['workflow'][$workflow->name] = $workflow->name;
    }
  }
  return $export;
}

/**
 * Implements hook_features_export_render().
 */
function workflow_features_export_render($module, $data) {
  $code = array();
  $workflows = workflow_get_workflows();
  foreach ($data as $name) {
    foreach ($workflows as $workflow) {
      if ($workflow->name == $name) {
        if ($this_workflow = workflow_get_workflows_full_object($workflow->name)) {
          unset($this_workflow->wid);
          $code[$name] = $this_workflow;
        }
      }
    }
  }
  $code = "  return " . features_var_export($code, '  ') . ";";
  return array('workflow_features_default_workflow' => $code);
}

/**
 * Implements hook_features_export_options().
 */
function workflow_features_export_options() {
  $workflows = array();
  foreach (workflow_get_workflows() as $workflow) {
    $workflows[$workflow->name] = $workflow->name;
  }
  return $workflows;
}

/**
 * Implements hook_features_revert().
 */
function workflow_features_revert($module) {
  $saved_workflows = module_invoke($module, 'workflow_features_default_workflow');
  foreach ($saved_workflows as $key => $workflow) {
    $saved = workflow_update_workflows_full_object($workflow);
  }
}

/**
 * Implements hook_features_export_rebuild().
 */
function workflow_features_export_rebuild($module) {
  workflow_features_revert($module);
}

/**
 * CRUD style functions below.
 */

/**
 * For use by CRUD only, save everything from the CRUD formed object.
 */
function workflow_update_workflows_full_object($workflow) {
  $workflow = (object) $workflow;
  // Given a workflow in the format returned from export.
  // First we grab the states, transitions and node_maps out.
  $states = array();
  if (isset($workflow->states)) {
    $states = $workflow->states;
    unset($workflow->states);
  }
  $transitions = array();
  if (isset($workflow->transitions)) {
    $transitions = $workflow->transitions;
    unset($workflow->transitions);
  }
  $node_types = array();
  if (isset($workflow->node_types)) {
    $node_types = $workflow->node_types;
    unset($workflow->node_types);
  }
  // Then make a workflow so we can track by wid.
  if ($orig_workflow = workflow_get_workflows_full_object($workflow->name)) {
    $workflow->wid = $orig_workflow->wid;
  }
  workflow_update_workflows($workflow, FALSE);
  if (!isset($workflow->wid) || empty($workflow->wid)) {
    return FALSE;
  }
  // Workflow is now a fully vetted workflow object. We have NOT created a creation state with this.
  // Then make states, marking state name to state sid.
  $active_states = array();
  foreach ($states as $state) {
    $state = (object) $state;
    $state->wid = $workflow->wid;
    if ($orig_state = workflow_get_workflow_states_by_wid_state($state->wid, $state->state)) {
      $state->sid = $orig_state->sid;
    }
    workflow_update_workflow_states($state);
    $active_states[$state->state] = $state->sid;
  }
  // Delete any states *not* in our original construction.
  foreach (workflow_get_workflow_states_by_wid($workflow->wid) as $state) {
    if(!in_array($state->sid, $active_states)) {
      workflow_delete_workflow_states_by_sid($state->sid);
    }
  }
  // Then make transitions with the state mapping.
  foreach ($transitions as $transition) {
    $transition = (object) $transition;
    $transition->sid = $active_states[$transition->sid];
    $transition->target_sid = $active_states[$transition->target_sid];
    // NOTE: There is an assumption here with roles, that the rids are the same between installs.
    // This is rather unavoidable at current configs. SO: either roles need to be exported too, or
    // The installers should check role states with additional code.
    // workflow_update_workflow_transitions checks preexisting transitions by sid, target_sid.
    workflow_update_workflow_transitions($transition);
  }
  // Then add the node_type mapping.
  foreach ($node_types as $node_type) {
    $node_type = (object) $node_type;
    $node_type->wid = $workflow->wid;
    // Insert, nodes only have one workflow. Insert will delete any prior workflow assoc.
    workflow_insert_workflow_type_map($node_type);
  }
  return TRUE;
}

/**
 * For use by CRUD only, gather everything into the CRUD formed object.
 */
function workflow_get_workflows_full_object($name) {
  if ($workflow = workflow_get_workflows_by_name($name)) {
    // Now we need to add data to the object for each state, an array of sub-objects.
    $options = array('status' => 1); // We only want active states for this export.
    $active_states = array();
    foreach (workflow_get_workflow_states_by_wid($workflow->wid, $options) as $index => $state) {
      $active_states[$state->sid] = $state->state;
      // Remove what we don't need to export.
      unset($state->sid);
      unset($state->wid);
      $workflow->states[] = $state;
    }
    // Now we need to add data to the export for each transition, an array of sub-objects.
    // Same goes for transitions, see above re: states.
    foreach ($active_states as $sid => $state) {
      // We're going to look everythign up by the start state, not state involved, to avoid dupes.
      foreach (workflow_get_workflow_transitions_by_sid($sid, $options) as $transition) {
        // And to get the target state (by name) we need to look it up too.
        $target_state = workflow_get_workflow_states_by_sid($transition->target_sid);
        $transition->sid = $state;
        $transition->target_sid = $target_state->state;
        // Remove what we don't need to export.
        unset($transition->tid);
        $workflow->transitions[] = $transition;
      }
    }
    // Now we need to add data to the export for each type map, an array of sub-objects.
    // Same goes for node mappings, see above re: states.
    foreach (workflow_get_workflow_type_map_by_wid($workflow->wid) as $index => $type_map) {
      $workflow->node_types[] = $type_map;
    }
  }
  return $workflow;
}