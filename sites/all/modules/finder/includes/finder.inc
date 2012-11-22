<?php

/**
 * @file
 * finder.inc
 *
 * Provides the finder object type and associated methods.
 */

/**
 * An object to contain all of the data to generate a finder, plus the member
 * functions to build the finder, and render the output.
 */
class finder {
  public $id;
  public $name;
  public $views_view;
  public $views_display;
  public $title;
  public $description;
  public $path;
  public $block;
  public $settings;
  public $elements;
  public $status;
  public $export_type;

  /**
   * Finder defaults.
   *
   * Set up defaults on database based finders.
   */
  function defaults() {
    if (!empty($this->id)) {
      if (!isset($this->title)) {
        $this->title = t('Finder') . ' ' . $this->id;
      }
      if (!isset($this->path)) {
        $this->path = 'finder_' . !empty($this->name) ? $this->name : $this->id;
      }
      if (!isset($this->block)) {
        $this->block = TRUE;
      }
      if (!isset($this->status)) {
        $this->status = TRUE;
      }
      $finder_settings = array(
        'block' => TRUE,
        'form_on_page' => TRUE,
        'find_button' => TRUE,
        'find_text' => t('Find'),
        'go_text' => t('Go'),
        'ajax_effect' => 'none',
        'show_results' => 'completed',
        'results_style' => 'views',
        'no_results' => 'default',
        'pager' => 10,
        'redirect' => 'never',
        'element_logic' => 'AND',
        'url' => 'enabled',
        'url_delimiter' => ',',
      );
      foreach ($finder_settings as $key => $default) {
        if (!isset($this->settings[$key])) {
          $this->settings[$key] = $default;
        }
      }
      if (!empty($this->elements)) {
        foreach ($this->elements as &$element) {
          if (!empty($element->id)) {
            if (!$element->title) {
              $element->title = t('Element !id', array('!id' => $element->id));
            }
            $element_settings = array(
              'field_logic' => 'OR',
              'value_logic' => 'AND',
              'match' => 'e',
            );
            foreach ($element_settings as $key => $default) {
              if (!isset($element->settings[$key])) {
                $element->settings[$key] = $default;
              }
            }
          }
        }
      }
      else {
        $this->elements = array();
      }
    }
  }

  /**
   * Finder create element.
   */
  function create_element($id = NULL) {
    if (empty($id)) {
      $id = 'e' . uniqid();
    }
    $element = (object)array(
      'id' => $id,
      'finder' => $this->name,
      'settings' => array(),
      'weight' => 0,
    );
    $this->elements[$element->id] = &$element;
    $this->defaults();
    $this->build_elements();
    return $element;
  }

  /**
   * Finder delete element.
   *
   * Remove the element from the finder.
   */
  function delete_element(&$element) {
    $this->choice_delete($element);
    unset($this->elements[$element->id]);
  }

  /**
   * Finder build elements.
   *
   * Does stuff after elements are loaded or when a new element is created.
   */
  function build_elements() {
    if (!empty($this->elements)) {
      foreach ($this->elements as &$element) {
        $element->finder = $this;
      }
    }
    $this->load_element_handlers();
  }

  /**
   * Order elements by weight.
   */
  function element_weights() {
    if (!empty($this->elements)) {
      uasort($this->elements, array($this, '_element_weights_sort'));
    }
  }

  /**
   * Sort callback for element ordering.
   */
  function _element_weights_sort($a, $b) {
    if ($a->weight == $b->weight) {
      return 0;
    }
    return ($a->weight < $b->weight) ? -1 : 1;
  }

  /**
   * Finder save.
   */
  function save() {
    module_invoke_all('finder_presave', $this);
    $update = array();
    $op = 'finder_insert';
    if (!empty($this->id)) {
      $update[] = 'id';
      $op = 'finder_update';
    }
    if (!empty($this->elements)) {
      foreach ($this->elements as &$element) {
        $element->finder = $this->name;
      }
    }
    $return = drupal_write_record('finder', $this, $update);
    $this->choice_repopulate();
    module_invoke_all($op, $this);
    cache_clear_all();
    menu_rebuild();
    return $return;
  }

  /**
   * Finder delete.
   */
  function delete($clear = TRUE) {
    $this->choice_delete();

    db_delete('finder')
      ->condition('id', $this->id)
      ->execute();
    module_invoke_all('finder_delete', $this);

    if ($clear) {
      cache_clear_all();
      menu_rebuild();
    }
  }

