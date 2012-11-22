<?php

/**
 * @file
 * The finder form.
 */

/**
 * FAPI definition for the finder form.
 *
 * @param &$form
 *   The Forms API form array.
 * @param &$form_state
 *   The Forms API form state.
 * @param $finder
 *   The finder object.
 * @see finder_form_submit()
 */
function finder_form($form, &$form_state, $finder) {
  global $user;

  // Make sure drupal core loads this file when handling this form.
  form_load_include($form_state, 'inc', 'finder', 'includes/form');

  $form_state['finder'] = $finder;

  // see if there is already a form state we should be using.
  $finder_form_state = finder_form_state($finder);
  if (is_array($finder_form_state)) {
    $form_state = array_merge($form_state, $finder_form_state);
  }

  module_invoke_all('finder_form', $finder);

  $form['finder_name'] = array(
    '#type' => 'value',
    '#value' => $finder->name,
  );

  $form['#action'] = url($finder->path);
  if ($user->uid) {
    $form['#token'] = FALSE;
  }
  $form['finder_form'] = array(
    '#weight' => 0,
    '#prefix' => '<div class="finder-form finder-' . $finder->name . '">',
    '#suffix' => '</div>',
  );

  $header = $finder->setting('header');

  if (!empty($header['value']) && isset($header['format'])) {
    $form['finder_form']['#prefix'] .=
      '<div class="prefix">'
      . check_markup(
          $header['value'],
          $header['format'],
          FALSE
        )
      . '</div>';
  }
  $max_weight = 0;

  foreach ($finder->elements as $element) {

    $max_weight = max($max_weight, $element->weight);
    if (isset($form_state['values'][$element->id])) {
      $element_default = $form_state['values'][$element->id];
    }
    else {
      $element_default = $finder->esetting($element, 'default_value');
    }

    $form['finder_form'][$element->id] = array(
      '#title' => check_plain($element->title),
      '#weight' => $element->weight,
      '#description' => check_markup($finder->esetting($element, 'description')),
      '#default_value' => $element_default,
      '#required' => $finder->esetting($element, 'required'),
      '#executes_submit_callback' => TRUE,
      '#field_prefix' => check_markup($finder->esetting($element, 'field_prefix')),
      '#field_suffix' => check_markup($finder->esetting($element, 'field_suffix')),
      '#title_display' => $finder->esetting($element, 'title_display', 'before'),
    );

    $header = $finder->esetting($element, 'header');
    if (!empty($header['value']) && isset($header['format'])) {
      $form['finder_form'][$element->id]['#prefix'] = check_markup($header['value'], $header['format'], FALSE);
    }

    $footer = $finder->esetting($element, 'footer');
    if (!empty($footer['value']) && isset($footer['format'])) {
      $form['finder_form'][$element->id]['#prefix'] = check_markup($footer['value'], $footer['format'], FALSE);
    }

    ctools_include('plugins');
    if ($handler_function = ctools_plugin_get_function($element->element_handler, 'element callback')) {
      $handler_function($element, $form['finder_form'][$element->id]);
    }
  }

  $form['finder_form']['actions']['#weight'] = $max_weight + 100;

  if ($finder->setting('find_button')) {

    $form['finder_form']['actions']['find'] = array(
      '#type' => 'submit',
      '#name' => 'find',
      '#value' => $finder->setting('find_text'),
    );

    if ($finder->setting('ajax')) {

      $wrapper = $finder->build_id;
      $form_state['ajax_display'] = 'block';

      // Check if the 'ajax_remote' setting is on, that we are on the finder page, and that we are in a block.
      if ($finder->setting('ajax_remote') && strpos($_GET['q'], $finder->path) === 0 && $finder->build_display === 'block') {
        // We are using the block as a remote control for the page.  Change the wrapper for Ajax purposes.
        $wrapper = 'finder-page-' . $finder->name . '-wrapper';
        $form_state['ajax_display'] = 'page';
      }

      $form['finder_form']['actions']['find']['#ajax'] = array(
        'callback' => 'finder_ajax',
        'wrapper' => $wrapper,
        'method' => 'replace',
        'effect' => $finder->setting('ajax_effect'),
      );

    }

  }

  if ($finder->setting('go_button')) {
    $form['finder_form']['actions']['go'] = array(
      '#type' => 'submit',
      '#name' => 'go',
      '#value' => $finder->setting('go_text'),
    );
  }

  $footer = $finder->setting('footer');
  if (!empty($footer['value']) && isset($footer['format'])) {
    $form['finder_form']['#suffix'] =
      '<div class="suffix">'
      . check_markup(
          $footer['value'],
          $footer['format'],
          FALSE
        )
      . '</div>' . $form['finder_form']['#suffix'];
  }

  $form['#submit'] = array(
    'finder_form_submit',
  );
  $form['#validate'] = array(
    'finder_form_validate',
  );

  return $form;

}

