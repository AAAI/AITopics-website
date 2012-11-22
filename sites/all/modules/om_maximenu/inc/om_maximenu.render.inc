<?php
// $Id$

/**
 * @file
 * OM Maximenu Render
 *
 * @author: Daniel Honrade http://drupal.org/user/351112
 *
 */

/**
 * Implementation of hook_init().
 *
 */
function om_maximenu_init() {
  global $_om_maximenu_variable;
  global $_om_maximenu_block_classes;  
  global $_om_maximenu_visible_blocks;
  
  $_om_maximenu_visible_blocks = om_maximenu_blocks_visibility();  
  // add om tools custom block classes    
  if (module_exists('om_tools')) {

    $om_tools_values = variable_get('om_tools', '');
    $om_blocks = isset($om_tools_values['block']) ? $om_tools_values['block']: array();
    
    if (!empty($om_blocks) && ($om_blocks['block_classes_switch'] == 1)) {
      $_om_maximenu_block_classes = array();
      foreach ($om_blocks as $block_key => $block_class) {
        $block  = explode('___', $block_key);
        if (isset($block[1])) { // filters switch/reset
          $module = $block[1];
          $delta  = $block[2];
          $_om_maximenu_block_classes[$module][$delta] = $block_class;
        }
      }
    }
  }  
  
  $_om_maximenu_variable  = variable_get('om_maximenu', array());  
}

  
/**
 * Loads styles and js
 *
 */
function om_maximenu_asset_loader($menu_content = array()) {

  // update change from tabbed to style
  $menu_content['style'] = isset($menu_content['tabbed']) ? $menu_content['tabbed']: $menu_content['style'];

  // options
  $action        = isset($menu_content['action']) ? $menu_content['action']: 'hover';  
  $active        = isset($menu_content['active']) ? $menu_content['active']: 0;  
  $longmenu      = isset($menu_content['longmenu']) ? $menu_content['longmenu']: FALSE;  
  $displace      = isset($menu_content['displace']) ? $menu_content['displace']: 0;  
  $skin          = isset($menu_content['skin']) ? $menu_content['skin']: 'bubble';
  $animated_bg   = isset($menu_content['animated_bg']) ? 1: 0; 
  $animated_link = isset($menu_content['animated_link']) ? 1: 0; 
  $jiggle        = isset($menu_content['jiggle']) ? 1: 0; 
  $scroll        = isset($menu_content['scroll']) ? 1: 0;                       
  $style         = isset($menu_content['style']) ? $menu_content['style']: '';                                                     
  $dock          = isset($menu_content['float_options']['dock']) ? 1: 0;
  $maximenu_name = om_string_name($menu_content['title']); 

  // add action per OM Maximenu
  om_maximenu_action($action, $maximenu_name, $displace, $style); 
              
  // add css per OM Maximenu     
  drupal_add_css(OM_MAXIMENU_PATH . '/skin/' . $skin . '/' . $skin . '.css');

  // add 'active-trail' to menu li when submenus are active
  if ($active) om_maximenu_active_js($maximenu_name, $animated_bg, $style); 
      
  // scroll long menu if it won't fit the container
  if ($longmenu) om_maximenu_longmenu_js($maximenu_name, $longmenu); 
 
  // animated bg effect
  if ($animated_bg) om_maximenu_bg_slider($maximenu_name); 

  // animated link effect
  if ($animated_link) om_maximenu_link_slider($maximenu_name); 

  // animated link jiggle
  if ($jiggle) om_maximenu_jiggle($maximenu_name);

  // scrolling float
  if ($scroll) om_maximenu_scroll($maximenu_name);
        
  // jquery fisheye dock effect        
  if ($dock) om_maximenu_dock($maximenu_name);
}


/**
 * Load all OM Maximenu blocks
 * 
 * This will process all created OM Maximenu blocks
 *
 */
