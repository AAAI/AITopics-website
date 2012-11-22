<?php

/**
 * @file
 * The finder build functions.
 */

/**
 * Get the raw link to an object given the table and id.
 *
 * @param $table
 *   Base table for this type of object.
 * @param $id
 *   The value of the primary key for this record.
 * @return
 *   A raw path that can be put into url() or l() that can be used to link to
 *   or redirect to the object.
 */
function finder_path($table, $id) {
  switch ($table) {
    case 'node':
      $path = 'node/' . $id;
      break;
    case 'users':
      $path = 'user/' . $id;
      break;
    case 'taxonomy_term_data':
      $path = 'taxonomy/term/' . $id;
      break;
    case 'node_revisions':
      $revision = node_load(NULL, $id);
      $path = 'node/' . $revision->nid . '/revisions/' . $id . '/view';
      break;
    case 'files':
      $path = file_load($id)->uri;
      break;
  }
  drupal_alter('finder_path', $path, $table, $id);
  return $path;
}


/**
 * Convert finder arguments text field entry to an array of arguments.
 *
 * @param $string
 *   The typed string of arguments, can include PHP code.
 * @param $variables
 *   Any variables that should be available to any PHP provided in the input.
 * @return
 *   The array of views arguments.
 */
function finder_get_args($string, $variables) {
  $args = array();
  $arguments = finder_eval($string, $variables);
  if ($arguments) {
    $args = explode('/', $arguments);
    foreach ($args as $k => $v) {
      $args[$k] = trim($v);
    }
  }
  return $args;
}

/**
 * Does stuff during the display handler's query.
 *
 * @param &$display_handler
 *  A reference to the views display handler object.
 */
