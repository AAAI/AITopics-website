<?php
/**
 * @file
 * Allows views admins to refresh views URL aliases.
 */


/**
 * Admin settings form.
 *
 * @ingroup forms
 */
function views_url_alias_node_admin() {
  $form = array();
  return confirm_form($form,
    t('Are you sure you want to rebuild the Views URL alias node table?'),
    'admin/structure/views/settings',
    t('This should only be needed if URL aliases have been updated outside the node or URL alias edit form.'),
    t('Rebuild table')
  );
}

/**
 * Submit callback for Views URL Alias admin form.
 */
function views_url_alias_node_admin_submit() {
  views_url_alias_node_rebuild();

  drupal_goto('admin/structure/views/settings');
}
