<?php

/**
 * @file
 * Processing functions for taxonomy_menu_trails.
 *
 * @author Dmitriy.trt      <http://drupal.org/user/329125>
 */

/**
 * Returns default taxonomy term path.
 * 
 * @param int $tid
 * 
 * @return string
 */
function _taxonomy_menu_trails_default_term_path($tid) {
  return 'taxonomy/term/' . $tid;
}

/**
 * Returns paths for specified term IDs.
 *
 * @param array $tids
 * @param object $entity
 * @param array $settings
 *
 * @return array
 *   Paths for passed tids.
 */
function _taxonomy_menu_trails_get_paths($tids, $entity, $settings) {
  $paths = array();
  switch ($settings['term_path']) {
    case 'custom':
      foreach ($tids as $tid) {
        $term_paths = str_replace('[tid]', $tid, $settings['term_path_patterns']);
        $paths = array_merge($paths, $term_paths);
      }
      break;

    case 'api':
      // We have to keep tids order for the final paths array.
      // First fill paths array with "tid => array()" map.
      $paths = array_combine($tids, array_fill(0, count($tids), array()));

      // Call hook implementations and fill paths array for each term.
      $hook = 'taxonomy_menu_trails_get_paths';
      foreach (module_implements($hook) as $module) {
        $function = $module . '_' . $hook;
        if (function_exists($function)) {
          // Use call_user_func() to force passing arguments by value, not by
          // reference.
          $results = call_user_func($function, $tids, $settings['entity_type'], $entity, $settings);
          if (empty($results) || !is_array($results)) {
            continue;
          }
          
          foreach ($results as $tid => $term_paths) {
            if (empty($term_paths)) {
              continue;
            }
            
            if (is_string($term_paths)) {
              $paths[$tid][] = $term_paths;
            }
            elseif (is_array($term_paths)) {
              $paths[$tid] = array_merge($paths[$tid], $term_paths);
            }
          }
        }
      }

      // For terms with empty mappings fallback to default path. For the rest
      // make sure paths are unique.
      foreach ($paths as $tid => $term_paths) {
        if (empty($term_paths)) {
          $paths[$tid][] = _taxonomy_menu_trails_default_term_path($tid);
        }
        else {
          $paths[$tid] = array_unique($paths[$tid]);
        }
      }

      // Merge paths into flat array. The order of tids will be kept.
      $paths = call_user_func_array('array_merge', $paths);
      break;

    case 'tm':
      if (module_exists('taxonomy_menu')) {
        // Unfortunately, there is no way to get tids-vids map without DB query.
        // If you know one, please create issue and describe method.
        $vids = db_select('taxonomy_term_data', 'ttd')
          ->fields('ttd', array('tid', 'vid'))
          ->condition('ttd.tid', $tids, 'IN')
          ->execute()
          ->fetchAllKeyed();
        foreach ($tids as $tid) {
          if (isset($vids[$tid])) {
            $paths[] = taxonomy_menu_create_path($vids[$tid], $tid);
          }
          else {
            $paths[] = _taxonomy_menu_trails_default_term_path($tid);
          }
        }
        break;
      }
      // Fallback to default path when Taxonomy Menu module not exists anymore.
      
    default:
      foreach ($tids as $tid) {
        $paths[] = _taxonomy_menu_trails_default_term_path($tid);
      }
  }
  return $paths;
}

/**
 * Set active trail to entitie's term selected by method specified in settings.
 *
 * @param object $entity
 * @param array $settings
 */
