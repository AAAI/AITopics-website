<?php

/**
 * @file
 * Includes the cache handler (e.g., CacheRouter, Memcache API, Drupal core),
 * retrieves cached paged via page_cache_fastpath(),
 * and executes Ajax phase if XML/JS request detected
 *
 * @var $conf['authcache_handler'] = display during debug mode
 * @see DRUPAL_BOOTSTRAP_EARLY_PAGE_CACHE in boostrap.inc
 */

global $conf;

// Check if in Ajax phase and return JSON.
// This is a custom HTTP header defined by the authcache.js XML request
if (isset($_SERVER['HTTP_AUTHCACHE'])) {
  require_once drupal_get_path('module', 'authcache') .'/ajax/authcache.php';
  exit;
}

if (empty($conf['cache_class_cache_page'])) {
	// fallback to Drupal core cache handler (database cache tables)
	$conf['authcache_is_db'] = TRUE;
	$conf['cache_class_cache_page'] = 'DrupalDatabaseCache';
	//require_once DRUPAL_ROOT . 'includes/cache.inc'; //simg: <-- is this still needed?
} else {
	$conf['authcache_is_db'] = FALSE;
}

// if not using db for cache then we can query the cache now before any further bootstrapping is done
if (authcache_retrieve_cache_page()) die();


/**
 * Main callback from DRUPAL_BOOTSTRAP_EARLY_PAGE_CACHE phase.
 * Sends cached page to browser, if found.
 * @return boolean for bootstrap (if TRUE, PHP will exit)
 */
function authcache_retrieve_cache_page(){
	global $base_root, $cache, $conf;

	// User is logged in but their role should not receive any cached pages
	// (i.e., cached anonymous pages, since they have no authcache key)
	if (isset($_COOKIE['drupal_user']) && !isset($_COOKIE['authcache'])) {
		return FALSE;
	}

	// Caching for browser session was temporarily disabled (most likely from drupal_set_message()/drupal_goto() redirect)
	if (isset($_COOKIE['nocache_temp'])) {
		setcookie('nocache', '', REQUEST_TIME - 36000, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
		setcookie('nocache_temp', '', REQUEST_TIME - 36000, ini_get('session.cookie_path'), ini_get('session.cookie_domain'), ini_get('session.cookie_secure') == '1');
		return FALSE;
	}

	// Caching disabled for browser session
	if (isset($_COOKIE['nocache'])) {
		return FALSE;
	}

	// Attempt to retrieve page from cache
	if (empty($_POST)) {
		
		// Connect to database if default database cache hander is selected
		if ($conf['authcache_is_db']) {
			require_once DRUPAL_ROOT . '/includes/database/database.inc';
			db_set_active();
		}

		// Memcache module's cache_get function requires variables to be loaded
		//if (isset($conf['authcache_handler']) && $conf['authcache_handler'] == 'memcache.inc' && $vars_exist = cache_get('variables', 'cache')) {
		//TODO D7: simg: not sure if cache_get(variables) still needed?
		if (variable_get('cache_class_cache_page', '') == 'MemCacheDrupal') {
			$conf = variable_init($conf);
		}

		// Authenticated cache role(s) key
		$key = (isset($_COOKIE['authcache']) && $_COOKIE['authcache']) ? $_COOKIE['authcache'] : '';
		$cache_key = $key . $base_root . request_uri();
		
		$cache = cache_get($cache_key, 'cache_page');
		
		if (!empty($cache)) {
			// Cached page found
			
			// render cache benchmark
			if (isset($_COOKIE['authcache_debug'])) {
				setcookie('cache_render', timer_read('page'));
			}
			drupal_serve_page_from_cache($cache);
				
			return TRUE;
			
		} else {
			//cache miss
		}
	} else {
		//form post detected so allow normal page processing
	}
}


