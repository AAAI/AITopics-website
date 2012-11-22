<?php

/**
 * @file
 * Import functions for menu_import module.
 */

/**
 * Helper function to find node associated to path.
 *
 * @param $path
 *   Path (node/* or any other) as described in the input file.
 * @param $title
 *   Node's title as described in the input file.
 * @param $options
 *   Array import options provided.
 *
 * @return
 *   An array('nid' => '<node_id_or_false>', 'link_path' => '<path_if_exists_or_empty>').
 */
function _menu_import_lookup_path($path, $title, array $options) {
  $result = array(
    'nid' => FALSE,
    'link_path' => ''
  );

  // Search by alias by default, but use only
  // path without query/argument part.
  $query = '';
  $path = explode('?', $path);
  if (count($path) != 1) {
    $query = '?' . $path[1];
  }
  $path = $path[0];

  // Does an alias exist in the system?
  $system_url = drupal_lookup_path('source', $path);
  if (!$system_url) {
    $system_url = $path;
  }

  // Try to detect a direct path to node.
  $matches = array();
  if (preg_match('/^node\/(\d+)/', $system_url, $matches)) {
    // The node exists and we can safely link to it.
    if (drupal_valid_path($system_url)) {
      if ($options['link_to_content']) {
        $result['link_path'] = $system_url . $query;
        $result['nid'] = $matches[1];
      }
    }
    // The node doesn't exist and we have to find it by title.
    elseif ($options['link_to_content']) {
      $nid = db_select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('n.title', $title)
        ->execute()->fetchField();

      // The node exists.
      if ($nid) {
        $result['link_path'] = 'node/' . $nid . $query;
        $result['nid'] = $nid;
      }
    }
  }
  // Is there any registered path?
  elseif (drupal_valid_path($system_url)) {
    $result['link_path'] = $system_url . $query;
  }

  return $result;
}

/**
 * Parse a line of text containing the menu structure.
 *
 * @param $line
 *   One line from input file.
 * @param $prev_level
 *   Previous level to build ierarchy.
 * @param $weights
 *   Array of menu items' weights.
 * @param $parents
 *   Array of menu items' parents.
 * @param $options
 *   Array of importing options.
 *
 * @return
 *   Array representing a menu item.
 */
function menu_import_parse_line($line, $prev_level, array $weights, array $parents, array $options) {
  $menuitem = array(
    'error' => FALSE,
    'link_title' => NULL,
    'children' => array(),
    'parent' => NULL,
    'nid' => FALSE,
    'path' => FALSE,
    'weight' => 0,
    'external' => FALSE,
    'level' => 0,
  );

  // Only * and - are allowed as indentation characters.
  // URL aliases are separated from title by vertical bar (|) or semi-colon (;).
  $opt_re = '([\|;]{1}[^\|;]*)?';
  $matches = array();
  preg_match('/^([\-]+|[\*]+)?(\s+)?([^\|;]*)' . str_repeat($opt_re, 3) . '/', $line, $matches);

  $level = strlen($matches[1]); // No sense to use drupal_strlen on indentation.
  $title = trim(strip_tags($matches[3]));
  $path  = isset($matches[4]) ? trim(str_replace(array('|', ';'), '', $matches[4])) : '';
  $description = isset($matches[5]) ? trim(str_replace(array('|', ';'), '', $matches[5])) : '';
  $expanded = !empty($matches[6]) && stripos($matches[6], 'expanded') !== FALSE;
  $disabled = !empty($matches[6]) && stripos($matches[6], 'disabled') !== FALSE;

  // Skip empty items
  if (!strlen($title)) {
    $menuitem['error'] = t('missing menu title');
    return $menuitem;
  }

  // Make sure this item is only 1 level below the last item.
  if ($level > $prev_level + 1) {
    $menuitem['error'] = t('wrong indentation');
    return $menuitem;
  }

  if (isset($weights[$level])) {
    if ($level > $prev_level) {
      $weight = 0;
    }
    else {
      $weight = $weights[$level] + 1;
    }
  }
  else {
    $weight = 0;
  }
  $menuitem['weight'] = $weight;
  $menuitem['parent'] = !$level ? 0 : $parents[$level - 1];
  $menuitem['link_title'] = $title;
  $menuitem['level'] = $level;
  $menuitem['path'] = $path;

  if (url_is_external($path)) {
    $menuitem['external'] = TRUE;
    $menuitem['link_path'] = $path;
  }
  else {
    $result = _menu_import_lookup_path($path, $title, $options);
    $menuitem['link_path'] = $result['link_path'];
    $menuitem['nid'] = $result['nid'];
  }

  if ($description) {
    $menuitem['description'] = $description;
  }

  if ($disabled) {
    $menuitem['hidden'] = 1;
  }

  if ($expanded) {
    $menuitem['expanded'] = 1;
  }

  return $menuitem;
}

