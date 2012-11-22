<?php
/**
 * @file om_maximenu_modal_content.tpl.php
 * Default theme implementation of om maximenu contents with modal blocks
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
 * @see template_preprocess_om_maximenu_modal()
 * @see template_preprocess_om_maximenu_modal_links()
 * @see template_preprocess_om_maximenu_modal_content()
 *
 */
?> 

<?php foreach ($links['links'] as $key => $content): ?>
  <?php $permission = om_maximenu_link_visible($content['roles']); ?>
    <?php if (!empty($permission) && !empty($content['content'])): ?>
      <div id="om-modal-content-<?php print $links['code'] . '-' . $key; ?>" class="om-maximenu-content om-maximenu-modal-content<?php print om_maximenu_classes($links); ?>"<?php print om_maximenu_inline_style($links); ?>>     
      <div class="om-maximenu-close">Close</div>  
      <div class="om-maximenu-top">
        <div class="om-maximenu-top-left"></div>
        <div class="om-maximenu-top-right"></div>
      </div><!-- /.om-maximenu-top --> 
      <div class="om-maximenu-middle">
        <div class="om-maximenu-middle-left">
          <div class="om-maximenu-middle-right">
            <?php print om_maximenu_content_render($content['content']); ?>
            <div class="om-clearfix"></div>
          </div><!-- /.om-maximenu-middle-right --> 
        </div><!-- /.om-maximenu-middle-left --> 
      </div><!-- /.om-maximenu-middle --> 
      <div class="om-maximenu-bottom">
        <div class="om-maximenu-bottom-left"></div>
        <div class="om-maximenu-bottom-right"></div>
      </div><!-- /.om-maximenu-bottom -->  
    </div><!-- /.om-modal-content -->
  <?php endif; ?> 
<?php endforeach; ?>

 