function _taxonomy_menu_trails_process($entity, $settings) {
  $instances = $settings['instances'];
  $tids = array();
  //TODO figure out: maybe we have to choose language in term reference fields
  foreach ($instances as $field) {
    foreach ($entity->$field as $lang => $terms) {
      foreach ($terms as $term) {
        $tids[] = $term['tid'];
      }
    }
  }

  // Filter out terms with different language if this option is enabled.
  if (module_exists('i18n_taxonomy') && $settings['terms_with_current_language']) {
    $terms = taxonomy_term_load_multiple($tids);
    $allowed_languages = array('und', $GLOBALS['language_content']->language);
    foreach($tids as $index => $tid) {
      if (empty($terms[$tid]) ||
          !in_array($terms[$tid]->language, $allowed_languages)) {
        unset($tids[$index]);
      }
    }
  }

  if ($settings['selection_method'] == 'last') {
    $tids = array_reverse($tids);
  }

  // Get paths for each tid.
  $paths = _taxonomy_menu_trails_get_paths($tids, $entity, $settings);

  // Retrieve the list of menu names sorted by preference. This list is used
  // by menu system to get order of menus and not as a filter, we do the same.
  // See menu_link_get_preferred().
  $menu_names = menu_get_active_menu_names();

  $query = db_select('menu_links', 'ml', array('fetch' => PDO::FETCH_ASSOC));
  $query->leftJoin('menu_router', 'm', 'm.path = ml.router_path');
  $query->fields('ml');
  // Weight must be taken from {menu_links}, not {menu_router}.
  $query->addField('ml', 'weight', 'link_weight');
  $query->fields('m');
  $query->condition('ml.link_path', $paths, 'IN');
  $query->condition('ml.hidden', '0');
  if ($settings['selection_method'] == 'deepest-in-menu') {
    $query->orderBy('ml.depth', 'DESC');
  }
  $query->orderBy('ml.weight', 'ASC');
  $results = $query->execute();

  $selected_item = FALSE;
  $vars = compact('menu_names', 'results', 'paths', 'selected_item');
  switch ($settings['selection_method']) {
    case 'first':
    case 'last':
      $selected_item = _taxonomy_menu_trails_fetch_link_simple($vars);
      break;
    case 'deepest-in-menu':
      $selected_item = _taxonomy_menu_trails_fetch_link_deepest($vars);
      break;
  }
  
  if (!$selected_item) {
    return;
  }

  // Set selected menu item as an active path for its menu.
  // Function menu_tree_set_path() available since Drupal 7.9, so we should
  // check it exists.
  if (function_exists('menu_tree_set_path')) {
    menu_tree_set_path($selected_item['menu_name'], $selected_item['link_path']);
  }
  
  // Save selected item and "set_breadcrumb" setting for
  // taxonomy_menu_trails_menu_breadcrumb_alter().
  if (!empty($settings['set_breadcrumb'])) {
    $selected_item['set_breadcrumb'] = $settings['set_breadcrumb'];
    taxonomy_menu_trails_selected_item_for_breadcrumb($selected_item);
  }
  
  // TODO: Remove workaround when Menu Block will remove its
  // _menu_link_get_preferred(). But it's safe to leave it here even after
  // Menu Block update to keep compatibility with old Menu Block releases.
  // So, probably it should be here forever or until the next major release.
  if (module_exists('menu_block')) {
    // Also set preferred item for Menu Block module
    $mb_preferred_links = &drupal_static('_menu_link_get_preferred');
    $mb_preferred_links[$_GET['q']][$selected_item['menu_name']] = $selected_item;
  }
}

/**
 * Selects menu item using "first" or "last" selection method. There is no
 * difference in these two methods because terms array is reversed in
 * _taxonomy_menu_trails_process() if "last" is used.
 *
 * @param array $vars
 *   Variables map:
 *   - menu_names - active menu names
 *   - results - menu items fetching query results
 *   - paths - paths of terms, order is important
 * @return array
 *   Selected menu item.
 */
function _taxonomy_menu_trails_fetch_link_simple($vars) {
  extract($vars);
  
  // Sort candidates by link path and menu name.
  $candidates = array();
  foreach ($results as $candidate) {
    //Prefer the lightest of items with the same menu and path
    if (!isset($candidates[$candidate['link_path']][$candidate['menu_name']])) {
      $candidate['weight'] = $candidate['link_weight'];
      $candidates[$candidate['link_path']][$candidate['menu_name']] = $candidate;

      // Add any menus not already in the menu name search list.
      if (!in_array($candidate['menu_name'], $menu_names)) {
        $menu_names[] = $candidate['menu_name'];
      }
    }
  }
  
  if (!empty($candidates)) {
    // Pick the most specific link, in the most preferred menu.
    foreach ($paths as $link_path) {
      if (!isset($candidates[$link_path])) {
        continue;
      }
      foreach ($menu_names as $menu_name) {
        if (!isset($candidates[$link_path][$menu_name])) {
          continue;
        }
        $candidate_item = $candidates[$link_path][$menu_name];
        $map = explode('/', $link_path);
        _menu_translate($candidate_item, $map);
        if ($candidate_item['access']) {
          $selected_item = $candidate_item;
          break 2;
        }
      }
    }
  }

  return $selected_item;
}

/**
 * Selects deepest menu item.
 *
 * @param array $vars
 *   @see _taxonomy_menu_trails_fetch_link_simple()
 * @return array
 *   Selected menu item.
 */
