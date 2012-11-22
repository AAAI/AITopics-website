<?php
// $Id$

/**
 * @file
 * OM Maximenu Blocks.
 *
 * @author: Daniel Honrade http://drupal.org/user/351112
 *
 */
 
/**
 * Adding Blocks
 * 
 * List of all available blocks in the system
 *
 */
function om_maximenu_blocks_get() {
  global $_om_maximenu_variable;
    
  drupal_set_title(t('OM Maximenu Blocks'));
  
  // get the variable to merge with enabled blocks
  $maximenu = $_om_maximenu_variable;
  
  // based on url variables
  $args            = arg();  
  $menu_title      = isset($maximenu[$args[4]]['title']) ? $maximenu[$args[4]]['title']: '';
  $link_title      = isset($maximenu[$args[4]]['links'][$args[5]]['link_title']) ? $maximenu[$args[4]]['links'][$args[5]]['link_title']: 'New Link';
  $link_title_path = isset($maximenu[$args[4]]['links'][$args[5]]['link_title_path']) ? $maximenu[$args[4]]['links'][$args[5]]['link_title_path']: '';  
  $blocks_enabled  = isset($maximenu[$args[4]]['links'][$args[5]]['content']) ? $maximenu[$args[4]]['links'][$args[5]]['content']: array();
  //dsm($blocks_enabled);

  // make sure there's theme name
  $theme_default = variable_get('theme_default', 'garland');
  
  // get all available blocks for the current theme
  $blocks = _block_rehash($theme_default);
  
  foreach ($blocks as $key => $block) {
    $block = (object) $block;
    
    // filter out om_maximenu module
    if ($block->module != 'om_maximenu') {
    
      $title  = $block->title;  // custom title
      $info   = $block->info;   // default title     
      $module = $block->module; // block's source
      $delta  = $block->delta;  // block's identifier
          
      // checked state is based on blocks existence in the array
      $modules[$module][$delta]['checked'] = isset($blocks_enabled[$module . '___' . $delta]) ? 1: 0; 
      
      // om block title has the highest priority
      if (isset($blocks_enabled[$module . '___' . $delta]) && !empty($blocks_enabled[$module . '___' . $delta]['title'])) {  
        // if custom title is present     
        $modules[$module][$delta]['title'] =  $blocks_enabled[$module . '___' . $delta]['title'];       
      }
      else {
        // will get custom title, and default block title
        $modules[$module][$delta]['title'] =  !empty($title) ? $title: $info; 
      }
      $modules[$module][$delta]['title_path']          =  (isset($blocks_enabled[$module . '___' . $delta]) && !empty($blocks_enabled[$module . '___' . $delta]['title_path'])) ? $blocks_enabled[$module . '___' . $delta]['title_path']: '';             
      $modules[$module][$delta]['title_path_query']    =  (isset($blocks_enabled[$module . '___' . $delta]) && !empty($blocks_enabled[$module . '___' . $delta]['title_path_query'])) ? $blocks_enabled[$module . '___' . $delta]['title_path_query']: '';             
      $modules[$module][$delta]['title_path_fragment'] =  (isset($blocks_enabled[$module . '___' . $delta]) && !empty($blocks_enabled[$module . '___' . $delta]['title_path_fragment'])) ? $blocks_enabled[$module . '___' . $delta]['title_path_fragment']: '';             
      $modules[$module][$delta]['weight']              =  (isset($blocks_enabled[$module . '___' . $delta]) && !empty($blocks_enabled[$module . '___' . $delta]['weight'])) ? $blocks_enabled[$module . '___' . $delta]['weight']: 0;             
      $modules[$module][$delta]['visibility']          =  (isset($blocks_enabled[$module . '___' . $delta]) && !empty($blocks_enabled[$module . '___' . $delta]['visibility'])) ? $blocks_enabled[$module . '___' . $delta]['visibility']: 0;             
    }
  }  
  
  //dsm($modules);
  if (module_exists('om_tools')) {
    $block_class_edit = l(t('<strong>Block Classes</strong> | '), 'admin/config/system/om-tools',
      array(
        'attributes' => array(
          'title' => t('Edit / Add custom block classes.'),
        ),
        'query' => array('destination' => 'admin/structure/om-maximenu/blocks/' . $args[4] . '/' . $args[5]),
        //'fragment' => '',
        'html' => TRUE,
      )
    );
  }
  else {
    $block_class_edit = 'Block Classes (OM Tools must be intalled)';
  }
    
  $form = array();  
  $form['om_maximenu_content'] = array(
    '#type'        => 'fieldset',
    '#title'       => t(om_string_name($menu_title, FALSE) . ' / ' . om_string_name($link_title, FALSE)),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
    '#tree'        => TRUE,
    '#description' => t('These are all the available blocks in your system. Choose the blocks which will appear under the navigation menu link.<br />') . 
      $block_class_edit . '<strong>'. l(t('Back to OM Settings'), 'admin/structure/om-maximenu') . '</strong>',
  );
  
  foreach ($modules as $module => $blocks) {
  
    // count all blocks in a module
    $total = count($modules[$module]);
    
    // count how many are enabled
    $count = 0;
    foreach ($blocks as $key => $checked) {
      if ($checked['checked']) $count++;
    } 
    
    $form['om_maximenu_content'][$module] = array(
      '#type'        => 'fieldset',
      '#title'       => t(om_string_name($module, FALSE) . ' (' . $count . ' of ' . $total . ')'),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
      '#theme'       => 'om_maximenu_blocks_attach', 
    );
    
    $form['om_maximenu_content'][$module] += om_maximenu_blocks_attach($module, $blocks);
  }   
   
  $form['save'] = array( '#type' => 'submit', '#value' => t('Save'), );
    
  return $form;
}  


