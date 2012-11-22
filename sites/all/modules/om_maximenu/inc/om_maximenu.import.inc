<?php
// $Id$

/**
 * @file
 * OM Maximenu Import
 *
 * @author: Daniel Honrade http://drupal.org/user/351112
 *
 */
 
/**
 * Import from existing menus
 *
 */
function om_maximenu_import() {
  global $_om_maximenu_variable;
  global $_om_maximenu_existing_menus;
  global $_om_maximenu_existing_terms;
    
  drupal_set_title(t('OM Maximenu Import'));
  
  $maximenu = $_om_maximenu_variable;
  
  //dsm($maximenu);
  $form = array();
  $form['om_import'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  // menus
  $form['om_import']['menus'] = array(
    '#type' => 'fieldset',
    '#title' => t('Menus'),    
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Menu tree will not be preserved on import. Any menu or link with submenus will be flattened.'),
  );
    
  // getting existing menu
  $result_menus = db_query("SELECT menu_name, mlid, plid, link_title FROM {menu_links} WHERE hidden = 0 AND TRIM(link_title) > '' ORDER BY menu_name ASC");
        
  $excluded_menus = array();
  while ($record_menus = $result_menus->fetchObject()) {
    if (!in_array($record_menus->menu_name, $excluded_menus)) {
      $_om_maximenu_existing_menus[$record_menus->menu_name][$record_menus->plid][$record_menus->mlid] = $record_menus->link_title;
    }
  }
  
  foreach ($_om_maximenu_existing_menus as $menu_name => $tree) {
    $options = array();
    om_menu_tree_options($options, $tree[0], $menu_name);
    $menu_title = ucwords(preg_replace('/-/',' ', $menu_name));   
    $none = array('' => '<none>', $menu_name =>  $menu_title);
    $options = $none + $options;

    $form['om_import']['menus'][$menu_name]['mlid'] = array(
      '#type'          => 'select',
      '#title'         => t($menu_title),
      '#options'       => $options,
      '#default_value' => '',
    );
    $form['om_import']['menus'][$menu_name]['menu_list'] = array(
      '#type'  => 'hidden',
      '#value' => implode(',', array_keys($options)),
    );
  }
   
  if (module_exists('taxonomy')) {
    // vocabularies
    $form['om_import']['vocabularies'] = array(
      '#type' => 'fieldset',
      '#title' => t('Vocabularies'),    
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#description' => t('Create menus from your existing vocabularies.'),
    );
    // getting existing vocabularies
    $result_terms = db_query("SELECT t.tid AS term_id, t.vid, t.name, t.description, t.weight, v.vid AS voc_id, v.name AS vocabulary 
                              FROM {taxonomy_term_data} t JOIN {taxonomy_vocabulary} v ON t.vid = v.vid");
  
    while ($record_terms = $result_terms->fetchObject()) {
      $vocabularies[$record_terms->voc_id] = $record_terms->vocabulary;
      $_om_maximenu_existing_terms[$record_terms->voc_id][$record_terms->term_id]['term_id']     = $record_terms->term_id;
      $_om_maximenu_existing_terms[$record_terms->voc_id][$record_terms->term_id]['term']        = $record_terms->name;
      $_om_maximenu_existing_terms[$record_terms->voc_id][$record_terms->term_id]['description'] = $record_terms->description;
      $_om_maximenu_existing_terms[$record_terms->voc_id][$record_terms->term_id]['weight']      = $record_terms->weight;    
    }
    
    if (isset($vocabularies) && !empty($vocabularies)) {  
      foreach ($vocabularies as $voc_id => $vocabulary) {
        $form['om_import']['vocabularies'][$voc_id]['enabled'] = array(
          '#type'          => 'checkbox',
          '#title'         => $vocabulary,
          '#default_value' => '',
        );    
        $form['om_import']['vocabularies'][$voc_id]['voc_list'] = array(
          '#type'  => 'hidden',
          '#value' => $vocabularies,
        );
      }
    }
  }
     
  $form['save'] = array( '#type' => 'submit', '#value' => t('Save'), );
  
  return $form;
}


/**
 * Import from existing menus
 *
 */
function om_maximenu_import_submit($form, $form_state) {
  global $_om_maximenu_variable;
  $om_maximenu = $_om_maximenu_variable;
    
  if (!empty($om_maximenu)) {  
    // check for existing imported menus
    foreach ($om_maximenu as $menu_id => $content) {
      $codes[] = $content['code'];
    }
  }
  else {
    $codes = array();
  }
  
  // New menu and links
  // new ids relies on highest menu key id
  // adding only 1 new blank menu
  $menu_id = !empty($om_maximenu) ? max(array_keys($om_maximenu)) + 1: 1;
    
  $menus = $form_state['values']['om_import']['menus'];

  $import = array();
  foreach ($menus as $menu => $select) {
    $mlid      = $select['mlid'];
    $menu_list = explode(',', $select['menu_list']);    

    if (is_numeric($mlid)) {
      // getting existing menus
      $link = db_query("SELECT link_title, has_children FROM {menu_links} WHERE mlid = :mlid", array(':mlid' => $mlid))->fetchObject();      

      if ($link->has_children) {
        $result = db_query("SELECT mlid, plid, link_path, link_title, options, weight FROM {menu_links} WHERE hidden = 0 AND plid = :plid AND TRIM(link_title) > '' ORDER BY p1, p2, p3, p4, p5, p6, p7, p8, p9, plid, mlid, link_title ASC", array(':plid' => $mlid));    
      }
      else {
        $result = db_query("SELECT mlid, link_path, link_title, options, weight FROM {menu_links} WHERE hidden = 0 AND mlid = :mlid", array(':mlid' => $mlid));                  
      }
      $menu = strtolower(preg_replace('/ /', '-', $link->link_title));
      $menu_title = $link->link_title;
    }
    elseif (is_string($mlid)) {
      $result = db_query("SELECT mlid, plid, link_path, link_title, options, weight FROM {menu_links} WHERE hidden = 0 AND menu_name = :menu_name AND TRIM(link_title) > '' ORDER BY p1, p2, p3, p4, p5, p6, p7, p8, p9, plid, mlid, link_title ASC", array(':menu_name' => $mlid));    
      $menu_title = ucwords(preg_replace('/-/', ' ', $menu));
    }
    else {
      // do nothing
    }

    if (!empty($mlid) && (!in_array($menu . '-imported', $codes)) && !empty($menu_title)) {
      $import = array(
        'code'        => $menu . '-imported',
        'title'       => $menu_title,
        'action'      => 'hover',        
        'skin'        => 'bubble',
        'tabbed'      => '', 
        'output'      => 'block',         
      );
      $import['output']  = 'block';
      $import['block_options'] = array(
        'stacking'  => 'horizontal',
        'direction' => 'left',
        'region'    => '',  
        'weight'    => 0,
        'region'    => '',
      );
      $import['menu_visibility'] = array(
        'visibility' => 0, 
        'pages'      => '',
      );

      // because we are handling the visibility, etc., we have to handle everything to synch with blocks settings           
      om_maximenu_update_blocks_db($menu_id, $import);    
      while ($record = $result->fetchObject()) {
        if (isset($record->link_title) && !empty($record->link_title) && in_array($record->mlid, $menu_list)) {      
          // this transforms paths with %
          $path = menu_link_load($record->mlid);
          //$path = menu_link_get_preferred($path['href']);
          // this will get the path alias if available
          $alias = drupal_get_path_alias($path['href']);
          $path = $alias ? $alias: $path['href'];
          
          $options = unserialize($record->options);
          
          $import['links'][$record->mlid] = array(
            'link_title'         => $record->link_title,
            'link_title_option'  => 'title',
            'path_icon'          => '',
            'icon_option'        => 0,
            'php_option'         => 0,            
            //'path'               => $record->link_path, 
            'path'               => $path, 
            'weight'             => $record->weight,                                    
            'id'                 => isset($options['attributes']['id']) ? $options['attributes']['id']: '',
            'class'              => isset($options['attributes']['class']) ? $options['attributes']['class']: '',
            'rel'                => isset($options['attributes']['rel']) ? $options['attributes']['rel']: '',
            'target'             => isset($options['attributes']['target']) ? $options['attributes']['target']: '_self',                        
            'description'        => isset($options['attributes']['title']) ? $options['attributes']['title']: '',
            'description_option' => 'hover',            
            'roles'              => array(1 => 0, 2 => 0),            
            'content'            => '',
          );
        }   
      } 
      drupal_set_message(t($menu_title . ' has been imported.'));  
      $om_maximenu[$menu_id] = $import;
      $menu_id++;
    }
  }
  
  if (module_exists('taxonomy') && isset($form_state['values']['om_import']['vocabularies'])) {
    global $_om_maximenu_existing_terms;
    $terms = $_om_maximenu_existing_terms;
  
    // taxonomy terms as menus  
    $vocabularies = $form_state['values']['om_import']['vocabularies'];

    $import = array();
    foreach ($vocabularies as $voc_id => $voc) {
      $enabled = $voc['enabled'];
      if ($enabled) {
        $voc_list   = $voc['voc_list'];
        $voc_title  = $voc_list[$voc_id];
        $code = 'taxonomy-' . $voc_id;
        if (!in_array($code . '-imported', $codes)) {
          $import = array(
            'code'        => $code . '-imported',
            'title'       => $voc_title,
            'action'      => 'hover',        
            'skin'        => 'bubble',
            'output'      => 'block',         
          );
          $import['output']  = 'block';
          $import['block_options'] = array(
            'stacking'  => 'horizontal',
            'direction' => 'left',
            'region'    => '',  
            'weight'    => 0,
            'region'    => '',
          );
          $import['menu_visibility'] = array(
            'visibility' => 0, 
            'pages'      => '',
          );

          // because we are handling the visibility, etc., we have to handle everything to synch with blocks settings           
          om_maximenu_update_blocks_db($voc_id, $import);   

          foreach ($terms[$voc_id] as $term_id => $term) {
            $path = 'taxonomy/term/' . $term_id;
            // this will get the path alias if available
            $alias = drupal_get_path_alias($path);
            $path = $alias ? $alias: $path;
          
            $import['links'][$term_id] = array(
              'link_title'         => $term['term'],
              'link_title_option'  => 'title',
              'path_icon'          => '',
              'icon_option'        => 0,
              'php_option'         => 0,            
              'path'               => $path,           
              'weight'             => $term['weight'],                                    
              'id'                 => '',
              'class'              => '',
              'rel'                => '',
              'target'             => '_self',                        
              'description'        => $term['description'],
              'description_option' => 'hover',            
              'roles'              => array(1 => 0, 2 => 0),            
              'content'            => '',
            );
          }   
        } 
        drupal_set_message(t($voc_title . ' has been imported.'));  
        $om_maximenu[$menu_id] = $import;
        $menu_id++;
      }   
    }     
  }  
  // Save all settings in 1 variable
  variable_set('om_maximenu', $om_maximenu);  
}
  