function om_maximenu_block_load($maximenu_delta = '') {
  global $_om_maximenu_variable;
  $maximenu = $_om_maximenu_variable;
   
  $maximenu_delta = explode('-', $maximenu_delta);
  // ex. $maximenu_delta = 'om-maximenu-1';
  $delta = $maximenu_delta[2];
  
  $out = array();
  // om delta always starts with 1
  if (isset($maximenu[$delta]) && ($delta != 0)) {
    if (!empty($maximenu) && ($maximenu[$delta]['output'] == 'block')) {
      $visibility_values = isset($maximenu[$delta]['menu_visibility']) ? $maximenu[$delta]['menu_visibility']: array('visibility' => 0, 'pages' => '');
      $visibility = om_maximenu_menu_visibility($visibility_values);
      if ($visibility) { 
      
        // update change from tabbed to style
        $maximenu[$delta]['style'] = isset($maximenu[$delta]['tabbed']) ? $maximenu[$delta]['tabbed']: $maximenu[$delta]['style'];
      
        // exclude modal window jquery scripts because these will be loaded on om_maximenu_load()
        // effects won't load on disabled blocks 
        if (!empty($maximenu[$delta]['block_options']['region']) && ($maximenu[$delta]['style'] == '')) {
          om_maximenu_asset_loader($maximenu[$delta]);
        } 
        elseif (!empty($maximenu[$delta]['block_options']['region']) && (isset($maximenu[$delta]['style']) && ($maximenu[$delta]['style'] != 'modal'))) {
          om_maximenu_asset_loader($maximenu[$delta]);
        }
        $out['title']   = check_plain($maximenu[$delta]['title']);
        $out['content'] = om_maximenu_links_render($maximenu[$delta], $delta);
      }
    }
  }
  return $out;  
}


/**
 * Load all OM Maximenu blocks
 * 
 * This will process the main menu and floating blocks
 *
 */
function om_maximenu_load() {
  global $_om_maximenu_variable;
  $maximenu = $_om_maximenu_variable;

  $out = array();  
  if (!empty($maximenu)) {
    foreach ($maximenu as $menu_key => $menu_content) {
    
      // update change from tabbed to style
      $menu_content['style'] = isset($menu_content['tabbed']) ? $menu_content['tabbed']: $menu_content['style'];
    
      $visibility_values = isset($menu_content['menu_visibility']) ? $menu_content['menu_visibility']: array('visibility' => 0, 'pages' => '');
      $visibility = om_maximenu_menu_visibility($visibility_values);
      // render for non-block only
      if ($visibility && ($menu_content['output'] != 'block')) {
        if (!isset($menu_content['style']) || empty($menu_content['style'])) {
          om_maximenu_asset_loader($menu_content);
        }
        elseif (isset($menu_content['style']) && ($menu_content['style'] != 'modal')) {
          om_maximenu_asset_loader($menu_content);
        }
        $out[$menu_key]['content'] = om_maximenu_links_render($maximenu[$menu_key], $menu_key);             
      }   
      // for modal
      if ($visibility) {  
      
        //$out[$menu_key]['title'] = $menu_content['title'];
        $out[$menu_key]['output'] = $menu_content['output'];
        
        // when to render modal content
        if ((isset($menu_content['style']) && ($menu_content['style'] == 'modal')) && (isset($menu_content['block_options']['region']) && !empty($menu_content['block_options']['region']))) {
          // if it's output is block and visible in region
          $render_modal_content = 1;
        }
        elseif ((isset($menu_content['style']) && ($menu_content['style'] == 'modal')) && (($menu_content['output'] == 'float') || ( $menu_content['output'] == 'main_menu'))) {
          // if the output is either float or main menu
          $render_modal_content = 1;
        }
        elseif ((($menu_content['output'] == 'block') && empty($menu_content['block_options']['region'])) && (isset($menu_content['modal_content']) && ( $menu_content['modal_content'] == 1))) {
          // force render modal content even if it's not visible in any region
          $render_modal_content = 1;
        }
        else {
          // or do not render modal content
          $render_modal_content = 0;
        }
        
        if ($render_modal_content) {  
          om_maximenu_asset_loader($menu_content);  
          $maximenu_name = om_string_name($menu_content['title']); 
               
          if (empty($out[$menu_key]['modal_windows'])) {
            $out[$menu_key]['modal_windows'] = theme('om_maximenu_modal_content', array('links' => $menu_content, 'maximenu_name' => $maximenu_name)); 
          }
          else {
            $out[$menu_key]['modal_windows'] .= theme('om_maximenu_modal_content', array('links' => $menu_content, 'maximenu_name' => $maximenu_name));           
          }
        }
                
      }
    }
  }
  return $out;  
}


