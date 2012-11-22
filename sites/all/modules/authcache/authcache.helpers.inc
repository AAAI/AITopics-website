<?php

/**
 * @file
 * Helper functions for the Authcache module (no Drupal hooks here).
 */


/**
 * Should the current page be cached?
 */
function _authcache_is_cacheable() {
  global $user, $_authcache_debug_info, $is_ajax_authcache, $_authcache_info, $conf;
  // Do not cache if...
  
  if (!empty($_POST)) {
    $_authcache_info['no_cache_reason'] = 'POST request';
    return FALSE;
  }
  
  // disabled this, as it's covered by the "role caching" test
  //if ($user->uid == 1) {
  //  return 'User is Superadmin (user id 1)';
  //}
  
  if ($user->uid && !isset($_COOKIE['has_js'])) {
    $_authcache_info['no_cache_reason'] = 'Users with JS disabled'; 
    return FALSE;
  }
  
/*
  if (isset($_COOKIE['nocache'])) {
    $_authcache_info['no_cache_reason'] = 'Caching disabled for session (nocache cookie set)';
    return FALSE;
  }
*/
  
  if ($is_ajax_authcache) {
    $_authcache_info['no_cache_reason'] = 'Authcache Ajax request';
    return FALSE;
  }

  if (isset($_SESSION['messages']) || drupal_set_message() != NULL) {
    $_authcache_info['no_cache_reason'] = 'User notification shown (drupal_set_message)';
    return FALSE;
  }

  if (($ar = explode('?', basename(request_uri()))) && substr(array_shift($ar), -4) == '.php') {
    $_authcache_info['no_cache_reason'] = 'PHP files (cron.php, update.php, etc)';
    return FALSE;
  }

  if (empty($conf['cache_backends'])) {
    $_authcache_info['no_cache_reason'] = 'no cache_backends defined in settings.php - please see README.txt';
    return FALSE;    
  }

  if (variable_get('authcache_noajax', FALSE) 
      && isset($_SERVER['HTTP_X_REQUESTED_WITH']) 
      && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest'
    ) {
    $_authcache_info['no_cache_reason'] = 'Ajax request';
    return FALSE;
  }
  
  // Check if caching is enabled for user's role
  if (!_authcache_is_account_cacheable($user)) {
    $_authcache_info['no_cache_reason'] = 'Caching disabled for this users role';
    return FALSE;
  }


  $alias = drupal_get_path_alias($_GET['q']);
  $path  = drupal_get_normal_path($_GET['q']); // Normalize path

  // Now check page caching settings, defined by the site admin
  $pagecaching = variable_get('authcache_pagecaching', array(
    array(
      'option' => 0,
      'pages' => AUTHCACHE_NOCACHE_DEFAULT,
      'roles' => array(DRUPAL_ANONYMOUS_RID),
    ),
  )
  );
  $i = 0;
  $is_cacheable = TRUE;
  
  foreach ($pagecaching as $page_rules) {
     
    // Do caching page roles apply to current user?
    $keys = array_keys($user->roles);

    // Match up authenticated roles correctly in case "authenticated user" is unchecked
    if (in_array(DRUPAL_AUTHENTICATED_RID, $keys) && !in_array(DRUPAL_AUTHENTICATED_RID, $page_rules['roles'])) {
      unset($keys[array_search(DRUPAL_AUTHENTICATED_RID, $keys)]);
    }

    $extra_roles = array_diff($keys, $page_rules['roles']);
    if (empty($extra_roles)) {
      switch ($page_rules['option']) {
        case '0': // Cache every page except the listed pages.
        case '1': // Cache only the listed pages.
          $regexp = '/^(' . preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/', '/(^|\|)\\\\<front\\\\>($|\|)/'), array('|', '.*', '\1' . preg_quote(variable_get('site_frontpage', 'node'), '/') . '\2'), preg_quote($page_rules['pages'], '/')) . ')$/';
          if (!(!($page_rules['option'] xor preg_match($regexp, $alias)))) {
            //TODO: find a clearer way to refactor the above expression
            $_authcache_info['no_cache_reason'] = 'Caching disabled by Page Ruleset ' . $i;
            $is_cacheable = FALSE;
          }
          break;
        case '2': // Cache pages for which the following PHP code returns TRUE
          if (module_exists('php')) {
            $result = php_eval($page_rules['pages']);
          }
          if (empty($result)) {
            $_authcache_info['no_cache_reason'] = 'Caching disabled by PHP Rule';
            $is_cacheable = FALSE;
          }
          break;
        default:
          break;
      }
    }
    $i++;
  }

  return $is_cacheable;
}

