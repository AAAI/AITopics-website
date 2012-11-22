<?php
/**
 * @file om_maximenu_roundabout_links.tpl.php
 * Default theme implementation of om maximenu links with roundabout blocks
 *
 * Available variables:
 * - $maximenu_name: Menu name given on configuration 
 * - $om_link: rendered span or a tag with attributes
 * - $content: array, used for link classes and content
 *
 * Helper variables:
 * - $zebra: Same output as $block_zebra but independent of any block region.
 * - $id: Same output as $block_id but independent of any block region.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 * - $user: (object) user properties
 * - $count: link counter
 * - $total: number of links
 * - $permission: TRUE/FALSE
 * - $key: numeric link order id
 * - $code: unique id given in the system
 *
 * @see template_preprocess_om_maximenu_roundabout()
 * @see template_preprocess_om_maximenu_roundabout_links()
 * @see template_preprocess_om_maximenu_roundabout_content()
 *
 */
?>  

<?php if (!empty($permission)): ?>   
  <li id="om-leaf-<?php print $code . '-' . $key; ?>" class="<?php print om_maximenu_link_classes($content, $permission, $count, $total); ?>">   
    <?php print $om_link; ?>
    <?php print theme('om_maximenu_roundabout_content', array('content' => $content['content'], 'maximenu_name' => $maximenu_name, 'key' => $key)); ?>      
  </li>
<?php endif; ?>  
    
  

  