/**
 * Adding Blocks Submit
 *
 */
function om_maximenu_blocks_get_submit($form, $form_state) {
  global $_om_maximenu_variable;
  
  $args = arg();
  $maximenu_content = $form_state['values']['om_maximenu_content'];
  //dsm($maximenu_content);
  $content = array();
  foreach ($maximenu_content as $module => $blocks) {
    foreach ($blocks['om_blocks'] as $block => $value) {
      if ($value['checked']) {
        $content[$block] = array(
          'title'               => $value['title'],
          'title_path'          => $value['title_path'],          
          'title_path_query'    => $value['title_path_query'],    
          'title_path_fragment' => $value['title_path_fragment'],           
          'module'              => $value['module'], 
          'delta'               => $value['delta'],  
          'weight'              => $value['weight'],            
          'visibility'          => $value['visibility'], 
        );                     
      }
    }
  }
  // get the om maximenu variable
  $om_maximenu = $_om_maximenu_variable;
  
  // added to om maximenu array the content
  $om_maximenu[$args[4]]['links'][$args[5]]['content'] = $content;
  //dsm($om_maximenu);
  drupal_set_message(t('Your settings have been saved.'));
  
  // Save all settings in 1 variable
  variable_set('om_maximenu', $om_maximenu);
} 


/**
 * Block list in table format
 *
 */
