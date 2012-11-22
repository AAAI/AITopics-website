
===========================================
Authenticated User Page Caching (Authcache)
===========================================

The Authcache module offers page caching for both anonymous users and logged-in
authenticated users. This allows Drupal/PHP to spend only 1-2 milliseconds
serving pages and greatly reduces server resources.

Please visit:

  http://drupal.org/project/authcache

For information, updates, configuration help, and support.

============
Installation
============

1. Enabled the module and configure the Authcache settings
   (Site Configuration -> Performance -> Authcache).

2. Setup a Drupal cache handler module (optional, but strongly recommended for vastly improved performance)

   Download and enable a cache handler module, such as:

   -- Memcache API @ http://drupal.org/project/memcache
   -- Filecache @ http://drupal.org/project/filecache
   
   (note: for most purposes Filecache is both easier to set up AND faster than memcache.
   
3.Open your settings.php file and configure the cache handler module.

  Here are some examples:

  ---------------
  MEMCACHE MODULE
  ---------------
  $conf['memcache_servers']  = array('localhost:11211' => 'default');
  
  $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';  
  $conf['cache_backends'][] = 'sites/all/modules/authcache/authcache.inc'; 
  $conf['cache_class_cache_page'] = 'MemCacheDrupal';
  
  ---------------
  FILECACHE MODULE
  ---------------  
  
  $conf['cache_backends'][] = 'sites/all/modules/filecache/filecache.inc'; 
  $conf['cache_backends'][] = 'sites/all/modules/authcache/authcache.inc';
  $conf['cache_class_cache_page'] = 'DrupalFileCache';

  ------------------------------------------------------------------------
  If you are using a cache module other than FileCache / Memcache, or if the module
  is in a different parent directory than Authcache, define the the cache include
  path using:
  -------------------------------------------------------------------------
  
  $conf['cache_backends'][] = './sites/path/to/module/cacheinclude.inc';
  $conf['cache_backends'][] = 'sites/all/modules/authcache/authcache.inc';
  $conf['cache_class_cache_page'] = 'your_cache_hander_name';
  
  -------------------------------------------------------------------
  If no cache handler is setup or defined, Authcache will fallback to Drupal core
  database cache tables and "Authcache Debug" will say "cache_inc: database"
  
  If you are experimenting with multiple caching systems (db, apc, memcache),
  make sure to clear the cache each time you switch to remove stale data.
    
4. Goto Configuration > Development > Performance > Authcache and enable with appropriate settings

5. Modify your theme by tweaking user-customized elements (the final HTML
   must be the same for each user role). Template files (e.g., page.tpl.php)
   will have several new variables:

    $user_name to display the logged-in user name
    $user_link to display the name linked to their profile (both work for
      cached and non-cached pages).
    $is_page_authcache is set to TRUE in all template hooks if the page
      is to be cached.

===================
UPGRADING FROM BETA
===================

If you are upgrading from a beta version (if you have been using Authcache before
2009-09-13), please delete the "authcache" module directory and then extract the new release.

"ajax_authcache.php" also no longer needs to be in Drupal root directory.  Make sure to follow
the new configuration settings above (like downloading the latest Cache Router and using
the correct $conf values in settings.php).

=================
CACHE FLUSH NOTES
=================

Page cache is cleared when cron.php is executed.  This is normal Drupal core behavior.

========================
Authcache Example Module
========================

Please see ./modules/authcache_example for a demonstration on how to cache
blocks of user-customized content.

======
Author
======

Developed & maintained by Jonah Ellison.

Demo: http://authcache.httpremix.com
Email: jonah [at] httpremix.com
Drupal: http://drupal.org/user/217669

D7 port by Simon Gardner
Email: slgard@gmail.com
Drupal: http://drupal.org/user/620692

================
Credits / Thanks
================

- "Cache Router" module (Steve Rude) for the caching system/API
- "Boost" module (Arto Bendiken) for some minor code & techniques
