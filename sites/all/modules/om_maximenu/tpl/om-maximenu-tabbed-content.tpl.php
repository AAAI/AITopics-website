<?php
/**
 * @file om_maximenu_tabbed_content.tpl.php
 * Default theme implementation of om maximenu contents with tabbed blocks
 *
 * Available variables:
 * - $links: link array
 *
 * Helper variables:
 * - $zebra: Same output as $block_zebra but independent of any block region.
 * - $id: Same output as $block_id but independent of any block region.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 * - $user: (object) user properties
 *
 * @see template_preprocess_om_maximenu_tabbed()
 * @see template_preprocess_om_maximenu_tabbed_links()
 * @see template_preprocess_om_maximenu_tabbed_content()
 *
 */
?> 

<div class="om-maximenu-tabbed-content">
  <div class="om-maximenu-tabbed-content-inner">
    <?php foreach ($links['links'] as $key => $content): ?>
      <?php $permission = om_maximenu_link_visible($content['roles']); ?>
      <?php if (!empty($permission) && !empty($content['content'])): ?>
        <div id="om-tabbed-content-<?php print $links['code'] . '-' . $key; ?>" class="om-tabbed-content om-tabbed-content-hide">
          <div class="om-tabbed-content-inner">
            <?php print om_maximenu_content_render($content['content']); ?>
            <div class="om-clearfix"></div>
          </div><!-- /.om-tabbed-content-inner -->
        </div><!-- /.om-tabbed-content -->
      <?php endif; ?> 
    <?php endforeach; ?>
    <div class="om-clearfix"></div>
  </div><!-- /.om-maximenu-tabbed-content-inner -->
</div><!-- /.om-maximenu-tabbed-content -->
      