/**
 * Should user account receive cached pages?
 */
function _authcache_is_account_cacheable($account = FALSE) {
  if (!$account) {
    global $user;
    $account = $user;
  }

  // Check if caching is enabled for user's role
  $cache_roles = variable_get('authcache_roles', array());

  // Anonymous
  if (!$account->uid && !in_array(DRUPAL_ANONYMOUS_RID, $cache_roles)) {
    return FALSE;
  }
  // Logged-in
  elseif ($account->uid) {
    unset($cache_roles[DRUPAL_ANONYMOUS_RID]);
    $extra_roles = array_diff(array_keys($account->roles), $cache_roles);

    // Admin selected a role, but did not selected "authenticated user"
    if ($extra_roles && count($extra_roles) == 1 && count($account->roles) > 1 && $extra_roles[1] == DRUPAL_AUTHENTICATED_RID  ) {
      return TRUE;
    }

    if (!empty($extra_roles)) {
      return FALSE;
    }
  }

  return TRUE;
}


/**
 * Save page to cache
 *
 * Called using PHP's register_shutdown_function().
 * This is better than an ob_start callback since global variables
 * are not deconstructed and the function is executed later.
 */
function _authcache_shutdown_save_page() {

  if (empty($_SERVER['REQUEST_METHOD'])) return; // don't do this if running in CLI mode (ie drush)

  global $user, $base_root, $_authcache_is_cacheable, $_authcache_info;

  $content_type = _authcache_get_content_type();

  // Find user-specified non-html pages.
  $alias = drupal_get_path_alias($_GET['q']);
  $regexp = '/^(' . preg_replace(array('/(\r\n?|\n)/', '/\\\\\*/', '/(^|\|)\\\\<front\\\\>($|\|)/'), array('|', '.*', '\1' . preg_quote(variable_get('site_frontpage', 'node'), '/') . '\2'), preg_quote(variable_get('authcache_nonhtml', AUTHCACHE_NONHTML_DEFAULT), '/')) . ')$/';
  $is_cached_nonhtml = (
    (preg_match($regexp, $alias)
    || (in_array($content_type, array('text/javascript', 'text/plain', 'application/xml', 'application/atom+xml'))) // Dynamic JS, text, XML
    || (isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest')) // Ajax
  );


  // Final check, in case variable was modified;
  
  /*if ($_authcache_is_cacheable !== true) {
    global $_authcache_debug_info;

    if (!$_authcache_debug_info) {
      $_authcache_debug_info = '$_authcache_is_cacheable = false';
    }

    print '<script type="text/javascript">
    Authcache.info = ' . drupal_json_encode(array('Reason' => $_authcache_debug_info)) . ';
    </script>';
    return;
  }*/
  // end TODO:D7

  //
  // Last minute checks to cancel saving to page cache.
  //
  if ((!$is_cached_nonhtml && strpos($content_type,'text/html' != 0))) {
    $_authcache_info['no_cache_reason'] = 'Only cache allowed HTTP content types (HTML, JS, etc)';
    $_authcache_is_cacheable = FALSE;
  }

  if (variable_get('authcache_http200', FALSE) && _authcache_get_http_status() != 200) {
    $_authcache_info['no_cache_reason'] = 'Don`t cache 404/403s/etc if specified by admin';
    $_authcache_is_cacheable = FALSE;
  } 

    
  /* simg: headers_sent() seems to always true since php sets a header. removing since I can't see what benefit this achieves?
   if (headers_sent()) {
    $_authcache_info['headers_list'] = implode(' - ',headers_list());
    $_authcache_info['no_cache_reason'] = 'Don`t cache private file transfers or if headers were unexpectly sent.';
    $_authcache_is_cacheable = FALSE ; 
  }*/

  if (empty($_SERVER['REQUEST_METHOD'])) {
    $_authcache_info['no_cache_reason'] = 'Don`t cache if running as a CLI script';
    $_authcache_is_cacheable = FALSE;
  }

  // Make sure "Location" redirect isn't used
  foreach (headers_list() as $header) {
    if (strpos($header, "Location:") === 0) {
      return;
    }
  }

  // Don't cache pages with PHP errors (Drupal can't catch fatal errors)
  if (function_exists('error_get_last') && $error = error_get_last()) {
    switch ($error['type']) {
      // Ignore these errors:
      case E_NOTICE: // run-time notices
      case E_USER_NOTICE: // user-generated notice message
      case E_DEPRECATED: // run-time notices
      case E_USER_DEPRECATED: // user-generated notice message
        break;
      default:
        // Let user know there is PHP error and return
        /*print '<script type="text/javascript">
        Authcache.info = ' . drupal_json_encode(array_merge(array('Reason' => 'PHP Error'), error_get_last())) . ';
        </script>';
        return;*/
        $_authcache_info['no_cache_reason'] = 'PHP Error:' . error_get_last();
        $_authcache_is_cacheable = FALSE;
        break;
    }
  }

  // Cache key, constructed from user role and URL
  $uid = (isset($_COOKIE['drupal_uid'])) ? $_COOKIE['drupal_uid']:0; //drupal_uid should always be set is general operation. sometimes during "set up" this isn't the case, so set anonymous user to prevent error messages
  $user = user_load($uid); //TODO: find out why $user is not already set prior to this point
  $key = _authcache_key($user) . $base_root . request_uri();
  
  // Authcache info JSON
  $_authcache_info = array_merge($_authcache_info, array(
    'page_render' => timer_read('page'), // Benchmark
    'page_queries' => '-1', // Database benchmark, if enabled
    'cache_render' => '-1', // Filled by cookie via JS on cache request
    'cache_uid' => $user->uid, // Required by JS for HTML updates
    'cache_class' => variable_get('cache_class_cache_page', 'unknown'),
    'cache_time' => REQUEST_TIME, // Required by JS for HTML updates,
    'is_cacheable' => $_authcache_is_cacheable,
    'cache_key' => $key,
  ));

  // Hide sensitive info from anonymous users
  if (!$user->uid && !variable_get('authcache_debug_all', FALSE)) {
    unset($_authcache_info['cache_uid']);
    unset($_authcache_info['cache_inc']);
  }

  // Database benchmarks
  //todo:D7 make this work (probably needs moving back into php shutdown function?)
  global $_authcache_queries;
  if (isset($_authcache_queries) && variable_get('dev_query', 0)) {
    $time_query = 0;
    foreach ($_authcache_queries as $q) {
      $time_query += $q[1];
    }
    $time_query = round($time_query * 1000, 2); // Convert seconds to milliseconds
    $percent_query = round(($time_query / $_authcache_info['page_render']) * 100);

    $_authcache_info['page_queries'] = count($_authcache_queries) . " queries @ {$time_query} ms ({$percent_query}%)";
  }
  else {
    unset($_authcache_info['page_queries']);
  }

  // JSON to send via Ajax
  // The "q" key is need during Ajax phase
  $authcache_ajax = array(
    'q' => $_GET['q'],
  );

  // Invoke hook_authcache_info() operation to allow modules to modify info array
  _authcache_invoke_hook('authcache_info', $_authcache_info);

  // Invoke hook_authcache() operation to allow modules to modify ajax array
  _authcache_invoke_hook('authcache_ajax', $authcache_ajax);
  
  // Get buffered HTML
  $buffer = ob_get_contents();
 //  ob_end_clean(); 

  if (substr($buffer, 0, 5) == '<?xml') {
    $is_cached_nonhtml = TRUE; // don't append JS to XML pages
  } 
  
  // Add JS for debug mode?
  if ((variable_get('authcache_debug_all', FALSE)
      || ($user->uid && ($debug_users = variable_get('authcache_debug_users', array()))))
      && !$is_cached_nonhtml
  ) {
    $js  = '<script type="text/javascript">
      if (!Authcache) Authcache = {};
      Authcache.info = ' . drupal_json_encode($_authcache_info) .'</script>';
      
    
    
    // Insert JSON before </body>, otherwise just append
    if (strripos($buffer, '</body>') !== FALSE) {
      $buffer = str_replace('</body>', $js . '</body>', $buffer);
    }
    else {
      $buffer .= $js;
    }
    
    print $js; //since by this point the drupal page has already been sent to the browser
    // Also see authcache_authcache_info() for user debug settings
    drupal_add_js($js, array('type' => 'inline', 'scope' => 'header'));
  }

  // Don't cache empty/dead pages
  if (!$buffer) {
    return;
  }
  
  $path = drupal_get_normal_path($_GET['q']); // normalize path

  // Only place JSON info for HTML pages
  if (!$is_cached_nonhtml) {
    $authcache_footer['info'] = $_authcache_info;
    $authcache_footer['ajax'] = $authcache_ajax;
    $authcache_json =
    "\n<!-- Authcache Footer JSON -->\n" .
    "<script type=\"text/javascript\">\nvar authcacheFooter = " .
    drupal_json_encode($authcache_footer) .
    ";\n</script>\n";

    // Insert JSON before </body>, otherwise just append
    if (strripos($buffer, '</body>') !== FALSE) {
      $buffer = str_replace('</body>', $authcache_json . '</body>', $buffer);
    }
    else {
      $buffer .= $authcache_json;
    }
  }
  
  //header("Content-Length: " . strlen($output)); //D7 seems to have sent headers earlier, so doesn't seem possible to set headers at this point
  flush();
  
  // Final check, in case variable was modified
  if ($_authcache_is_cacheable !== true) {
    return;
  }

  $cache = (object) array(
    'cid' => $key,
    'data' => array(
      'path' => $_GET['q'],
      'body' => $buffer,
      'title' => drupal_get_title(),
      'headers' => array(),
    ),
    'expire' => CACHE_PERMANENT, //simg: changed to CACHE_PERMANENT from CACHE_TEMPORARY - why not?
    'created' => REQUEST_TIME,
  );

  // Restore preferred header names based on the lower-case names returned by drupal_get_http_header().
  $header_names = _drupal_set_preferred_header_name();
  foreach (drupal_get_http_header() as $name_lower => $value) {
    $cache->data['headers'][$header_names[$name_lower]] = $value;
    if ($name_lower == 'expires') {
      // Use the actual timestamp from an Expires header if available.
      $cache->expire = strtotime($value);
    }
  }

  if ($cache->data['body']) {
    // Check for page compression
    if (variable_get('page_compression', TRUE) && extension_loaded('zlib')) {
      $cache->data['body'] = gzencode($buffer, 9, FORCE_GZIP); 
    }
    cache_set($cache->cid, $cache->data, 'cache_page', $cache->expire); 
  } 
}

/**
 * Helper function: Invoke hook_$hook() in all modules; merge recursively.
 *
 * Similar to module_invoke_all(), except $var is passed by reference.
 */
function _authcache_invoke_hook($hook, &$var) {
  foreach (module_implements($hook) as $name) {
    $function = "{$name}_{$hook}";
    $result = $function();
    if (isset($result) && is_array($result)) {
      $var = array_merge_recursive($var, $result);
    }
  }
}

/**
 * Returns caching key based on user's role.
 *
 * This is prefixed to the URL in the cache_page bin.
 */
function _authcache_key($account) {

  if (!$account->uid) {
    return '';
  }

  $keys = implode('.', array_keys($account->roles));
  return substr(md5($keys . drupal_get_private_key()), 0, 6);
}

/**
 * Determines the MIME content type of the current page response based on
 * the currently set Content-Type HTTP header.
 *
 * This should normally return the string 'text/html' unless another module
 * has overridden the content type.
 */
function _authcache_get_content_type($default = NULL) {
  return drupal_get_http_header("content-type");
}

/**
 * Determines the HTTP response code that the current page request will be
 * returning by examining the HTTP headers that have been output so far.
 */
function _authcache_get_http_status($status = 200) {
  $value = drupal_get_http_header("status");
  return isset($value) ? (int) $value : $status;
}