  /**
   * Make sure the finder is valid.
   *
   * @return
   *   TRUE if the finder is valid; an array of error strings if it is not.
   */
  function validate() {
    $errors = array();

    $url_delimiter = preg_replace("/[\/\-\_\s]/", "", $this->setting('url_delimiter'));
    if (!$url_delimiter) {
      $errors[] = t('Multiple value URL arguments separator must contain at least one character that is not a space ( ), forward-slash (/), hyphen (-), or underscore (_).');
    }

    $empty_symbol = preg_replace("/[\/\-\_\s]/", "", $this->setting('url_empty'));
    if ($this->setting('url_empty') && !$empty_symbol) {
      $errors[] = t('Empty value URL arguments symbol must contain at least one character that is not a space ( ), forward-slash (/), hyphen (-), or underscore (_).');
    }
    if ($url_delimiter == $empty_symbol) {
      $errors[] = t('The "empty value URL arguments symbol" should not be the same as the "multiple value URL arguments separator".');
    }

    if (empty($this->elements)) {
      $errors[] = t('You must create at least one element.');
    }
    else {
      $element_ids = array();
      foreach ($this->elements as $element) {
        if (!$this->esetting($element, 'fields')) {
          $errors[] = t('You must create at least one field for %name.', array('%name' => $element->title));
        }
        if (in_array($element->id, $element_ids)) {
          $errors[] = t('The machine name %id is used by more than one element.  Please delete the elements and try again.', array('%id' => $element->id));
        }
        $element_ids[] = $element->id;
      }

    }


    return !empty($errors) ? $errors : TRUE;
  }

  /**
   * Finder load element handlers.
   *
   * Attach element handler data to the finder.
   */
  function load_element_handlers() {
    if (!empty($this->elements)) {
      $element_handlers = finder_element_handlers();
      foreach ($this->elements as &$element) {
        if (!empty($element_handlers[$element->element])) {
          $element->element_handler = $element_handlers[$element->element];
        }
      }
    }
  }

  /**
   * Get an option from the element handler.
   *
   * Useful if unsure if element handler info is present as it will load the
   * element info if it is missing.  <-- No it doesn't :/ DERPrecate?
   */
  function element_handler($element, $setting) {
    if (isset($element->element_handler[$setting])) {
      return $element->element_handler[$setting];
    }
  }

  /**
   * Finder page.
   *
   * @return
   *   Render array of a finder page.
   */
  function page() {
    module_invoke_all('finder_page', $this);
    $this->display = 'page';
    return array(
      '#prefix' => '<div id="finder-page-' . $this->name . '" class="finder-page">',
      'finder' => $this->render(),
      '#suffix' => '</div>',
    );
  }

  /**
   * Finder block.
   *
   * @return
   *   Render array of a finder block.
   */
  function block() {
    module_invoke_all('finder_block', $this);
    $this->display = 'block';
    return array(
      'subject' => check_plain($this->title),
      'content' => array(
        '#prefix' => '<div id="finder-block-' . $this->name . '" class="finder-block">',
        'finder' => $this->render(),
        '#suffix' => '</div>',
      ),
    );
  }

  /**
   * Finder build.
   *
   * Prepare a finder for execution and rendering.
   */
  function build() {
    if (empty($this->built)) {
      finder_inc('build');
      $this->build_elements();
    }
    $this->built = TRUE;
  }

  /**
   * Finder render.
   *
   * Generate output of the finder.
   */
  function render() {
    finder_inc('form');
    module_invoke_all('finder_view', $this);

    $this->build();

    $display = $this->display;

    $this->build_id = 'finder-' . $display . '-' . $this->name . '-wrapper';
    $this->display = $display;

    // Always get the form in order to populate the form_state in case there are results we need to present.
    // The form building function will not spend resources building elements if it doesn't need to.
    // to do: this non form_on_page get_form may only be needed when hiding url args?
    $form = drupal_get_form('finder_form_' . $this->name, $this);

    if ($display != 'page' || ($display == 'page' && $this->setting('form_on_page'))) {
      $output['form'] = $form;
    }
    if ($display != 'block' || ($display == 'block' && !empty($this->ajax))) {
      $output['results'] = $this->results();

    }

    drupal_add_css(drupal_get_path('module', 'finder') . '/finder.css');

    $this->render = $output;

    // Don't add the wrapper in if this is an ajax call.
    //if (empty($this->ajax)) {
      $this->render['#prefix'] = '<div id="' . $this->build_id . '" class="finder-wrapper">';
      $this->render['#suffix'] = '</div>';
    //}

    return $this->render;
  }

