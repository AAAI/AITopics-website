<?php

/**
 * @file
 * Provides a page and time-based caching option for panel panes.
 */
// Plugin definition
$plugin = array(
  'title' => t("Page based cache"),
  'description' => t('Page caching is a time-based cache based on page arg\'s. This is a hard limit, and once cached it will remain that way until the time limit expires or the panel is updated.'),
  'cache get' => 'panels_page_cache_get_cache',
  'cache set' => 'panels_page_cache_set_cache',
  'cache clear' => 'panels_page_cache_clear_cache',
  'settings form' => 'panels_page_cache_settings_form',
  'settings form submit' => 'panels_page_cache_settings_form_submit',
  'defaults' => array(
    'lifetime' => 15,
  ),
);

/**
 * Get cached content.
 */
function panels_page_cache_get_cache($conf, $display, $args, $contexts, $pane = NULL) {
  $cid = panels_page_cache_get_id($conf, $display, $args, $contexts, $pane);

  $cache = cache_get($cid, 'cache_panels_page_cache');
  if (!$cache) {
    return FALSE;
  }

  if ((time() - $cache->created) > $conf['lifetime']) {
    return FALSE;
  }

  return $cache->data;
}

/**
 * Set cached content.
 */
function panels_page_cache_set_cache($conf, $content, $display, $args, $contexts, $pane = NULL) {
  $cid = panels_page_cache_get_id($conf, $display, $args, $contexts, $pane);
  cache_set($cid, $content, 'cache_panels_page_cache');
}

/**
 * Clear cached content.
 *
 * Cache clears are always for an entire display, regardless of arguments.
 */
function panels_page_cache_clear_cache($display) {
  $cid = 'panels_page_cache';

  // This is used in case this is an in-code display, which means did will be something like 'new-1'.
  if (isset($display->owner) && isset($display->owner->id)) {
    $cid .= ':' . $display->owner->id;
  }
  $cid .= ':' . $display->did;

  cache_clear_all($cid, 'cache_panels_page_cache', TRUE);
}

/**
 * Figure out an id for our cache based upon input and settings.
 */
function panels_page_cache_get_id($conf, $display, $args, $contexts, $pane) {
  $id = 'panels_page_cache';

  // This is used in case this is an in-code display, which means did will be something like 'new-1'.
  if (isset($display->owner) && isset($display->owner->id)) {
    $id .= ':' . $display->owner->id;
  }
  $id .= ':' . $display->did;

  if ($pane) {
    $id .= ':' . $pane->pid;
  }

  if (user_access('view pane admin links')) {
    $id .= ':admin';
  }

  if (!empty($conf['granularity'])) {
    switch ($conf['granularity']) {
      case 'args':
        foreach ($args as $arg) {
          $id .= ':' . $arg;
        }
        break;

      case 'context':
        if (!is_array($contexts)) {
          $contexts = array($contexts);
        }
        foreach ($contexts as $context) {
          if (isset($context->argument)) {
            $id .= ':' . $context->argument;
          }
        }
    }
  }

  if (module_exists('locale')) {
    global $language;
    $id .= ':' . $language->language;
  }

  if (!empty($pane->configuration['use_pager'])) {
    $id .= ':p' . check_plain($_GET['page']);
  }

  // Add page arguments to cache id
  $id .= ':' . implode('/', arg());

  return $id;
}

function panels_page_cache_settings_form($conf, $display, $pid) {
  $options = drupal_map_assoc(array(15, 30, 60, 120, 180, 240, 300, 600, 900, 1200, 1800, 3600, 7200, 14400, 28800, 43200, 86400, 172800, 259200, 345600, 604800), 'format_interval');
  $form['lifetime'] = array(
    '#title' => t('Lifetime'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $conf['lifetime'],
  );

  return $form;
}