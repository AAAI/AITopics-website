<?php
/**
 * @file
 * finder.inc
 *
 * Provides the Finder UI finder related functions.
 */

finder_inc('common', 'finder_ui');

/**
 * Finder UI page.
 *
 * @param $finder
 *  The finder object.
 * @return
 *  The form array.
 */
function finder_ui_page($finder) {
  ctools_include('modal');
  ctools_modal_add_js();
  finder_inc('element', 'finder_ui');
  finder_inc('item', 'finder_ui');

  $form['#tree'] = TRUE;

  $form['admin']['#prefix'] = '<div class="finder-ui">';
  $form['admin']['#suffix'] = '</div>';

  $form['admin']['info']['#prefix'] = '<div class="finder-ui-info">';
  $form['admin']['info']['#suffix'] = '</div>';

  $form['admin']['info']['edit']['#prefix'] = '<div class="finder-ui-info-edit">';
  $menu = array(
    'links' => array(
      'edit' => array(
        'title' => t('Edit finder info'),
        'href' => 'admin/structure/finder/list/' . $finder->name . '/edit/info',
      ),
    ),
  );
  $form['admin']['info']['edit']['#markup'] = theme('links__ctools_dropbutton', $menu);
  $form['admin']['info']['edit']['#suffix'] = '</div>';

  $form['admin']['info'] += finder_ui_info($finder);

  $form['admin']['columns']['#prefix'] = '<div class="finder-ui-columns">';
  $form['admin']['columns']['#suffix'] = '</div>';

  $finder_ui = finder_ui($finder);
  $finder_items = array();
  $stripe = 'odd';

  foreach ($finder_ui['items'] as $item_key => $item) {
    $stripe = $stripe == 'odd' ? 'even' : 'odd';
    $finder_items[$item['#group']][$item_key]['#prefix'] = '<div class="finder-ui-item ' . $stripe . '" id="finder-ui-item-' . $item_key . '">';
    $finder_items[$item['#group']][$item_key]['#markup'] = finder_ui_item($finder, $item_key, $item);
    $finder_items[$item['#group']][$item_key]['#suffix'] = '</div>';
    //$finder_items[$item['#group']][$item_key] = $item_output;
  }

  // Elements group is a special case.

  $finder_items['elements']['elements'] = finder_ui_element_list($finder);
  $finder_items['elements']['elements']['#prefix'] = '<div id="finder-ui-elements-list">';
  $finder_items['elements']['elements']['#suffix'] = '</div>';

  foreach (array(1, 2, 3) as $column) {
    $form['admin']['columns'][$column]['#prefix'] = '<div class="finder-ui-column" id="finder-ui-column-' . $column . '">';
    $form['admin']['columns'][$column]['#suffix'] = '</div>';
    foreach ($finder_ui['groups'][$column] as $group_key => $group) {
      $extra = '';
      if ($group_key == 'elements') {
        // Elements group is a special case.

        $menu = array(
          'links' => array(
            'edit' => array(
              'title' => t('Add'),
              'href' => 'admin/structure/finder/list/' . $finder->name . '/element/new/nojs',
              'html' => TRUE,
              'attributes' => array('class' => array('ctools-use-modal', 'ctools-modal-finder-modal-style')),
            ),
          ),
        );

        $extra = theme('links__ctools_dropbutton', $menu);

      }
      $form['admin']['columns'][$column][$group_key]['#prefix'] = '<div class="finder-ui-group" id="finder-ui-group-' . $group_key . '">';
      $form['admin']['columns'][$column][$group_key]['#suffix'] = '</div>';
      $form['admin']['columns'][$column][$group_key]['title']['#prefix'] = '<div class="finder-ui-group-title" id="finder-ui-group-' . $group_key . '-title">';
      $form['admin']['columns'][$column][$group_key]['title']['#markup'] = $extra . '<h3>' . $group . '</h3>';
      $form['admin']['columns'][$column][$group_key]['title']['#suffix'] = '</div>';

      $form['admin']['columns'][$column][$group_key]['items'] = $finder_items[$group_key];

    }
  }

  ctools_object_cache_set('finder', $finder->name, $finder);

  return $form;
}

