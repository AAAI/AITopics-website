<?php

/**
 * @file
 * Provide views data and handler information for Views URL alias (node) module.
 */

/**
 * Implements hook_views_data().
 */
function views_url_alias_node_views_data() {
  // Define table group
  $data['views_url_alias_node']['table']['group'] = t('Node');

  $data['views_url_alias_node']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('URL alias'),
    'help' => t('Mapping table contains node IDs and their associated path aliases.'),
    'weight' => -10,
  );

  $data['views_url_alias_node']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['views_url_alias_node']['alias'] = array(
    'title' => t('URL alias'),
    'help' => t('The alias for a node.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  return $data;
}
