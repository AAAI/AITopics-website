<?php

/**
 * @file
 * The finder autocomplete element handler plugin.
 */

/**
 * The CTools plugin definition.
 */
$plugin = array(
  'autocomplete' => array(
    'title' => t('Autocomplete textfield'),
    'settings callback' => 'finder_autocomplete_settings',
    'element callback' => 'finder_autocomplete_element',
    'menu' => array(
      'finder_autocomplete/autocomplete' => array(
        'page callback' => 'finder_autocomplete_autocomplete',
        'access arguments' => array('use finder'),
        'type' => MENU_CALLBACK,
        'file' => 'autocomplete.inc',
        'file path' => drupal_get_path('module', 'finder') . '/plugins/element_handler/',
      ),
    ),
    'theme' => array(
      'finder_autocomplete_textfield' => array(
        'render element' => 'element',
        'file' => 'autocomplete.inc',
        'path' => drupal_get_path('module', 'finder') . '/plugins/element_handler/',
      ),
    ),
  ),
);

/**
 * Settings callback.
 */
function finder_autocomplete_settings(&$data, $finder, $finder_element_id) {
  $items = &$data['items'];
  $element = &$finder->elements[$finder_element_id];

  // Set some defaults.
  if (!isset($element->settings['max_suggestions'])) {
    $element->settings['max_suggestions'] = 10;
  }
  if (!isset($element->settings['autocomplete_field_logic'])) {
    $element->settings['autocomplete_field_logic'] = 'OR';
  }
  if (!isset($element->settings['autocomplete_value_logic'])) {
    $element->settings['autocomplete_value_logic'] = 'OR';
  }
  if (!isset($element->settings['autocomplete_match'])) {
    $element->settings['autocomplete_match'] = 'c';
  }

  $items['maxlength'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Max length'),
      '#value' => $finder->esetting($element, 'maxlength') ? $finder->esetting($element, 'maxlength') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'maxlength' => array(
          '#type' => 'textfield',
          '#title' => t('Max length'),
          '#default_value' => $finder->esetting($element, 'maxlength'),
          '#description' => t('
            The maximum amount of characters to accept as input.
            <em>Using this may truncate longer field values and fail with exact matching.</em>'),
        ),
      ),
    ),
  );

  $items['size'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Size'),
      '#value' => $finder->esetting($element, 'size') ? $finder->esetting($element, 'size') : t('Default'),
    ),
    '#form' => array(
      'settings' => array(
        'size' => array(
          '#type' => 'textfield',
          '#title' => t('Size'),
          '#default_value' => $finder->esetting($element, 'size'),
          '#description' => t('Width of the textfield (in characters).'),
        ),
      ),
    ),
  );

  $items['max_suggestions'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Max suggestions'),
      '#value' => $finder->esetting($element, 'max_suggestions') ? $finder->esetting($element, 'max_suggestions') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'max_suggestions' => array(
          '#type' => 'textfield',
          '#title' => t('Max suggestions'),
          '#default_value' => $finder->esetting($element, 'max_suggestions'),
          '#description' => t('It is a good idea to limit the number of autocomplete suggestions that appear.  Note that duplicate suggestions are removed, and when using multiple fields a record may be split into multiple suggestions, these factors affect the number of suggestions that are actually shown.'),
        ),
      ),
    ),
  );

  $items['autosubmit'] = array(
    '#group' => 'form',
    '#item' =>  array(
      '#title' => t('Auto submit'),
      '#value' => $finder->esetting($element, 'autosubmit') ? t('Yes') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'autosubmit' => array(
          '#type' => 'checkbox',
          '#title' => t('Auto submit upon selection'),
          '#default_value' => $finder->esetting($element, 'autosubmit'),
          '#description' => t('
            Normally selecting a suggested autocomplete value (with mouse click or
            enter button) does not submit the form, even if the entire value was
            typed by the user, this option shortens the workflow for single element
            finders by automatically submitting the form when the selection is
            made.'),
        ),
      ),
    ),
  );

  $items['autocomplete_delimit'] = array(
    '#group' => 'choices',
    '#item' =>  array(
      '#title' => t('Delimit value'),
      '#value' => $finder->esetting($element, 'autocomplete_delimit') ? $finder->esetting($element, 'autocomplete_delimit') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'autocomplete_delimit' => array(
          '#type' => 'textfield',
          '#title' => t('Delimit value'),
          '#default_value' => $finder->esetting($element, 'autocomplete_delimit'),
          '#description' => t('Treat delimited values as separate keywords for autocomplete choices.  For example, if you type a space here, the autocompleted value will be expanded into a value for each word.  Leave empty to disable this feature.'),
        ),
      ),
    ),
  );

  $items['autocomplete_field_logic'] = array(
    '#group' => 'choices',
    '#item' =>  array(
      '#title' => t('Field logic'),
      '#value' => $finder->esetting($element, 'autocomplete_field_logic'),
    ),
    '#form' => array(
      'settings' => array(
        'autocomplete_field_logic' => array(
          '#type' => 'radios',
          '#title' => t('Field logic'),
          '#default_value' => $finder->esetting($element, 'autocomplete_field_logic'),
          '#description' => t('With multiple fields being select above, how should the fields be combined when calculating autocomplete choices?'),
          '#options' => array(
            'AND' => t('Match all fields using the AND operator. (Conjunction)'),
            'OR' => t('Match any field using the OR operator. (Disjunction)'),
          ),
        ),
      ),
    ),
  );

  $items['autocomplete_value_logic'] = array(
    '#group' => 'choices',
    '#item' =>  array(
      '#title' => t('Value logic'),
      '#value' => $finder->esetting($element, 'autocomplete_value_logic'),
    ),
    '#form' => array(
      'settings' => array(
        'autocomplete_value_logic' => array(
          '#type' => 'radios',
          '#title' => t('Value logic'),
          '#default_value' => $finder->esetting($element, 'autocomplete_value_logic'),
          '#description' => t('With multiple submitted values for this element, how should the values be combined when calculating results?'),
          '#options' => array(
            'AND' => t('Match all values using the AND operator. (Conjunction)'),
            'OR' => t('Match any value using the OR operator. (Disjunction)'),
          ),
        ),
      ),
    ),
  );

  $items['autocomplete_nesting_order'] = array(
    '#group' => 'choices',
    '#item' =>  array(
      '#title' => t('Nesting order'),
      '#value' => $finder->esetting($element, 'autocomplete_nesting_order') ? t('Values first') : t('Fields first'),
    ),
    '#form' => array(
      'settings' => array(
        'autocomplete_nesting_order' => array(
          '#type' => 'radios',
          '#title' => t('Nesting order'),
          '#default_value' => $finder->esetting($element, 'autocomplete_nesting_order'),
          '#description' => t('With multiple values or fields, how should fields and values be matched together?  This is difficult to explain.  Suppose you have an element that selects two fields and a user submits two values (X and Y), the first option here will do matching like this: <em>(field_1 matches X; field_2 matches X), (field_1 matches Y; field_2 matches Y)</em> whereas the second will do it like this: <em>(field_1 matches X, field_1 matches Y); (field_2 matches X, field_2 matches Y)</em>.  The semicolons represent <em>field logic</em>, the commas represent <em>value logic</em>, and the word <em>matches</em> refers to the <em>matching</em> option.  So simple.'),
          '#options' => array(
            0 => t('Match multiple fields for each value first, then combine the results of multiple values. (recommended)'),
            1 => t('Match multiple values for each field first, then combine the results of multiple fields.'),
          ),
        ),
      ),
    ),
  );

  $default_match = NULL;
  $matches = $finder->matches();
  $matches['x'] = array(
    'name' => t('Custom'),
    'description' => t('Specify below like: <code> field [operator] [prefix]value[suffix]</code>'),
  );
  foreach ($matches as $key => $match) {
    $matches[$key] = finder_ui_matches_label($match, t('results'), t('submitted values'));
    if ($key == $finder->esetting($element, 'autocomplete_match')) {
      $default_match = $match['name'];
    }
  }

  $match_custom_operator_size = 5;
  if ($finder->esetting($element, 'autocomplete_match_custom_operator')) {
    $match_custom_operator_size = min(drupal_strlen($finder->esetting($element, 'autocomplete_match_custom_operator')), 30);
  }

  $match_custom_prefix_size = 3;
  if ($finder->esetting($element, 'autocomplete_match_custom_prefix')) {
    $match_custom_prefix_size = min(drupal_strlen($finder->esetting($element, 'autocomplete_match_custom_prefix')), 30);
  }

  $match_custom_suffix_size = 3;
  if ($finder->esetting($element, 'autocomplete_match_custom_suffix')) {
    $match_custom_suffix_size = min(drupal_strlen($finder->esetting($element, 'autocomplete_match_custom_suffix')), 30);
  }

  $items['autocomplete_match'] = array(
    '#group' => 'choices',
    '#item' =>  array(
      '#title' => t('Matching'),
      '#value' => $default_match,
    ),
    '#form' => array(
      'settings' => array(
        'autocomplete_match' => array(
          '#type' => 'radios',
          '#title' => t('Matching'),
          '#default_value' => $finder->esetting($element, 'autocomplete_match'),
          '#options' => $matches,
          '#description' => t('<em>Contains</em> is the most common autocomplete matching method.'),
        ),
        'autocomplete_match_custom_operator' => array(
          '#type' => 'textfield',
          '#title' => t('Operator'),
          '#title_display' => 'none',
          '#default_value' => $finder->esetting($element, 'autocomplete_match_custom_operator'),
          '#size' => $match_custom_operator_size,
          '#maxlength' => 512,
          '#field_prefix' => t('field'),
          '#prefix' => '<div class="finder-ui-match-custom"><span class="finder-ui-match-custom-operator">',
          '#suffix' => '</span>',
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('radio:settings[autocomplete_match]' => array('x')),
          '#translatable' => FALSE,
        ),
        'autocomplete_match_custom_prefix' => array(
          '#type' => 'textfield',
          '#title' => t('Prefix'),
          '#title_display' => 'none',
          '#default_value' => $finder->esetting($element, 'autocomplete_match_custom_prefix'),
          '#size' => $match_custom_prefix_size ,
          '#maxlength' => 512,
          '#field_suffix' => t('value'),
          '#prefix' => '<span class="finder-ui-match-custom-value-prefix">',
          '#suffix' => '</span>',
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('radio:settings[autocomplete_match]' => array('x')),
          '#translatable' => FALSE,
        ),
        'autocomplete_match_custom_suffix' => array(
          '#type' => 'textfield',
          '#title' => t('Suffix'),
          '#title_display' => 'none',
          '#default_value' => $finder->esetting($element, 'autocomplete_match_custom_suffix'),
          '#size' => $match_custom_suffix_size,
          '#maxlength' => 512,
          '#prefix' => '<span class="finder-ui-match-custom-value-suffix">',
          '#suffix' => '</span></div>',
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('radio:settings[autocomplete_match]' => array('x')),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

}

/**
 * Element callback.
 */
function finder_autocomplete_element($element, &$form_element) {
  $finder = $element->finder;
  $form_element['#type'] = 'textfield';
  $form_element['#theme'] = 'finder_autocomplete_textfield';
  $autocomplete_path = 'finder_autocomplete/autocomplete/' . $finder->name . '/' . $element->id;
  $form_element['#autocomplete_path'] = $autocomplete_path;
  $properties = array(
    'maxlength',
    'size',
    'autosubmit',
  );
  foreach ($properties as $property) {
    $form_element['#' . $property] = $finder->esetting($element, $property);
  }
}

/**
 * Menu callback; get autocomplete suggestions.
 */
function finder_autocomplete_autocomplete($finder_name, $element_id, $keywords = '') {
  // If the request has a '/' in the search text, then the menu system will have
  // split it into multiple arguments, recover the intended $keywords.
  $args = func_get_args();
  // Shift off the $finder_name argument.
  array_shift($args);
  // Shift off the $element_id argument.
  array_shift($args);
  $keywords = implode('/', $args);


  $choices = array();

  if ($keywords === '') {
    drupal_json_output($choices);
  }

  $keywords = array($keywords);

  $finder = finder_load($finder_name);
  $finder->build();
  $element = &$finder->elements[$element_id];

  if ($finder->esetting($element, 'autocomplete_delimit')) {
    foreach ($keywords as $k => $v) {
      unset($keywords[$k]);
      $exploded = explode($finder->esetting($element, 'autocomplete_delimit'), $v);
      foreach ($exploded as $e) {
        $keywords[] = trim($e);
      }
    }
  }

  $finder->find = array(
    'mode' => 'choices',
    'match' => $finder->esetting($element, 'autocomplete_match'),
    'field_logic' => $finder->esetting($element, 'autocomplete_field_logic'),
    'value_logic' => $finder->esetting($element, 'autocomplete_value_logic'),
    'nesting_order' => $finder->esetting($element, 'autocomplete_nesting_order'),
    'pager' => $finder->esetting($element, 'max_suggestions'),
    'keywords' => array($element->id => $keywords),
    'element' => $element,
  );
  $finder->find();
  $choices = !empty($finder->find['results']) ? $finder->find['results'] : array();

  drupal_json_output($choices);
}


/**
 * Format a finder autocomplete textfield.
 *
 * @param $variables
 *   An array with keys:
 *   'element' - The Forms API form element.
 */
function theme_finder_autocomplete_textfield($variables) {
  $element = $variables['element'];
  $element['#attributes']['type'] = 'text';
  element_set_attributes($element, array('id', 'name', 'value', 'size', 'maxlength'));
  _form_set_class($element, array('form-text'));

  $extra = '';
  if (!empty($element['#autocomplete_path']) && drupal_valid_path($element['#autocomplete_path'])) {
    drupal_add_js(drupal_get_path('module', 'finder') . '/plugins/element_handler/autocomplete.js');

    $settings['finder_autocomplete'][$element['#attributes']['id']]['autosubmit'] = (bool)$element['#autosubmit'];

    drupal_add_js($settings, 'setting');

    $element['#attributes']['class'][] = 'form-autocomplete';

    $attributes = array();
    $attributes['type'] = 'hidden';
    $attributes['id'] = $element['#attributes']['id'] . '-autocomplete';
    $attributes['value'] = url($element['#autocomplete_path'], array('absolute' => TRUE));
    $attributes['disabled'] = 'disabled';
    $attributes['class'][] = 'finder-autocomplete';
    $extra = '<input' . drupal_attributes($attributes) . ' />';
  }

  $output = '<input' . drupal_attributes($element['#attributes']) . ' />';

  return $output . $extra;
}
