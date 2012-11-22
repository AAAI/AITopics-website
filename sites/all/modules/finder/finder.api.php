<?php

/**
 * @file
 * Documents finder's hooks for api reference.
 */

/**
 * Alter finders on load.
 *
 * The Finders have been loaded from the database, modules can
 * make changes to the Finders here.
 *
 * @param &$finders
 *   An array of finder objects.
 * @return
 *   No return value.
 */
function hook_finder_load(&$finders) {
  // no example code
}

/**
 * Alter the finder on presave.
 *
 * The Finder is about to be inserted or updated into the
 * database.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_presave(&$finder) {
  // no example code
}

/**
 * Alter the finder on insert.
 *
 * The Finder has been created in the database.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_insert(&$finder) {
  // no example code
}

/**
 * Alter the finder on update.
 *
 * The Finder has been changed in the database.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_update(&$finder) {
  // no example code
}

/**
 * Alter the finder on delete.
 *
 * The Finder is being deleted.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_delete(&$finder) {
  // no example code
}

/**
 * Alter the finder on page.
 *
 * The Finder page is being displayed.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_page(&$finder) {
  // no example code
}

/**
 * Alter the finder on results.
 *
 * The Finder results are being displayed.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_results(&$finder) {
  // no example code
}

/**
 * Alter the finder on block.
 *
 * The Finder block is being displayed.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_block(&$finder) {
  // no example code
}

/**
 * Alter the finder on render.
 *
 * The Finder is being displayed.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_render(&$finder) {
  // no example code
}

/**
 * Alter the finder before a find.
 *
 * The Finder is preparing a query.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_find(&$finder) {
  // no example code
}

/**
 * Alter the finder after a find.
 *
 * The Finder has the query results.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_find_alter(&$finder) {
  // no example code
}

/**
 * Alter the finder on form.
 *
 * The Finder is preparing a Finder form.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_form(&$finder) {
  // no example code
}

/**
 * Alter the finder on export.
 *
 * The Finder is about to be exported.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_export(&$finder) {
  // no example code
}

/**
 * Alter the finder on import.
 *
 * The Finder has just been imported.
 *
 * @param &$finder
 *   The finder object.
 * @return
 *   No return value.
 */
function hook_finder_import(&$finder) {
  // no example code
}

/**
 * Redirect to the first result.
 *
 * The base handler module is expected to redirect the user based on the
 * $result by implementing this function.
 *
 * @param $finder
 *   The finder object.
 */
function hook_finder_goto($finder) {
  // no example code
}

/**
 * Alter the form state before it is set by finder_form_state().
 *
 * This is a chance to undo any changes made by the finder form submit
 * function. If you set $form_state['storage']['finished'] to FALSE here then
 * it will prevent any automatic redirects and allow you to make multistep
 * forms.
 *
 * @param &$form_state
 *   The Forms API form state.
 * @param $finder
 *   The finder.
 */
function hook_finder_form_state_alter(&$form_state, $finder) {
  // no example code
}

/**
 * Provide or alter the path to a views result item.
 *
 * @param &$path
 *   A raw path that can be put into url() or l() that can be used to link to
 *   or redirect to the object.  If set to FALSE will prevent redirect.
 * @param $table
 *   Base table for this type of object.
 * @param $id
 *   The value of the primary key for this record.
 *
 * @see finder_path()
 * @see finder::goto()
 */
function hook_finder_path_alter(&$path, $table, $id) {
  // no example code
}
