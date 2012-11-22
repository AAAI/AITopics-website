<?php

/**
 * @file
 * Plugin definition for CTools Export UI integration.
 */

$plugin = array(
  'schema' => 'finder',
  'access' => 'administer finder',

  'export' => array(
    'admin_title' => 'title',
    'admin_description' => 'description',
  ),

  'form' => array(
    'settings' => 'finder_ui_form',
    'submit' => 'finder_ui_form_submit',
  ),

  'menu' => array(
    'menu item' => 'finder',
    'menu title' => 'Finder',
    'menu description' => 'Finders are configurable forms to allow users to find objects in the system.',
  ),

  'title singular' => t('finder'),
  'title singular proper' => t('Finder'),
  'title plural' => t('finders'),
  'title plural proper' => t('Finders'),

  'handler' => 'finder_ui',

  'strings' => array(
    'confirmation' => array(
      'revert' => array(
        'information' => t('This action will permanently remove any customizations made to this finder.'),
        'success' => t('The finder has been reverted.'),
      ),
      'delete' => array(
        'information' => t('This action will permanently remove the finder from your database.'),
        'success' => t('The finder has been deleted.'),
      ),
    ),
  ),
);