  /**
   * Finder find.
   *
   * Get a list of choices for form or results.
   *
   * @return
   *   An array of choices/results.
   */
  function find() {
    static $finder_find_cache = array();

    $this->build();

    $keywords = &$this->find['keywords'];
    $mode = &$this->find['mode'];
    $match = &$this->find['match'];
    $pager = &$this->find['pager'];
    $element = &$this->find['element'];
    $reset = &$this->find['reset'];
    $field_logic = &$this->find['field_logic'];
    $value_logic = &$this->find['value_logic'];
    $nesting_order = &$this->find['nesting_order'];

    // Some defaults.
    $mode = $mode ? $mode : 'choices';
    $match = $match ? $match : 'e';
    $pager = $pager ? $pager : 0;
    $element = $element ? $element : NULL;
    $reset = $reset ? $reset : FALSE;

    // Create an ID using the function params so we can cache the return value.
    $id = ($mode == 'choices' ? 'e' . $element->id : 'f' . $this->id) . '|';
    $id_length = strlen($id);
    $cache_id = ($match != 'e' ? $match : '')
              . ($pager ? $pager . (isset($_GET['page']) ? '.' . $_GET['page'] : '') : '')
              . serialize($keywords);
    $cache_id_length = strlen($cache_id);
    if (($cache_id_length + $id_length) > 255) {
      // For ID's that (will) exceed 255 we will try to represent them a unique way and pray for the best :/
      $cache_id = md5($cache_id)
                . $cache_id_length
                . substr($cache_id, 0, 223 - strlen($cache_id_length) - $id_length);  // 223 = 255 - 32
    }
    $cache_id = $id . $cache_id;

    if (isset($finder_find_cache[$cache_id]) && !$reset) {
      // Use the static data.
      $this->find = $finder_find_cache[$cache_id];
    }
    elseif (
      $this->setting('cache')
      && !$reset
      && ($cache = cache_get($cache_id, 'cache_finder_find'))
      && !empty($cache->data)
    ) {
      // Use the cached data.
      $this->find = $cache->data;
    }
    else {
      // Allow other modules to react to and alter the finder.
      module_invoke_all('finder_find', $this);

      if ($mode == 'choices' && $this->esetting($element, 'choices_style', 'used_values') != 'used_values') {
        // Calculate the values from the finder_choice table.
        $this->choice_find();
      }
      else {
        // Calculate the values from the original tables..
        $this->views_find();
      }

      if ($mode == 'choices' && $this->esetting($element, 'sort')) {
        natcasesort($this->find['results']);
      }

      // Allow other modules to react to and alter the finder.
      module_invoke_all('finder_find_alter', $this);

      // Add the resulting $finder_find to the drupal cache.
      if ($this->setting('cache')) {
        cache_set($cache_id, $this->find, 'cache_finder_find', REQUEST_TIME + $this->setting('cache'));
      }

      // Add the resulting $finder_find to the static internal load cache.
      $finder_find_cache[$cache_id] = $this->find;

    }

  }

  /**
   * Finder choice find.
   *
   * A subfunction of find() which deals specifically with the finder_choice table.
   */
  function choice_find() {
    $keywords = &$this->find['keywords'];
    $match = &$this->find['match'];
    $pager = &$this->find['pager'];
    $element = &$this->find['element'];

    $query = db_select('finder_choice', 'fc')
      ->fields('fc', array('choice_key', 'choice_value'))
      ->condition('fc.finder', $this->id)
      ->condition('fc.element', $element->id)
      ->distinct();

    if (!empty($keywords[$element->id])) {
      $placeholder_count = 0;

      foreach (array_values($keywords[$element->id]) as $keyword_position => $keyword) {
        if (!empty($keyword)) {
          $key_placeholder = ':finder_keyword_' . $placeholder_count++;
          list($key_field, $key_value, $key_op) = array_values((array)$this->match_args('fc.choice_key', $keyword, $match));
          $val_placeholder = ':finder_keyword_' . $placeholder_count++;
          list($val_field, $val_value, $val_op) = array_values((array)$this->match_args('fc.choice_value', $keyword, $match));
          $expression = '(' . $key_field . ' ' . $key_op . ' ' . $key_placeholder . ' OR ' . $val_field . ' ' . $val_op . ' ' . $val_placeholder . ')';
          $query->where($expression, array($key_placeholder => $key_value, $val_placeholder => $val_value));
        }
      }

    }

    if ($pager) {
      $query->range(0, $pager);
    }

    $this->find['results'] = $query->execute()->fetchAllKeyed(0, 1);
  }

