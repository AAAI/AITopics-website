<?php
/**
 * @file
 * Provides block configuration routines.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * Implements hook_block_configure().
 */
function _accordion_menu_block_configure($delta) {
  // Gather information.
  $config = accordion_menu_config($delta);
  $menus = menu_get_names();
  $options = array();
  foreach ($menus as $menu) {
    $options[$menu] = $menu;
  }

  // Build form.
  $fs1['basic_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic options'),
    '#description' => t('To be completed.'),
    '#tree' => FALSE,
  );
  $form1['accordion_menu_menu_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Menu Name'),
    '#default_value' => $config['menu_name'],
  );
  $form1['accordion_menu_menu_source'] = array(
    '#type' => 'select',
    '#title' => t('Menu source'),
    '#description' => t('Select the menu to display as an accordion.'),
    '#default_value' => $config['menu_source'],
    '#options' => $options,
  );
  $form1['accordion_menu_script_scope'] = array(
    '#type' => 'textfield',
    '#title' => t('Script scope'),
    '#description' => t('The location in which to place the script. The location must be implemented by the theme. Values include "header" and "footer" by default.'),
    '#default_value' => $config['script_scope'],
    '#size' => 10,
  );
  $form1['accordion_menu_header_link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Header link'),
    '#description' => t('If checked, header menu items will be output as links. Clicking on the menu item may trigger the accordion effect and a redirect to the header link. <strong>Undesirable</strong>'),
    '#default_value' => $config['header_link'],
  );
  $fs1['basic_options'] += $form1;

  $fs2['jquery_noticeable'] = array(
    '#type' => 'fieldset',
    '#title' => t('Noticeable options'),
    '#description' => t('These jQuery Accordion options noticeably affect a menu displayed as an accordion. Some are marked as "undesirable" for use with a menu.'),
    '#tree' => FALSE,
  );
  $form2['accordion_menu_header'] = array(
    '#type' => 'textfield',
    '#title' => t('Header'),
    '#description' => t('Selector for the header element.'),
    '#default_value' => $config['header'],
    '#size' => 15,
  );
  $form2['accordion_menu_animated'] = array(
    '#type' => 'textfield',
    '#title' => t('Animation'),
    '#description' => t('Enter the name of an animation, or set to "false" to disable them. In addition to the default of "slide", "bounceslide" and all defined easing methods are supported ("bounceslide" requires UI Effects Core). With menu items, the alternative effects are mostly unnoticeable.'),
    '#default_value' => $config['animated'],
    '#size' => 10,
  );
  $form2['accordion_menu_event'] = array(
    '#type' => 'select',
    '#title' => t('Event'),
    '#description' => t('The event on which to trigger the accordion. Suggested use: If the top-level menu item does not link to content, then select "click" to expand the menu. If the top-level menu item links to content, then select "mousedown" to expand the menu and display the linked content. Use "mouseover" at your own risk.'),
    '#default_value' => $config['event'],
    '#options' => drupal_map_assoc(array('click', 'mousedown', 'mouseover')),
  );
  $form2['accordion_menu_collapsible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#description' => t('Whether all the sections can be closed at once. Allows collapsing the active section by the triggering event.'),
    '#default_value' => $config['collapsible'],
  );
  $form2['accordion_menu_auto_height'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto height'),
    '#description' => t('If checked, the highest content part is used as height reference for all other parts. Provides more consistent animations. <strong>Undesirable</strong>'),
    '#default_value' => $config['auto_height'],
  );
  $form2['accordion_menu_clear_style'] = array(
    '#type' => 'checkbox',
    '#title' => t('Clear style'),
    '#description' => t('If checked, clears height and overflow styles after finishing animations. This enables accordions to work with dynamic content, but does not work with auto_height.'),
    '#default_value' => $config['clear_style'],
  );
  $form2['accordion_menu_fill_space'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fill space'),
    '#description' => t('If checked, the accordion completely fills the height of the parent element. Overrides auto_height. <strong>Undesirable</strong>'),
    '#default_value' => $config['fill_space'],
  );
  $fs2['jquery_noticeable'] += $form2;

  $fs3['jquery_unnoticeable'] = array(
    '#type' => 'fieldset',
    '#title' => t('Unnoticeable options'),
    '#description' => t('These jQuery Accordion options do not noticeably affect a menu displayed as an accordion.'),
    '#tree' => FALSE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form3['accordion_menu_navigation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Navigation'),
    '#description' => t('If checked, looks for the anchor that matches location.href and activates it. Great for href-based state-saving. Use navigationFilter to implement your own matcher.'),
    '#default_value' => $config['navigation'],
  );
  $fs3['jquery_unnoticeable'] += $form3;


  // Build the form.
  $form['tabs'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => 'edit-directories',
  );
  $form['tabs']['basic'] = $fs1['basic_options'];
  $form['tabs']['noticeable'] = $fs2['jquery_noticeable'];
  $form['tabs']['unnoticeable'] = $fs3['jquery_unnoticeable'];

//   $form['#validate'][] = 'accordion_menu_block_validate';

  return $form;
}

/**
 * Form validation handler for hook_block_configure().
 */
function _accordion_menu_block_validate($form, &$form_state) {
  foreach (array('animated', 'script_scope') as $key) {
    // Value may include letters only.
    // This is not validating the string, but simply requiring one.
    $value = $form_state['values']['accordion_menu_' . $key];
    if (preg_match('@[^a-z]@', strtolower($value))) {
      form_set_error('accordion_menu_' . $key, t('Enter a string with only letters. (' . $key . ')'));
    }
  }
  // Value may include letters and a digit only.
  // This is not a true test for an HTML tag, but should prevent foul play.
  $key = 'header';
  $value = $form_state['values']['accordion_menu_' . $key];
  preg_match('@^([a-z]+([1-9]|)).*@', strtolower($value), $matches);
  if (empty($matches) || $matches[0] != $matches[1]) {
    form_set_error('accordion_menu_' . $key, t('Enter a valid HTML tag. (' . $key . ')'));
  }
}

/**
 * Implements hook_block_save().
 */
function _accordion_menu_block_save($delta, $edit) {
  if (!empty($delta)) {
    $config = accordion_menu_config();
    unset($config['delta']);
    // Save the block configuration settings.
    foreach ($config as $key => $value) {
      variable_set("accordion_menu_{$delta}_{$key}", $edit['accordion_menu_' . $key]);
    }
  }
}