function _taxonomy_menu_trails_fetch_link_deepest($vars) {
  extract($vars);

  $candidates = array();
  foreach ($results as $candidate) {
    //Prefer the lightest of items with the same menu and path
    if (!isset($candidates[$candidate['menu_name']][$candidate['link_path']])) {
      $candidate['weight'] = $candidate['link_weight'];
      $candidates[$candidate['menu_name']][$candidate['link_path']] = $candidate;

      // Add any menus not already in the menu name search list.
      if (!in_array($candidate['menu_name'], $menu_names)) {
        $menu_names[] = $candidate['menu_name'];
      }
    }
  }
  
  if (!empty($candidates)) {
    foreach ($menu_names as $menu_name) {
      if (!empty($candidates[$menu_name])) {
        foreach ($candidates[$menu_name] as $candidate_item) {
          $map = explode('/', $candidate_item['link_path']);
          _menu_translate($candidate_item, $map);
          if ($candidate_item['access']) {
            $selected_item = $candidate_item;
            break 2;
          }
        }
      }
    }
  }

  return $selected_item;
}

/**
 * Returns list of possible front page paths.
 *
 * @return array
 */
function _taxonomy_menu_trails_homepage_paths() {
  return array(
    '<front>',
    variable_get('site_frontpage', 'node'),
  );
}

/**
 * Returns TRUE when passed breadcrumb is empty.
 * 
 * "Empty" here means there are no items between "Home" (or breadcrumb start)
 * and current tab root menu item.
 *
 * @param array $active_trail
 *
 * @return bool
 */
function _taxonomy_menu_trails_menu_breadcrumb_is_empty($active_trail) {
  if (empty($active_trail)) {
    return TRUE;
  }
  
  // Skip first item if it has homepage href.
  $first_item = reset($active_trail);
  $frontpage_hrefs = _taxonomy_menu_trails_homepage_paths();
  if (in_array($first_item['href'], $frontpage_hrefs)) {
    array_shift($active_trail);
  }
  
  // Look into the next item (if any) and check if it has current tab root href.
  $next_item = reset($active_trail);
  if (empty($next_item)) {
    return TRUE;
  }
  $menu_item = menu_get_item();
  if ($next_item['href'] == $menu_item['tab_root_href']) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Injects selected item and its parents into the breadcrumb.
 * 
 * @param array $active_trail
 * @param array $selected_item
 *
 * @todo Simplify function code, probably menu_build_tree() is not necessary
 *   here and we could just load parents from {menu_links} and translate them.
 */
function _taxonomy_menu_trails_menu_breadcrumb_alter(&$active_trail, $selected_item) {
  if ($selected_item['set_breadcrumb'] == 'if_empty' && !_taxonomy_menu_trails_menu_breadcrumb_is_empty($active_trail)) {
    return;
  }

  // Iterate over menu item parents and collect mlids of current active trail.
  $active_trail_mlids = array();
  for ($i = 1; $i <= $selected_item['depth']; $i++) {
    $parent = $selected_item["p$i"];
    $active_trail_mlids[$parent] = $parent;
  }

  // Build menu tree containing only selected menu item itself and its parents.
  $parameters = array(
    'active_trail' => $active_trail_mlids,
    'only_active_trail' => TRUE,
  );
  $tree = menu_build_tree($selected_item['menu_name'], $parameters);

  // Flatten menu tree into injection array. This part was copied
  // from menu_set_active_trail().
  $injection = array();
  list($key, $curr) = each($tree);
  while ($curr) {
    $link = $curr['link'];
    if ($link['in_active_trail']) {
      // Add the link to the trail, unless it links to its parent.
      if (!($link['type'] & MENU_LINKS_TO_PARENT)) {
        // The menu tree for the active trail may contain additional links
        // that have not been translated yet, since they contain dynamic
        // argument placeholders (%). Such links are not contained in regular
        // menu trees, and have only been loaded for the additional
        // translation that happens here, so as to be able to display them in
        // the breadcumb for the current page.
        // @see _menu_tree_check_access()
        // @see _menu_link_translate()
        if (strpos($link['href'], '%') !== FALSE) {
          _menu_link_translate($link, TRUE);
        }
        if ($link['access']) {
          $injection[] = $link;
        }
      }
      $tree = $curr['below'] ? $curr['below'] : array();
    }
    list($key, $curr) = each($tree);
  }

  // Inject our active trail to the passed active trail right after the "Home"
  // link (if any).
  $trail_start = array();
  if (!empty($active_trail)) {
    $first_item = reset($active_trail);
    $frontpage_hrefs = _taxonomy_menu_trails_homepage_paths();
    if (in_array($first_item['href'], $frontpage_hrefs)) {
      $trail_start[] = array_shift($active_trail);
    }
  }
  $active_trail = array_merge($trail_start, $injection, $active_trail);
}
