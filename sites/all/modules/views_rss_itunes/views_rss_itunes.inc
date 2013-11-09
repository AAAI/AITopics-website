<?php

/**
 * @file
 * Preprocess functions for Views RSS: iTunes Elements module.
 */

/**
 * Preprocess function for channel <itunes:category> element.
 */
function views_rss_itunes_preprocess_channel_category(&$variables) {
  // No value = no preprocessing.
  if (empty($variables['elements'][0]['value'])) {
    return;
  }
  $elements = array();
  foreach ($variables['elements'][0]['value'] as $tid) {
    // Load term object.
    $term = taxonomy_term_load($tid);
    // Try to load parent term objects.
    $parents = taxonomy_get_parents($tid);
    // Term has a parent.
    if (count($parents)) {
      $parent = array_shift($parents);
      if (!isset($elements[$parent->tid])) {
        $elements[$parent->tid] = array(
          'key' => 'itunes:category',
          'attributes' => array(
            'text' => $parent->name,
          ),
        );
      }
      $elements[$parent->tid]['value'][$term->tid] = array(
        'key' => 'itunes:category',
        'attributes' => array(
          'text' => $term->name,
        ),
      );
    }
    // Term has no parent.
    else {
      $elements[$term->tid] = array(
        'key' => 'itunes:category',
        'attributes' => array(
          'text' => $term->name,
        ),
      );
    }
  }
  $variables['elements'] = $elements;
}

/**
 * Preprocess function for channel <itunes:image> element.
 */
function views_rss_itunes_preprocess_channel_image(&$variables) {
  // No value = no preprocessing.
  if (empty($variables['elements'][0]['value'])) {
    return;
  }
  $variables['elements'][0]['attributes'] = array(
    'href' => file_create_url($variables['elements'][0]['value']),
  );
  unset($variables['elements'][0]['value']);
}

/**
 * Preprocess function for channel <itunes:owner> element.
 */
function views_rss_itunes_preprocess_channel_owner(&$variables) {
  // No value = no preprocessing.
  if (empty($variables['elements'][0]['value'])) {
    return;
  }
  $values = explode(',', $variables['elements'][0]['value']);
  $variables['elements'][0]['value'] = array();
  // Add email subelement.
  $email = array_shift($values);
  if (!empty($email)) {
    $variables['elements'][0]['value']['itunes:email'] = trim($email);
  }
  // Add name subelement.
  $name = array_shift($values);
  if (!empty($name)) {
    $variables['elements'][0]['value']['itunes:name'] = trim($name);
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
