<?php
// $Id$

/**
 * @file
 * OM Maximenu Admin Utilities
 *
 * @author: Daniel Honrade http://drupal.org/user/351112
 *
 */
define('OM_MAXIMENU_PATH', drupal_get_path('module', 'om_maximenu')); 

/**
 * Sorting by weight
 *
 */
function om_sort_by_weight($a, $b) {
  return $a['weight'] - $b['weight'];
}

/**
 * Converts string either to a title or a class
 *
 */
function om_string_name($string = NULL, $class = TRUE) {
  // for the purpose of just showing the classes and title
  // any tags are stripped off
  $string = trim(strip_tags($string));
  // if there are slashes
  $string = stripslashes($string);  
  // ignores characters other than a-z, A-Z, 0-9
  $string_class = preg_replace('/[^a-zA-Z0-9-]+/', ' ', $string);  
  // allows all characters
  $string_title = $string; 
  // space and underscore are converted depending on use
  $pattern = '/[ _]+/';     
  $string = ($class == FALSE) ? ucwords(preg_replace($pattern, ' ', $string_title)): strtolower(preg_replace($pattern, '-', $string_class));
  // if result is none for non-class
  if (empty($string) && $class == FALSE) $string = 'Script Link';
  
  return $string;
}



/**
 * Link Options
 *
 */
function om_maximenu_link_option($content = array()) {

  // img hover replacement class
  $icon_option = (isset($content['icon_option']) && ($content['icon_option'] == 1)) ? 'class="om-hover"': '';

  // title, icon options
  $link_title        = stripslashes($content['link_title']);
  $link_title_option = $content['link_title_option'];
  $link_icon         = !empty($content['path_icon']) ? '<img ' . $icon_option . ' src="' . url($content['path_icon']) . '" alt="' . om_string_name($link_title, FALSE) . '" />': '';
      
  if ($link_title_option == 'title_icon') {
    $link_option = $link_icon . ' ' . $link_title;
  }
  elseif ($link_title_option == 'icon') {
    $link_option = $link_icon;
  }
  else {
    $link_option = $link_title;
  }
  
  // description as subtitle
  $link_subtitle = (!empty($content['description']) && ($content['description_option'] != 'hover')) ? '<span class="om-subtitle">' . check_plain($content['description']) . '</span>': ''; 
        
  return $link_option . $link_subtitle;
}
  
  
/**
 * Link attribute
 *
 */
function om_maximenu_link_attrib($content = array()) {
  // initialize link attributes and classes
  $attributes = array();
      
  // id link attributes
  if (!empty($content['id'])) $attributes['id'] = om_string_name($content['id']);

  // user added custom classes
  $custom_classes = '';
  $classes = explode(' ', $content['class']);
  foreach ($classes as $ckey => $class) {
    $custom_classes .= ' ' . om_string_name($class);
  }
  
  $title_class = om_string_name($content['link_title']);
  if (empty($title_class)) $title_class = 'script';
  
  // class link attributes      
  $attributes['class'] = 'om-link' . $custom_classes . ' link-' . $title_class;  
  
  // active class   
  $uri = trim($_SERVER['REQUEST_URI']);
  $path_relative = base_path() . $content['path'];	
	    
  if (($_GET['q'] == $content['path']) || ($path_relative == $uri)) $attributes['class'] .= ' active'; 
  
  if ((empty($content['path']) || ($content['path'] == '<front>')) && drupal_is_front_page()) $attributes['class'] .= ' active';   
  
  // link description options
  if (!empty($content['description']) && ($content['description_option'] == 'hover')) $attributes['title'] = check_plain($content['description']); 

  // relationship
  if (!empty($content['rel'])) $attributes['rel'] = $content['rel'];

  // target
  if (!empty($content['target']) && ($content['target'] != '_self')) $attributes['target'] = $content['target'];

  return $attributes;
} 


/**
 * Classes aggregator
 *
 */