/**
 * Validate function for finder form.
 *
 * Implements the 'validate_empty' functionality.
 *
 * @see finder_form()
 */
function finder_form_validate($form, &$form_state) {
  $finder = $form_state['finder'];
  if ($finder->setting('validate_empty')) {
    $all_empty = TRUE;
    foreach ($finder->elements as $element) {
      if (!empty($form_state['values'][$element->id])) {
        $all_empty = FALSE;
        break;
      }
    }
    if ($all_empty) {
      form_set_error(
        'form',
        t(
          'Please complete the %finder form.',
          array('%finder' => $finder->title)
        )
      );
    }
  }
}

/**
 * Submit function for finder form.
 *
 * Adds some needed data to $form_state and calls finder_form_state().
 *
 * @see finder_form()
 */
function finder_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $finder = $form_state['finder'];
  $form_state['storage']['finder'] = $finder;
  $form_state['storage']['values'] = $form_state['values'];
  $form_state['storage']['finished'] = TRUE;
  if (!$finder->setting('ajax') || $form_state['clicked_button']['#name'] != 'find') {
    finder_form_state($finder, $form_state);
  }
}

/**
 * Statically 'get' or 'set' the FAPI form state in a per-finder cache.
 *
 * When used to 'set' the form state it will also check to see if a redirect
 * is required to go to the results path with arguments.  When used to 'get'
 * the form state it will check the static cache for a stored form state, then
 * it will check the session for a form state carried over from another page,
 * and finally it will attempt to build a form state out of the path arguments.
 *
 * @param $finder
 *   The finder.
 * @param $form_state
 *   The Forms API form state (if supplied, will 'set' the form state).
 * @return
 *   A copy of the Forms API form state.
 */
function finder_form_state($finder, $form_state = NULL) {

  static $finder_form_state = NULL;

  if ($form_state) { // we are setting the form_state in a submit.
    // last chance for modules to intefere before potential redirect.
    drupal_alter('finder_form_state', $form_state, $finder);
    $finder_form_state[$finder->name] = $form_state;

    // handle URL stuff.
    if ($finder->setting('url') == 'enabled') {
      $sep = $finder->setting('url_delimiter');
      $empty_symbol = $finder->setting('url_empty') ? $finder->setting('url_empty') : ' ';
      $query = array();
      foreach ($finder->elements as $element) {
        $keyword = (array)$form_state['values'][$element->id];
        foreach ($keyword as $k => $v) {

          // Handle forward slashes in input.
          $v = str_replace("/", "%2f%2f", $v);

          // Handle new lines in input.
          $v = str_replace("\r\n", "\n", $v);
          $v = str_replace("\n", urlencode("***BREAK***"), $v);

          if (strpos($v, $sep) !== FALSE) {
            $v = '"' . $v . '"';
          }
          $keyword[$k] = $v ? trim($v) : $empty_symbol;
        }
        $keywords[$element->id] = implode(',', $keyword);
      }
      if (!$form_state['storage']['finished']) {
        $query['finished'] = '0';
      }
      if ($form_state['clicked_button']['#name'] == 'go' && $form_state['storage']['finished']) {
        $query['go'] = '1';
      }
      finder_form_goto($sep, $finder->path . '/' . implode('/', $keywords), $query);
    }

  }
  elseif (!isset($finder_form_state[$finder->name])) {

    if ($finder->setting('url') == 'disabled' && isset($_GET['finder'])) {  // check the session
      $finder_form_state[$finder->name] = $_SESSION['finder'][$_GET['finder']];
    }
    elseif (!isset($_GET['finder']) &&
            strlen($finder->path) < strlen($_GET['q']) &&
            stripos($_GET['q'], $finder->path) === 0) { // check the URL

      // Get the seperator for element values - this is usually a comma.
      $sep = $finder->setting('url_delimiter');

      // Get the finder arguments.
      $args = str_replace($finder->path . '/', '', $_GET['q']);

      // Handle new lines from input.
      $args = str_replace("***BREAK***", "\n", $args);

      // Forward slashes were encoded as double forward slashes.  We must temporarily replace those here to prevent the explode() affecting this.
      // Rawurlencode() doesn't work but can be fixed using Apache's AllowEncodedSlashes Directive, but how do you tell people to switch that on?
      $args = str_replace('//', '[-finder-forward-slash-]', $args);

      // Double seperators break the $csv_regex below, and I'm not clever enough to fix the regex.
      $args = str_replace($sep . $sep, '[-finder-double-sep-]', $args);

      // Break arguments apart into a string for each element.
      $args = explode('/', $args);

      $form_state['storage']['finished'] = TRUE;

      $empty_symbol = !$finder->setting('url_empty') ? $finder->setting('url_empty') : ' ';
      $csv_regex = "/" . $sep . "(?!(?:[^\\\"" . $sep . "]|[^\\\"]" . $sep . "[^\\\"])+\\\")/";

      $args_key = 0;
      foreach ($finder->elements as $key => $element) {
        $keywords = preg_split($csv_regex, $args[$args_key++]);

        foreach ($keywords as $k => $v) {
          $v = str_replace('[-finder-double-sep-]', $sep . $sep, $v);
          $v = str_replace('[-finder-forward-slash-]', '/', $v);
          $v = str_replace(urlencode($sep), $sep, trim($v));
          if (trim($v) == trim($empty_symbol)) {
            $v = NULL;
          }
          if (strpos($v, $sep) !== FALSE && $v[0] == '"' && $v[strlen($v) - 1] == '"') {
            $v = substr($v, 1, strlen($v) - 2);
          }
          unset($keywords[$k]);
          if ($v) {
            $keywords[$v] = $v;
          }
        }
        if (count($keywords) === 1) {
          $keywords = current($keywords);
        }
        elseif (!count($keywords)) {
          $keywords = NULL;
        }
        $form_state['values'][$element->id] = $keywords;
      }
      $finder_form_state[$finder->name] = $form_state;
    }

  }
  return $finder_form_state[$finder->name];
}