function finder_build_display_query(&$display_handler) {
  $finder = $display_handler->get_option('finder');
  foreach (array_keys($finder->find) as $key) {
    $$key = &$finder->find[$key];
  }

  if ($mode == 'choices') {
    // We set distinct to reduce results sets somewhat if possible.
    $display_handler->view->query->set_distinct(TRUE, TRUE);
  }

  if (!empty($keywords)) {
    $placeholder_count = 0;

    // Create a where group called 'finder', for element combination.
    $element_combination = $finder->setting('element_logic');
    $display_handler->view->query->set_where_group($element_combination, 'finder');

    if ($mode == 'choices') {
      $display_handler->view->query->set_where_group('OR', 'finder_choices');
    }

    foreach ($keywords as $eid => $keyword_array) {
      $finder_element = &$finder->elements[$eid];
      $element_fields = $finder->esetting($finder_element, 'fields');

      if ($mode == 'results') {
        // For choices these have to be passed in manually through $finder->find, but for results we get them like this:
        $field_logic = $finder->esetting($finder_element, 'field_logic');
        $value_logic = $finder->esetting($finder_element, 'value_logic');
        $nesting_order = $finder->esetting($finder_element, 'nesting_order');
        $match = $finder->esetting($finder_element, 'match');
      }

      if (!empty($keyword_array)) {

        $clauses = array();
        $clause_args = array();

        foreach ($element_fields as $key => $field) {

          $field_info[$eid][$key] = $field;

          foreach (array_values($keyword_array) as $keyword_position => $keyword) {
            // Get the info we need to add the table/field.
            $delta = ($value_logic == 'AND' && count($keyword_array) > 1) ? $keyword_position : 0;

            $field->table_alias[$delta] = finder_alias('table', $eid, $field->table, $field->field, $delta);
            $field->field_alias[$delta] = finder_alias('field', $eid, $field->table, $field->field, $delta);

            $relationship = NULL;
            $join = NULL;

            if (!empty($field->relationship)) {
              $relationship = $display_handler->view->relationship[$field->relationship]->alias;
            }

            // This chunk of code will fix the join directly to the left.  Should we actually be recursing through all the left tables right back to the base table?
            if (empty($relationship)) {
              $relationship = $display_handler->view->query->base_table;
            }
            $join = $display_handler->view->query->get_join_data($field->table, $display_handler->view->query->relationships[$relationship]['base']);
            if (!empty($join)) {
              $join = $display_handler->view->query->adjust_join($join, $relationship);
            }

            if (!empty($join->left_table) && $join->left_table != $display_handler->view->query->base_table) {
              // Check for the long-chain join case that we probably screwed up, and plead for assistance.
              $left_table_join_data = $display_handler->view->query->get_join_data($join->left_table, $display_handler->view->query->relationships[$relationship]['base']);
              if (!empty($left_table_join_data->left_table) && $left_table_join_data->left_table != $display_handler->view->query->base_table && user_access('administer finder')) {
                // Purposefully not enclosed in t() because this is not a typical UI string.
                drupal_set_message("Views join configuration not supported by finder.  Please post in the <a href=\"http://drupal.org/node/add/project-issue/finder\">finder issue queue</a> and attach an export of your finder, or explain what fields you are using.  We need your help to solve this problem.", 'error');
              }
              // Table name supplied here is a combo of the table joined here, and the table we ultimately want to join.
              $join_alias = finder_alias('table', $eid, $field->table . "_" . $join->left_table, $field->field, $delta);
              $join_alias = $display_handler->view->query->add_table($join->left_table, NULL, NULL, $join_alias);
              $join->left_table = $join_alias;
            }

            $join = $join ? $join : $display_handler->view->query->get_join_data($field->table_alias[$delta], $display_handler->view->base_table);

            $table = isset($join->table) ? $join->table : $field->table;
            if ($table == $display_handler->view->base_table) {
              // Don't alias base table here.
              // @todo: rather than 'undoing' stuff here... don't do it in the first place.
              $field->table_alias[$delta] = $display_handler->view->base_table;
            }

            $display_handler->view->query->add_table($field->table, $relationship, $join, $field->table_alias[$delta]);

            // Select the field.
            if (($mode == 'results' && $finder->setting('results_style') == 'custom') || ($mode == 'choices' && $eid == $element->id)) {
              // If we're doing a custom results page or a choices list, grab the field value, and store the alias.
              $field->field_alias[$delta] = $display_handler->view->query->add_field($field->table_alias[$delta], $field->field, $field->field_alias[$delta]);
              // This if-statement also assumes that $eid == $element->id, but it is not needed to specify this because of the outer if-statement.
              if ($mode == 'choices') {
                $groups[] = $field->field_alias[$delta];
                $display_handler->view->query->add_where_expression('finder_choices', $field->table_alias[$delta] . '.' . $field->field . ' IS NOT NULL');
              }
            }

            // Add the field to where clauses.
            if ($keyword !== '' && $keyword !== NULL) {
              list($field_name, $value, $op) = array_values((array)$finder->match_args($field->table_alias[$delta] . '.' . $field->field, $keyword, $match));
              $outer_key = $nesting_order ? $key : $keyword_position;
              $placeholder = ':finder_keyword_' . $placeholder_count++;
              $expression = '(' . $field_name . ' ' . $op . ' ' . $placeholder . ')';
              $clauses[$outer_key][] = $expression;
              $clause_args[$outer_key][$placeholder] = $value;

              // Using same conditions as for when we add a field, because there's no point in adding the match field if not selecting the field.
              if (($mode == 'results' && $finder->setting('results_style') == 'custom') || ($mode == 'choices' && $eid == $element->id)) {
                $field->field_matched[$delta] = $field->field_alias[$delta] . '_matched';
                $display_handler->view->query->add_field(NULL, $expression, $field->field_matched[$delta]);
              }
            }
          }
        }

        // Convert where clauses into sql strings and add to view with add_where().
        if (!empty($clauses)) {
          $inner_operator = $nesting_order ? $value_logic : $field_logic;
          $outer_operator = $nesting_order ? $field_logic : $value_logic;
          $inner_clauses = array();
          $inner_clause_args = array();
          foreach ($clauses as $clause) {
            if (!empty($clause)) {
              $inner_clauses[] = '(' . implode(' ' . $inner_operator . ' ', $clause) . ')';
            }
          }
          foreach ($clause_args as $clause_arg) {
            foreach ($clause_arg as $inner_clause_arg_key => $inner_clause_arg) {
              $inner_clause_args[$inner_clause_arg_key] = $inner_clause_arg;
            }
          }
          $display_handler->view->query->add_where_expression('finder', implode(' ' . $outer_operator . ' ', $inner_clauses), $inner_clause_args);
        }

      }
    }
  }

  // Add groups.
  foreach ($groups as $group) {
    $display_handler->view->query->add_groupby($group);
  }
}

/**
 * Does stuff during the style plugin's render.
 *
 * @param &$style_plugin
 *  A reference to the views style plugin object.
 */
