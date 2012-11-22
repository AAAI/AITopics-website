<?php

/**
 * @file
 * Interface between finder.module and views.module.
 */

/**
 * Implements hook_views_plugins().
 */
function finder_views_plugins() {
  $plugins = array(
    'module' => 'finder', // This just tells our themes are elsewhere.
    'display' => array(
      'finder' => array(
        'path' => drupal_get_path('module', 'finder') . '/views',
        'title' => 'Finder',
        'help' => 'Destination-agnostic display. Mostly useful for programmatic views.',
        'handler' => 'finder_plugin_display',
        'no ui' => TRUE, // Programmatic use only.
        'uses hook menu' => FALSE,
        'use ajax' => FALSE,
        'use pager' => TRUE,
        'accept attachments' => FALSE,
      ),
    ),
    'style' => array(
      'finder' => array(
        'path' => drupal_get_path('module', 'finder') . '/views',
        'title' => 'Results array (with title)',
        'help' => 'Returns the view as a PHP array.',
        'handler' => 'finder_plugin_style',
        'no ui' => TRUE, // Programmatic use only.
        'uses row plugin' => TRUE,
        'uses fields' => TRUE,
        'type' => 'normal',
        'even empty' => TRUE,
      ),
    ),
  );
  return $plugins;
}

/**
 * Implements hook_views_data().
 */
function finder_views_data() {
  $data = array();
  if (module_exists("taxonomy")) {
    $vocabs = taxonomy_get_vocabularies();
    foreach ($vocabs as $vid => $vocab) {
      $data['taxonomy_term_data_'. $vid]['table']['group']  = t('Taxonomy');
      $data['taxonomy_term_data_'. $vid]['table']['join'] = array(
        'node' => array(
          'left_table' => 'taxonomy_index',
          'left_field' => 'tid',
          'field' => 'tid',
          'table' => 'taxonomy_term_data',
          'extra' => array(
            'vid' => array(
              'field' => 'vid',
              'value' => $vid,
              'numeric' => TRUE,
            ),
          ),
        ),
        'node_revision' => array(
          'left_table' => 'taxonomy_index',
          'left_field' => 'tid',
          'field' => 'tid',
          'table' => 'taxonomy_term_data',
          'extra' => array(
            'vid' => array(
              'field' => 'vid',
              'value' => $vid,
              'numeric' => TRUE,
            ),
          ),
        ),
      );
      // Term name field.
      $data['taxonomy_term_data_'. $vid]['name'] = array(
        'title' => t('Terms for !vocab', array('!vocab' => $vocab->name)),
        'help' => t('Taxonomy terms for !vocab.  Provided by the <em>Finder</em> module.', array('!vocab' => $vocab->name)),
        'field' => array(
          'handler' => 'views_handler_field_taxonomy',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string',
          'help' => t('Taxonomy term name.'),
        ),
      );
    }
  }
  return $data;
}

