<?php

/**
 * @file
 * Authcache Ajax Callback (authcache.php)
 *
 * The Authcache Ajax phase, included by ../authcache.inc during
 * Drupal's index.php EARLY_PAGE_CACHE bootstrap. Executed within
 * _drupal_bootstrap() function.
 *
 * Calls functions as defined in GET request: _authcache_{key} => value(s)
 * (Uses Authcache:ajax JSON from authcache.js)
 * Outputs JSON object of values returned by functions, if any.
 *
 * DO NOT MODIFY THIS FILE!
 * For custom functions, use "authcache_custom.php"
 *************************************************************/

// Attempt to prevent "cross-site request forgery" by requiring a custom header.
if (!isset($_SERVER['HTTP_AUTHCACHE'])) {
  header($err = 'HTTP/1.1 400 Bad Request (No Authcache Header)');
  die($err);
}

// GET is faster than POST, but has a character limit and less secure (easier to log)
$SOURCE = ($_SERVER['REQUEST_METHOD'] == 'POST') ? $_POST : $_GET;

// Set current page for bootstrap
if (isset($_POST['q'])) {
  $_GET['q'] = $_POST['q'];
}

// Continue Drupal bootstrap. Establish database connection and validate session.
drupal_bootstrap(DRUPAL_BOOTSTRAP_SESSION);