function finder_build_style_render(&$style_plugin) {
  $results = array();
  // Group the rows according to the grouping field, if specified.
  $sets = $style_plugin->render_grouping($style_plugin->view->result, $style_plugin->options['grouping']);
  $finder = $style_plugin->display->display_options['finder'];
  foreach (array_keys($finder->find) as $key) {
    $$key = &$finder->find[$key];
  }
  $style_plugin->view->row_index = 0;
  foreach ($sets as $title => $records) {
    foreach ($records as $label => $row) {
      if ($mode == 'choices') {
        foreach ($field_info[$element->id] as $key => $field) {
          $format = isset($field->format) ? $field->format : 'filter_xss';
          foreach (array_keys($field->field_alias) as $delta) {
            // If there is no match field, or if there is and it's set.
            if (!isset($field->field_matched[$delta]) || (!empty($field->field_matched[$delta]) && !empty($row->{$field->field_matched[$delta]}))) {
              if (!empty($row->{$field->field_alias[$delta]}) && !isset($results[$row->{$field->field_alias[$delta]}])) {

                $value = $row->{$field->field_alias[$delta]};
                $display = $row->{$field->field_alias[$delta]};

                if ($finder->esetting($element, 'choices_rewrite')) {
                  $variables = array(
                    'element' => $element,
                    'value' => $value,
                    'display' => $display,
                    'row' => $row,
                    'field' => $field,
                    'delta' => $delta,
                  );
                  $display = finder_eval($finder->esetting($element, 'choices_rewrite'), $variables);
                }

                if ($finder->esetting($element, 'value_rewrite')) {
                  $variables = array(
                    'element' => $element,
                    'value' => $value,
                    'display' => $display,
                    'row' => $row,
                    'field' => $field,
                    'delta' => $delta,
                  );
                  $value = finder_eval($finder->esetting($element, 'value_rewrite'), $variables);
                }

                $results[$value] = finder_sanitize($display, $format);
                $style_plugin->view->row_index++;
              }
            }
          }
        }
      }
      else {
        if ($finder->setting('results_style') == 'views' || ($finder->setting('results_style') == 'custom' && $finder->setting('custom_results_style_render')) ) {
          $row->render = $style_plugin->row_plugin->render($row);
          // This is code to get rid of extra whitespace, could be useful for some future feature?
          //$row->render = preg_replace('/\s+/', ' ', trim($row->render));
          $row->display_key = 'render';
        }
        $results[] = $row;
      }
      $style_plugin->view->row_index++;
    }
  }
  unset($style_plugin->view->row_index);

  if ($mode == 'results' && empty($results)) {
    return $style_plugin->view->display_handler->render_empty();
  }
  //if ($mode == 'results' && ($finder->setting('results_style') == 'custom' && $finder->setting('custom_results_style_render'))) {
  //  return "[%FINDER_RESULTS%]";
  //}
  elseif ($mode == 'choices' || ($mode == 'results' && $finder->setting('results_style') == 'custom' && !$finder->setting('custom_results_style_render'))) {
    return $results;
  }
  else {
    return "[%FINDER_RESULTS%]";
  }
}

/**
 * Evaluate a string of PHP code.
 *
 * This is a wrapper around PHP's eval(). It uses output buffering to capture
 * both returned value and printed text. Allows to use variables with the given
 * code.
 * Using this wrapper also ensures that the PHP code which is evaluated can not
 * overwrite any variables in the calling code, unlike a regular eval() call.
 * In other words, we evaluate the code with independent variable scope.
 *
 * @param $code
 *   The code to evaluate.
 * @param $variables
 *   Variables to import to local variable scope.
 * @return
 *   A string containing the printed output of the code, followed by the
 *   returned output of the code.
 */
function finder_eval($code, $variables = array()) {
  global $theme_path, $theme_info, $conf;

  // Store current theme path.
  $old_theme_path = $theme_path;

  // Restore theme_path to the theme, as long as drupal_eval() executes,
  // so code evaluted will not see the caller module as the current theme.
  // If theme info is not initialized get the path from theme_default.
  if (!isset($theme_info)) {
    $theme_path = drupal_get_path('theme', $conf['theme_default']);
  }
  else {
    $theme_path = dirname($theme_info->filename);
  }

  extract($variables);

  ob_start();
  print eval('?>' . $code);
  $output = ob_get_contents();
  ob_end_clean();

  // Recover original theme path.
  $theme_path = $old_theme_path;

  return $output;
}

/**
 * Build aliases in consistent manner.
 *
 * Only the $type and $eid are required, but you can supply as many params as
 * is needed, as long as your usage is consistent between places where it needs
 * to be.
 *
 * @param $type
 *  The type of alias ('table', or 'field').
 * @param $eid
 *  The finder element id.
 * @param $table
 *  The raw table name.
 * @param $field
 *  The raw field name.
 * @param $delta
 *  A meaningful extra value.
 * @return
 *  The alias string.
 */
function finder_alias($type, $eid, $table = NULL, $field = NULL, $delta = NULL) {
  static $aliases = array();
  static $alias_count = array();
  if (!isset($alias_count[$type])) {
    $alias_count[$type] = 0;
  }
  $alias = &$aliases[$type][$eid][$table][$field][$delta];
  if (empty($alias)) {
    $alias = ++$alias_count[$type];
  }
  return 'finder_' . $type . '_' . $alias;
}

/**
 * Sanitize a finder choice.
 *
 * @param $option
 *  The option object with unsanitized display field.
 * @param $filter
 *  The filter to use.
 * @return
 *  The option object with sanitized display field.
 */
function finder_sanitize($option, $filter = 'filter_xss') {
  switch ($filter) {
    case 'filter_xss':
    case 'filter_xss_admin':
    case 'check_plain':
    case 'check_url':
      $option = $filter($option);
      break;
    default:
      $option = check_markup($option, $filter, FALSE);
  }
  return $option;
}




