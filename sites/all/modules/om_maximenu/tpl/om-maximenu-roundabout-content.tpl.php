<?php
/**
 * @file om_maximenu_roundabout_content.tpl.php
 * Default theme implementation of om maximenu contents with roundabout blocks
 *
 * Available variables:
 * - $content: array, used for link classes and content
 *
 * Helper variables:
 * - $zebra: Same output as $block_zebra but independent of any block region.
 * - $id: Same output as $block_id but independent of any block region.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 * - $user: (object) user properties
 *
 * @see template_preprocess_om_maximenu_roundabout()
 * @see template_preprocess_om_maximenu_roundabout_links()
 * @see template_preprocess_om_maximenu_roundabout_content()
 *
 */
?> 

<?php if (!empty($content)): ?>
  <div class="om-maximenu-roundabout-content">
    <?php print om_maximenu_content_render($content); ?>
    <div class="om-clearfix"></div>  
    <div class="om-maximenu-arrow"></div>
  </div><!-- /.om-maximenu-roundabout-content -->
<?php endif; ?>