/**
 * Form handler callback for finder_ui plugin.
 *
 * More like a form alter of the one ctools_export_ui makes.
 */
function finder_ui_form(&$form, &$form_state) {
  ctools_include('object-cache');
  $finder = &$form_state['item'];
  $finder->defaults();
  if (empty($finder->title) || arg(6) == 'info') {
    $view_default_value = NULL;
    if (!empty($finder->views_view) && !empty($finder->views_display)) {
      $view_default_value = $finder->views_view . ':' . $finder->views_display;
    }
    $form['info']['title']['#title'] = t('Title');
    $form['info']['title']['#required'] = TRUE;
    unset($form['info']['title']['#description']);
    $form['info']['description']['#title'] = t('Description');
    $form['info']['view'] = array(
      '#type' => 'select',
      '#title' => t('View'),
      '#default_value' => $view_default_value,
      '#description' => t('The Views display of the potential results.'),
      '#required' => TRUE,
      '#options' => finder_ui_views(TRUE),
    );
    $form['actions']['submit']['#value'] = t('Continue');
  }
  else {
    unset($form['info']);
    $js_hide = " js-hide";
    $cached = ctools_object_cache_get('finder', $finder->name);
    if (!empty($cached)) {
      $finder = $cached;
      $js_hide = "";
    }
    $finder->build_elements();
    $finder->load_element_handlers();
    $form['changed'] = array(
      '#prefix' => '<div class="finder-changed messages warning form-wrapper' . $js_hide . '" id="edit-changed">',
      '#markup' => t('All changes are stored temporarily. Click Save to make your changes permanent. Click Cancel to discard your changes.'),
      '#suffix' => '</div>',
      '#weight' => 0,
    );
    $form['admin'] = finder_ui_page($finder);
  }
  if (!finder_ui_check_lock($finder)) {
    unset($form['actions']['submit']);
  }
}

/**
 * Form submit callback for finder_ui plugin.
 */
function finder_ui_form_submit(&$form, &$form_state) {
  ctools_include('object-cache');
  $finder = &$form_state['item'];

  if (!finder_ui_check_lock($finder)) {
    return;
  }

  if ($form_state['triggering_element']['#parents'][0] == 'cancel') {
    ctools_object_cache_clear('finder', $finder->name);
    drupal_goto('admin/structure/finder');
  }

  $finder->defaults();

  // Expand view:display into views_view and views_display.
  if (isset($form_state['values']['view'])) {
    list($view, $display) = explode(':', $form_state['values']['view']);

    $finder->title = $form_state['values']['title'];
    $finder->name = $form_state['values']['name'];
    $finder->description = $form_state['values']['description'];

    $finder->views_view = $view;
    $finder->views_display = $display;

    finder_save($finder);

    ctools_object_cache_set('finder', $finder->name, $finder);
    drupal_goto('admin/structure/finder/list/' . $finder->name);
  }
  else {
    $finder = ctools_object_cache_get('finder', $finder->name);

    if (isset($form_state['values']['admin']['admin']['columns'][3]['elements']['items']['elements']['elements'])) {
      $weights = $form_state['values']['admin']['admin']['columns'][3]['elements']['items']['elements']['elements'];
      foreach ($weights as $feid => $weight) {
        $element = &$finder->elements[$feid];
        $element->weight = $weight['weight'];
      }
      $finder->element_weights();
    }
    $validate = $finder->validate();
    if (is_array($validate)) {
      // Validation failed.  Set errors and redirect to form.
      foreach ($validate as $error) {
        drupal_set_message($error, 'error');
      }
      drupal_goto($_GET['q']);
    }
    $form_state['values'] = (array)unserialize(serialize($finder));
    $elements = $form_state['values']['elements'];
    unset($form_state['values']['elements']);
    foreach ($elements as $key => $element) {
      $form_state['values']['elements'][$key] = new stdClass();
      $save_keys = array(
        'id',
        'description',
        'finder',
        'element',
        'title',
        'weight',
        'settings',
      );
      foreach ($element as $k => $v) {
        if (in_array($k, $save_keys)) {
          $form_state['values']['elements'][$key]->$k = $v;
        }
      }
    }
  }

  ctools_object_cache_clear('finder', $finder->name);

}

