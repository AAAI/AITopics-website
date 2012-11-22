<?php

/**
 * @file
 * The finder on/off plugin handler.
 */

/**
 * The CTools plugin definition.
 */
//$plugin = array(
  // Not properly implemented yet.
  //'onoff' => array(
  //  'title' => t('Single on/off checkbox'),
  //  'settings callback' => 'finder_onoff_settings',
  //  'element callback' => 'finder_onoff_element',
  //),
//);

/**
 * Settings callback.
 */
function finder_onoff_settings(&$data, $finder, $finder_element_id) {
  $items = &$data['items'];
  $element = &$finder->elements[$finder_element_id];

  $items['return_value'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Return value'),
      '#value' => $finder->esetting($element, 'return_value') ? $finder->esetting($element, 'return_value') : t('Default'),
    ),
    '#form' => array(
      'settings' => array(
        'return_value' => array(
          '#type' => 'textfield',
          '#title' => t('Return value'),
          '#default_value' => $finder->esetting($element, 'return_value'),
          '#description' => t('Value element should return when selected.  Leave blank to default to 1.'),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

}

/**
 * Element callback.
 */
function finder_onoff_element($element, &$form_element) {
  $finder = $element->finder;

  $properties = array();

  $form_element['#type'] = 'checkbox';
  $properties = array(
    'return_value',
  );

  if (isset($properties)) {
    foreach ($properties as $property) {
      $form_element['#' . $property] = $finder->esetting($element, $property);
    }
  }

  $finder->find = array(
    'mode' => 'choices',
    'keywords' => array($element->id => array(NULL)),
    'element' => $element,
  );
  $finder->find();
  $form_element['#options'] = !empty($finder->find['results']) ? $finder->find['results'] : array();

  if ($finder->esetting($element, 'empty')) {
    $empty = array('' => $finder->esetting($element, 'empty_text'));
    if ($finder->esetting($element, 'empty') == 'prepend') {
      $form_element['#options'] = $empty + $form_element['#options'];
    }
    elseif ($finder->esetting($element, 'empty') == 'append') {
      $form_element['#options'] = $form_element['#options'] + $empty;
    }
  }

}