/**
 * File parser function. Reads through the text file and constructs the menu.
 *
 * @param $uri
 *   uri of the uploaded file.
 * @param $menu_name
 *   internal name of existiong menu.
 * @param $options
 *   An associative array of search options.
 *   - search_title: search node by title.
 *
 * @return array
 *   array structure of menu.
 */
function menu_import_parse_menu_from_file($uri, $menu_name, array $options) {
  $menu = array(
    'errors'    => array(),
    'warnings'  => array(),
    0 => array(
      'menu_name' => $menu_name,
      'children' => array(),
    )
  );

  // Keep track of actual weights per level.
  // We have to append to existing items not to mess up the menu.
  $weights = array(0 => menu_import_get_max_weight($menu_name));
  // Keep track of actual parents per level.
  $parents = array();

  $handle = fopen($uri, "r");
  if (!$handle) {
    $menu['errors'][] = t("Couldn't open the uploaded file for reading.");
    return $menu;
  }

  $level = $current_line = $empty_lines = 0;
  while ($line = fgets($handle)) {
    $current_line++;

    // Skip empty lines.
    if (preg_match('/^\s*$/', $line)) {
      $empty_lines++;
    }
    else {
      $menuitem = menu_import_parse_line($line, $level, $weights, $parents, $options);
      if ($menuitem['error']) {
        $menu['errors'][] = t('Error on line @line_number: @error.', array('@line_number' => $current_line, '@error' => $menuitem['error']));
      }
      $menu[$current_line] = $menuitem;
      $menu[$menuitem['parent']]['children'][] = $current_line;

      $level = $menuitem['level'];
      $parents[$level] = $current_line;
      $weights[$level] = $menuitem['weight'];
    }
  }

  if ($empty_lines) {
    $menu['warnings'][] = t('Empty lines skipped: @line_number.', array('@line_number' => $empty_lines));
  }

  fclose($handle);

  return $menu;
}

/**
 * Text parser function. Reads through the text and constructs the menu.
 *
 * @param $text
 *   Text containing the menu structure.
 *
 * @see menu_import_parse_menu_from_file()
 */
function menu_import_parse_menu_from_string($text, $menu_name, array $options) {
  $menu = array(
    'errors' => array(),
    'warnings'  => array(),
    0 => array(
      'menu_name' => $menu_name,
      'children' => array(),
    )
  );

  // Keep track of actual weights per level.
  $weights = array();
  // Keep track of actual parents per level.
  $parents = array();

  $level = $current_line = $empty_lines = 0;
  $lines = explode("\n", $text);
  foreach ($lines as $line) {
    $current_line++;

    // Skip empty lines.
    if (preg_match('/^\s*$/', $line)) {
      $empty_lines++;
    }
    else {
      $menuitem = menu_import_parse_line($line, $level, $weights, $parents, $options);
      if ($menuitem['error']) {
        $menu['errors'][] = t('Error on line @line_number: @error.', array('@line_number' => $current_line, '@error' => $menuitem['error']));
      }
      $menu[$current_line] = $menuitem;
      $menu[$menuitem['parent']]['children'][] = $current_line;

      $level = $menuitem['level'];
      $parents[$level] = $current_line;
      $weights[$level] = $menuitem['weight'];
    }
  }

  if ($empty_lines) {
    $menu['warnings'][] = t('Empty lines skipped: @line_number.', array('@line_number' => $empty_lines));
  }

  return $menu;
}

