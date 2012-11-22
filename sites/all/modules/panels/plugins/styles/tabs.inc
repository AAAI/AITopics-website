<?php

/**
 * @file
 * Definition of the 'vertical tabs' panel style.
 */

// Plugin definition
$plugin = array(
  'title' => t('Tabs'),
  'description' => t('Presents the panes in a tabs fieldset.'),
  'render region' => 'panels_tabs_style_render_region',
  'settings form' => 'panels_tabs_style_settings_form',
  'settings validate' => 'panels_tabs_style_settings_validate',
);

/**
 * Render callback.
 *
 * @ingroup themeable
 */
function theme_panels_tabs_style_render_region($vars) {

  $display = $vars['display'];
  $region_id = $vars['region_id'];
  $panes = array_reverse($vars['panes'], TRUE);
  $settings = $vars['settings'];
  
  //Build items and fieldset
  $items = array();
  $owner = $vars['owner_id'];
  
  $build[$owner] = array(
    '#type' => 'vertical_tabs', 
    '#weight' => 99,
  );
  foreach ($panes as $pane_id => $item) {
    if (is_numeric($pane_id)) {
    
      // Get pane title. Use the "Override Title" if set, otherwise use the pane title if set.
      if (!empty($display->content[$pane_id]->configuration['override_title_text'])) {
        $title = $display->content[$pane_id]->configuration['override_title_text'];
      }
      elseif (!empty($display->content[$pane_id]->configuration['title'])) {
        $title = $display->content[$pane_id]->configuration['title'];
      }
      
      if (!empty($title)) {
        // Override title manually. Find title first, via context if available, or manually otherwise.
        if (!empty($display->content[$pane_id]->configuration['context'])) {
          $context_id = $display->content[$pane_id]->configuration['context'];
          $title = str_replace('%title', $display->context[$context_id]->title, $title); 
        }
        elseif (!empty($display->content[$pane_id]->configuration['title'])) {
          $title = str_replace('%title', $display->content[$pane_id]->configuration['title'], $title); 
        }
        elseif (!empty($display->content[$pane_id]->configuration['nid'])) {
          $node_title = db_query('SELECT title FROM {node} WHERE nid = :nid', array(
            ':nid' => $display->content[$pane_id]->configuration['nid']
            ))->fetchField();
          $title = str_replace('%title', $node_title, $title); 
        }
      }
                   
      $pane_class = "pane_" . $pane_id;
    
      $build[$pane_class] = array(
        '#type' => 'fieldset', 
        '#title' => (!empty($title) ? $title : ''), 
        '#collapsible' => TRUE, 
        '#collapsed' => TRUE,
        '#group' => $owner,
        '#attributes' => array(
          'class' => array($pane_class),
        ), 
        '#tree' => TRUE, 
        '#weight' => $display->content[$pane_id]->position,
      );
    
      $build[$pane_class]['items'] = array(
        '#markup' => $item,
      );
    }
  }

  return theme($settings['tabs_type'], array(
      'element' => array(
        '#children' => render($build),
      ),
  ));
}

/**
 * Settings form callback.
 */
function panels_tabs_style_settings_form($style_settings) {
  // Vertical tabs are in core, so use them.
  $options['vertical_tabs'] = t('Vertical');
  // field_group.module allows same structure, for horizontal tabs.
  if (module_exists('field_group')) $options['horizontal_tabs'] = t('Horizontal');
  
  $form['tabs_type'] = array(
    '#type' => 'select',
    '#title' => t('Tabs type'),
    '#options' => $options,
    '#default_value' => (isset($style_settings['tabs_type'])) ? $style_settings['tabs_type'] : 'vertical_tabs',
  );

  return $form;
}