  /**
   * Finder choice repopulate.
   *
   * Repopulate the finder_choice table for this finder, or a particular element
   * if an element object is passed in.
   */
  function choice_repopulate($element = NULL) {
    if (!empty($element)) {
      $elements = array($element);
    }
    else {
      $elements = &$this->elements;
    }

    foreach ($elements as $finder_element) {
      $this->choice_delete($finder_element);

      $choices_style = $this->esetting($finder_element, 'choices_style');

      $list = array();
      if ($choices_style == 'available_options' || $choices_style == 'available_options_php') {
        if ($choices_style == 'available_options_php') {
          finder_inc('build');
          $available_options = finder_eval($this->esetting($finder_element, 'available_options_php'), array('finder_element' => $finder_element));
        }
        else {
          $available_options = $this->esetting($finder_element, 'available_options');
        }

        $available_options = explode("\n", $available_options);

        foreach ($available_options as $available_option) {
          if (strpos($available_option, '|') !== FALSE) {
            list($key, $value) = explode('|', $available_option);
            $list[$key] = (isset($value) && $value !== '') ? $value : $key;
          }
          else {
            $list[$available_option] = $available_option;
          }
        }

      }

      foreach ($list as $choice_key => $choice_value) {
        // insert list items into table.
        db_insert('finder_choice')->fields(array(
          'finder' => $this->id,
          'element' => $finder_element->id,
          'choice_key' => $choice_key,
          'choice_value' => $choice_value,
        ))
        ->execute();
      }
    }

  }

  /**
   * Finder choice delete.
   *
   * Delete the finder_choice table choices for this finder, or a particular element
   * if an element object is passed in.
   */
  function choice_delete($element = NULL) {
    $query = db_delete('finder_choice')
      ->condition('finder', $this->id);
    if (!empty($element)) {
      $query->condition('element', $element->id);
    }
    $query->execute();
  }

  /**
   * Finder views find.
   *
   * A subfunction of find() which deals specifically with the Views module.
   */
  function views_find() {
    $this->find['field_info'] = array();
    $this->find['groups'] = array();

    foreach (array_keys($this->find) as $key) {
      $$key = &$this->find[$key];
    }

    if ($view = views_get_view($this->views_view)) {
      $this->view = $view;

      $view_args = array();

      if ($mode == 'choices' || ($mode == 'results' && $this->setting('results_style') == 'custom' && !$this->setting('custom_results_style_render'))) {

        $display = $view->add_display('finder');

        // Change the display options based on 'query display' setting.
        if ($mode == 'results') {
          $custom_display = $this->views_display;
        }
        elseif ($mode == 'choices') {

          $custom_display = $this->esetting($element, 'display');
          if (!$custom_display) {
            $custom_display = $this->views_display;
          }
        }

        $view->display[$display]->display_options = $view->display[$custom_display]->display_options;

      }
      elseif ($mode == 'results') {
        $display = $view->add_display($this->views_display);
        $view->finder_results_display = TRUE;
      }

      // Activate the display
      $view->set_display($display);

      if ($mode != 'results' || !$this->setting('results_style_views_format')) {
        $view->display_handler->set_option('style_plugin', 'finder');
      }

      if ($mode == 'choices' || ($mode == 'results' && $this->setting('results_style') == 'custom' && !$this->setting('custom_results_style_render'))) {
        $view->display_handler->set_option('row_plugin', 'fields');
      }

      $view->display_handler->set_option('finder', $this);

      // Limit result set size.
      if (isset($pager)) {
        $view->display_handler->set_option('use_pager', TRUE);
        $view->display_handler->set_option('items_per_page', $pager);
        $pager_element = ($mode == 'results') ? 0 : 'finder_choices_' . $element->id;
        $view->display_handler->set_option('pager_element', $pager_element);
      }

      if ($mode == 'results') {
        $view_args = $this->arguments();

        // Limit result for redirect.
        if (isset($this->go) && $this->go) {
          $view->display_handler->set_option('use_pager', TRUE);
          $view->display_handler->set_option('items_per_page', 1);
          $view->display_handler->set_option('pager_element', 0);
        }
      }
      elseif ($mode == 'choices') {
        $view_args = $this->element_arguments($element);

        $view->display_handler->set_option('distinct', TRUE);
      }

      // Make sure the query is not cached
      // @todo: what happens is we do let it get cached?
      $view->is_cacheable = FALSE;

      $view->set_items_per_page($pager);

      // Removed because of issue #1345930.
      //$view->build($display);

      $this->find['results'] = $view->execute_display($display, $view_args);

      if ($mode == 'results' && ($this->setting('results_style') == 'views' || ($this->setting('results_style') == 'custom' && $this->setting('custom_results_style_render')))) {
        if (is_array($this->find['results']) && isset($this->find['results']['content'])) {
          $this->find['results'] = $this->find['results']['content'];
        }

        $wrapper = explode('[%FINDER_RESULTS%]', $this->find['results']);
        $this->find['results_prefix'] = isset($wrapper[0]) ? $wrapper[0] : '';
        $this->find['results_suffix'] = isset($wrapper[1]) ? $wrapper[1] : '';
        $this->find['results'] = $view->result;
      }

    }

  }