function om_maximenu_classes($links = array()) {
  $classes = array();
  
  // bubble, simple, no_style  
  $classes[] = $links['skin'];

  // style
  if (isset($links['style']) && !empty($links['style'])) {
    if (($links['style'] == 'scrollh') || ($links['style'] == 'scrollv')) {
      $classes[] = 'tabbed-' . $links['style'];    
    }
    else {
      $classes[] = 'style-' . $links['style'];
    }
  } 
    
  // block, float, main_menu  
  $classes[] = $links['output'];
  
  if ($links['output'] == 'block') {
    // row, column
    $classes[] = isset($links['block_options']['stacking']) ? $links['block_options']['stacking']: 'row';   
    // up, right, down, left
    $classes[] = isset($links['block_options']['direction']) ? $links['block_options']['direction']: 'block-down';   
  }
  elseif ($links['output'] == 'float') {
    // left, middle, right
    $classes[] = isset($links['float_options']['orientation']) ? $links['float_options']['orientation']: 'horizontal';       
    $classes[] = isset($links['float_options']['stacking']) ? $links['float_options']['stacking']: 'row';   
    $classes[] = isset($links['float_options']['x_origin']) ? $links['float_options']['x_origin']: 'left';   
    $classes[] = isset($links['float_options']['y_origin']) ? $links['float_options']['y_origin']: 'bottom';   
  }
  elseif ($links['output'] == 'main_menu') {
    // no class
  }  
  
  // fisheye dock class
  if (isset($links['float_options']['dock'])) $classes[] = 'dock';
  
  $out = '';
  foreach ($classes as $key => $class) {
    $out .= ' om-maximenu-' . om_string_name($class);   
  }
  return $out;
}   


/**
 * OM Maximenu inline styling
 *
 */
function om_maximenu_inline_style($links = array()) {
  $out = '';
  if ($links['output'] == 'block') {
    // do nothing now, maybe in the future
  }  
  elseif ($links['output'] == 'float') {
    // fixed/absolute
    $position    = (isset($links['float_options']['position']) && ($links['float_options']['position'] == 'absolute')) ? 'absolute': 'fixed';   
    // left/middle/right
    if (isset($links['float_options']['x_origin']) && ($links['float_options']['x_origin'] != 'middle')) {
      $x_origin    = isset($links['float_options']['x_origin']) ? $links['float_options']['x_origin']: 'left';     
      $x_value     = isset($links['float_options']['x_value']) ? $links['float_options']['x_value']: 10;
      $x_px        = ($x_value != 0) ? 'px': '';
      $x_prop      = $x_origin . ': ' . $x_value . $x_px . '; '; 
    } 
    else {
      $x_prop = ''; // jquery will determine the position
    }  
    // top/bottom
    $y_origin    = isset($links['float_options']['y_origin']) ? $links['float_options']['y_origin']: 'bottom';     
    $y_value     = isset($links['float_options']['y_value']) ? $links['float_options']['y_value']: 10; 
    $y_px        = ($y_value != 0) ? 'px': '';    
    $y_prop      = $y_origin . ': ' . $y_value . $y_px . '; ';
               
    $out = ' style="position: ' . $position . '; ' . $x_prop  . $y_prop . '" ';   
  }
  elseif ($links['output'] == 'main_menu') {
    // do nothing now, maybe in the future
  }
  
  return $out;
}


/**
 * Link classes processor
 *
 */
function om_maximenu_link_classes($content = array(), $permission = array(), $count = 0, $total = 0) {

  // initialize link classes
  $classes = array();     
                        
  // standard li classes
  if ($count == 1) $classes[] = 'first';
  if ($count == $total) $classes[] = 'last';  
      
  // link title class
  $classes[] = 'leaf-' . $content['link_title'];     

  // active class       
  if ($_GET['q'] == $content['path']) $classes[] = 'active'; 
      
  if ((empty($content['path']) || ($content['path'] == '<front>')) && drupal_is_front_page()) $classes[] = 'active';   
      
  // user role class
  if (is_array($permission)) {
    foreach ($permission as $pkey => $pval) {
      $classes[] = 'leaf-role-' . $pval; 
    }  
  }      
        
  // title, icon options
  $link_title_option = $content['link_title_option'];
  if ($link_title_option == 'title_icon') {
    $classes[] = 'om-leaf-title-icon';
  }
  elseif ($link_title_option == 'icon') {
    $classes[] = 'om-leaf-icon';
  }
  
  $out = 'om-leaf';
  foreach ($classes as $key => $class) {
    $out .= ' ' . om_string_name($class);   
  }
  return $out;
}


/**
 * Link visibility per user role
 *
 */
function om_maximenu_link_visible($roles = array()) {
  global $user;

  // user roles  
  $user_id = $user->uid;  
  $user_roles = $user->roles;  
  // user permission to view the link
  // roles
  // 1 - anonymous
  // 2 - authenticated

  $permission = array();
  $anyone = 0;
  foreach ($roles as $role => $value) {
     // check roles
     if (($value != 0) && isset($user_roles[$role])) $permission[$role] = $user_roles[$role];
     // for all roles are unchecked
     if ($value != 0) $anyone++;       
  }
  // the link will be visible if the user has permission 
  if (!empty($permission)) {
    return $permission;
  }
  elseif ($anyone == 0) { // the links is for everybody
    return 1;
  }
  //elseif ($user_id == 1) { // the user is a super user
    //return 1;
  //}
  else {
    return 0;
  }
}      


/**
 * OM Maximenu get styles from /skin directory
 * - reads directory names
 */
