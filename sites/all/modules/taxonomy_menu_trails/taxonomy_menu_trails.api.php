<?php

/**
 * Map taxonomy term ids to their paths.
 *
 * This allows override of the default taxonomy/term/[tid] path for terms. As an
 * example, this can be used to set the menu trail to a view where these terms
 * are used, a specific node and so on.
 *
 * If you don't want to provide mappings for some terms then just skip them and
 * Taxonomy Menu Trails will use default path for these terms.
 *
 * Results from multiple implementations are merged together.
 *
 * @param array $tids
 *   The list of taxonomy term ids.
 * @param string $entity_type
 *   The type of entity passed to the next argument.
 * @param object $entity
 *   The entity object, e.g. node.
 * @param array $settings
 *   The taxonomy menu trails settings for this entity type.
 *
 * @return null|array
 *   List of mappings. Keys are tids and values can be:
 *   - string: single term path.
 *   - array: list of paths sorted by preference (the most preferred path should
 *     be the first).
 */
function hook_taxonomy_menu_trails_get_paths($tids, $entity_type, $entity, $settings) {
  // It is safe to load the same list of terms multiple times, because results
  // are saved in static cache.
  $terms = taxonomy_term_load_multiple($tids);

  $paths = array();
  foreach ($terms as $term) {
    if ($term->tid == 987) {
      // Specific term mapped to node and custom path. When selection method is
      // first/last the first path existing in menu wins. So, node path will be
      // preferred in this case.
      $paths[$term->tid] = array(
        'node/23',
        'some/path',
      );
      continue;
    }
    
    switch ($term->vid) {
      case 123:
        // Some specific vocabulary mapped to a view.
        $paths[$term->tid] = 'path/to/my/view' . $term->tid;
        break;

      case 456:
        // Skip vocabulary. Terms will be mapped to default path.
        break;

      default:
        // Map the rest to some default view.
        $paths[$term->tid] = 'path/to/default/view/' . $term->tid;
    }
    
  }
  return $paths;
}