  /**
   * Finder results.
   *
   * Create finder results output.
   *
   * @return
   *   Themed output of finder results.
   */
  function results() {
    $results = array();

    $this->form_state = finder_form_state($this);

    module_invoke_all('finder_results', $this);

    if (!empty($this->form_state['storage']['finished']) || $this->setting('show_results') == 'filter') {
      /*
      // I can't remember what this is for...
      // @todo: comment this out and see if anyone says anything?
      foreach ($_REQUEST as $k => $v) {
        unset($_REQUEST[$k]);
      }
      */

      $keywords = array();

      $pager = $this->setting('pager');

      foreach ($this->elements as $element) {

        $keyword = array(NULL);
        if (!is_null($this->form_state['values'][$element->id])) {

          $keyword = (array)$this->form_state['values'][$element->id];

          if ($this->esetting($element, 'delimit')) {

            // This enables escaped characters to work in delimiters.
            $delimiter = stripcslashes($this->esetting($element, 'delimit'));

            foreach ($keyword as $k => $v) {
              unset($keyword[$k]);
              $exploded = explode($delimiter, $v);
              foreach ($exploded as $e) {
                $keyword[] = trim($e);
              }
            }
          }

        }
        $keywords[$element->id] = $keyword;
      }

      $redirect = $this->setting('redirect');
      if (!empty($_GET['go']) || (isset($this->form_state['clicked_button']) && $this->form_state['clicked_button']['#name'] == 'go' && $this->form_state['storage']['finished']) || $redirect == 'always') {
        $this->go = TRUE;
      }

      $this->find = array(
        'mode' => 'results',
        'keywords' => $keywords,
        'pager' => $pager,
      );

      $this->find();

      if ($this->setting('url') == 'disabled') {
        if ($pager && !isset($this->form_state['storage']['pager_token'])) {
          $token = drupal_get_token();
          $this->form_state['storage']['pager_token'] = $token;
          $_SESSION['finder'][$token] = $this->form_state;
        }
      }

      if ((!empty($this->go) && !empty($this->find['results'])) || ($redirect == 'best' && count($this->find['results']) === 1 && empty($_GET['page']))) {
        $this->go();
      }

/*
      $params = array();
      if (!empty($this->form_state['storage']['pager_token'])) {
        $params['finder'] = $this->form_state['storage']['pager_token'];
      }
*/

      // Search module integration.
      if ($this->setting('search') && empty($this->find['results']) && module_exists('search') && !empty($keywords)) {

        $keys = array();
        // @todo ability to exclude certain fields/elements from being included in the search keywords.
        foreach ($keywords as $finder_element_id => $finder_element_keywords) {
          foreach ((array)$finder_element_keywords as $keyword) {
            $keys[] = $keyword;
          }
        }
        $search_module_output = search_data(implode(' OR ', $keys), $this->setting('search_tab'));
        if (!empty($search_module_output['#results'])) {
          $results['results'] = $search_module_output;
        }

      }

      // Results handling.
      if (empty($results['results']) && (!empty($this->find['results']) || $this->setting('no_results') == 'default')) {
        if ($this->setting('results_style') == 'custom') {
          // Custom themed results.
          $results['results'] = array(
            '#theme' => 'finder_results',
            '#finder' => $this,
          );
          $results['pager'] = array('#theme' => 'pager');
        }
        elseif ($this->setting('results_style') == 'views') {
          // @todo: see if we can use a views_handler_area here to put in footer/header/empty.
          // Results from views module.
          $results['results']['#prefix'] = $this->find['results_prefix'];
          if (!$this->setting('results_style_views_format')) {
            foreach ($this->find['results'] as $result) {
              $results['results'][]['#markup'] = $result->{$result->display_key};
            }
          }
          $results['results']['#suffix'] = $this->find['results_suffix'];
        }
      }
      // Default no results handling.
      elseif (empty($results['results']) && (/*!empty($this->find['results']) ||*/ $this->setting('no_results') == 'configured')) {
        // Configured output.
        $variables = array(
          'finder' => $this,
          'keywords' => $keywords,
          'form_state' => $this->form_state,
        );
        $results['results']['#markup'] = finder_eval($this->setting('no_results_configured'), $variables);
      }
    }

    $this->find['results_render'] = $results;

    return $results;
  }

