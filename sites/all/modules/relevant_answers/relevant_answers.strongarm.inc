<?php
/**
 * @file
 * relevant_answers.strongarm.inc
 */

/**
 * Implementation of hook_strongarm().
 */
function relevant_answers_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'statistics_count_content_views';
  $strongarm->value = 1;
  $export['statistics_count_content_views'] = $strongarm;

  return $export;
}