/**
 * OM Maximenu links rendering engine
 *
 */
function om_maximenu_links_render($links = array(), $menu_key = 1) {
  // immediately returns nothing if no links
  if (!isset($links['links'])) return NULL;
  
  // style content
  $style = (isset($links['style']) && ($links['style'] <> '')) ? 1: 0; 
  
  if ($style) {
    // style links        
    if ($links['style'] == 'accordion') {
      $out = theme('om_maximenu_accordion', array('links' => $links, 'menu_key' => $menu_key));  
    }
    elseif ($links['style'] == 'roundabout') {
      $out = theme('om_maximenu_roundabout', array('links' => $links, 'menu_key' => $menu_key));  
    }    
    elseif ($links['style'] == 'modal') {
      $out = theme('om_maximenu_modal', array('links' => $links, 'menu_key' => $menu_key));  
    }
    else {
      $out = theme('om_maximenu_tabbed', array('links' => $links, 'menu_key' => $menu_key));  
    }
  }
  else {
    $out = theme('om_maximenu_submenu', array('links' => $links, 'menu_key' => $menu_key));  
  }
           
  // access on editing
  if (user_access('administer OM Maximenu')) $out .= '<div class="om-maximenu-edit">' . l(t('Maxedit'), 'admin/structure/om-maximenu/' . $menu_key . '/edit', array('query' => drupal_get_destination())) . '</div>';    
  
  return theme('om_maximenu_wrapper', array('links' => $links, 'content' => $out));
}
 
 
/**
 * OM Maximenu content rendering engine
 *
 */
function om_maximenu_content_render($content = array()) {
  global $_om_maximenu_block_classes;
  global $_om_maximenu_visible_blocks;
    
  $visible_blocks = $_om_maximenu_visible_blocks;
  //dsm($visible_blocks);  
  $block_classes = !empty($_om_maximenu_block_classes) ? $_om_maximenu_block_classes: array();  

  $count = 0;
  uasort($content, 'om_sort_by_weight');  
  $total = count($content);
  $out = '';
  foreach ($content as $key => $prop) {
    $count++;
        
    $module     = $prop['module'];
    $delta      = $prop['delta'];        
    $visibility = (isset($prop['visibility']) && ($prop['visibility'] == 1)) ? in_array($module . '__' . $delta, $visible_blocks) ? 1: 0: 1;

    if ($visibility) {    
      $title               = stripslashes(trim($prop['title'])); // allows markups
      $path                = ($prop['title_path'] == '<front>') ? '': $prop['title_path'];      
      $options             = array();
      $options['query']    = isset($prop['title_path_query']) ? om_path_query($prop['title_path_query']): '';
      $options['fragment'] = isset($prop['title_path_fragment']) ? $prop['title_path_fragment']: '';        
      $block_title         = (!empty($prop['title_path'])) ? '<a href="' . url($path, $options) .'" title="' . $title . '">' . $title . '</a>': $title;      
      $block               = module_invoke($module, 'block_view', $delta ); 

      $om_classes = ($count == 1) ? ' first': '';
      if ($count == $total) $om_classes .= ' last'; 
      $om_classes .=  isset($block_classes[$module][$delta]) ? ' ' . $block_classes[$module][$delta]: ''; 

      $out .= theme('om_maximenu_content', array('block' => $block, 'module' => $module, 'delta' => $delta, 'om_classes' => $om_classes, 'title' => $title, 'block_title' => $block_title));                               
    }
  }
  return $out;
} 


