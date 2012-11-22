<?php

/**
 * @file
 * The finder text element handler plugin.
 */

/**
 * The CTools plugin definition.
 */
$plugin = array(
  'text' => array(
    'title' => t('Text'),
    'settings callback' => 'finder_text_settings',
    'element callback' => 'finder_text_element',
  ),
);

/**
 * Settings callback.
 */
function finder_text_settings(&$data, $finder, $finder_element_id) {
  $items = &$data['items'];
  $element = &$finder->elements[$finder_element_id];

  unset($items['groups'][2]['choices']);

  $items['default_value']['#form']['#settings']['default_value']['#type'] = 'textarea';

  $items['maxlength'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Max length'),
      '#value' => $finder->esetting($element, 'maxlength') ? $finder->esetting($element, 'maxlength') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'maxlength' => array(
          '#type' => 'textfield',
          '#title' => t('Max length'),
          '#default_value' => $finder->esetting($element, 'maxlength'),
          '#description' => t('The maximum amount of characters to accept as input.'),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

  $items['minlength'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Min length'),
      '#value' => $finder->esetting($element, 'minlength') ? $finder->esetting($element, 'minlength') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'minlength' => array(
          '#type' => 'textfield',
          '#title' => t('Min length'),
          '#default_value' => $finder->esetting($element, 'minlength'),
          '#description' => t('The minimum amount of characters to accept as input (if input is given).'),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

  $items['size'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Size'),
      '#value' => $finder->esetting($element, 'size') ? $finder->esetting($element, 'size') : t('Default'),
    ),
    '#form' => array(
      'settings' => array(
        'size' => array(
          '#type' => 'textfield',
          '#title' => t('Size'),
          '#default_value' => $finder->esetting($element, 'size'),
          '#description' => t('Width (in characters).'),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

  $items['rows'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Rows'),
      '#value' => $finder->esetting($element, 'rows') ? $finder->esetting($element, 'rows') : t('Textfield'),
    ),
    '#form' => array(
      'settings' => array(
        'rows' => array(
          '#type' => 'textfield',
          '#title' => t('Rows'),
          '#default_value' => $finder->esetting($element, 'rows'),
          '#description' => t('Leave blank for a single line textfield.'),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

}

/**
 * Element callback.
 */
function finder_text_element($element, &$form_element) {
  $finder = $element->finder;
  $properties = array();
  if ($finder->esetting($element, 'rows')) {
    $form_element['#type'] = 'textarea';
    $properties[] = 'rows';
  }
  else {
    $form_element['#type'] = 'textfield';
    $properties[] = 'maxlength';
    $properties[] = 'minlength';
    $properties[] = 'size';
  }
  foreach ($properties as $property) {
    $value = $finder->esetting($element, $property);
    if ($value) {
      $form_element['#' . $property] = $value;
    }
  }
  $form_element['#element_validate'][] = 'finder_text_element_validate';
}

/**
 * Validation callback for text element.
 */
function finder_text_element_validate($element, &$form_state) {
  if (isset($element['#minlength']) && $element['#minlength'] && $element['#value'] &&
    drupal_strlen($element['#value']) < $element['#minlength']) {
    form_error($element, t('%title requires a minimum of %char characters.',
      array('%title' => $element['#title'], '%char' => $element['#minlength'])));
  }
  if (isset($element['#maxlength']) && $element['#maxlength'] && $element['#value'] &&
    drupal_strlen($element['#value']) > $element['#maxlength']) {
    form_error($element, t('%title allows a maximum of %char characters.',
      array('%title' => $element['#title'], '%char' => $element['#maxlength'])));
  }
}