// If user session is invalid/expired, delete Authcache-defined cookies.
global $user;
if (!$user->uid && isset($_COOKIE['authcache'])) {
  setcookie('drupal_user', "", time() - 86400, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
  setcookie('drupal_uid', "", time() - 86400, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
  setcookie('authcache', "", time() - 86400, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
  setcookie('nocache', "", time() - 86400, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
}

// Initialize configuration variables, using values from settings.php if available.
$conf = variable_init(isset($conf) ? $conf : array());

$is_ajax_authcache = true;

// Add your own custom functions to authcache_custom.php and place in your settings.php directory.
if (file_exists($authcache_custom_inc = conf_path() . '/authcache_custom.php')) {
  include $authcache_custom_inc;
}
elseif (file_exists($authcache_custom_inc = dirname(__FILE__) . '/authcache_custom.php')) {
  include $authcache_custom_inc;
}

$response = NULL; // Ajax response

// Loop through GET or POST key/value pairs, call functions, and return results.
if (is_array($SOURCE)) { // GET or POST
  foreach ($SOURCE as $key => $value) {
    $func_name = "_authcache_$key";
    if (function_exists($func_name)) {
      $r = $func_name($value);
      if ($r !== NULL) {
        $response[$key] = $r;
      }
    }
  }
}


// Calculate database benchmarks, if enabled.
if (variable_get('dev_query', FALSE)) {
  $response['db_queries'] = _authcache_dev_query();
}

// Should browser cache this response? (See authcache_example for possible usage).
// This must be placed after bootstrap since drupal_page_header()
// will define header to make pages not cache
if (isset($SOURCE['max_age']) && is_numeric($SOURCE['max_age'])) {
  // Tell browser to cache response for 'max_age' seconds
  header("Cache-Control: max-age={$SOURCE['max_age']}, must-revalidate");
  header('Expires: ' . gmdate('D, d M Y H:i:s', time()+24*60*60) . ' GMT'); // 1 day
}

header("Content-type: text/javascript");

if (function_exists('json_encode')) { // Found in PHP 5.2
  print json_encode($response);
}
else {
  require_once './includes/common.inc'; // drupal_to_js
  print drupal_to_js($response);
}


//
// Drupal Core functions
//

/**
 * Form tokens (prevents CSRF)
 *
 * form_token_id is a hidden field added by authcache.module's hook_form_alter()
 * @see form.inc
 */
function _authcache_form_token_id($vars) {
  include_once './includes/common.inc';
  foreach ($vars as $form_token_id) {
    switch ($form_token_id) {
      case 'contact_mail_page':
        global $user;
        $tokens[$form_token_id] = drupal_get_token($user->name . $user->mail);
        break;
      default;
        $tokens[$form_token_id] = drupal_get_token($form_token_id);
        break;
    }
  }
  return $tokens;
}


/**
 * Node history
 * @see node.module
 */
function _authcache_node_history($nid) {
  include_once './modules/node/node.module';

  // Update the 'last viewed' timestamp of the specified node for current user.
  node_tag_new($nid);

  // Retrieves the timestamp at which the current user last viewed the specified node
  return node_last_viewed($nid);
}

/**
 * Node counter and access log statistics
 * @see statistics.module
 */
function _authcache_statistics($vars) {
  include_once './modules/statistics/statistics.module';
  statistics_exit();
}

/**
 * Number of new forum topics for user
 * @see forum.module
 */
function _authcache_forum_topic_new($vars) {
  global $user;
  $new = array();

  drupal_bootstrap(DRUPAL_BOOTSTRAP_PATH);
  include_once './modules/node/node.module';  // Need NODE_NEW_LIMIT definition
  include_once './modules/forum/forum.module';
  include_once './modules/filter/filter.module'; // XSS filter for l()

  foreach ($vars as $tid) {
    $new_topics = (int) _forum_topics_unread($tid, $user->uid);
    if ($new_topics) {
      $new[$tid] = l(format_plural($new_topics, '1 new', '@count new'), "forum/$tid", array('fragment' => 'new'));
    }
  }
  return $new;
}

/**
 * Number of new topic replies for user or topic is unread
 * @see forum.module
 */
function _authcache_forum_topic_info($vars) {
  global $user;
  $info = array();

  drupal_bootstrap(DRUPAL_BOOTSTRAP_PATH);
  include_once './modules/node/node.module';  // Need NODE_NEW_LIMIT definition
  include_once './modules/forum/forum.module';
  include_once './modules/comment/comment.module';

  foreach ($vars as $nid => $timestamp) {
    $history = _forum_user_last_visit($nid);
    $new_topics = (int)comment_num_new($nid, $history);
    if ($new_topics) {
      $info[$nid] = format_plural($new_topics, '1 new', '@count new');
    }
    elseif ($timestamp > $history) { // unread
      $info[$nid] = 1;
    }
  }

  return $info;
}


/**
 * Return default form values for site contact form
 * @see contact.module
 */
function _authcache_contact($vars) {
  global $user;
  return array('name' => $user->name, 'mail' => $user->mail);
}

/**
 * Get poll results/form for user
 * Response will be cached.
 * @see poll.module
 */
function _authcache_poll($vars) {
  // FULL bootstrap required in case custom theming is used
  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
  $node = node_load($vars['nid']);
  $node = poll_view($node, TRUE, FALSE, $vars['block']);
  $output = $node->content['body']['#value'];

  return array(
    'nid' => $vars['nid'],
    'block' => $vars['block'],
    'html' => $output,
  );
}

/**
 * Render primary & secondary tabs.
 * Response will be cached.
 * @see menu.inc
 */
function _authcache_menu_local_tasks($vars) {
  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
  return theme('menu_local_tasks');
}

/**
 * Render blocks. Grab from cache if available.
 * @param <array> $blocks
 *   [block id] => [block cache id]
 * @see block.module
 */
function _authcache_blocks($blocks) {
  global $user, $theme_key;
  $return = array();

  foreach ($blocks as $block_id => $block_cid) {
    // If block cache is per user, then specify current user id.
    $block_cid = preg_replace('/:u.[0-9]+/', ":u.$user->uid", $block_cid);

    // Validate user roles with block visibility roles.
    // (In case someone is trying to hack into viewing certain blocks.)
    if (strpos($block_cid, ':r.') !== FALSE) {
      $matches = array();
      preg_match('/:r.([0-9,]+)/', $block_cid, $matches);
      if (isset($matches[1])) {
        // Cache id is built using exact user roles, so a direct comparison works. @see _block_get_cache_id().
        if ($matches[1] != implode(',', array_keys($user->roles))) {
          continue;
        }
      }
    }

    // Check cache_block bin first
    if ($block_cached = cache_get($block_cid, 'cache_block')) {

      if (variable_get('authcache_debug_all', FALSE)) {
        $block_cached->data['content'] .= '<!-- block cached -->';
      }

      $block_view = $block_cached->data;
    }
    else {
      // Full bootstrap required for correct theming.
      drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
      init_theme();

      $id = explode('-', $block_id, 2);
      $block_view = module_invoke($id[0], 'block', 'view', $id[1]);
    }

    $return[$block_id] = $block_view;
  }

  return $return;
}

//
// Authcache reserved/internal functions
//

function _authcache_q($vars) { }        // query string
function _authcache_max_age($vars) { }  // cache time (seconds)
function _authcache_time($vars) { }     // cache invalidation

/**
 * Database benchmarks for Authcache Ajax phase
 */
function _authcache_dev_query() {
  global $queries;
  if (!$queries) return;

  $time_query = 0;
  foreach ($queries as $q) {
    $time_query += $q[1];
  }
  $time_query = round($time_query * 1000, 2); // Convert seconds to milliseconds
  $percent_query = round(($time_query / timer_read('page')) * 100);

  return count($queries) . " queries @ {$time_query} ms";
}


//
// Contributed Module functions
//


/**
 * Example of customized block info being returned
 * @see authcache_example.module
 */
function _authcache_authcache_example($vars) {
  include_once './includes/common.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_PATH); // Use FULL if needed for additional functions

  include_once dirname(drupal_get_filename('module', 'authcache_example')) . '/authcache_example.module';
  return authcache_example_display_block_0();
}



/**
 * @todo Add support for additional contributed modules!
 ********************************************************/
