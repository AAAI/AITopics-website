<?php

/**
 * @file
 * common.inc
 *
 * Provides some functions for the finder ui.
 */

/**
 * Render text as a link. This will automatically apply an AJAX class
 * to the link and add the appropriate javascript to make this happen.
 *
 * Note: 'html' => true so be sure any text is vetted! Chances are these kinds of buttons will
 * not use user input so this is a very minor concern.
 *
 * @param $text
 *   The text to display as the link.
 * @param $dest
 *   The destination of the link.
 * @param $alt
 *   The alt text of the link.
 * @param $class
 *   Any class to apply to the link. @todo this should be a options array.
 */
function finder_ui_modal_link($text, $dest, $alt = '', $class = '') {
  ctools_include('ajax');
  return ctools_ajax_text_button($text, $dest, $alt, $class, 'ctools-use-modal');
}

/**
 * Get an array of views.
 *
 * @param $append_id
 *   Whether to append the id to the returned display names.
 * @param $filter
 *   A list of IDs in the format view_name:display_key to restrict results by.
 * @param $full
 *   If TRUE will return all the data, rather than just the title.
 * @param $string
 *   String to match against the title to filter results by.
 * @param $exact_string
 *   If TRUE the $string parameter must match exactly.
 * @param $long_key
 *   If TRUE will key array by the title and ID, not just the ID.
 *
 * @return
 *   The array of views.
 */
function finder_ui_views($append_id = FALSE, $filter = NULL, $full = FALSE, $string = '', $exact_string = FALSE, $long_key = FALSE) {
  $views = array();
  $loaded_views = views_get_all_views();
  $filter = $filter ? array_filter($filter) : NULL;
  $finder_views = array(
    'finder_node',
    'finder_user',
  );
  $finder_group = t('Finder Views');
  foreach ((array)$loaded_views as $view_name => $view) {
    foreach ((array)$view->display as $display_key => $display) {
      // Skip this one if it's a 'default' view.
      if ($display_key != 'default') {
        $id = $view_name . ':' . $display_key;
        // Skip this one if it's not 'allowed'.
        if (empty($filter) || in_array($id, $filter)) {
          // Get display title.
          $display_title = finder_ui_views_title($view, $view_name, $display_key, $append_id);
          // Determine whether and what to return.
          $key = $long_key ? $display_title . ($append_id ? '' : ' [' . $id . ']') : $id;
          $type = in_array($view_name, $finder_views) ? $finder_group : ($view->type == 'default' ? t('default Views') : t('Custom Views'));
          if ($string) {
            if (!$exact_string && (stripos($display_title, $string) !== FALSE || stripos($key, $string) !== FALSE)) {
              $views[$type][$key] = $full ? $display : $display_title;
            }
            elseif ($display_title == $string) {
              $views[$type][$key] = $full ? $display : $display_title;
            }
          }
          else {
            $views[$type][$key] = $full ? $display : $display_title;
          }
        }
      }
    }
  }
  if (isset($views[$finder_group])) {
    $finder_views = array($finder_group => $views[$finder_group]);
    unset($views[$finder_group]);
    $views = $finder_views + $views;
  }
  return $views;
}

/**
 * Get the display title for this view display.
 *
 * @param $view
 *  The view object.
 * @param $view_name
 *  The name of the view.
 * @param $display_key
 *  The name of the display to use.
 * @param $append_id
 *  Boolean indicating whether to append a unique id.
 * @return
 *  The display title of this views display.
 */
function finder_ui_views_title($view, $view_name, $display_key, $append_id) {
  if (!$view) {
    return t('Missing view');
  }
  $view->set_display($display_key);
  $display_title = $view->get_title();
  if (!$display_title) {
    // No title, we have to construct a title.
    $display_title = drupal_ucfirst($view_name) . ' ' . drupal_strtolower($view->display[$display_key]->display_title);
  }
  if ($append_id) {
    // Append ID for disambiguation in forms (views displays can have the same title).
    $display_title .= ' [' . $view_name . ':' . $display_key . ']';
  }
  return $display_title;
}

/**
 * Get an array of content types for use in select forms.
 */
function finder_ui_views_displays($view_name) {
  $options = array();
  $view = views_get_view($view_name);
  if ($view) {
    foreach ($view->display as $key => $display) {
      if ($key != 'default') {
        $options[$key] = finder_ui_views_title($view, $view_name, $key, TRUE);
      }
    }
  }
  return $options;
}

/**
 * Modify a PHP setting element.
 *
 * Used for security reasons to prevent an unauthorized user editing the
 * field.  Also makes variables available for the PHP input.
 *
 * @param $element
 *   The original array for the element.
 * @param $variables
 *   Array where keys are variable names (without the $) to make available in
 *   the PHP, and the values are descriptions already passed through t().
 */
function finder_ui_php_setting($element, $variables = array()) {
  if (user_access('use PHP for settings')) {
    $var_list = array();
    foreach ($variables as $variable => $description) {
      $var_list[] = '<em>$' . $variable . '</em> - ' . $description;
    }
    if (!empty($var_list)) {
      $element['#description'] =
        (isset($element['#description']) ? $element['#description'] : '')
        . '<div>'
        . t('Available variables')
        . ':'
        . theme('item_list', array('items' => $var_list))
        . '</div>';
    }
  }
  else {
    $element['#disabled'] = TRUE;
    $element['#prefix'] =
      '<div class="messages warning">'
      . t("You don't have permission to modify %setting.", array('%setting' => $element['#title']))
      . '</div>'
      . (isset($element['#prefix']) ? $element['#prefix'] : '');

    $element['#value'] = $element['#default_value'];

  }
  return $element;
}

