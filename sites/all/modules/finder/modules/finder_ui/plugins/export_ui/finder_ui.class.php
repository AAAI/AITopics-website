<?php

/**
 * @file
 * Contains the CTools Export UI integration code.
 *
 * Note that this is only a partial integration.
 */

/**
 * CTools Export UI class handler for Finder UI.
 */
class finder_ui extends ctools_export_ui {

  function init($plugin) {
    parent::init($plugin);
    finder_inc('finder', 'finder_ui');
    ctools_include('ajax');
    drupal_add_css(drupal_get_path('module', 'finder_ui') . '/finder_ui.css');

    // These must be added up front because of a situation where all dropbuttons
    // start as ordinary buttons but then get ajaxed to dropbuttons.
    ctools_add_js('dropbutton');
    ctools_add_css('dropbutton');

    $modal_options = array('opacity' => .7, 'background' => '#000');

    drupal_add_js(array(
      'finder-modal-style' => array(
        'modalSize' => array(
          'type' => 'fixed',
          'width' => 1024,
          'height' => 576,
          'contentRight' => 0,
        ),
        'modalTheme' => 'FinderUIModal',

        'modalOptions' => $modal_options,
        'closeImage' => '',
      ),
    ), 'setting');
    drupal_add_js(drupal_get_path('module', 'finder_ui') . '/finder_ui.js');

  }

  /**
   * Create the filter/sort form at the top of a list of exports.
   */
  function list_form(&$form, &$form_state) {
    // Put a wrapper around the form so it can be hidden.  Unsetting the form
    // rows or doing nothing here doesn't seem to do the trick.
    parent::list_form($form, $form_state);
    $form['#prefix'] = '<div class="finder-ui-list-wrapper">' . $form['#prefix'];
    $form['#suffix'] = $form['#prefix'] . '</div>';

  }

  /**
   * Build a row based on the item.
   */
  function list_build_row($item, &$form_state, $operations) {
    // Set up sorting
    $name = $item->{$this->plugin['export']['key']};
    $schema = ctools_export_get_schema($this->plugin['schema']);

    // Note: $item->{$schema['export']['export type string']} should have already been set up by export.inc so
    // we can use it safely.
    switch ($form_state['values']['order']) {
      case 'disabled':
        $this->sorts[$name] = empty($item->disabled) . $name;
        break;
      case 'title':
        $this->sorts[$name] = $item->{$this->plugin['export']['admin_title']};
        break;
      case 'name':
        $this->sorts[$name] = $name;
        break;
      case 'storage':
        $this->sorts[$name] = $item->{$schema['export']['export type string']} . $name;
        break;
    }

    $this->rows[$name]['data'] = array();
    $this->rows[$name]['class'] = !empty($item->disabled) ? array('ctools-export-ui-disabled') : array('ctools-export-ui-enabled');

    $this->rows[$name]['data'][] = array('data' => finder_ui_info($item), 'class' => array('ctools-export-ui-title'));
    $this->rows[$name]['data'][] = array('data' => check_plain($item->{$schema['export']['export type string']}), 'class' => array('ctools-export-ui-storage'));
    $this->rows[$name]['data'][] = array('data' => l($item->path, $item->path), 'class' => array('ctools-export-ui-path'));

    $ops = theme('links__ctools_dropbutton', array('links' => $operations, 'attributes' => array('class' => array('links', 'inline'))));

    $this->rows[$name]['data'][] = array('data' => $ops, 'class' => array('ctools-export-ui-operations'));

    // Add an automatic mouseover of the description if one exists.
    if (!empty($this->plugin['export']['admin_description'])) {
      $this->rows[$name]['title'] = $item->{$this->plugin['export']['admin_description']};
    }
  }

  /**
   * Provide the table header.
   */
  function list_table_header() {
    $header = array();

    $header[] = array('data' => t('Finder'), 'class' => array('ctools-export-ui-title'));
    $header[] = array('data' => t('Storage'), 'class' => array('ctools-export-ui-storage'));
    $header[] = array('data' => t('Path'), 'class' => array('ctools-export-ui-path'));
    $header[] = array('data' => t('Operations'), 'class' => array('ctools-export-ui-operations'));

    return $header;
  }

  /**
   * Provide the actual editing form.
   */
  function edit_form(&$form, &$form_state) {
    $export_key = $this->plugin['export']['key'];
    $item = $form_state['item'];
    $schema = ctools_export_get_schema($this->plugin['schema']);

    if (!empty($this->plugin['export']['admin_title'])) {
      $form['info'][$this->plugin['export']['admin_title']] = array(
        '#type' => 'textfield',
        '#title' => t('Administrative title'),
        '#description' => t('This will appear in the administrative interface to easily identify it.'),
        '#default_value' => $item->{$this->plugin['export']['admin_title']},
      );
    }

    $form['info'][$export_key] = array(
      '#title' => t($schema['export']['key name']),
      '#type' => 'textfield',
      '#default_value' => $item->{$export_key},
      '#description' => t('The unique ID for this @export.', array('@export' => $this->plugin['title singular'])),
      '#required' => TRUE,
      '#maxlength' => 255,
    );

    if (!empty($this->plugin['export']['admin_title'])) {
      $form['info'][$export_key]['#type'] = 'machine_name';
      $form['info'][$export_key]['#machine_name'] = array(
        'exists' => 'ctools_export_ui_edit_name_exists',
        'source' => array('info', $this->plugin['export']['admin_title']),
      );
    }

    if ($form_state['op'] === 'edit') {
      $form['info'][$export_key]['#disabled'] = TRUE;
      $form['info'][$export_key]['#value'] = $item->{$export_key};
    }

    if (!empty($this->plugin['export']['admin_description'])) {
      $form['info'][$this->plugin['export']['admin_description']] = array(
        '#type' => 'textarea',
        '#title' => t('Administrative description'),
        '#default_value' => $item->{$this->plugin['export']['admin_description']},
      );
    }

    // Add the buttons if the wizard is not in use.
    if (empty($form_state['form_info'])) {
      // Add buttons.
      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
      );
      // Cancel buttons.
      $form['actions']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
      );
    }

    // Add plugin's form definitions.
    if (!empty($this->plugin['form']['settings'])) {
      // Pass $form by reference.
      $this->plugin['form']['settings']($form, $form_state);
    }

  }

  /**
   * Main entry point to edit an item.
   */
  function edit_page($js, $input, $item, $step = NULL) {
    ctools_include('object-cache');
    drupal_set_title($this->get_page_title('edit', $item));

    $cached = ctools_object_cache_get('finder', $item->name);
    if (!empty($cached)) {
      $item = $cached;
    }

    $form_state = array(
      'plugin' => $this->plugin,
      'object' => &$this,
      'ajax' => $js,
      'item' => $item,
      'op' => 'edit',
      'form type' => 'edit',
      'rerender' => TRUE,
      'no_redirect' => TRUE,
      'step' => $step,
      // Store these in case additional args are needed.
      'function args' => func_get_args(),
    );

    $output = $this->edit_execute_form($form_state);
    if (!empty($form_state['executed'])) {
      $export_key = $this->plugin['export']['key'];
      drupal_goto(str_replace('%ctools_export_ui', $form_state['item']->{$export_key}, $this->plugin['redirect']['edit']));
    }

    return $output;
  }

  /**
   * Render a header to go before the list.
   */
  function list_header($form_state) {
    return '<div class="finder-ui finder-ui-list">';
  }

  /**
   * Render a footer to go after thie list.
   */
  function list_footer($form_state) {
    return '</div>';
  }

}

