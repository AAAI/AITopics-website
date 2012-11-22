<?php

/**
 * @file
 * Preprocess functions for Views RSS: iTunes Elements module.
 */

/**
 * Preprocess function for channel <itunes:category> element.
 */
function views_rss_itunes_preprocess_channel_category(&$variables) {
  if (!empty($variables['value'])) {
    $element = array();
    foreach ($variables['value'] as $tid) {
      $term = taxonomy_term_load($tid);
      $parents = taxonomy_get_parents($tid);
      if (count($parents)) {
        $parent = array_shift($parents);
        if (!isset($elements[$parent->tid])) {
          $elements[$parent->tid] = array(
            'element' => $variables['element'],
            'arguments' => array(
              'text' => htmlspecialchars($parent->name),
            ),
          );
        }
        $elements[$parent->tid]['subelements'][$term->tid] = array(
          'element' => $variables['element'],
          'arguments' => array(
            'text' => htmlspecialchars($term->name),
          ),
        );
      }
      else {
        $elements[$term->tid] = array(
          'element' => $variables['element'],
          'arguments' => array(
            'text' => htmlspecialchars($term->name),
          ),
        );
      }
    }
    $variables['value'] = array('elements' => $elements);
  }
}

/**
 * Preprocess function for channel <itunes:category> element.
 */
function views_rss_itunes_preprocess_channel_image(&$variables) {
  if (!empty($variables['value'])) {
    $variables['value'] = array(
      'arguments' => array(
        'href' => file_create_url($variables['value']),
      ),
    );
  }
}

/**
 * Preprocess function for channel <itunes:owner> element.
 */
function views_rss_itunes_preprocess_channel_owner(&$variables) {
  $values = explode(',', $variables['value']);
  $variables['value'] = array();
  // Add email subelement.
  $email = array_shift($values);
  if (!empty($email)) {
    $variables['value']['subelements']['email'] = array(
      'element' => 'itunes:email',
      'value' => trim($email),
    );
  }
  // Add name subelement.
  $name = array_shift($values);
  if (!empty($name)) {
    $variables['value']['subelements']['name'] = array(
      'element' => 'itunes:name',
      'value' => trim($name),
    );
  }
}

/**
 * Fetches and returns media file information from file ID3 tags,
 * also storing it in cache for subsequent requests.
 */
function views_rss_itunes_get_file_info($uri, $rebuild = FALSE) {
  $cid = 'views_rss_itunes:' . $uri;
  $cached = cache_get($cid, 'cache_views');
  if (is_object($cached) && isset($cached->data) && $rebuild === FALSE) {
    $file_info = $cached->data;
  }
  else {
    $file_info = getid3_analyze(drupal_realpath($uri));
    cache_set($cid, $file_info, 'cache_views');
  }
  return $file_info;
}

/**
 * Returns array of iTunes categories for channel <itunes:category> 
 * element configuration in view options form.
 */
function views_rss_itunes_get_category_options() {
  $options = array();
  $vid = variable_get('views_rss_itunes_category_vid');
  if (!empty($vid)) {
    foreach (taxonomy_get_tree($vid) as $term) {
      $options[$term->tid] = str_pad(t($term->name), strlen(t($term->name)) + $term->depth * 2, '- ', STR_PAD_LEFT);
    }
  }
  return $options;
}