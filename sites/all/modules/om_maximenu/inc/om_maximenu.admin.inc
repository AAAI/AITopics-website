<?php
// $Id$

/**
 * @file
 * OM Maximenu Admin Configuration
 *
 * @author: Daniel Honrade http://drupal.org/user/351112
 *
 */

/**
 * Admin Form - Simple Editing
 *
 */
function om_maximenu_admin($form, &$form_state, $op = NULL) {
  global $_om_maximenu_variable;
  $maximenu = $_om_maximenu_variable;
  
  if (!empty($maximenu)) ksort($maximenu);
  
  // output options
  om_maximenu_admin_js();

  if ($op == NULL) drupal_set_title(t('OM Maximum Edit'));

  //dsm($maximenu);
  $form = array();
  $form['om_maximenus'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  // Existing menus and links
  $maximenu_array = array();
      
  if ($op == 'list') { // Basic Editing

    drupal_set_title(t('OM Maximenu Basic Editing'));  

    $header = array(t('Menu'), t('Output'), t('Style'), t('Links'), '', '');
    $rows = array();
    $style_options = array('' => 'Dropdown', 'normal' => 'Tab', 'scrollh' => 'Tab with Horizontal Scroll', 'scrollv' => 'Tab with Vertical Scroll', 'roundabout' => 'Round About', 'accordion' => 'Horizontal Accordion', 'modal' => 'Modal Window');          
    if (!empty($maximenu)) {      
      foreach ($maximenu as $menu_key => $menu_content) {

        // update change from tabbed to style
        $menu_content['style'] = isset($menu_content['tabbed']) ? $menu_content['tabbed']: $menu_content['style'];

        $row = array();
        $row[] = om_string_name($menu_content['title'], FALSE);
        $row[] = om_string_name($menu_content['output'], FALSE); 
        $row[] = $style_options[$menu_content['style']];         
        $row[] = (isset($menu_content['links']) ? count($menu_content['links']): 0);                       
        $row[] = l(t('Edit'), 'admin/structure/om-maximenu/' . $menu_key . '/edit', array('attributes' => array('title' => t('Edit this menu.'))));
        $row[] = l(t('Delete'), 'admin/structure/om-maximenu/' . $menu_key . '/delete', array('attributes' => array('title' => t('Delete this menu.'))));
        $rows[] = array('data' => $row);
      }
    }  
    $rows[] = array(
      'data' => array(
        'data' => l(t('Add menu'), 'admin/structure/om-maximenu/add', array('attributes' => array('title' => t('Add another menu.')))),
        'attributes' => array('colspan' => 4),
      ), 
    );      
    $menus = theme('table', array('header' => $header, 'rows' => $rows));

    $maximenu_array[0] = array(
      '#type'        => 'markup',
      '#markup'       => $menus,
    );        
    $form['om_maximenus'] += $maximenu_array;       
  }  
  else {
  
    // getting existing roles
    $result = db_query("SELECT rid, name, weight FROM {role} ORDER BY rid ASC");
  
    $roles = array();  
    while ($record = $result->fetchObject()) {
      $roles[$record->rid] = $record->name;
    }

    if (!empty($maximenu)) {
    
      if ($op == 'delete') {
        $args = arg();
        $menu_key = $args[3];
        drupal_set_title(t('Are you sure you want to delete @menu?', array('@menu' => $maximenu[$menu_key]['title'])));   
        $maximenu_array[$menu_key]['delete'] = array(
          '#type'  => 'hidden',
          '#default_value' => 1,
        );
      }    
      // edit only 1 menu
      if ($op == 'edit') {
      
        drupal_set_title(t('OM Maximenu Edit'));   
         
        $args = arg();
        $menu_key = $args[3];
        $this_menu[$menu_key] = isset($maximenu[$menu_key]) ? $maximenu[$menu_key]: drupal_goto('admin/structure/om-maximenu');
        $maximenu = $this_menu;
      }    
      if (($op == 'edit') || ($op == NULL)) {    
      
        foreach ($maximenu as $menu_key => $menu_content) {
          // counting active links
          $active_links = isset($menu_content['links']) ? count($menu_content['links']): 0;
      
          $maximenu_array[$menu_key] = array(
            '#type'        => 'fieldset',
            '#title'       => om_string_name($menu_content['title'], FALSE),
            '#collapsible' => TRUE,
            '#collapsed'   => (arg(3) == 'maxedit') ? TRUE: FALSE,
            '#attributes' => array('id' => 'om-maximenu-fieldset-' . om_string_name($menu_content['title']), 'class' => array('om-maximenu-fieldset')),                
            '#description' => ($active_links != 0) ? t('Active Link/s: ') . '(' . $active_links . ')': t('This menu will only show if you have added link/s.'),
          );  
          //dsm($menu_content);
          $menu_content['menu_key']   = $menu_key;
          $menu_content['roles']      = $roles;    
          $maximenu_array[$menu_key] += _om_maximenu_admin($menu_content);
        }
      }
    }

    if (($op == 'add') || ($op == NULL)) {
    
      if ($op == 'add') drupal_set_title(t('OM Maximenu Add'));  
      // New menu and links
      // new ids relies on highest menu key id
      // adding only 1 new blank menu
      $count = !empty($maximenu) ? max(array_keys($maximenu)) + 1: 1;

      $menu_content_new = array(
        'title'    => 'New Menu',
        'skin'     => 'bubble',        
        'style'    => '', 
        'fade'     => 0,   
        'output'   => 'block',        
        'links'    => array(),
        'menu_key' => $count,
        'roles'    => $roles,
      );   

      $maximenu_array[$count] = array(
        '#type'        => 'fieldset',
        '#title'       => 'New Menu',
        '#collapsible' => TRUE,
        '#collapsed'   => (arg(3) == 'maxedit') ? TRUE: FALSE,
        '#attributes' => array('id' => 'om-maximenu-fieldset-new-menu', 'class' => array('om-maximenu-fieldset')),                
        '#description' => t('Change "New Menu" title to active this menu.'),
      );  

      $maximenu_array[$count] += _om_maximenu_admin($menu_content_new);

    }
    
    $form['om_maximenus'] += $maximenu_array; 

    if ($op == 'delete') {
      $form['delete'] = array( '#type' => 'submit', '#value' => t('Delete'), );
    }
    else {
      $form['save'] = array( '#type' => 'submit', '#value' => t('Save'), );
    }  
    $form['cancel'] = array( '#type' => 'submit', '#value' => t('Cancel'), );   
  }
  return $form;
}


/**
 * Admin Form
 *
 */
function _om_maximenu_admin($menu_content = array()) {
  global $user;
  static $first_menu = 0;
  $skin     = om_maximenu_skin_get();
  $roles    = $menu_content['roles'];
  $menu_key = $menu_content['menu_key'];
  $code     = 'om-u'. $user->uid . '-' . mt_rand();  
  $out = array();
  // delete field is for saved menus only
  if (trim($menu_content['title']) != 'New Menu') {
    $out['delete'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Delete this menu.'),    
      '#default_value' => 0,
    );
  }
  $out['code'] = array(
    '#type'  => 'hidden',
    '#default_value' => isset($menu_content['code']) ? trim($menu_content['code']): $code,
  );  
  $out['title'] = array(
    '#type'  => 'textfield',
    '#title' => t('Menu Title'),  
    '#required' => TRUE,  
    '#default_value' => isset($menu_content['title']) ? stripslashes(trim($menu_content['title'])): 'New Menu',
  );
  $out['action'] = array(
    '#type'  => 'select',
    '#title' => t('Mouse Action'),
    '#options' => array('hover' => 'Hover', 'hover_fade' => 'Hover (fade/slow)', 'click_fast' => 'Click (fast)', 'click_slow' => 'Click (slow)'),    
    '#default_value' => isset($menu_content['action']) ? $menu_content['action']: 'hover',
    '#description' => t('The link content will show depending on mouse action.'),   
  ); 
  $out['skin'] = array(
    '#type'  => 'select',
    '#title' => t('Skin'),
    '#options' => $skin,    
    '#default_value' => isset($menu_content['skin']) ? $menu_content['skin']: 'bubble',
    '#description' => t('Currently, supports dropdown/floating submenu only.'),   
  ); 
  $out['style'] = array(
    '#type'  => 'select',
    '#title' => t('Style'),
    '#attributes' => array('class' => array('om-maximenu-style')),
    '#default_value' => isset($menu_content['style']) ? $menu_content['style']: '',
    '#options' => array('' => 'Dropdown', 'normal' => 'Tab', 'scrollh' => 'Tab with Horizontal Scroll', 'scrollv' => 'Tab with Vertical Scroll', 'roundabout' => 'Round About', 'accordion' => 'Horizontal Accordion', 'modal' => 'Modal Window', ),
    '#description' => t('
      For Tabs, Round About and Accordion, all menu links will be converted to span tags, <br />
      all settings on menu path, path query and anchor will be ignored.<br />
      The default height is 100px, you can override this by your own css. <br />
      Tab style height can be overriden by adding this to your stylesheet: <br />
      Ex. #om-maximenu-your-menu .om-maximenu-tabbed-content { height: 200px; } <br />
      <ul>
      <li><strong>Dropdown</strong> - blocks will appear as submenus</li>
      <li><strong>Tab</strong> - Normal tab without any effect</li>
      <li><strong>Tab with Horizontal Scroll</strong> - menu content scrolls left and right</li>
      <li><strong>Tab with Vertical Scroll</strong> - menu content scrolls up and down</li>      
      <li><strong>Round About</strong> - menu content moves in circular motion</li>      
      <li><strong>Horizontal Accordion</strong> - menu link and content scrolls left and right (accordion style)</li>      
      <li><strong>Modal Window</strong> - attached blocks will be displayed in popup window</li>      
      </ul>
      '),        
  );
  $out['style_options'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Dropdown Options'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE, 
    '#attributes' => array(
      'class' => array('om-maximenu-nontabbed-options'), 
      'style' => (!isset($menu_content['style']) || ($menu_content['style'] == '')) ? 'display: block;': 'display: none;',
    ),    
  );   
  $out['style_options']['disabled'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Disable link when active'),
    '#default_value' => isset($menu_content['disabled']) ? $menu_content['disabled']: 0,
    '#description' => t('This will make the link unclickable when active.'),       
  ); 
  $out['style_options']['displace'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Displace'),
    '#default_value' => isset($menu_content['displace']) ? $menu_content['displace']: 0,
    '#description' => t('This will move the next link item from its position, which creates a sliding menu effect.'),   
  );    
  $out['other_options'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Other Options'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE, 
    '#attributes' => array(
      'class' => array('om-maximenu-other-options'), 
      'style' => (isset($menu_content['style']) && ($menu_content['style'] != 'roundabout') && ($menu_content['style'] != 'accordion')) ? 'display: block;': 'display: none;',
    ),    
  ); 
  $out['other_options']['active'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Add "active-trail" to menu li when submenus are active'),
    '#default_value' => isset($menu_content['active']) ? $menu_content['active']: 0,
    '#description' => t('This will only be applied to menus with attached menu blocks'),       
  );
  $out['other_options']['longmenu'] = array(
    '#type'  => 'select',
    '#title' => t('Long horizontal menu options'),
    '#options' => array('' => 'No', 'hover' => 'Hover', 'prev_next' => 'Prev/Next'),    
    '#default_value' => isset($menu_content['longmenu']) ? $menu_content['longmenu']: '',
    '#description' => t('
     Set this menu to scroll if it won\'t fit the container.
      <ul>
        <li><strong>Hover</strong> - long menus will scroll on hover</li>
        <li><strong>Prev/Next</strong> - adds prev and next buttons for long menus</li>
      </ul>
      <strong>Note:</strong> If used in menu with dropdown style, it will hide all its attached blocks.
    '),            
  );  
  $out['other_options']['animated_bg'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Animated Background'),
    '#default_value' => isset($menu_content['animated_bg']) ? $menu_content['animated_bg']: 0,
    '#description' => t('Links with sliding background'),   
  ); 
  $out['other_options']['animated_link'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Animated Link'),
    '#default_value' => isset($menu_content['animated_link']) ? $menu_content['animated_link']: 0,
    '#description' => t('Links sliding vertically'),   
  ); 
  $out['other_options']['jiggle'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Jiggly Links'),
    '#default_value' => isset($menu_content['jiggle']) ? $menu_content['jiggle']: 0,
    '#description' => t('Make the links jiggle on hover.'),       
  );   
  $out['other_options']['modal_content'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Render Modal Content'),
    '#default_value' => isset($menu_content['modal_content']) ? $menu_content['modal_content']: 0,
    '#description' => t('For Style: Modal Window with Output: Block - checking this will force render Modal content even if its menu is not visible in any region.'),       
  );    
  $out['scroll'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Auto-Scroll Menu'),
    '#default_value' => isset($menu_content['scroll']) ? $menu_content['scroll']: 0,
    '#description' => t('Make your menu automatically scroll to stay on screen.'),       
  ); 
  $output_options = array('block' => 'Block', 'float' => 'Floating'); 
  // Only 1 main menu is allowed
  // the first menu is always the priority to be with main menu option
  $main_menu_switch = (($first_menu == 0) || (isset($menu_content['main_menu']) && ($menu_content['main_menu'] == 1))) ? 1: 0;
  if ($main_menu_switch == 1) { 
    $output_options = array('block' => 'Block', 'float' => 'Floating', 'main_menu' => 'Main Menu');
    $main_menu_option_desc = t('<li><strong>Main Menu</strong> - You have the option to make this as your main menu. <br />
      This will automatically replace existing main menu for OM Base Theme. <br />
      For other themes, add this &lt;?php print $main_menu_tree; ?&gt; to your page.tpl.php</li>');   
  }
  else {
    $main_menu_option_desc = '';
  }
  
  $first_menu++; // ensures above is executed once of value is 0

  $out['output'] = array(
    '#type'  => 'select',
    '#title' => t('Output Options'),
    '#options' => $output_options,    
    '#default_value' => isset($menu_content['output']) ? $menu_content['output']: 'block',
    '#attributes' => array('class' => array('om-maximenu-output')),        
    '#description' => t('
      <ul>
        <li><strong>Block</strong> - will appear in the block list.</li>
        <li><strong>Floating</strong> - goes left, middle, right of the screen.</li>
        '  . $main_menu_option_desc . '
      </ul>
     '),    
  );  

  $out['block_options'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Block Options'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE, 
    '#attributes' => array(
      'class' => array('om-maximenu-block-options'), 
      'style' => ($menu_content['output'] == 'block') ? 'display: block;': 'display: none;',
    ),    
  );  
  $out['block_options']['stacking'] = array(
    '#type'  => 'select',
    '#title' => t('Stacking'),
    '#options' => array('row' => 'Row', 'column' => 'Column'),    
    '#default_value' => isset($menu_content['block_options']['stacking']) ? $menu_content['block_options']['stacking']: 'row',        
    '#description' => t('
      <ul>
        <li><strong>Row</strong> - all links in 1 row</li>
        <li><strong>Column</strong> -all links in 1 column</li>
      </ul>
     '), 
  ); 
  $out['block_options']['direction'] = array(
    '#type'  => 'select',
    '#title' => t('Block Menu Direction'),
    '#options' => array('block-down' => 'Dropdown','block-up' => 'Dropup', 'block-right' => 'Right', 'block-left' => 'Left'),    
    '#default_value' => isset($menu_content['block_options']['direction']) ? $menu_content['block_options']['direction']: 'block-down', 
    '#attributes' => array('id' => 'block-options-direction-' . $menu_key), 
    '#description' => t('OM Maximenu will popup on this direction. Applicable only with dropdown style.'),                   
  );     

  $theme_default = variable_get('theme_default', 'garland');          
  
  $region_none = array('' => 'none');
  $regions = system_region_list($theme_default);
  $regions = array_merge($region_none, $regions);
  
  $out['block_options']['region'] = array(
    '#type'  => 'select',
    '#title' => t('Region'),
    '#options' => $regions,    
    '#default_value' => isset($menu_content['block_options']['region']) ? $menu_content['block_options']['region']: '', 
    '#description' => t('Select theme region.'),                   
  ); 
  $out['block_options']['weight'] = array(
    '#type'  => 'weight',
    '#title' => t('Block Weight'),
    '#delta' => 200,  
    '#default_value' => isset($menu_content['block_options']['weight']) ? $menu_content['block_options']['weight']: 0, 
    '#description' => t('Negative values have the highest position in the order, so -1 comes first before 0 then 1, ...'),      
  );   
  
  $out['float_options'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Float Options'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE, 
    '#attributes' => array(
      'class' => array('om-maximenu-float-options'), 
      'style' => ($menu_content['output'] == 'float') ? 'display: block;': 'display: none;',
    ),    
  ); 
  
  $out['float_options']['position'] = array(
    '#type'  => 'select',
    '#title' => t('Fixed / Absolute'),
    '#options' => array('fixed' => 'Fixed', 'absolute' => 'Absolute'),    
    '#default_value' => isset($menu_content['float_options']['position']) ? $menu_content['float_options']['position']: 'fixed',
    '#description' => t('
      <ul>
        <li><strong>Fixed</strong> - stays on screen on scrolling</li>
        <li><strong>Absolute</strong> - moves with the page on scrolling</li>
      </ul>
     '),         
  );     
  $out['float_options']['y_origin'] = array(
    '#type'  => 'select',
    '#title' => t('Y-Origin: Top / Bottom'),
    '#options' => array('top' => 'Top', 'bottom' => 'Bottom'),    
    '#default_value' => isset($menu_content['float_options']['y_origin']) ? $menu_content['float_options']['y_origin']: 'bottom',        
  ); 
  $out['float_options']['y_value'] = array(
    '#type'  => 'textfield',
    '#title' => t('Y-Axis Value'),
    '#size'  => 5,
    '#default_value' => isset($menu_content['float_options']['y_value']) ? $menu_content['float_options']['y_value']: 10,        
  );
  $out['float_options']['x_origin'] = array(
    '#type'  => 'select',
    '#title' => t('X-Origin: Left / Middle / Right'),
    '#options' => array('left' => 'Left', 'middle' => 'Middle', 'right' => 'Right'),    
    '#attributes' => array('class' => array('om-maximenu-x-origin')),
    '#default_value' => isset($menu_content['float_options']['x_origin']) ? $menu_content['float_options']['x_origin']: 'left',        
  ); 
  $out['float_options']['non_middle_options'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Non-Middle Options'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE, 
    '#attributes' => array(
      'class' => array('om-maximenu-non-middle-options'), 
      'style' => (isset($menu_content['float_options']['x_origin']) && ($menu_content['float_options']['x_origin'] == 'middle')) ? 'display: none;': 'display: block;',
    ),    
  );  

  $out['float_options']['non_middle_options']['x_value'] = array(
    '#type'  => 'textfield',
    '#title' => t('X-Axis Value'),
    '#size'  => 5,    
    '#default_value' => isset($menu_content['float_options']['x_value']) ? $menu_content['float_options']['x_value']: 10,        
    '#attributes' => array('class' => array('om-maximenu-x-value')),
  );

  $out['float_options']['non_middle_options']['stacking'] = array(
    '#type'  => 'select',
    '#title' => t('Stacking'),
    '#options' => array('row' => 'Row', 'column' => 'Column'),    
    '#default_value' => isset($menu_content['float_options']['stacking']) ? $menu_content['float_options']['stacking']: 'row',        
    '#attributes' => array('class' => array('om-maximenu-stacking')),
    '#description' => t('
      <ul>
        <li><strong>Row</strong> - all links in 1 row</li>
        <li><strong>Column</strong> -all links in 1 column</li>
      </ul>
     '), 
  ); 
  $out['float_options']['non_middle_options']['orientation'] = array(
    '#type'  => 'select',
    '#title' => t('Orientation'),
    '#options' => array('horizontal' => 'Horizontal', 'vertical' => 'Vertical'),    
    '#default_value' => isset($menu_content['float_options']['orientation']) ? $menu_content['float_options']['orientation']: 'horizontal',        
    '#attributes' => array('class' => array('om-maximenu-orientation')),
    '#description' => t('
      <ul>
        <li><strong>Horizontal</strong> - normal text/icon orientation</li>
        <li><strong>Vertical</strong> - left(90deg rotation), right(270deg rotation)</li>
      </ul>
     '), 
  ); 

  $out['float_options']['dock'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Dock Effect'),
    '#default_value' => isset($menu_content['float_options']['dock']) ? $menu_content['float_options']['dock']: 0,
    '#attributes' => array('id' => 'float-options-position-' . $menu_key),                     
    '#description' => t('Mac-like zoom effect on menu icons.'),
  ); 

  // counting active links
  $active_links = isset($menu_content['links']) ? count($menu_content['links']): 0;
  
  $out['links'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Menu Links') . ' (' . $active_links . ')',
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,   
    '#theme' => 'om_maximenu_links_order',     
  );   
  
  $out['links'] += om_maximenu_links_order($menu_content, $roles, $menu_key);
      
  $out['menu_visibility'] = array(
    '#type' => 'fieldset',
    '#title' => t('Menu Visibility'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE, 
    '#description' => t('This will also update block visibility settings on block configuration.'),
  );

  $access = user_access('use PHP for settings');

  if (isset($menu_content['menu_visibility']['visibility']) && $menu_content['menu_visibility']['visibility'] == 2 && !$access) {
    $out['menu_visibility'] = array();
    $out['menu_visibility']['visibility'] = array('#type' => 'value', '#value' => 2);
    $out['menu_visibility']['pages'] = array('#type' => 'value', '#value' => $menu_content['menu_visibility']['pages']);
  }
  else {
    $options = array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.'));
    $description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcardfor every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

    if (module_exists('php') && $access) {
      $options[] = t('Show if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
      $description .= ' '. t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code canbreak your Drupal site.', array('%php' => '<?php ?>'));
    }
    $out['menu_visibility']['visibility'] = array(
      '#type' => 'radios',
      '#title' => t('Show menu on specific pages'),
      '#options' => $options,
      '#default_value' => isset($menu_content['menu_visibility']['visibility']) ? $menu_content['menu_visibility']['visibility']: 0,
    );
    $out['menu_visibility']['pages'] = array(
      '#type' => 'textarea',
      '#title' => t('Pages'),
      '#default_value' => isset($menu_content['menu_visibility']['pages']) ? $menu_content['menu_visibility']['pages']: '',
      '#description' => $description,
    );
  }
  
  return $out;
}


/**
 * OM Links
 *
 */
function om_maximenu_links_order($menu_content = array(), $roles = array(), $menu_key = 0) {

  if (!isset($menu_content['links'])) $menu_content['links'] = array();
  
  // This makes sure that even if existing links are deleted
  // it won't have an id below the existing ones
  // new link ids will depend on the highest existing link id;
  $count = !empty($menu_content['links']) ? max(array_keys($menu_content['links'])) + 1: 1;

  // sort by weight
  uasort($menu_content['links'], 'om_sort_by_weight'); 
    
  $menu_content['links'] += _om_new_link($count, $roles);
 
  $links = array();
  foreach ($menu_content['links'] as $link => $prop) {
    // count enabled blocks 
    $enabled_blocks = (isset($prop['content']) && !empty($prop['content'])) ? count($prop['content']): 0;  

    // field description attached text
    $attached       = ($enabled_blocks) ? ($enabled_blocks == 1) ? '@enabled_blocks block': '@enabled_blocks blocks': 'none';
    $edit_attached  = l(t('<strong>Edit</strong> attached blocks.'), 'admin/structure/om-maximenu/blocks/' . $menu_key . '/' . $link, 
      array(
        'attributes' => array(
          'title' => t('Enable / Disabled blocks attached to this link.'),
        ),
        'query' => drupal_get_destination(),
        'html' => TRUE,
      )
    );
    $new_message = t('You can only enable/disable blocks on this link after you have modified the link title and saved.');    
    
    $link_title     = (isset($prop['link_title']) && !empty($prop['link_title'])) ? trim($prop['link_title']): 'New Link';
   // php on title
    if (isset($prop['php_option']) && ($prop['php_option'] == 1)) {
      ob_start();
      $link_title = eval($prop['link_title']);
      //$output = ob_get_contents();
      ob_end_clean(); 
    } 

    $links['menu_key'] = array(
      '#type'   => 'hidden',
      '#default_value'  => $menu_key,
    ); 
        
    $links[$link] = array(
      '#type'   => 'fieldset',
      '#title'  =>  t(om_string_name($link_title, FALSE)),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
      '#description' => (!empty($link_title) && ($link_title != 'New Link')) ? t('<strong>Attached:</strong> ' . $attached . '<br /><br />' . 
        $edit_attached, array('@enabled_blocks' => $enabled_blocks)): $new_message,         
    );
    
    // delete field is only for existing links
    if ($link_title != 'New Link') {
      $links[$link]['link_delete'] = array(
        '#type'   => 'checkbox',
        '#title'  => t('Delete this link.'),
        '#default_value'  => 0,      
      );    
    }
    $links[$link]['link_title'] = array(
      '#type'   => 'textarea',
      '#title'  => t('Link Title'),
      '#rows' => 1,
      '#required' => TRUE,        
      '#default_value'  => (isset($prop['link_title']) && !empty($prop['link_title'])) ? stripslashes(trim($prop['link_title'])): 'New Link',      
      '#description' => t('You can add inline styling or other attributes, ex. &lt;span class="special-link" style="font-weight: bold;"&gt;Home&lt;/span&gt;'),      
    ); 
    $links[$link]['title_options'] = array(
      '#type'   => 'fieldset',
      '#title'  => t('Title Options'),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );      
    $links[$link]['title_options']['link_title_option'] = array(
      '#type'   => 'select',
      '#title'  => t('Options for Link Title'),
      '#default_value'  => isset($prop['link_title_option']) ? $prop['link_title_option']: 'title',
      '#options' => array('title' => 'Title only', 'title_icon' => 'Title with Icon', 'icon' => 'Icon only'),
    );        
    $links[$link]['title_options']['path_icon'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Icon Path'),
      '#default_value'  => isset($prop['path_icon']) ? trim($prop['path_icon']): '',
      '#description' => t('
        Ex. sites/default/themes/mytheme/css/images/icon_1.png or<br />
        sites/default/files/icon_1.png <br />        
        (By default, this icon will go to the left side of the link title.). <br />
        This can also be done via css properties, ex .link-home { background: url(...)...'),      
    );  
    $links[$link]['title_options']['icon_option'] = array(
      '#type'   => 'checkbox',
      '#title'  => t('Icon Hover Image'),
      '#default_value'  => isset($prop['icon_option']) ? $prop['icon_option']: 0,
      '#description' => t('You can have a mouseover icon, just upload another image, Ex. icon.png, icon_hover.png, <br />
        just add "_hover" to the second image and this will automatically replace your active state icon on mouseover.'),      
    ); 
    $links[$link]['title_options']['php_option'] = array(
      '#type'   => 'checkbox',
      '#title'  => t('Title has PHP'),
      '#default_value'  => isset($prop['php_option']) ? $prop['php_option']: 0,
      '#description' => t('DO NOT include &lt;?php and ?&gt;, always add a return value, <br />
         ex. <strong> global $user; return \'&lt;strong&gt;\' . $user->name . \'&lt;/strong&gt;\';</strong>",<br />
         make sure your code is valid php, any error on this can potentially break your site.'),      
    );    
    $links[$link]['path'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Path'),
      '#default_value'  => isset($prop['path']) ? trim($prop['path']): '',
      '#autocomplete_path' => module_exists('mpac') ? 'mpac/autocomplete': '',
      '#description' => t('
        <ul>
          <li><strong>Front Page</strong> - put &lt;front&gt; as path.</li>
          <li><strong>No Link</strong> - empty path is valid and will transform "a" tag to "span" tag.</li>
        </ul>  
          '),      
    );   
    $links[$link]['path_query'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Path Query'),
      '#default_value'  => isset($prop['path_query']) ? trim($prop['path_query']): array(),
      '#description' => t('Ex. ?destination=node&me=you, but DO NOT include \'?\'.'),      
    ); 
    $links[$link]['path_fragment'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Anchor'),
      '#default_value'  => isset($prop['path_fragment']) ? trim($prop['path_fragment']): '',
      '#description' => t('Ex. #section-1, but DO NOT include \'#\'. This can correspond to the id name of a tag, ex. &lt;div id="section-1",<br /> 
        so you can now target this section by doing "about#section-1", and the page will automatically scroll to that section.'), 
    );          

    $links[$link]['weight'] = array(
      '#type'   => 'weight',
      '#title'  => t('Weight'),
      '#delta' => 50,  
      '#attributes' => array('class' => array('om-weight')),             
      '#default_value'  => isset($prop['weight']) ? $prop['weight']: 0,
      '#description' => t('Negative values have the highest position in the order, so -1 comes first before 0 then 1, ...'),      
    );    
    $links[$link]['attributes'] = array(
      '#type'   => 'fieldset',
      '#title'  => t('Link Attributes'),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );                    
    $links[$link]['attributes']['id'] = array(
      '#type'   => 'textfield',
      '#title'  => t('ID'),
      '#default_value'  => isset($prop['id']) ? trim($prop['id']): '',
      '#description' => t('Make sure this ID is unique.'),      
    );
    $links[$link]['attributes']['class'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Classes'),
      '#default_value'  => isset($prop['class']) ? trim($prop['class']): '',
      '#description' => t('It\'s a good practice to have classes in lowercase and dashes separating words.'),
    );
    $links[$link]['attributes']['rel'] = array(
      '#type'   => 'textfield',
      '#title'  => t('Relationship'),
      '#default_value'  => isset($prop['rel']) ? trim($prop['rel']): '',
      '#description' => t('
        <ul>
          <li><strong>nofollow</strong> - for search engines not to follow the link.</li>
          <li><strong>lightbox</strong> - if you installed Lighbox 2, see your other options on Lightbox 2 README.txt.</li>
          <li>Other modules may have uses for this, so I just kept it as textfield.</li>
        </ul> 
      '),            
    );  
    $links[$link]['attributes']['target'] = array(
      '#type'   => 'select',
      '#title'  => t('Target Window'),
      '#options' => array(
        '_self'   => 'Self', 
        '_blank'  => 'Blank',
        '_parent' => 'Parent',         
        '_top'    => 'Top',                                          
      ),
      '#default_value'  => isset($prop['target']) ? $prop['target']: '_self',
    );       
    $links[$link]['attributes']['description'] = array(
      '#type'   => 'textarea',
      '#title'  => t('Description'),
      '#default_value'  => isset($prop['description']) ? $prop['description']: '',
      '#description' => t('Link additional information'),            
    );
    $links[$link]['attributes']['description_option'] = array(
      '#type'   => 'select',
      '#title'  => t('Options for description'),
      '#options' => array('hover' => 'Hover', 'subtitle' => 'Subtitle'),
      '#default_value'  => isset($prop['description_option']) ? $prop['description_option']: 'hover',
      '#description' => t('
        <ul>
          <li><strong>Hover</strong> - appears on mouse over the link.</li>
          <li><strong>Subtitle</strong> - appears as text under the link.</a>.</li>
        </ul> 
      '),
    );  
    $links[$link]['permission'] = array(
      '#type'   => 'fieldset',
      '#title'  => t('Permission'),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
    );         
    $links[$link]['permission']['roles'] = array(
      '#type'    => 'checkboxes',
      '#title'   => t('User Roles'),
      '#options' => $roles,
      '#default_value'  => isset($prop['roles']) ? $prop['roles']: array(),
      '#description' => t('If none is checked, this link will appear for all users.'),      
    );    
    //dsm($prop['content']);
    // these are the blocks
    if (isset($prop['content']) && is_array($prop['content'])) {
      foreach ($prop['content'] as $block => $block_prop) {
        foreach ($block_prop as $block_key => $block_val) {    
          $links[$link]['content'][$block][$block_key] = array(
            '#type'   => 'hidden',
            '#default_value'  => isset($block_val) ? trim($block_val): '',
          );
        }
      }
    }
  }
  return $links;
}
  
  
/**
 * OM Default New Links
 *
 */
function _om_new_link($count = 0, $roles = array()) {

  $num = $count + 5; // additional 5 new blank links
  
  $out = array();
  for ($i = $count; $i < $num; $i++) {
    // new links with default values
    $out[$i] = array(
      'link_title'         => 'New Link',
      'link_title_option'  => 'title',  
      'path_icon'          => '',                
      'icon_option'        => 0,
      'php_option'         => 0, 
      'path'               => '',      
      'path_query'         => '',      
      'path_fragment'      => '', 
      'weight'             => 0,  
      'id'                 => '',
      'class'              => '',
      'rel'                => '',
      'target'             => '_self',            
      'description'        => '',
      'description_option' => 'hover',      
      'roles'              => $roles,      
    );
  }
  return $out;
}


/**
 * 1 Submit for all settings
 *
 */
function om_maximenu_admin_submit($form, $form_state) {
  global $_om_maximenu_variable;
  $om_maximenu = $_om_maximenu_variable;

  $form_values = $form_state['values'];
    
  $om_maximenu_array = $form_state['values']['om_maximenus'];
  //dsm($om_maximenu_array); 
  
  foreach ($om_maximenu_array as $menu_id => $maximenu_content) {
    
    if (!isset($maximenu_content['delete'])) $maximenu_content['delete'] = 0; 
    // menu will not be included in the array if the title is not modified and delete field is checked
    if (!empty($maximenu_content['title']) && ($maximenu_content['title'] != 'New Menu') && ($maximenu_content['delete'] != 1)) {
      
      $om_maximenu[$menu_id] = array(
        'code'        => $maximenu_content['code'],
        'title'       => $maximenu_content['title'],
        'action'      => $maximenu_content['action'],        
        'skin'        => $maximenu_content['skin'],        
        'style'       => $maximenu_content['style'], 
        'output'      => $maximenu_content['output'],        
      ); 
      if ($maximenu_content['style'] == '') {
        if (isset($maximenu_content['style_options']['disabled']) && ($maximenu_content['style_options']['disabled'] == 1)) $om_maximenu[$menu_id]['disabled'] = 1;        
        if (isset($maximenu_content['style_options']['displace']) && ($maximenu_content['style_options']['displace'] == 1)) $om_maximenu[$menu_id]['displace'] = 1;        
      }
      if (($maximenu_content['style'] != 'roundabout') && ($maximenu_content['style'] != 'accordion')) {
        if (isset($maximenu_content['other_options']['active']) && ($maximenu_content['other_options']['active'] == 1)) $om_maximenu[$menu_id]['active'] = 1;
        if (isset($maximenu_content['other_options']['longmenu']) && !empty($maximenu_content['other_options']['longmenu'])) $om_maximenu[$menu_id]['longmenu'] = $maximenu_content['other_options']['longmenu'];
        if (isset($maximenu_content['other_options']['animated_bg']) && ($maximenu_content['other_options']['animated_bg'] == 1)) $om_maximenu[$menu_id]['animated_bg'] = 1;  
        if (isset($maximenu_content['other_options']['animated_link']) && ($maximenu_content['other_options']['animated_link'] == 1)) $om_maximenu[$menu_id]['animated_link'] = 1;  
        if (isset($maximenu_content['other_options']['jiggle']) && ($maximenu_content['other_options']['jiggle'] == 1)) $om_maximenu[$menu_id]['jiggle'] = 1;        
        if (isset($maximenu_content['other_options']['modal_content']) && ($maximenu_content['other_options']['modal_content'] == 1)) $om_maximenu[$menu_id]['modal_content'] = 1;        
      }      
      if (isset($maximenu_content['scroll']) && ($maximenu_content['scroll'] == 1)) $om_maximenu[$menu_id]['scroll'] = 1;        
                  
      $om_maximenu[$menu_id]['menu_visibility']['visibility'] = (isset($maximenu_content['menu_visibility']['visibility']) && !empty($maximenu_content['menu_visibility']['visibility'])) ? $maximenu_content['menu_visibility']['visibility']: 0;
      $om_maximenu[$menu_id]['menu_visibility']['pages'] = (isset($maximenu_content['menu_visibility']['pages']) && !empty($maximenu_content['menu_visibility']['pages'])) ? $maximenu_content['menu_visibility']['pages']: '';   
            
      if ($maximenu_content['output'] == 'block') {
        $om_maximenu[$menu_id]['block_options']['stacking']  = $maximenu_content['block_options']['stacking'];
        $om_maximenu[$menu_id]['block_options']['direction'] = $maximenu_content['block_options']['direction'];
        $om_maximenu[$menu_id]['block_options']['region']    = $maximenu_content['block_options']['region']; 
        $om_maximenu[$menu_id]['block_options']['weight']    = $maximenu_content['block_options']['weight'];                                      
        // because we are handling the visibility, etc., we have to handle everything to synch with blocks settings
        om_maximenu_update_blocks_db($menu_id, $maximenu_content);
      }
      elseif ($maximenu_content['output'] == 'float') {
        $om_maximenu[$menu_id]['float_options']['position']    = $maximenu_content['float_options']['position'];
        $om_maximenu[$menu_id]['float_options']['y_origin']    = $maximenu_content['float_options']['y_origin'];        
        $om_maximenu[$menu_id]['float_options']['y_value']     = $maximenu_content['float_options']['y_value'];        
        $om_maximenu[$menu_id]['float_options']['x_origin']    = $maximenu_content['float_options']['x_origin'];
        if ($maximenu_content['float_options']['x_origin'] != 'middle') {
          $om_maximenu[$menu_id]['float_options']['x_value']     = $maximenu_content['float_options']['non_middle_options']['x_value'];               
          $om_maximenu[$menu_id]['float_options']['stacking']    = $maximenu_content['float_options']['non_middle_options']['stacking'];  
          $om_maximenu[$menu_id]['float_options']['orientation'] = $maximenu_content['float_options']['non_middle_options']['orientation'];
        }                
        if (isset($maximenu_content['float_options']['dock']) && ($maximenu_content['float_options']['dock'] == 1)) $om_maximenu[$menu_id]['float_options']['dock'] = 1;          
        // menu as float must be deleted from block table
        om_maximenu_update_blocks_db($menu_id, $maximenu_content);
      }
      elseif ($maximenu_content['output'] == 'main_menu') {
        // menu as main_menu must be deleted from block table
        om_maximenu_update_blocks_db($menu_id, $maximenu_content);
      }
      
      foreach ($maximenu_content['links'] as $link => $link_prop) {
        
        if (!isset($link_prop['link_delete'])) $link_prop['link_delete'] = 0; 
        // links will not be included in the array if the title is not modified and delete field is checked
        if (!empty($link_prop['link_title']) && ($link_prop['link_title'] != 'New Link') && ($link_prop['link_delete'] != 1) && is_numeric($link)) {
          $om_maximenu[$menu_id]['links'][$link] = array(
            'link_title'         => $link_prop['link_title'],
            'link_title_option'  => $link_prop['title_options']['link_title_option'],
            'path_icon'          => $link_prop['title_options']['path_icon'],
            'icon_option'        => $link_prop['title_options']['icon_option'],  
            'php_option'         => $link_prop['title_options']['php_option'],
            'path'               => $link_prop['path'], 
            'path_query'         => $link_prop['path_query'], 
            'path_fragment'      => $link_prop['path_fragment'],             
            'weight'             => $link_prop['weight'],                                
            'id'                 => $link_prop['attributes']['id'],
            'class'              => $link_prop['attributes']['class'],
            'rel'                => $link_prop['attributes']['rel'],
            'target'             => $link_prop['attributes']['target'],                        
            'description'        => $link_prop['attributes']['description'],
            'description_option' => $link_prop['attributes']['description_option'],            
            'roles'              => $link_prop['permission']['roles'],            
            'content'            => isset($link_prop['content']) ? $link_prop['content']: '',

          );
        }
      }
      if ($form_values['op'] == t('Cancel')) drupal_goto('admin/structure/om-maximenu');       
      drupal_set_message(t($om_maximenu[$menu_id]['title'] . ' has been saved.'));          
    }
    elseif (isset($maximenu_content['delete']) && ($maximenu_content['delete'] == 1)) {
 
      if ($form_values['op'] == t('Cancel')) drupal_goto('admin/structure/om-maximenu'); 
      drupal_set_message(t($om_maximenu[$menu_id]['title'] . ' has been deleted.'));               
      unset($om_maximenu[$menu_id]); 
    }    
  }
  
  // Save all settings in 1 variable
  variable_set('om_maximenu', $om_maximenu);
  
  if ($_GET['q'] <> 'admin/structure/om-maximenu/maxedit') drupal_goto('admin/structure/om-maximenu');  
} 



/**
 * Implementation of theme_hook()
 * - View in table format
 *
 */
function theme_om_maximenu_links_order($form) {
  //dsm($form['form']);
  $form = $form['form'];
  $menu_key = $form['menu_key']['#value'];
  $output = '';
  //dsm($form['layout']);
  $rows = array();
  foreach ($form as $key => $prop) {
    if (is_numeric($key)) {
      $row = array();
      // Strips labels
      unset($prop['weight']['#title']);
      unset($prop['weight']['#description']);      
      unset($prop['link_delete']['#title']); 
      
      unset($form[$key]['#description']);      
      unset($form[$key]['weight']);      
      unset($form[$key]['link_delete']);                             
      $row[] = array('class' => 'col-link', 'data' => drupal_render($form[$key]));
      $row[] = array('class' => 'col-attached', 'data' => $prop['#description']);
      $row[] = array('class' => 'col-weight', 'data' => drupal_render($prop['weight']));
      $row[] = array('class' => 'col-delete', 'data' => drupal_render($prop['link_delete']));
            
      $rows[] = array(
        'data' => $row,
        'id' => 'om-row-'. $menu_key . '-' . $key,
        'class' => array('draggable row-'. $key, 'tabledrag-leaf'),
      ); 
    }
  }
  $header = array(t('Link'), t('Attached Blocks'), t('Weight'), t('Delete')); //, 'Update'

  // Header
  $form['form']['#children'] = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' =>  array('id' => 'om-maximenu-links-' . $menu_key)));  

  //drupal_add_tabledrag($table_id, $action, $relationship, $group, $subgroup = NULL, $source = NULL, $hidden = TRUE, $limit = 0);
  drupal_add_tabledrag('om-maximenu-links-' . $menu_key, 'order', 'group', 'om-weight');
  
  $output .= drupal_render_children($form);
  
  return $output;
}