/**
 * Import menu items.
 *
 * @param $menu
 *   An associative array containing the menu structure.
 * @param $options
 *   An associative array of import options.
 *   - link_to_content: look for existing nodes and link to them
 *   - create_content: create new content (also if link_to_content not set)
 *   - remove_menu_items: removes current menu items
 *   - node_type: node type
 *   - node_body: node body
 *   - node_author: node author
 *   - node_status: node status
 *
 * @return
 *   Array of different statistics accumulated during the import.
 */
function menu_import_save_menu($menu, $options) {
  $nodes_deleted_cnt = $unknown_links_cnt = $external_links_cnt = 0;
  $nodes_matched_cnt = $nodes_new_cnt = $failed_cnt = 0;

  // Delete existing menu items.
  $menu_name = $menu[0]['menu_name'];
  if ($options['remove_menu_items']) {
    $nodes_deleted_cnt = menu_import_delete_menuitems($menu_name);
  }

  $menu[0]['mlid'] = 0;

  foreach ($menu as $item) {
    if (!isset($item['children'])) {
      continue;
    }

    foreach ($item['children'] as $index) {
      $menuitem = $menu[$index];
      $menuitem['plid'] = $menu[$menuitem['parent']]['mlid'];
      $menuitem['menu_name'] = $menu_name;

      // Determine if this is the path to view a node (no edit or whatever).
      $link_path = $menuitem['link_path'];
      if ($link_path && arg(0, $link_path) == 'node'
            && is_numeric(arg(1, $link_path)) && !arg(2, $link_path)) {
        $node_path = true;
      }
      else {
        $node_path = false;
      }

      // Do not create nodes for external links.
      if ($menuitem['external']) {
        $external_links_cnt++;
      }
      // Internal link to not-a-content (settings, custom module path etc).
      elseif ($link_path && !$node_path) {
        $unknown_links_cnt++;
      }
      // Handle links to nodes or missing links.
      else {
        // Node exists.
        if ($menuitem['nid']) {
          // Need to create new content since no linking was selected.
          if ($options['create_content']) {
            menu_import_delete_menuitem($menuitem);
            $options['node_title'] = $menuitem['link_title'];
            $nid = menu_import_create_node($options);
            $menuitem['nid'] = $nid;
            $nodes_new_cnt++;
            $menuitem['link_path'] = 'node/' . $menuitem['nid'];
          }
          elseif ($options['link_to_content']) {
            menu_import_delete_menuitem($menuitem);
            $nodes_matched_cnt++;
          }
        }
        // Node doesn't exist.
        else {
          // Create new link and node.
          if ($options['create_content']) {
            menu_import_delete_menuitem($menuitem);
            $options['node_title'] = $menuitem['link_title'];
            if ($options['node_alias'] && !empty($menuitem['path'])
                && arg($menuitem['path'], 0) != 'node') {
              $options['path_alias'] = $menuitem['path'];
            }
            $nid = menu_import_create_node($options);
            $menuitem['nid'] = $nid;
            $nodes_new_cnt++;
            $menuitem['link_path'] = 'node/' . $menuitem['nid'];
          }
          // Recreate menu item.
          else {
            menu_import_delete_menuitem($menuitem);
            $unknown_links_cnt++;
            $menuitem['link_path'] = '<front>';
          }
        }
      }

      // Ensure we link to at least front page.
      if (empty($menuitem['link_path'])) {
        $menuitem['link_path'] = '<front>';
      }

      // Save description if available.
      if (isset($menuitem['description'])) {
        $menuitem['options']['attributes']['title'] = $menuitem['description'];
      }

      // Save menuitem and set mlid.
      $mlid = menu_link_save($menuitem);
      if (!$mlid) {
        $failed_cnt++;
      }
      $menu[$index]['mlid'] = $mlid;
    }
  }

  return array(
    'external_links' => $external_links_cnt,
    'unknown_links' => $unknown_links_cnt,
    'matched_nodes' => $nodes_matched_cnt,
    'new_nodes' => $nodes_new_cnt,
    'deleted_menu_items' => $nodes_deleted_cnt,
    'failed' => $failed_cnt,
  );
}

