<?php

/**
 * @file
 * Theme functions for the Finder UI module.
 */

/**
 * Theme the admin table of draggable elements.
 *
 * @param $element
 *   The form element to theme.
 */
function theme_finder_ui_elements_table($variables) {
  $element = $variables['element'];
  $output = '';
  $children = element_children($element);
  if (!empty($children)) {
    $css_id = 'finder-ui-elements-table';
    $css_class = 'finder-ui-elements-table-order';
    drupal_add_tabledrag($css_id, 'order', 'sibling', $css_class);
    $rows = array();
    foreach ((array)$children as $key) {
      $value = &$element[$key];
      $value['weight']['#attributes']['class'] = array($css_class);
      $rows[] = array(
        'data' => array(
          array(
            'data' => drupal_render($value['value']),
            'class' => array('finder-ui-elements-table-value'),
          ),
          array(
            'data' => drupal_render($value['weight']),
            'class' => array('finder-ui-elements-table-weight'),
          )
        ),
        'class' => array('draggable'),
      );
    }
    $output = theme('table', array('rows' => $rows, 'attributes' => array('id' => $css_id)));
  }

  return $output;
}