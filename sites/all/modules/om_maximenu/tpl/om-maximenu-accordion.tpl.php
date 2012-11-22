<?php
/**
 * @file om_maximenu_accordion.tpl.php
 * Default theme implementation of om maximenu with accordion blocks
 *
 * Available variables:
 * - $maximenu_name: Menu name given on configuration
 * - $links: All menu items which also contents each link property
 *
 * Helper variables:
 * - $zebra: Same output as $block_zebra but independent of any block region.
 * - $id: Same output as $block_id but independent of any block region.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 * - $user: (object) user properties
 *
 * @see template_preprocess_om_maximenu_accordion()
 * @see template_preprocess_om_maximenu_accordion_links()
 * @see template_preprocess_om_maximenu_accordion_content()
 *
 */
?>  

<dl id="om-menu-<?php print $maximenu_name; ?>" class="easy-accordion">
  <?php foreach ($links['links'] as $key => $content): ?>
    <?php print theme('om_maximenu_accordion_links', array('content' => $content, 'maximenu_name' => $maximenu_name, 'key' => $key)); ?>          
  <?php endforeach; ?>
</dl><!-- /.easy-accordion -->    