/**
 * Finder UI.
 *
 * @param $finder
 *  The finder object.
 * @return
 *  The data array.
 */
function finder_ui($finder) {
  ctools_include('dependent');

  $groups = array();
  $groups[1]['basic'] = t('Basic');
  $groups[1]['form'] = t('Form');
  $groups[2]['results'] = t('Results');
  $groups[2]['other'] = t('Other');
  $groups[3]['elements'] = t('Elements');

  $items = array();

  $items['path'] = array(
    '#group' => 'basic',
    '#item' =>  array(
      '#title' => t('Path'),
      '#value' => $finder->path,
      '#extra' => l(t('Open link'), $finder->path),
    ),
    '#form' => array(
      'path' => array(
        '#type' => 'textfield',
        '#title' => t('Path'),
        '#default_value' => $finder->path,
        '#description' => t('Finder needs a path in order to create the results page.  No leading or trailing slash.'),
        '#size' => 30,
        '#maxlength' => 128,
        '#required' => TRUE,
        '#field_prefix' => url('<front>', array('absolute' => TRUE)),
        '#translatable' => FALSE,
      ),
    ),
  );
  $items['block'] = array(
    '#group' => 'basic',
    '#item' => array(
      '#title' => t('Provide block'),
      '#value' => $finder->block ? t('Yes') : t('No'),
    ),
    '#form' => array(
      'block' => array(
        '#type' => 'checkbox',
        '#title' => t('Provide block'),
        '#default_value' => $finder->block,
        '#description' => t('Create a block for the finder form.'),
      ),
    ),
  );
  $items['form_on_page'] = array(
    '#group' => 'basic',
    '#item' => array(
      '#title' => t('Form on page'),
      '#value' => $finder->setting('form_on_page') ? t('Yes') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'form_on_page' => array(
          '#type' => 'checkbox',
          '#title' => t('Form on page'),
          '#default_value' => $finder->setting('form_on_page'),
          '#description' => t('Put a finder form on the page.'),
        ),
      ),
    ),
  );
  $header = $finder->setting('header');
  $header_format = filter_format_load($header['format']);
  $items['header'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Header'),
      '#value' => $header['value'] ? $header_format->name : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'header' => array(
          '#type' => 'text_format',
          '#base_type' => 'textarea',
          '#format' => $header['format'],
          '#title' => t('Header'),
          '#default_value' => $header['value'],
          '#description' => t('Displayed before the form.'),
        ),
      ),
    ),
  );
  $footer = $finder->setting('footer');
  $footer_format = filter_format_load($footer['format']);
  $items['footer'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Footer'),
      '#value' => $footer['value'] ? $footer_format->name : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'footer' => array(
          '#type' => 'text_format',
          '#base_type' => 'textarea',
          '#format' => $footer['format'],
          '#title' => t('Footer'),
          '#default_value' => $footer['value'],
          '#description' => t('Displayed after the form.'),
        ),
      ),
    ),
  );
  $items['find_button'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Find button'),
      '#value' => $finder->setting('find_button') ? $finder->setting('find_text') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'find_button' => array(
          '#type' => 'checkbox',
          '#title' => t('Use the <em>Find button</em>'),
          '#default_value' => $finder->setting('find_button'),
          '#description' => t('The ability to disable the find button does not imply the finder can be submitted without any buttons.'),
        ),
        'find_text' => array(
          '#type' => 'textfield',
          '#title' => t('Find button text'),
          '#default_value' => $finder->setting('find_text'),
          '#description' => t('Text to display on the submit button.'),
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('edit-settings-find-button' => array(1)),
        ),
      ),
    ),
  );
  $items['go_button'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Go button'),
      '#value' => $finder->setting('go_button') ? $finder->setting('go_text') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'go_button' => array(
          '#type' => 'checkbox',
          '#title' => t('Use the <em>Go button</em>'),
          '#default_value' => $finder->setting('go_button'),
        ),
        'go_text' => array(
          '#type' => 'textfield',
          '#title' => t('Go button text'),
          '#default_value' => $finder->setting('go_text'),
          '#description' => t('Text to display on the go button.'),
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('edit-settings-go-button' => array(1)),
        ),
      ),
    ),
  );
  $items['ajax'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Ajax'),
      '#value' => $finder->setting('ajax') ? t('Yes') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'ajax' => array(
          '#type' => 'checkbox',
          '#title' => t('Use Ajax on submit button'),
          '#default_value' => $finder->setting('ajax'),
          '#description' => t('Dynamically update the form and results using Ajax instead of loading a whole new page.'),
        ),
        'ajax_effect' => array(
          '#type' => 'select',
          '#title' => t('Ajax effect'),
          '#default_value' => $finder->setting('ajax_effect', 'none'),
          '#description' => t('If using Ajax, this will be the transition effect when the update occurs.'),
          '#options' => array(
            'none' => t('None'),
            'fade' => t('Fade'),
            'slide' => t('Slide'),
          ),
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('edit-settings-ajax' => array(1)),
        ),
        'ajax_remote' => array(
          '#type' => 'checkbox',
          '#title' => t('Ajax block is remote control'),
          '#default_value' => $finder->setting('ajax_remote'),
          '#description' => t('If Ajax is being used in a finder block that happens to be on the finder page, make the finder block act as a remote control for the finder page.'),
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('edit-settings-ajax' => array(1)),
        ),
      ),
    ),
  );
  $items['validate'] = array(
    '#group' => 'form',
    '#item' => array(
      '#title' => t('Validate'),
      '#value' => $finder->setting('validate_empty') ? t('Prevent empty') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'validate_empty' => array(
          '#type' => 'checkbox',
          '#title' => t('Prevent empty'),
          '#default_value' => $finder->setting('validate_empty'),
          '#description' => t('Prevent submission where all elements have empty values.'),
        ),
      ),
    ),
  );

  $items['show_results'] = array(
    '#group' => 'results',
    '#item' => array(
      '#title' => t('Show results'),
      '#value' => t(drupal_ucfirst($finder->setting('show_results'))),
    ),
    '#form' => array(
      'settings' => array(
        'show_results' => array(
          '#type' => 'radios',
          '#title' => t('Show results'),
          '#default_value' => $finder->setting('show_results'),
          '#options' => array(
            'completed' => t('Show results when the form has been completed'),
            'filter' => t('Always show results and filter them with the form'),
          ),
        ),
      ),
    ),
  );

  $items['results_style'] = array(
    '#group' => 'results',
    '#item' => array(
      '#title' => t('Style'),
      '#value' => t(drupal_ucfirst($finder->setting('results_style'))),
    ),
    '#form' => array(
      'settings' => array(
        'results_style' => array(
          '#type' => 'radios',
          '#title' => t('Style'),
          '#default_value' => $finder->setting('results_style'),
          '#options' => array(
            'views' => t('Views display - Build a results page from views result items.'),
            'custom' => t('Custom themed - Can be overriden using theme_finder_results().'),
          ),
        ),
        'custom_results_style_render' => array(
          '#type' => 'checkbox',
          '#title' => t('Include rendered data from Views.'),
          '#default_value' => $finder->setting('custom_results_style_render'),
          '#description' => t('Adds a little overhead, but allows you to create something similar to the <em>Finder results</em> style.'),
          '#dependency' => array('radio:settings[results_style]' => array('custom')),
        ),
        'results_style_views_format' => array(
          '#type' => 'checkbox',
          '#title' => t('Use the Views display style format.'),
          '#default_value' => $finder->setting('results_style_views_format'),
          '#description' => t('This is EXPERIMENTAL functionality, some configuration settings may break.'),
          '#dependency' => array(
            'radio:settings[results_style]' => array('views'), 
            'radio:settings[custom_results_style_render]' => array(TRUE),
          ),
        ),
      ),
    ),
  );
  $items['no_results'] = array(
    '#group' => 'results',
    '#item' => array(
      '#title' => t('No results'),
      '#value' => t(drupal_ucfirst($finder->setting('no_results'))),
    ),
    '#form' => array(
      'settings' => array(
        'no_results' => array(
          '#type' => 'radios',
          '#title' => t('When finder returns zero results'),
          '#default_value' => $finder->setting('no_results'),
          '#options' => array(
            'default' => t('Show results page'),
            'configured' => t('Configured output'),
          ),
          '#description' => t("<em>Show results page</em> will display an empty view in the <em>Views display</em> style and the output from the theme in the <em>Custom themed</em> style."),
        ),
        'no_results_configured' => finder_ui_php_setting(
          array(
            '#type' => 'textarea',
            '#title' => t('Configured output'),
            '#default_value' => $finder->setting('no_results_configured'),
            '#rows' => 1,
            '#description' => t('You can use a mixture of text and PHP code to print or return the output when there are no results.  PHP must be enclosed in PHP tags.  This can also be used to make context-aware redirects.'),
            '#process' => array('ctools_dependent_process'),
            '#dependency' => array('radio:settings[no_results]' => array('configured')),
          ),
          array(
            'keywords' => t('An array keyed by the Finder element ID, containing structured data about submitted keywords.'),
            'form_state' => t('The Forms API form state array.'),
            'finder' => t('Object containing data about this finder.'),
          )
        ),
      ),
    ),
  );
  $items['pager'] = array(
    '#group' => 'results',
    '#item' => array(
      '#title' => t('Pager'),
      '#value' => $finder->setting('pager') ? $finder->setting('pager') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'pager' => array(
          '#type' => 'textfield',
          '#title' => t('Pager'),
          '#default_value' => $finder->setting('pager'),
          '#description' => t('Number of items to show per results page.  Set to 0 to disable pager.'),
          '#size' => 5,
          '#maxlength' => 8,
          '#translatable' => FALSE,
        ),
      ),
    ),
  );
  $items['redirect'] = array(
    '#group' => 'results',
    '#item' => array(
      '#title' => t('Redirect to result'),
      '#value' => t(drupal_ucfirst($finder->setting('redirect'))),
    ),
    '#form' => array(
      'settings' => array(
        'redirect' => array(
          '#type' => 'radios',
          '#title' => t('Redirect to result'),
          '#default_value' => $finder->setting('redirect'),
          '#options' => array(
            'never' => t('Never') . ': ' . t('Never redirect to result, always use results page.'),
            'best' => t('Best') . ': ' . t('Redirect to the only result, or show results page if there are multiple results.'),
            'always' => t('Always') . ': ' . t('Always redirect to the first result.'),
          ),
          '#description' => t('These settings affect only the <em>Find button</em>.  The <em>Go button</em> will always go to the first result.'),
        ),
      ),
    ),
  );
  $items['element_logic'] = array(
    '#group' => 'results',
    '#item' => array(
      '#title' => t('Element logic'),
      '#value' => $finder->setting('element_logic'),
    ),
    '#form' => array(
      'settings' => array(
        'element_logic' => array(
          '#type' => 'radios',
          '#title' => t('Element logic'),
          '#default_value' => $finder->setting('element_logic'),
          '#description' => t('With multiple elements, how should the results of each element be combined to get the results?'),
          '#options' => array(
            'AND' => t('Match all elements using the AND operator. (Conjunction)'),
            'OR' => t('Match any element using the OR operator. (Disjunction)'),
          ),
        ),
      ),
    ),
  );
  $items['contextual_filter'] = array(
    '#group' => 'results',
    '#item' => array(
      '#title' => t('Contextual filter'),
      '#value' => $finder->setting('contextual_filter') ? $finder->setting('contextual_filter') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'contextual_filter' => finder_ui_php_setting(
          array(
            '#type' => 'textarea',
            '#default_value' => $finder->setting('contextual_filter'),
            '#title' => t('Contextual filter'),
            '#rows' => 1,
            '#description' => t('
              This can be used to provide contextual filter arguments to the Views display.
              You can provide a list of arguments seperated by a forward-slash.
              e.g: <em>term_1/term_2</em>.<br />Or insert PHP code to generate the
              list of arguments. e.g: <em>term_1/&lt;?php print "term_x/term_y";
              ?&gt;/term_2</em>.'),
          ),
          array(
            'finder' => t('Object containing data about this finder.'),
          )
        ),
      ),
    ),
  );
  if (module_exists('search')) {
    $search_tabs = array();
    foreach (search_get_info() as $module => $search_info) {
      $search_tabs[$module] = $search_info['title'];
    }
    $items['search'] = array(
      '#group' => 'results',
      '#item' => array(
        '#title' => t('Search module'),
        '#value' => $finder->setting('search') ? t('Yes') : t('No'),
      ),
      '#form' => array(
        'settings' => array(
          'search' => array(
            '#type' => 'radios',
            '#title' => t('Use search module'),
            '#default_value' => $finder->setting('search'),
            '#options' => array(
              0 => t('Do not use search module.'),
              1 => t('Attempt to fetch results from a content search when finder results are empty.'),
            ),
          ),
          'search_tab' => array(
            '#type' => 'select',
            '#title' => t('Search module tab'),
            '#default_value' => $finder->setting('search_tab'),
            '#options' => $search_tabs,
            '#required' => TRUE,
            '#process' => array('ctools_dependent_process'),
            '#dependency' => array('radio:settings[search]' => array(1)),
            '#description' => t('Which tab to use in the search module.'),
          ),
        ),
      ),
    );
  }

  $items['url'] = array(
    '#group' => 'other',
    '#item' => array(
      '#title' => t('URL parameters'),
      '#value' => $finder->setting('url') == 'enabled' ? t('Yes') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'url' => array(
          '#type' => 'radios',
          '#title' => t('URL parameters'),
          '#default_value' => $finder->setting('url'),
          '#description' => t("Using URL parameters is the normal way to pass data to the results page.  When disabled; finder uses the user's session to store submitted values across multiple pages.  It also creates a token for multipage results to identify the stored submission."),
          '#options' => array(
            'enabled' => t('Enabled'),
            'disabled' => t('Disabled'),
          ),
        ),
        'url_delimiter' => array(
          '#type' => 'textfield',
          '#title' => t('Multiple value URL arguments separator'),
          '#default_value' => $finder->setting('url_delimiter'),
          '#description' => t('Must contain at least one character other than \' \', \'/\', \'_\', and \'-\'.  If this string exists within a keyword, the keyword will be automatically enclosed in double quotes to prevent parsing issues.'),
          '#size' => 5,
          '#required' => TRUE,
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('radio:settings[url]' => array('enabled')),
          '#translatable' => FALSE,
        ),
        'url_empty' => array(
          '#type' => 'textfield',
          '#title' => t('Empty value URL arguments symbol'),
          '#default_value' => $finder->setting('url_empty'),
          '#description' => t('Leave blank for default symbol (a space).  You may need to set this to some other symbol or text if you are getting "forbidden" errors on results pages.  Must contain at least one character other than \' \', \'/\', \'_\', and \'-\'.'),
          '#size' => 5,
          '#required' => FALSE,
          '#process' => array('ctools_dependent_process'),
          '#dependency' => array('radio:settings[url]' => array('enabled')),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

  $items['cache'] = array(
    '#group' => 'other',
    '#item' => array(
      '#title' => t('Caching'),
      '#value' => $finder->setting('cache_duration') ? $finder->setting('cache_duration') . ' ' . t('minutes') : t('No'),
    ),
    '#form' => array(
      'settings' => array(
        'cache_duration' => array(
          '#type' => 'textfield',
          '#title' => t('Cache duration of options and results'),
          '#default_value' => $finder->setting('cache_duration'),
          '#description' => t('Set to 0 to disable.'),
          '#size' => 5,
          '#maxlength' => 8,
          '#required' => TRUE,
          '#field_suffix' => t('minutes'),
          '#translatable' => FALSE,
        ),
      ),
    ),
  );

  $data = array(
    'items' => $items,
    'groups' => $groups,
  );

  drupal_alter('finder_ui', $data, $finder);

  return $data;

}

