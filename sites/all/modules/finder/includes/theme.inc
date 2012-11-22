<?php

/**
 * @file
 * Theme functions for the finder module.
 */

/**
 * Theme the finder results.
 *
 * @param $variables
 *   An associative array with keys:
 *     'finder' - The finder object.
 */
function theme_finder_results($variables) {
  // Create a string to append the output parts to.
  $output = '';

  // Get the finder.
  $finder = &$variables['finder'];
  // Tip: Use $finder->name to check for a particular finder.
  // Get the results.
  $results = &$finder->find['results'];

  /*
    // If you'd like to make output similar to the 'finder results' style use
    // code like this, here, and remove everything else below:
    $output .= $finder->find['results_prefix'];
    if (!empty($results)) {
      foreach ($results as $result) {
        $output .= $result->{$result->display_key};
      }
    }
    else {
      // Default 'no results' message.  Note: it depends on the settings if
      // this function is even called when there are no results.
      $output .= '<p>' . t('There are no results to display'). '</p>';
    }
    $output .= $finder->find['results_suffix'];
    return $output;
  */

  // Get the names of all finder fields.  Using different parameters the
  // finder->fields() method can also be used to get more info about the
  // fields from the views module.  The usage in this case is to get a
  // one-dimensional array of field names keyed by 'table.field'.
  $field_names = $finder->fields(FALSE, TRUE);

  // Check if there are results.
  if (!empty($results)) {
    // Iterate through the results.
    foreach ($results as $key => $result) {
      // Open the results wrapper
      $output .= '<div id="finder-result-' . $key . '" class="finder-result">';

      // Output the identity of this result object.
      // Note that information from the views object is being pulled from $finder->view.
      $id = $result->{$finder->view->base_field};
      // finder_path() attempts to get a path to the result object.
      $path = finder_path($finder->view->base_table, $id);
      // If finder_path() was successfull then make the id a link, otherwise plain.
      $id_output = $path ? l($id, $path) : $id;
      // Format the output of the identity and add it to the output.
      $identity = $field_names[$finder->view->base_table . '.' . $finder->view->base_field] . ': ' . $id_output;
      $output .= '<div class="identity">' . $identity . '</div>';

      // Iterate through the elements.
      foreach ($finder->elements as $element) {
        // Iterate the fields on this element.
        foreach ($element->settings['fields'] as $field) {
          // Output a wrapper for the field.
          $output .= '<div class="field">';
          // Output the field title.
          $output .= '<span class="field-name">' . $field_names[$field->table . '.' . $field->field] . ': </span>';
          $values = array();
          // Iterate through the field's values and collect them.  Since a field
          // can potentially accept multiple values, the values and aliases are
          // stored as an array, so we always act as though they were multiple.
          foreach (array_keys($field->field_alias) as $delta) {
            if (!empty($result->{$field->field_alias[$delta]})) {
              // Use finder_sanitize() when outputting a result item in HTML,
              // especially if the expected value is user-supplied. You can of
              // course just hard code other sanitization techniques:
              // http://api.drupal.org/api/drupal/includes--common.inc/group/sanitization/7
              $values[] = finder_sanitize($result->{$field->field_alias[$delta]}, $field->format);
            }
          }
          // Output the values separated by a comma.
          $output .= implode(', ', $values);
          // Close the field wrapper.
          $output .= '</div>';
        }
      }

      // Close the results wrapper
      $output .= '</div>';
    }
  }
  else {
    // Default 'no results' message.  Note: it depends on the settings if
    // this function is even called when there are no results.
    $output .= '<p>' . t('There are no results to display'). '</p>';
  }

  // Return the output.
  return $output;
}
