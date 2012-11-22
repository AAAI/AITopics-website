<?php
// $Id$

/**
 * @file
 * OM Maximenu Help
 *
 * @author: Daniel Honrade http://drupal.org/user/351112
 * 
 *
 */

/**
 * Implementation of hook_help().
 *
 */
function om_maximenu_help($path, $arg) {
  switch ($path) {
    // Main module help for the block module
    case 'admin/help#om_maximenu':
      return '
        <div class="om-maximenu-help">' . t('
         <ul>
           <li><strong>' . l(t('Basic Editing'), 'admin/structure/om-maximenu/basic-editing') . '</strong> - A summary list of all menus created by OM Maximenu. You can add/edit/delete individual menus here.</li>
           <li><strong>' . l(t('Maxedit'), 'admin/structure/om-maximenu/maxedit') . '</strong> - This is a powerful editor which you can add/edit/delete all menus at the same time. As your menus grow in number, this may need a lot of memory and cpu resources since it will load all forms required for editing all menus.</li>
           <li><strong>' . l(t('Advanced Editing'), 'admin/structure/om-maximenu/advanced-editing') . '</strong> - Like Maxedit, this is also a powerful editor which you can add/edit/delete all menus at the same time. But since it is not using forms for editing every menu and link, it won\'t need a lot of memory and cpu resources.</li> 
           <li><strong>' . l(t('Import'), 'admin/structure/om-maximenu/import') . '</strong> - You can import all available menus in the system, including vocabulary terms as menu items.</li>                                
         </ul>                
        ') . '</div>';

    case 'admin/structure/om-maximenu':
      return '<p>' . t('
        A summary list of all menus created by OM Maximenu. You can add/edit/delete individual menus here.
       ') . '</p>';

    case 'admin/structure/om-maximenu/basic-editing':
      return '<p>' . t('
        A summary list of all menus created by OM Maximenu. You can add/edit/delete individual menus here.
       ') . '</p>';

    case 'admin/structure/om-maximenu/maxedit':
      return '<p>' . t('
        This is a powerful editor which you can add/edit/delete all menus at the same time. As your menus grow in number, this may need a lot of memory and cpu resources since it will load all forms required for editing all menus.
       ') . '</p>';

    case 'admin/structure/om-maximenu/advanced-editing':
      return '<p>' . t('
        Like Maxedit, this is also a powerful editor which you can add/edit/delete all menus at the same time. But since it is not using forms for editing every menu and link, it won\'t need a lot of memory and cpu resources. 
       ') . '</p>';
    case 'admin/structure/om-maximenu/import':
      return '<p>' . t('
        You can import all available menus in the system, including vocabulary terms as menu items.                               
       ') . '</p>';
  }
}