/**
 * OM Maximenu content block visibility check
 *
 */
function om_maximenu_blocks_visibility() {
  global $user, $theme_key;
  
  $visible_blocks = array();
  
  $query = db_select('block', 'b');
  $result = $query
    ->fields('b')
    ->condition('b.theme', $theme_key)
    //->condition('b.status', 1)
    ->orderBy('b.region')
    ->orderBy('b.weight')
    ->orderBy('b.module')
    ->addTag('block_load')
    ->addTag('translatable')
    ->execute();

  $block_info = $result->fetchAllAssoc('bid');
  // Allow modules to modify the block list.
  drupal_alter('block_list', $block_info);

  $blocks = array();
  foreach ($block_info as $block) {
    $blocks[$block->module . '__' . $block->delta] = $block;
  }

  // Build an array of roles for each block.
  $block_roles = array();
  $result = db_query('SELECT module, delta, rid FROM {block_role}');
  foreach ($result as $record) {
    $block_roles[$record->module][$record->delta][] = $record->rid;
  }

  foreach ($blocks as $key => $block) {
    //if (!isset($block->theme) || !isset($block->status) || $block->theme != $theme_key || $block->status != 1) {
      // This block was added by a contrib module, leave it in the list.
      //continue;
    //}

    // If a block has no roles associated, it is displayed for every role.
    // For blocks with roles associated, if none of the user's roles matches
    // the settings from this block, remove it from the block list.
    if (isset($block_roles[$block->module][$block->delta]) && !array_intersect($block_roles[$block->module][$block->delta], array_keys($user->roles))) {
      // No match.
      unset($blocks[$key]);
      continue;
    }

    // Use the user's block visibility setting, if necessary.
    if ($block->custom != BLOCK_CUSTOM_FIXED) {
      if ($user->uid && isset($user->data['block'][$block->module][$block->delta])) {
        $enabled = $user->data['block'][$block->module][$block->delta];
      }
      else {
        $enabled = ($block->custom == BLOCK_CUSTOM_ENABLED);
      }
    }
    else {
      $enabled = TRUE;
    }

    // Limited visibility blocks must list at least one page.
    if ($block->visibility == BLOCK_VISIBILITY_LISTED && empty($block->pages)) {
      $enabled = FALSE;
    }

    if (!$enabled) {
      unset($blocks[$key]);
      continue;
    }

    // Match path if necessary.
    if ($block->pages) {
      // Convert path to lowercase. This allows comparison of the same path
      // with different case. Ex: /Page, /page, /PAGE.
      $pages = drupal_strtolower($block->pages);
      if ($block->visibility < BLOCK_VISIBILITY_PHP) {
        // Convert the Drupal path to lowercase
        $path = drupal_strtolower(drupal_get_path_alias($_GET['q']));
        // Compare the lowercase internal and lowercase path alias (if any).
        $page_match = drupal_match_path($path, $pages);
        if ($path != $_GET['q']) {
          $page_match = $page_match || drupal_match_path($_GET['q'], $pages);
        }
        // When $block->visibility has a value of 0 (BLOCK_VISIBILITY_NOTLISTED),
        // the block is displayed on all pages except those listed in $block->pages.
        // When set to 1 (BLOCK_VISIBILITY_LISTED), it is displayed only on those
        // pages listed in $block->pages.
        $page_match = !($block->visibility xor $page_match);
      }
      elseif (module_exists('php')) {
        $page_match = php_eval($block->pages);
      }
      else {
        $page_match = FALSE;
      }
    }
    else {
      $page_match = TRUE;
    }

    if (!$page_match) {
      unset($blocks[$key]);
    }
    if ($page_match) $visible_blocks[] = $key;    
  }  
  //dsm($visible_blocks);
  return $visible_blocks;
}


/**
 * Process variables for om_maximenu_submenu.tpl.php
 *
 */