/**
 * Redirect from a finder form.
 *
 * The difference between this and drupal_goto() is that this undoes the
 * encoding of the arguments seperator, as such encoding inteferes with finder.
 *
 * @param $sep
 *   The arguments seperator string.
 * @param $path
 *   A Drupal path or a full URL.
 * @param $query
 *   A query string component, if any.
 * @param $fragment
 *   A destination fragment identifier (named anchor).
 * @param $http_response_code
 *   Valid values for an actual "goto" as per RFC 2616 section 10.3 are:
 *   - 301 Moved Permanently (the recommended value for most redirects)
 *   - 302 Found (default in Drupal and PHP, sometimes used for spamming search
 *         engines)
 *   - 303 See Other
 *   - 304 Not Modified
 *   - 305 Use Proxy
 *   - 307 Temporary Redirect (alternative to "503 Site Down for Maintenance")
 *   Note: Other values are defined by RFC 2616, but are rarely used and poorly
 *   supported.
 * @see drupal_goto()
 */
function finder_form_goto($sep, $path = '', $query = NULL, $fragment = NULL, $http_response_code = 302) {

  if (isset($_REQUEST['destination'])) {
    extract(parse_url(urldecode($_REQUEST['destination'])));
  }
  elseif (isset($_REQUEST['edit']['destination'])) {
    extract(parse_url(urldecode($_REQUEST['edit']['destination'])));
  }

  $url = url($path, array('query' => $query, 'fragment' => $fragment, 'absolute' => TRUE));

  // custom changes - undo separator encoding
  $url = str_replace(urlencode($sep), $sep, $url);

  // Remove newlines from the URL to avoid header injection attacks.
  $url = str_replace(array("\n", "\r"), '', $url);

  // Allow modules to react to the end of the page request before redirecting.
  // We do not want this while running update.php.
  if (!defined('MAINTENANCE_MODE') || MAINTENANCE_MODE != 'update') {
    module_invoke_all('exit', $url);
  }

  // Even though session_write_close() is registered as a shutdown function, we
  // need all session data written to the database before redirecting.
  session_write_close();

  header('Location: ' . $url, TRUE, $http_response_code);

  // The "Location" header sends a redirect status code to the HTTP daemon. In
  // some cases this can be wrong, so we make sure none of the code below the
  // drupal_goto() call gets executed upon redirection.
  exit();
}