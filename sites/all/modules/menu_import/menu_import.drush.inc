<?php

/**
 * @file
 *   Menu import module drush integration.
 */

/**
 * Implementation of hook_drush_command().
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function menu_import_drush_command() {
  $items = array();

  $items['menu-import'] = array(
    'description' => 'Imports a menu from a text file.',
    'callback' => 'drush_menu_import_import',
    'drupal dependencies' => array('menu_import'),
    'examples' => array(
      'drush menu-import menu_file.txt main_menu' => 'Import menu_file.txt into menu called main_menu using no specific options.',
      'drush menu-import menu_file.txt main_menu --create-content --link-content --clean-import'
        => 'Import menu_file.txt into menu called main_menu, creates empty content, tries to link to existsing and deletes all menu items before importing new ones.',
    ),
    'arguments' => array(
      'file' => 'Required. Path to the menu text file.',
      'menu-name' => 'Required. Name of the menu to import in.',
    ),
    'options' => array(
      'create-content'=> 'Creates empty nodes referenced by respective menu items. Useful for stubbing-out a new site.'
        . ' Possible option value: type:page,author:1,status:1 where type is content type machine name,'
        . ' author is uid and status is publication status (0 - unpublished, 1 - published).',
      'link-content'  => 'Tries to link menu items to existing content/pages.',
      'clean-import'  => 'Removes old menu items. Use carefully!',
    ),
    'aliases' => array('mi-import', 'mi'),
  );

  $items['menu-export'] = array(
    'description' => 'Exports menu to a text file.',
    'callback' => 'drush_menu_import_export',
    'drupal dependencies' => array('menu_import'),
    'examples' => array(
      'drush menu-export menu_file.txt main_menu' => 'Export menu called main_menu to a text file menu_file.txt.',
    ),
    'arguments' => array(
      'file' => 'Required. Path to the resulting menu text file.',
      'menu-name' => 'Required. Name of the menu to export.',
    ),
    'options' => array(
      'line-ending' => 'Line endings to be used. Allowed values are: unix, mac, dos.'
       . ' Defaults to current system\'s line ending.'
    ),
    'aliases' => array('mi-export', 'me'),
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 */
function menu_import_drush_help($section) {
  switch ($section) {
    case 'drush:menu-import':
      return dt('Imports a menu from a text file.');
    case 'drush:menu-export':
      return dt('Exports menu to a text file.');
  }
}

function drush_menu_import_import() {
  $args = func_get_args();

  if (count($args) != 2) {
    return drush_set_error('', dt('Two arguments required - file and menu name.'));
  }

  $file = $args[0];
  $menu_name = $args[1];

  // Validate file path.
  if (!file_exists($file) || !is_readable($file)) {
    return drush_set_error('', dt('File "!file" doesn\'t exist or is not readable.', array('!file' => $file)));
  }

  // Validate menu name.
  $menu_exists = db_select('menu_custom', 'mc')->fields('mc', array('menu_name'))
    ->condition('menu_name', $menu_name)->execute()->fetchColumn();

  if (!$menu_exists) {
    return drush_set_error('', dt('Menu "!menu" doesn\'t exist.', array('!menu' => $menu_name)));
  }

  // Prepare options and import menu.
  $options = array(
    'create_content'  => FALSE,
    'link_to_content' => FALSE,
    'remove_menu_items' => FALSE
  );

  $link_content = drush_get_option_list('link-content');
  if (count($link_content)) {
    $options['link_to_content'] = 1;
  }

  $clean_import = drush_get_option_list('clean-import');
  if (count($clean_import)) {
    $options['remove_menu_items'] = 1;
  }

  $create_content = drush_get_option_list('create-content');
  if (count($create_content)) {
    foreach ($create_content as $option) {
      if (!is_numeric($option)) {
        list($opt, $val) = explode(':', $option);
        switch ($opt) {
          case 'type':
            $options['node_type'] = $val;
            break;
          case 'body':
            $options['node_body'] = check_plain($val);
            break;
          case 'author':
            if (!user_load($val)) {
              return drush_set_error('', dt('"author" must be existing user ID'));
            }
            $options['node_author'] = $val;
            break;
          case 'status':
            $options['node_status'] = (int)$val;
            break;
          default:
            return drush_set_error('', dt('Unknown option "!opt"', array('!opt' => $opt)));
        }
      }
    }
  }

  module_load_include('inc', 'menu_import', 'includes/import');
  $result = menu_import_file($file, $menu_name, $options);

  if (!empty($result['errors'])) {
    $rows = array(array(dt('Import failed:')));
    foreach ($result['errors'] as $error) {
      $rows[] = array($error);
    }
  }
  else {
    $rows = array(array(dt('--- Import results ---')));
    if (!empty($result['warnings'])) {
      foreach ($result['warnings'] as $warn) {
        $rows[] = array($warn);
      }
      unset($result['warnings']);
    }

    $msgs = menu_import_get_messages();
    $total_items = $result['new_nodes'] + $result['matched_nodes'] + $result['external_links'] + $result['unknown_links'];

    $rows[] = array(dt($msgs['items_imported'], array('@count' => $total_items)));
    foreach ($result as $type => $value) {
      $rows[] = array(dt($msgs[$type], array('@count' => $value)));
    }
  }

  drush_print_table($rows);
}

function drush_menu_import_export() {
  $args = func_get_args();

  if (count($args) != 2) {
    return drush_set_error('', dt('Two arguments required - file and menu name.'));
  }

  $file = $args[0];
  $menu_name = $args[1];

  // Validate file path.
  if (file_exists($file)) {
    return drush_set_error('', dt('File "!file" already exists or cannot be created.', array('!file' => $file)));
  }

  // Validate menu name.
  $menu_exists = db_select('menu_custom', 'mc')->fields('mc', array('menu_name'))
    ->condition('menu_name', $menu_name)->execute()->fetchColumn();

  if (!$menu_exists) {
    return drush_set_error('', dt('Menu "!menu" doesn\'t exist.', array('!menu' => $menu_name)));
  }

  // Prepare options and import menu.
  $allowed_les = array('mac', 'unix', 'dos');
  $options = array(
    'line_ending'  => PHP_EOL,
  );

  $line_ending = drush_get_option_list('line-ending');
  if (count($line_ending) && in_array(current($line_ending), $allowed_les)) {
    switch (current($line_ending)) {
      case 'unix':
        $options['line_ending'] = "\n";
        break;
      case 'mac':
        $options['line_ending'] = "\r";
        break;
      case 'dos':
        $options['line_ending'] = "\r\n";
        break;
    }
  }

  module_load_include('inc', 'menu_import', 'includes/export');
  $result = menu_import_export_menu($menu_name, $options);

  if (!empty($result['errors'])) {
    $rows = array(array(dt('Export failed:')));
    foreach ($result['errors'] as $error) {
      $rows[] = array($error);
    }
  }
  else {
    $write_ok = @file_put_contents($file, $result['menu']);

    if ($write_ok) {
      $rows[] = array(dt('Menu "!menu" has been exported to "!file".',
        array('!menu' => $menu_name, '!file' => $file)));
    }
    else {
      $rows[] = array(dt('Couldn\'t write to "!file".', array('!file' => $file)));
    }
  }

  drush_print_table($rows);
}