function template_preprocess_om_maximenu_submenu(&$vars) {

  // sort by weight
  uasort($vars['links']['links'], 'om_sort_by_weight'); 
  
  $links = $vars['links'];
  $vars['skin'] = $links['skin'];  
  // menu name
  $vars['maximenu_name'] = om_string_name($links['title']); 

  // disable link when active
  $vars['disabled'] = (isset($links['disabled']) && ($links['disabled'] == 1)) ? 1: 0; 
  
  $vars['code'] = $links['code'];

  $vars['count'] = 0;
  $vars['total'] = count($links['links']);
        
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu__' . $vars['menu_key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu__' . preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu__' . $links['output'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_submenu_links.tpl.php
 *
 */
function template_preprocess_om_maximenu_submenu_links(&$vars) {
    
  $disabled = $vars['disabled'];
  $content  = $vars['content'];

  // user roles and permissions
  $vars['permission'] = om_maximenu_link_visible($vars['content']['roles']);

  // php on title
  if (isset(  $content['php_option']) && ($content['php_option'] == 1)) {
    ob_start();
      $content['link_title'] = eval($content['link_title']);
      //$output = ob_get_contents();
    ob_end_clean(); 
  }
        
  // span id
  $span_id = (!empty($content['id'])) ? ' id="' . om_string_name($content['id']) . '"': '';        

  // span title
  $span_title = (!empty($content['description']) && ($content['description_option'] == 'hover')) ? 'title="' . check_plain($content['description']) . '"': ''; 
    
  // link content      
  $link_option = om_maximenu_link_option($content);
      
  // a link attributes
  $attributes = om_maximenu_link_attrib($content);

  // link is disabled when it's on its page
  $uri = trim($_SERVER['REQUEST_URI']);
  $qpath =  $_GET['q'];
	$path_relative = base_path() . $content['path'];

  // Drupal doesn't accept # as valid path on field input
  $options = array();       
  $options['query']    = isset($content['path_query']) ? om_path_query($content['path_query']): '';
  $options['fragment'] = isset($content['path_fragment']) ? $content['path_fragment']: '';                
				
	// disables link when active
  $disable_active_link = ($disabled) ? (($path_relative != $uri) && ($content['path'] != $qpath)) ? 1: 0: 1;
			
  if (!empty($content['path']) && $disable_active_link) {
    $path = ($content['path'] == '<front>') ? '': $content['path'];
    // there are too many things inside this $link_options which a regular l() can't handle
    //$out .= l($content['link_title'], $content['path'], array('attributes' => $attributes));
    $vars['om_link'] = '<a ' . drupal_attributes($attributes) . ' href="' . url($path, $options) . '">' . $link_option . '</a>';
  }
  elseif (!empty($options['query']) || !empty($options['fragment'])) {
    // base path is not necessary when path is empty but anchor is not or query, 
    $vars['om_link'] = '<a ' . drupal_attributes($attributes) . ' href="' . url('', $options) . '">' . $link_option . '</a>';
  }      
  else {
    // title with javascript should have div tag
    $script_link = om_string_name($content['link_title'], FALSE);
      
    // title contains some block elements 
    $link_tag = ((isset($content['php_option']) && ($content['php_option'] == 1)) || ($script_link == 'Script Link')) ? 'div': 'span';

    // useful when you just want a button for getting the content to show and not actually linking to anything
    $vars['om_link'] = '<' . $link_tag . $span_id . ' class="' . $attributes['class'] . '" ' . $span_title . '>' . $link_option . '</' . $link_tag . '>';
  }
  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);              
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu_links__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu_links__' . $maximenu_name . '_' . $vars['key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu_links__' . om_string_name($content['link_title']);
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_submenu_content.tpl.php
 *
 */
function template_preprocess_om_maximenu_submenu_content(&$vars) {
  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);                
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu_content__' . $vars['skin'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu_content__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_submenu_content__' . $maximenu_name . '_' . $vars['key'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_tabbed.tpl.php
 *
 */
function template_preprocess_om_maximenu_tabbed(&$vars) {

  // sort by weight
  uasort($vars['links']['links'], 'om_sort_by_weight'); 
  
  $links = $vars['links'];
  
  // menu name
  $vars['maximenu_name'] = om_string_name($links['title']); 

  // disable link when active
  $vars['disabled'] = (isset($links['disabled']) && ($links['disabled'] == 1)) ? 1: 0; 
  
  $vars['code'] = $links['code'];

  $vars['count'] = 0;
  $vars['total'] = count($links['links']);
        
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed__' . $vars['menu_key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed__' . preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed__' . $links['output'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_tabbed_links.tpl.php
 *
 */
function template_preprocess_om_maximenu_tabbed_links(&$vars) {
  
  $content = $vars['content'];
  
    // user roles and permissions
  $vars['permission'] = om_maximenu_link_visible($vars['content']['roles']);

  // php on title
  if (isset(  $content['php_option']) && ($content['php_option'] == 1)) {
    ob_start();
      $content['link_title'] = eval($content['link_title']);
      //$output = ob_get_contents();
    ob_end_clean(); 
  }
        
  // span id
  $span_id = (!empty($content['id'])) ? ' id="' . om_string_name($content['id']) . '"': '';        

  // span title
  $span_title = (!empty($content['description']) && ($content['description_option'] == 'hover')) ? 'title="' . check_plain($content['description']) . '"': ''; 
    
  // link content      
  $link_option = om_maximenu_link_option($content);
      
  // a link attributes
  $attributes = om_maximenu_link_attrib($content);

  // link is disabled when it's on its page
  $uri = trim($_SERVER['REQUEST_URI']);
  $path_relative = base_path() . $content['path'];

  // Drupal doesn't accept # as valid path on field input
  $options = array();
  $options['query']    = isset($content['path_query']) ? $content['path_query']: '';
  $options['fragment'] = isset($content['path_fragment']) ? $content['path_fragment']: '';        
			
  // title with javascript should have div tag
  $script_link = om_string_name($content['link_title'], FALSE);
      
  // title contains some block elements 
  $link_tag = ((isset($content['php_option']) && ($content['php_option'] == 1)) || ($script_link == 'Script Link')) ? 'div': 'span';

  // useful when you just want a button for getting the content to show and not actually linking to anything
  $vars['om_link'] = '<' . $link_tag . $span_id . ' class="' . $attributes['class'] . '" ' . $span_title . '>' . $link_option . '</' . $link_tag . '>';

  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);                
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed_links__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed_links__' . $maximenu_name . '_' . $vars['key'];  
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed_links__' . om_string_name($content['link_title']);
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_tabbed_content.tpl.php
 *
 */
function template_preprocess_om_maximenu_tabbed_content(&$vars) {
  $links = $vars['links'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed_content__' . $links['style'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_tabbed_content__' . preg_replace('/-/', '_', $vars['maximenu_name']);
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_modal.tpl.php
 *
 */
function template_preprocess_om_maximenu_modal(&$vars) {

  // sort by weight
  uasort($vars['links']['links'], 'om_sort_by_weight'); 
  
  $links = $vars['links'];
  
  // menu name
  $vars['maximenu_name'] = om_string_name($links['title']); 

  // disable link when active
  $vars['disabled'] = (isset($links['disabled']) && ($links['disabled'] == 1)) ? 1: 0; 
  
  $vars['code'] = $links['code'];

  $vars['count'] = 0;
  $vars['total'] = count($links['links']);
        
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal__' . $vars['menu_key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal__' . preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal__' . $links['output'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_modal_links.tpl.php
 *
 */
function template_preprocess_om_maximenu_modal_links(&$vars) {
  
  $content = $vars['content'];
  
    // user roles and permissions
  $vars['permission'] = om_maximenu_link_visible($vars['content']['roles']);

  // php on title
  if (isset(  $content['php_option']) && ($content['php_option'] == 1)) {
    ob_start();
      $content['link_title'] = eval($content['link_title']);
      //$output = ob_get_contents();
    ob_end_clean(); 
  }
        
  // span id
  $span_id = (!empty($content['id'])) ? ' id="' . om_string_name($content['id']) . '"': '';        

  // span title
  $span_title = (!empty($content['description']) && ($content['description_option'] == 'hover')) ? 'title="' . check_plain($content['description']) . '"': ''; 
    
  // link content      
  $link_option = om_maximenu_link_option($content);
      
  // a link attributes
  $attributes = om_maximenu_link_attrib($content);

  // link is disabled when it's on its page
  $uri = trim($_SERVER['REQUEST_URI']);
  $path_relative = base_path() . $content['path'];

  // Drupal doesn't accept # as valid path on field input
  $options = array();
  $options['query']    = isset($content['path_query']) ? $content['path_query']: '';
  $options['fragment'] = isset($content['path_fragment']) ? $content['path_fragment']: '';        
			
  // title with javascript should have div tag
  $script_link = om_string_name($content['link_title'], FALSE);
      
  // title contains some block elements 
  if ((isset($content['php_option']) && ($content['php_option'] == 1)) || ($script_link == 'Script Link')) {
    // any link with scripts cannot be processed as modal  
    $vars['om_link'] = '<div' . $span_id . ' class="' . $attributes['class'] . '" ' . $span_title . '>' . $link_option . '</div>';  
  }
  else {
    // useful when you just want a button for getting the content to show and not actually linking to anything
    $vars['om_link'] = '<a' . $span_id . ' href="#om-modal-content-' . $vars['code'] . '-' . $vars['key'] . '" rel="om-maximenu-modal" class="' . $attributes['class'] . '" ' . $span_title . '>' . $link_option . '</a>';
  }          
  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);  
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal_links__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal_links__' . $maximenu_name . '_' . $vars['key'];  
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal_links__' . om_string_name($content['link_title']);
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_modal_content.tpl.php
 *
 */
function template_preprocess_om_maximenu_modal_content(&$vars) {
  $links = $vars['links'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal_content__' . $links['style'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal_content__' . $links['skin'];  
  $vars['theme_hook_suggestions'][] = 'om_maximenu_modal_content__' . preg_replace('/-/', '_', $vars['maximenu_name']);
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_accordion.tpl.php
 *
 */
function template_preprocess_om_maximenu_accordion(&$vars) {

  // sort by weight
  uasort($vars['links']['links'], 'om_sort_by_weight'); 
  
  $links = $vars['links'];
  
  // menu name
  $vars['maximenu_name'] = om_string_name($links['title']); 

  // disable link when active
  $vars['disabled'] = (isset($links['disabled']) && ($links['disabled'] == 1)) ? 1: 0; 
  
  $vars['code'] = $links['code'];

  $vars['count'] = 0;
  $vars['total'] = count($links['links']);
        
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion__' . $vars['menu_key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion__' . preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion__' . $links['output'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_accordion_links.tpl.php
 *
 */
function template_preprocess_om_maximenu_accordion_links(&$vars) {
  
  $content = $vars['content'];
  
    // user roles and permissions
  $vars['permission'] = om_maximenu_link_visible($vars['content']['roles']);

  // php on title
  if (isset(  $content['php_option']) && ($content['php_option'] == 1)) {
    ob_start();
      $content['link_title'] = eval($content['link_title']);
      //$output = ob_get_contents();
    ob_end_clean(); 
  }
        
  // span id
  $span_id = (!empty($content['id'])) ? ' id="' . om_string_name($content['id']) . '"': '';        

  // span title
  $span_title = (!empty($content['description']) && ($content['description_option'] == 'hover')) ? 'title="' . check_plain($content['description']) . '"': ''; 
    
  // link content      
  $link_option = om_maximenu_link_option($content);
      
  // a link attributes
  $attributes = om_maximenu_link_attrib($content);

  // dt style doesn't need any a tag
  $vars['om_accordion_dt'] = '<dt' . $span_id . ' class="' . $attributes['class'] . '" ' . $span_title . '>' . $link_option . '</dt>';

  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion_links__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion_links__' . $maximenu_name . '_' . $vars['key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion_links__' . om_string_name($content['link_title']);
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_accordion_content.tpl.php
 *
 */
function template_preprocess_om_maximenu_accordion_content(&$vars) {
  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion_content__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_accordion_content__' . $maximenu_name . '_' . $vars['key'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_roundabout.tpl.php
 *
 */
function template_preprocess_om_maximenu_roundabout(&$vars) {

  // sort by weight
  uasort($vars['links']['links'], 'om_sort_by_weight'); 
  
  $links = $vars['links'];
  
  // menu name
  $vars['maximenu_name'] = om_string_name($links['title']); 

  // disable link when active
  $vars['disabled'] = (isset($links['disabled']) && ($links['disabled'] == 1)) ? 1: 0; 
  
  $vars['code'] = $links['code'];

  $vars['count'] = 0;
  $vars['total'] = count($links['links']);
        
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout__' . $vars['menu_key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout__' . preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout__' . $links['output'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_roundabout_links.tpl.php
 *
 */
function template_preprocess_om_maximenu_roundabout_links(&$vars) {
    
  $content = $vars['content'];
  
    // user roles and permissions
  $vars['permission'] = om_maximenu_link_visible($vars['content']['roles']);

  // php on title
  if (isset(  $content['php_option']) && ($content['php_option'] == 1)) {
    ob_start();
      $content['link_title'] = eval($content['link_title']);
      //$output = ob_get_contents();
    ob_end_clean(); 
  }
        
  // span id
  $span_id = (!empty($content['id'])) ? ' id="' . om_string_name($content['id']) . '"': '';        

  // span title
  $span_title = (!empty($content['description']) && ($content['description_option'] == 'hover')) ? 'title="' . check_plain($content['description']) . '"': ''; 
    
  // link content      
  $link_option = om_maximenu_link_option($content);
      
  // a link attributes
  $attributes = om_maximenu_link_attrib($content);

  // title with javascript should have div tag
  $script_link = om_string_name($content['link_title'], FALSE);
      
  // title contains some block elements 
  $link_tag = ((isset($content['php_option']) && ($content['php_option'] == 1)) || ($script_link == 'Script Link')) ? 'div': 'span';

  // useful when you just want a button for getting the content to show and not actually linking to anything
  $vars['om_link'] = '<' . $link_tag . $span_id . ' class="' . $attributes['class'] . '" ' . $span_title . '>' . $link_option . '</' . $link_tag . '>';

  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);              
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout-links__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout-links__' . $maximenu_name . '_' . $vars['key'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout-links__' . om_string_name($content['link_title']);
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_roundabout_content.tpl.php
 *
 */
function template_preprocess_om_maximenu_roundabout_content(&$vars) {
  $maximenu_name = preg_replace('/-/', '_', $vars['maximenu_name']);
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout_content__' . $maximenu_name;
  $vars['theme_hook_suggestions'][] = 'om_maximenu_roundabout_content__' . $maximenu_name . '_' . $vars['key'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_wrapper.tpl.php
 *
 */
function template_preprocess_om_maximenu_wrapper(&$vars) {

  $links = $vars['links'];
  $vars['maximenu_name'] = om_string_name($links['title']); 
  $vars['link_code']     = isset($links['code']) ? trim($links['code']): 'om-u'. $vars['user']->uid . '-' . mt_rand();  

  $vars['theme_hook_suggestions'][] = 'om_maximenu_wrapper__' . $vars['link_code'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_wrapper__' . $vars['maximenu_name'];
  //dsm($vars);
}


/**
 * Process variables for om_maximenu_content.tpl.php
 *
 */
function template_preprocess_om_maximenu_content(&$vars) {
  if (!isset($vars['block']['content'])) $vars['block']['content'] = '';   
  $vars['theme_hook_suggestions'][] = 'om_maximenu_content__' . $vars['module'];
  $vars['theme_hook_suggestions'][] = 'om_maximenu_content__' . $vars['module'] . '_' . $vars['delta'];
  //dsm($vars);
}


