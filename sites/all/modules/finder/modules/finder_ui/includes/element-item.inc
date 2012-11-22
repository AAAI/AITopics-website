<?php
/**
 * @file
 * element-item.inc
 *
 * Provides the Finder UI element item related functions.
 */

finder_inc('common', 'finder_ui');

/**
 * Finder UI element item page.
 *
 * @param $finder
 *  The finder object.
 * @param $feid
 *  The element machine name.
 * @param $item_key
 *  The item key.
 * @param $js
 *  The $js param required by ctools.
 * @return
 *  The output.
 */
function finder_ui_element_item_page($finder, $feid, $item_key, $js) {
  ctools_include('modal');
  ctools_include('ajax');
  ctools_include('object-cache');
  finder_inc('element', 'finder_ui');
  $finder = ctools_object_cache_get('finder', $finder->name);

  $finder_ui_element = finder_ui_element($finder, $feid);
  $title = $finder_ui_element['items'][$item_key]['#item']['#title'];
  $item = $finder_ui_element['items'][$item_key];

  $form_state = array(
    'title' => $title,
    'ajax' => $js,
    'finder' => $finder,
    'feid' => $feid,
    'item_key' => $item_key,
    'item' => $item,
  );

  $output = ctools_modal_form_wrapper('finder_ui_element_item_form', $form_state);

  if ($form_state['executed'] && $js) {
    if ($form_state['triggering_element']['#parents'][1] == 'submit') {
      $finder = ctools_object_cache_get('finder', $finder->name);
      $element = &$finder->elements[$feid];
      foreach ($form_state['values'] as $key => $value) {
        if ($key != 'settings') {
          $element->{$key} = $value;
        }
      }
      foreach ($form_state['values']['settings'] as $key => $value) {
        $element->settings[$key] = $value;
      }
      ctools_object_cache_set('finder', $finder->name, $finder);
      $finder_ui_element = finder_ui_element($finder, $feid);
      $item = $finder_ui_element['items'][$item_key];
      $commands = array();
      $output = finder_ui_element_item($finder, $feid, $item_key, $item);
      $commands[] = ajax_command_html('#finder-ui-element-' . $feid . '-item-' . $item_key, $output);
      $commands[] = ajax_command_css('#edit-changed', array('display' => 'block'));
    }
    $commands[] = ctools_modal_command_dismiss();
    print ajax_render($commands);
    exit;
  }

  if ($js) {
    print ajax_render($output);
    exit;
  }

  // Otherwise, just return the output.
  return $output;
}

/**
 * Finder UI element item form.
 *
 * @param $form
 *  The form array.
 * @param &$form_state.
 *  The form state array.
 * @return
 *  The form.
 */
function finder_ui_element_item_form($form, &$form_state) {
  finder_inc('item', 'finder_ui');
  return finder_ui_item_form($form, $form_state);
}

/**
 * Finder UI element item form submit.
 *
 * @param $form
 *  The form array.
 * @param &$form_state
 *  The form state array.
 * @return
 *  The form.
 */
function finder_ui_element_item_form_submit($form, &$form_state) {
  finder_inc('item', 'finder_ui');
  return finder_ui_item_form_submit($form, $form_state);
}

/**
 * Finder UI element item.
 *
 * @param $finder
 *  The finder object.
 * @param $feid
 *  The element machine name.
 * @param $item_key
 *  The item key.
 * @param $item
 *  The item.
 * @return
 *  The output.
 */
function finder_ui_element_item($finder, $feid, $item_key, $item) {
  $element = &$finder->elements[$feid];

  $output = $item['#item']['#title'] ? '<span class="label">' . $item['#item']['#title'] . ':</span>' : '';
  $value = strip_tags($item['#item']['#value']);

  // Ensure the value works out to something clickable.
  if (!$item['#item']['#value']) {
    // There is no original value.
    $value = '<em>' . t('empty') . '</em>';
  }
  elseif (!trim($value)) {
    // There is a value, but it is dodgy to display.
    $value = t('Yes');
  }

  $output .= finder_ui_modal_link($value, 'admin/structure/finder/list/' . $finder->name . '/element-item/' . $feid . '/' . $item_key . '/nojs', t('Edit'), 'ctools-modal-finder-modal-style');
  if (!empty($item['#item']['#extra'])) {
    $output .= '<span class="extra">' . filter_xss($item['#item']['#extra']) . '</span>';
  }
  return $output;
}