/**
 * Create new node of given content type.
 *
 * @param $options
 *   Array relevant array keys are:
 *   - node_title
 *   - node_type
 *   - node_body
 *   - node_author
 *   - node_status
 *
 * @return
 *   Node's nid field.
 */
function menu_import_create_node($options) {
  $node = new stdClass();

  $node->type     = $options['node_type'];
  $node->language = LANGUAGE_NONE;
  $node->title    = $options['node_title'];

  $node->body[$node->language][0]['value']   = $options['node_body'];
  $node->body[$node->language][0]['summary'] = text_summary($options['node_body']);
  $node->body[$node->language][0]['format']  = 'filtered_html';

  $node->status = $options['node_status'];
  $node->uid    = $options['node_author'];

  if (!empty($options['path_alias'])) {
    $node->path = array('alias' => $options['path_alias']);
    // Make sure pathauto is not being used
    if (module_exists('pathauto')) {
      $node->path['pathauto'] = FALSE;
    }
  }

  node_save($node);
  return $node->nid;
}

/**
 * Delete nodes attached to menu and menu items.
 *
 * @param $menu_name
 *   The machine name of the menu.
 * @param $with_nodes
 *   Delete nodes as well. Not used, reserved for future.
 *
 * @return
 *   Number of items deleted .
 */
function menu_import_delete_menuitems($menu_name, $with_nodes = FALSE) {
  $menuitems = db_select('menu_links', 'ml')
    ->fields('ml', array('mlid', 'link_path'))
    ->condition('ml.menu_name', $menu_name)
    ->execute()->fetchAllAssoc('mlid', PDO::FETCH_ASSOC);

  $deleted_cnt = 0;
  foreach ($menuitems as $menuitem) {
    db_delete('menu_links')->condition('mlid', $menuitem['mlid'])->execute();
    if ($with_nodes) {
      // Delete nodes only
      $link = explode('/', $menuitem['link_path']);
      if ($link[0] == 'node' && is_numeric($link[1])) {
        $nid = $link[1];
        db_delete('node')->condition('nid', $nid)->execute();
        db_delete('node_revision')->condition('nid', $nid)->execute();
      }
    }
    $deleted_cnt++;
  }

  return $deleted_cnt;
}

/**
 * Delete menu item by nid.
 *
 * @param $menuitem
 *   Array describing the menu item.
 */
function menu_import_delete_menuitem($menuitem) {
  db_delete('menu_links')
    ->condition('menu_name', $menuitem['menu_name'])
    ->condition('link_path', 'node/' . $menuitem['nid'])
    ->execute();
}

/**
 * Returns the array of messages shown when import is done
 * with the same keys as menu_import_save_menu returns.
 * @return array
 */
function menu_import_get_messages() {
  return array(
    'items_imported'=> 'Items imported: @count.',
    'failed'        => 'Items failed: @count.',
    'new_nodes'     => 'New content created: @count items.',
    'matched_nodes' => 'Existing content matched: @count items.',
    'deleted_menu_items'  => 'Menu items deleted: @count.',
    'external_links'      => 'External URLs: @count items.',
    'unknown_links'       => 'Content not found: @count items.'
  );
}

/**
 * Get max weight for first level.
 */
function menu_import_get_max_weight($menu_name) {
  $weight = db_query("SELECT MAX(weight) FROM {menu_links} WHERE menu_name = :menu_name AND depth = 1",
    array(':menu_name' => $menu_name))->fetchField();
  if (empty($weight)) {
    $weight = 0;
  }
  return $weight;
}
