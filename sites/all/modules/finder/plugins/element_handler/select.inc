<?php

/**
 * @file
 * The finder select element handler plugin.
 */

/**
 * The CTools plugin definition.
 */
$plugin = array(
  'select' => array(
    'title' => t('Select list'),
    'settings callback' => 'finder_select_settings',
    'element callback' => 'finder_select_element',
  ),
);

/**
 * Settings callback.
 */
function finder_select_settings(&$data, $finder, $finder_element_id) {
  $items = &$data['items'];
  $element = &$finder->elements[$finder_element_id];

  $items['size'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Size'),
      '#value' => $finder->esetting($element, 'size') ? $finder->esetting($element, 'size') : t('default'),
    ),
    '#form' => array(
      'settings' => array(
        'size' => array(
          '#type' => 'textfield',
          '#title' => t('Size'),
          '#default_value' => $finder->esetting($element, 'size'),
          '#description' => t('The height (in lines) of the select list if multiple values are allowed.'),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

  $items['multiple'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Multiple'),
      '#value' => $finder->esetting($element, 'multiple') ? t('Yes') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'multiple' => array(
          '#type' => 'checkbox',
          '#title' => t('Allow multiple values'),
          '#default_value' => $finder->esetting($element, 'multiple'),
        ),
      ),
    ),
  );

  $items['default_value']['#form']['settings']['default_value']['#type'] = 'textarea';
  $items['default_value']['#form']['settings']['default_value']['#size'] = 6;
  $items['default_value']['#form']['settings']['default_value']['#description'] = t('One per line for multiple values.');

  $items['empty'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Empty choice'),
      '#value' => $finder->esetting($element, 'empty') ? $finder->esetting($element, 'empty_text') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'empty' => array(
          '#type' => 'radios',
          '#title' => t('Add an empty choice'),
          '#default_value' => $finder->esetting($element, 'empty'),
          '#options' => array(
            '' => t('Do not add an empty choice.'),
            'prepend' => t('Prepend an empty choice to the top of the list.'),
            'append' => t('Append an empty choice to the bottom of the list.'),
          ),
        ),
        'empty_text' => array(
          '#type' => 'textfield',
          '#title' => t('Empty choice text'),
          '#default_value' => $finder->esetting($element, 'empty_text'),
          '#description' => t("This can be used to create a choice such as 'please choose' or 'none of the above'."),
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('radio:settings[empty]' => array('prepend', 'append')),
        ),
      ),
    ),
  );
}

/**
 * Element callback.
 */
function finder_select_element($element, &$form_element) {
  $finder = $element->finder;

  $properties = array();

  $form_element['#type'] = 'select';
  $properties = array(
    'multiple',
    'size',
  );
  if ($finder->esetting($element, 'multiple')) {
    if (!is_array($form_element['#default_value'])) {
      $form_element['#default_value'] = explode("\n", $form_element['#default_value']);
    }
  }

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

  $form_element['#element_validate'][] = 'finder_select_element_validate';

}

/**
 * Validation callback for text element.
 */
function finder_select_element_validate($form_element, &$form_state) {
  $finder = $form_state['finder'];
  foreach ($finder->elements as $key => $element) {
    if ($finder->esetting($element, 'multiple')) {
      // remove values in the array that are empty or 0
      foreach ($form_state['values'][$element->id] as $k => $v) {
        if (!$v) {
          unset($form_state['values'][$element->id][$k]);
        }
      }
      // append an 'empty string' choice if we just wiped the whole array and we need a value for the url args
      if ($finder->setting('url') && empty($form_state['values'][$element->id])) {
        $form_state['values'][$element->id][] = '';
      }
    }
  }
}