function om_maximenu_blocks_attach($module = NULL, $blocks = array()) {
  
  $out = array();
  foreach ($blocks as $delta => $value) {

    $block_title = (trim($value['title']) != '<none>') ? t(om_string_name($value['title'], FALSE)): t(ucwords(t($module . ' ' . $delta)));

    $out['module'] = array(
      '#type'          => 'hidden',
      '#default_value' => $module,
    );          
    $out['om_blocks'][$module . '___' . $delta] = array(
      '#type'          => 'fieldset',
      '#title'         => t($block_title),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );     
    $out['om_blocks'][$module . '___' . $delta]['checked'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Enabled'), 
      '#default_value' => isset($value['checked']) ? $value['checked']: 0,   
    );     
    $out['om_blocks'][$module . '___' . $delta]['title'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Title'), 
      '#default_value' => isset($value['title']) ? trim(stripslashes($value['title'])): '',  
      '#description' => t('Put &lt;none&gt; for no title. You can add inline styling or other attributes, <br />
        ex. &lt;span class="special-link" style="font-weight: normal;"&gt;My Title&lt;/span&gt;'), 
    );
    $out['om_blocks'][$module . '___' . $delta]['title_path'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Path'), 
      '#default_value' => trim($value['title_path']),  
      '#autocomplete_path' => module_exists('mpac') ? 'mpac/autocomplete/alias': '',      
      '#description' => t('You can make the title clickable by adding a path. Ex. &lt;front&gt;, somepath, node/1'), 
    );      
    $out['om_blocks'][$module . '___' . $delta]['title_path_query'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Path Query'),
      '#default_value'  => trim($value['title_path_query']),
      '#description' => t('Ex. ?destination=node&me=you, but DO NOT include \'?\'.'),      
    ); 
    $out['om_blocks'][$module . '___' . $delta]['title_path_fragment'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Anchor'),
      '#default_value'  => trim($value['title_path_fragment']),
      '#description' => t('Ex. #section-1, but DO NOT include \'#\'. This can correspond to the id name of a tag, ex. &lt;div id="section-1",<br /> 
        so you can now target this section by doing "about#section-1", and the page will automatically scroll to that section.'),      
    );       
    $out['om_blocks'][$module . '___' . $delta]['module'] = array(
      '#type'          => 'hidden',
      '#default_value' => $module,   
    );        
    $out['om_blocks'][$module . '___' . $delta]['delta'] = array(
      '#type'          => 'hidden',
      '#default_value' => $delta,   
    ); 
    $out['om_blocks'][$module . '___' . $delta]['weight'] = array(
      '#type'          => 'weight',
      '#title'         => t('Weight'),
      '#delta'         => 200,
      '#default_value' => $value['weight'],   
      '#description' => t('Negative values have the highest position in the order, so -1 comes first before 0 then 1, ...'),              
    ); 
    $out['om_blocks'][$module . '___' . $delta]['visibility'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Use Drupal\'s block visibility settings'),
      '#default_value' => $value['visibility'],
      '#description' => t('You can set visibility in ') . om_maximenu_block_edit($module, $delta, $block_title),
    ); 
  }
  return $out;
}


/**
 * Implementation of theme_hook()
 * - View in table format
 *
 */
function theme_om_maximenu_blocks_attach($form) {
  //dsm($form);
  $form = $form['form'];  
  $module = $form['module']['#default_value'];
  $output = '';
  //dsm($form['layout']);

  $rows = array();
  
  foreach ($form['om_blocks'] as $key => $prop) {
    $module_check = explode('__', $key);
    if ($module_check[0] == $module) {
      $row = array();
      // Strips labels
      unset($prop['weight']['#title']);      
      unset($prop['weight']['#description']);      
      unset($prop['checked']['#title']); 
      
      unset($form['om_blocks'][$key]['weight']);      
      unset($form['om_blocks'][$key]['checked']);                             
      $row[] = array('class' => 'col-block', 'data' => drupal_render($form['om_blocks'][$key]));
      $row[] = array('class' => 'col-weight', 'data' => drupal_render($prop['weight']));
      $row[] = array('class' => 'col-checked', 'data' => drupal_render($prop['checked']));
            
      $rows[] = array(
        'data' => $row,
        'id' => 'om-row-'. $module . '-' . $key,
      ); 
    }
  }
  
  $header = array(t('Blocks'), t('Weight'), t('Enabled')); //, 'Update'

  // Header
  $form['form']['#children'] = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'om-maximenu-blocks-' . $module . '-' . $key)));

  $output .= drupal_render_children($form);
  
  return $output;
}


    

