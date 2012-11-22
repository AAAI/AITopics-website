
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Installation
 * Related Issues
 * Notes
 * Todo


INTRODUCTION
------------

The 'Views URL alias' module allows views to be filtered by path aliases.

This module is useful if your website uses heirachical paths. It allows you to
filter and sort a view by URL aliases. When combined with the
Views bulk operation (VBO) module (http://drupal.org/project/views_bulk_operations)
you can apply operations to a specific section of your website based on a
URL alias.

Currently, only node aliases are supported.


REQUIREMENTS
------------

- Views 3.x: Create customized lists and queries from your database.
  http://drupal.org/project/views


INSTALLATION
------------

1. Copy/upload the view_url_alias.module to the sites/all/modules directory
   of your Drupal installation.

2. Enable the 'Views URL alias {type}' module in 'Modules'. (admin/modules)

3. Create or view and select 'Node: URL alias' for the field or filter


RELATED ISSUES
--------------

- Path (alias) integration
  http://drupal.org/node/257046


NOTES
-----

- This module creates and maintains separate 'views_url_alias_{type}' tables
  to provide clean and fast joins between the primary {type} table and its url
  aliases.


TODO
----

- Support multiple path alias languages, which I have no experience doing.

- Add support for taxonomy terms, users, etc URL aliases.


AUTHOR/MAINTAINER
-----------------

- Jacob Rockowitz
  http://drupal.org/user/371407