/**
 * Get element handlers and convert to options array.
 */
function finder_ui_get_element_options() {
  $element_handlers = finder_element_handlers();
  $element_options = array('' => t('- Select an element type -'));
  if (!empty($element_handlers)) {
    foreach ($element_handlers as $element_handler) {
      $element_options[$element_handler['name']] = $element_handler['title'];
    }
  }
  else {
    drupal_set_message(t('Error: You must activate a finder element handler module such as <em>Finder autocomplete</em> or <em>Finder options</em> to begin.'), 'error');
    return array('' => t('- No element types to choose from -'));
  }
  return $element_options;
}

/**
 * Check if Finder is locked for editing.
 *
 * Also does the unlock message.
 *
 * @param $finder
 *   The finder.
 * @return TRUE or FALSE.
 */
function finder_ui_check_lock(&$finder) {
  $lock = ctools_object_cache_test('finder', $finder->name);
  if (!empty($lock)) {
    if (isset($_GET['breaklock']) && $_GET['breaklock'] == $lock->uid) {
      ctools_object_cache_clear_all('finder', $finder->name);
      drupal_goto($_GET['q']);
    }
    else {
      drupal_set_message(
        t(
          'This finder is being edited by user !user, and is therefore locked from saving by others. This lock is !age old. Click here to <a href="!break">break this lock</a>.',
          array('!user' => theme('username', array('account' => user_load($lock->uid))), '!age' => format_interval(REQUEST_TIME - $lock->updated), '!break' => '?breaklock=' . $lock->uid)
        ),
        'warning',
        FALSE
      );
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Get label for a condition args entry.
 *
 * @param $match
 *   A matches entry.
 * @param $matches
 *  The translated string to replace !matches with.
 * @param $keywords
 *  The translated string to replace !keywords with.
 * @return
 *  The formatted label.
 */
function finder_ui_matches_label($match, $matches, $keywords) {
  $output = $match['name'];
  $output .= '<div class="description">';
  $output .= drupal_ucfirst(strtr(
    $match['description'],
    array('!matches' => $matches, '!keywords' => $keywords)
  ));
  if (isset($match['operator']) || isset($match['value_prefix']) || isset($match['value_suffix'])) {
    $output .= '<div><code>' . strtr(
      'field !operator !prefixvalue!suffix',
      array(
        '!operator' => $match['operator'],
        '!prefix' => $match['value_prefix'],
        '!suffix' => $match['value_suffix'],
      )
    ) . '</code></div>';
  }
  $output .= '</div>';
  return $output;
}

/**
 * Pre render modal forms to add stuff without screwing the tree.
 */
function finder_ui_modal_form_pre_render($form) {
  $children = element_children($form);
  foreach ($children as $child) {
    if ($child != 'actions') {
      $form['form'][$child] = $form[$child];
      unset($form[$child]);
    }
  }
  $form['form']['#prefix'] = '<div class="finder-ui-modal-form-wrapper">';
  $form['form']['#suffix'] = '</div>';
  $form['actions']['#prefix'] = '<div class="finder-ui-modal-buttons">';
  $form['actions']['#suffix'] = '</div>';
  $actions = $form['actions'];
  unset($form['actions']);
  $form['actions'] = $actions;
  return $form;
}

/**
 * Build the render array for the finder info output.
 *
 * @param $finder
 *   The finder.
 * @return
 *   The render array.
 */
function finder_ui_info($finder) {
  $finder_info['title']['#prefix'] = '<div class="finder-ui-title">';
  $finder_info['title']['#markup'] = '<strong>' . check_plain($finder->title) . '</strong>';
  $finder_info['title']['#suffix'] = '</div>';

  $finder_info['name']['#prefix'] = '<div class="finder-ui-name">';
  $finder_info['name']['#markup'] = '<small>' . t('Machine name') . ': ' . check_plain($finder->name) . '</small>';
  $finder_info['name']['#suffix'] = '</div>';

  $view = finder_ui_views_title(views_get_view($finder->views_view), $finder->views_view, $finder->views_display, FALSE);
  $finder_info['view']['#prefix'] = '<div class="finder-ui-view">';
  $finder_info['view']['#markup'] = t('View') . ': ' . check_plain($view);
  $finder_info['view']['#suffix'] = '</div>';

  $finder_info['description']['#prefix'] = '<div class="finder-ui-description">';
  $finder_info['description']['#markup'] = check_plain($finder->description);
  $finder_info['description']['#suffix'] = '</div>';

  return $finder_info;
}

/**
 *  This is the 'exists' callback for the element machine name form element.
 *
 *  However, there does not seem to be any way to properly implement this.
 *  That's OK since creating an element with the same name as an existing one
 *  will just replace the existing one in the interface.
 *  Plus the validate() method of the finder class ensures there are no
 *  duplicate element ids.
 */
function finder_ui_element_exists($value) {}