function om_maximenu_skin_get() {
  $files = array();
  $dir = OM_MAXIMENU_PATH . '/skin/';
  $folders = scandir($dir);
  $excluded_files = array('.', '..', '.cvs', '.svn', '.git');

  foreach ($folders as $key => $val) {
    if (!in_array($val, $excluded_files)) (is_dir($dir . $val)) ? $files[$val] = om_string_name($val, FALSE): '';     
  }
  return $files;
}


/**
 * Block edit link
 *
 */
function om_maximenu_block_edit($module, $delta, $title = NULL) {
  $vars = array();
  // Display 'edit block' for custom blocks.
  if ($module == 'block') {
    $options = array(
      'attributes' => array(
        'title' => t('Edit this block\'s content.'),
        'class' => array('block-edit'),
      ),
      'query' => drupal_get_destination(),
      'html' => TRUE,
    );     
    $vars[] = l( t('edit block'), 'admin/structure/block/manage/' . $module . '/' . $delta . '/configure', $options);
  }
  // Display 'configure' for other blocks.
  else {
    $options = array(
      'attributes' => array(
        'title' => t('Configure ' . $title),
        'class' =>  array('block-config'),
      ),
      'query' => drupal_get_destination(),
      'html' => TRUE,
    );    
    $vars[] = l( t('configure'), 'admin/structure/block/manage/' . $module . '/' . $delta . '/configure', $options);
  }
  // Display 'edit view' for Views blocks.
  if ($module == 'views' && user_access('administer views')) {
    //list($view_name, $view_block) = explode('-block', $delta);
    $view = explode('-block', $delta);   
    $view_name = isset($view[0]) ? $view[0]: ''; 
    $view_block = isset($view[1]) ? $view[1]: ''; 
    
    $options = array(
      'attributes' => array(
        'title' => t('Edit this view.'),
        'class' =>  array('block-edit-view'),
      ),
      'query' => drupal_get_destination(),
      'html' => TRUE,
    );
    $vars[] = l( t('edit view'), 'admin/structure/views/view/' . $view_name . '/edit/block' . $view_block, $options);
  }
  // Display 'edit menu' for Menu blocks.
  elseif ((($module == 'menu') || ($module == 'system')) && user_access('administer menu')) {
    $menu_name = $delta;
    $options = array(
      'attributes' => array(
        'title' => t('Edit this menu.'),
        'class' =>  array('block-edit-menu'),
      ),
      'query' => drupal_get_destination(),
      'html' => TRUE,
    );
    $vars[] = l( t('edit menu'), 'admin/structure/menu/manage/' . $menu_name, $options);
  }

  $out = '<div class="edit-block">' . implode(' ', $vars) . '</div>';
  return $out;
}


/**
 * Menu Visibility
 *
 */
function om_maximenu_menu_visibility($menu_visibility = FALSE) {
  if ($menu_visibility['pages']) {
    if ($menu_visibility['visibility'] < 2) {
      $path = drupal_get_path_alias($_GET['q']);
      $page_match = drupal_match_path($path, $menu_visibility['pages']);
      if ($path != $_GET['q']) {
        $page_match = $page_match || drupal_match_path($_GET['q'], $menu_visibility['pages']);
      }
      $page_match = !($menu_visibility['visibility'] xor $page_match);
    }
    elseif (module_exists('php')) {
        $page_match = php_eval($menu_visibility['pages']);
    }
    else {
      $page_match = FALSE;
    }
  }
  else {
      $page_match = TRUE;
  }
  return $page_match;
}


/**
 * Block Configure Page - Get delta visiblity value form _om_maximenu_variable;
 *
 */
function om_maximenu_form_alter(&$form, $form_state, $form_id) { 
  switch($form_id) {
    case 'block_admin_configure':
      if($form['module']['#value'] == 'om_maximenu') $form['#submit'][] = '_om_maximenu_block_admin_configure_submit_handler';
      break;
    case 'block_admin_display_form':
      $form['#submit'][] = '_om_maximenu_block_admin_display_submit_handler';
      break;
  }
}


/**
 * Updates OM Maximenu variable on saving block configuration
 *
 */
function _om_maximenu_block_admin_configure_submit_handler(&$form, $form_state) {
  $delta = $form_state['values']['delta'];
  $delta = explode('-', $delta);
  
  if($delta[1] == 'maximenu') {
    global $_om_maximenu_variable;
    $maximenu = $_om_maximenu_variable;
    $delta = $delta[2];
    $maximenu[$delta]['menu_visibility']['visibility'] = $form_state['values']['visibility'];   
    $maximenu[$delta]['menu_visibility']['pages'] = $form_state['values']['pages'];
    variable_set('om_maximenu', $maximenu);
  }
}