  /**
   * Finder fields.
   *
   * Gets the list of possible fields that can be used with this finder.
   */
  function fields($groups = TRUE, $title_only = TRUE) {
    if ($view = views_get_view($this->views_view)) {
      views_include('admin');
      $display = $view->add_display('finder_views');
      $view->set_display($display);
      $base_tables = $view->get_base_tables();
      $views_fields = views_fetch_fields(array_keys($base_tables), 'filter');

      // Special case, allow search indexes to work. #1410862
      $views_fields['search_index.word'] = $views_fields['search_index.keys'];
      unset($views_fields['search_index.keys']);

      $options = array();
      foreach ($views_fields as $k => $v) {
        if ($groups) {
          $options[$v['group']][$k] = $title_only ? $v['title'] : $v;
        }
        else {
          $options[$k] = $title_only ? $v['title'] : $v;
        }
      }
      return $options;
    }
    return FALSE;
  }

  /**
   * Finder relationships.
   *
   * Gets the list of possible relationships that can be used with this finder.
   */
  function relationships() {
    if ($view = views_get_view($this->views_view)) {
      $options = array();
      $display = $this->views_display;
      if (isset($view->display[$display]->display_options['relationships'])) {
        foreach ($view->display[$display]->display_options['relationships'] as $rel_key => $rel) {
          $options[$rel_key] = $rel['label'];
        }
      }
      return $options;
    }
    return FALSE;
  }

  /**
   * Finder go.
   */
  function go() {
    module_invoke_all('finder_go', $this);
    $result = reset($this->find['results']);
    $id = &$result->{$this->view->base_field};
    $path = finder_path($this->view->base_table, $id);
    if ($path) {
      if ($this->view->base_table == 'files') {
        file_download($path);
      }
      drupal_goto($path);
    }
  }

  /**
   * Finder element arguments.
   *
   * Get the contextual filter arguments for an element.
   *
   * @param $element
   *   The finder element object.
   * @return
   *   The array of views arguments.
   */
  function element_arguments($element) {
    $variables = array(
      'finder_element' => $element,
    );
    $string = $this->esetting($element, 'contextual_filter');
    return finder_get_args($string, $variables);
  }

  /**
   * Finder arguments.
   *
   * @param $element
   *   The finder element object.
   * @return
   *   The array of views arguments.
   */
  function arguments() {
    $variables = array(
      'finder' => $this,
    );
    $string = $this->setting('contextual_filter');
    return finder_get_args($string, $variables);
  }

  /**
   * Finder setting.
   *
   * @param $setting
   *   The name of the setting.
   * @param $default
   *   The value to return if there is no setting.
   * @return
   *   The setting or default value.
   */
  function setting($setting, $default = NULL) {
    if (isset($this->settings[$setting])) {
      return $this->settings[$setting];
    }
    return $default;
  }