/**
 * Updates OM Maximenu variable on saving block display
 *
 */
function _om_maximenu_block_admin_display_submit_handler(&$form, $form_state) {
  global $_om_maximenu_variable;
  $maximenu = $_om_maximenu_variable;
  $blocks = $form_state['values']['blocks'];
  
  $count = 0;
  foreach ($blocks as $module_delta => $values) {
    if ($values['module'] == 'om_maximenu') {
      $delta = explode('-', $values['delta']);
      $delta = $delta[2];
      $maximenu[$delta]['block_options']['region'] = $values['region'];     
      $maximenu[$delta]['block_options']['weight'] = $values['weight']; 
      $count++;           
    }
  }
  // will only process if OM Maximenu blocks are present
  if ($count != 0) variable_set('om_maximenu', $maximenu);  
}


/**
 * Block db updater
 *
 */
function om_maximenu_update_blocks_db($menu_id = 0, $maximenu_content = array()) {
  if ($menu_id != 0) {
    if ($maximenu_content['output'] == 'block') {
    
      $theme_default = variable_get('theme_default', 'garland');

      $existing = db_select('block', 'n')
        ->fields('n')
        ->condition('module', 'om_maximenu', '=')
        ->condition('delta', 'om-maximenu-' . $menu_id, '=') 
        ->condition('theme', $theme_default, '=')                           
        ->execute()
        ->fetchObject(); 

      if (!empty($existing->delta)) {                      
        // if the menu is existing   
        $status = (!empty($maximenu_content['block_options']['region'])) ? 1: 0;
        
        db_update('block')
          ->fields(array(
            'status'     => $status,
            'weight'     => $maximenu_content['block_options']['weight'],                                  
            'region'     => $maximenu_content['block_options']['region'],  
            'visibility' => (int) $maximenu_content['menu_visibility']['visibility'],
            'pages'      => trim($maximenu_content['menu_visibility']['pages']),
          ))
          ->condition('module', 'om_maximenu')
          ->condition('delta', 'om-maximenu-' . $menu_id) 
          ->condition('theme', $theme_default)                             
          ->execute();
      }
      else {  
        // because we are handling the visibility, etc., we have to handle everything to synch with blocks settings
        $status = (!empty($maximenu_content['block_options']['region'])) ? 1: 0;

        db_insert('block')
          ->fields(array(
            'module'     => 'om_maximenu', 
            'delta'      => 'om-maximenu-' . $menu_id,                                     
            'theme'      => $theme_default,   
            'status'     => $status,                   
            'weight'     => $maximenu_content['block_options']['weight'],                                 
            'region'     => $maximenu_content['block_options']['region'],  
            'visibility' => (int) $maximenu_content['menu_visibility']['visibility'],
            'pages'      => trim($maximenu_content['menu_visibility']['pages']),
            'cache'      => -1,  // no cache
          ))->execute();
      }                     
    }
    elseif ($maximenu_content['output'] == 'float') {      
      // menu as float must be deleted from block table
      db_delete('block')->condition('module', 'om_maximenu')->condition('delta', 'om-maximenu-' . $menu_id)->execute();      
    }
    elseif ($maximenu_content['output'] == 'main_menu') {
      // menu as main_menu must be deleted from blocks table
      db_delete('block')->condition('module', 'om_maximenu')->condition('delta', 'om-maximenu-' . $menu_id)->execute();      
    }  
  }
}

          
/**
 * Get the hierarchy of menus
 *
 */
function om_menu_tree_options(&$options, $menus, $menu_name = 'navigation') {
  global $_om_maximenu_existing_menus;
  $existing = $_om_maximenu_existing_menus[$menu_name];
        
  static $level = 1; // count level for indents on divs on html file
  $tab = '-';        // init tab for indents on divs on html file
  // for source formating
  // calculates number of tabs
  for ($i = 1; $i < $level; $i++) {
    $tab .= '-';
  }    
  foreach ($menus as $mlid => $link_title) {
    if (isset($existing[$mlid])) {
      $level++; 
      $options[$mlid] = $tab . $link_title;
      om_menu_tree_options($options, $existing[$mlid], $menu_name);
      $level--;       
    }
    else {
      $options[$mlid] = $tab . $link_title;
    }
  }
}


/**
 * Path queries 
 *
 * On D7 $option['query'] becomes purely an array
 * cannot accept anymore a string
 *
 */
function om_path_query($path_query = array()) { 
  //dsm($path_query);       
  $out = array();
  $queries = explode('&', $path_query);

  if (!empty($queries)) {
    foreach ($queries as $kquery => $query) {
      if (!empty($query)) {
        $query = explode('=', $query);
        if (!empty($query)) $out[$query[0]] = $query[1];
      }
    }
  }
  return $out;
} 