  /**
   * Finder element setting.
   *
   * @param $element
   *   The element object.
   * @param $setting
   *   The name of the setting.
   * @param $default
   *   The value to return if there is no setting.
   * @return
   *   The setting or default value.
   */
  function esetting($element, $setting, $default = NULL) {
    if (isset($element->settings[$setting])) {
      return $element->settings[$setting];
    }
    return $default;
  }

  /**
   * Finder matches.
   *
   * Get preconfigured condition match methods.
   */
  function matches() {
    $matches = array(
      'c' => array(
        'name' => t('Contains'),
        'description' => t('!matches <em>contain</em> the !keywords.'),
        'operator' => 'LIKE',
        'value_prefix' => '%',
        'value_suffix' => '%',
      ),
      'cw' => array(
        'name' => t('Contains word'),
        'description' => t('!matches <em>contain</em> the !keywords as whole words.'),
        'operator' => 'REGEXP',
        'value_prefix' => '[[:<:]]',
        'value_suffix' => '[[:>:]]',
      ),
      'e' => array(
        'name' => t('Equals'),
        'description' => t('!matches must match the !keywords <em>exactly</em>.'),
        'operator' => '=',
        'value_prefix' => '',
        'value_suffix' => '',
      ),
      'sw' => array(
        'name' => t('Starts with'),
        'description' => t('!matches must <em>start with</em> the !keywords.'),
        'operator' => 'LIKE',
        'value_prefix' => '',
        'value_suffix' => '%',
      ),
      'ew' => array(
        'name' => t('Ends with'),
        'description' => t('!matches must <em>end with</em> the !keywords.'),
        'operator' => 'LIKE',
        'value_prefix' => '%',
        'value_suffix' => '',
      ),
      'lt' => array(
        'name' => t('Less than'),
        'description' => t('!matches must be <em>less than</em> the !keywords.'),
        'operator' => '<',
        'value_prefix' => '',
        'value_suffix' => '',
      ),
      'lte' => array(
        'name' => t('Less than or equals'),
        'description' => t('!matches must be <em>less than or equal to</em> the !keywords.'),
        'operator' => '<=',
        'value_prefix' => '',
        'value_suffix' => '',
      ),
      'gt' => array(
        'name' => t('Greater than'),
        'description' => t('!matches must be <em>greater than</em> the !keywords.'),
        'operator' => '>',
        'value_prefix' => '',
        'value_suffix' => '',
      ),
      'gte' => array(
        'name' => t('Greater than or equals'),
        'description' => t('!matches must be <em>greater than or equal to</em> the !keywords.'),
        'operator' => '>=',
        'value_prefix' => '',
        'value_suffix' => '',
      ),
      'nc' => array(
        'name' => t("Doesn't contain"),
        'description' => t("!matches <em>don't contain</em> the !keywords."),
        'operator' => 'NOT LIKE',
        'value_prefix' => '%',
        'value_suffix' => '%',
      ),
      'ncw' => array(
        'name' => t("Doesn't contain word"),
        'description' => t("!matches <em>don't contain</em> the !keywords as whole words."),
        'operator' => 'NOT REGEXP',
        'value_prefix' => '[[:<:]]',
        'value_suffix' => '[[:>:]]',
      ),
      'ne' => array(
        'name' => t('Not equals'),
        'description' => t('!matches must <em>not</em> match the !keywords.'),
        'operator' => '<>',
        'value_prefix' => '',
        'value_suffix' => '',
      ),
    );
    return $matches;
  }

  /**
   * Get data about finder match methods.
   *
   * @param $field
   *   The field to match.
   * @param $value
   *   The value to match.
   * @param $match
   *   The match method according to finder's settings.
   * @return
   *   An object cast from an array with keys field/value/match if $match is set.
   */
  function match_args($field, $value, $match) {
    $matches = $this->matches();
    if ($matches[$match]['value_prefix']) {
      if (strpos($matches[$match]['operator'], 'LIKE') !== FALSE) {
        $value = db_like($value);
      }
      $value = $matches[$match]['value_prefix'] . $value;
    }
    if ($matches[$match]['value_suffix']) {
      $value = $value . $matches[$match]['value_suffix'];
    }
    return (object)array('field' => $field, 'value' => $value, 'match' => $matches[$match]['operator']);
  }

}

