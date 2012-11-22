<?php
/**
 * @file om_maximenu_wrapper.tpl.php
 * Default theme implementation of om maximenu wrapper
 *
 * Available variables:
 * - $maximenu_name: menu title
 * - $links: array, link properties
 * - $link_code: menu unique identifier
 * - $content: rendered content
 *
 * @see template_preprocess_om_maximenu_wrapper()
 *
 */
?> 

<?php if (!empty($content)): ?>
  <div id="om-maximenu-<?php print $maximenu_name; ?>" class="om-maximenu<?php print om_maximenu_classes($links); ?> code-<?php print $link_code; ?>"<?php print om_maximenu_inline_style($links); ?>>     
    <?php if($links['output'] == 'float'): ?>
      <div id="om-maximenu-<?php print $maximenu_name; ?>-inner" class="om-maximenu-wrapper-inner">
        <?php print $content; ?>
      </div><!-- /.om-maximenu-wrapper-inner -->
    <?php else: ?>
      <?php print $content; ?>
    <?php endif; ?>
  </div><!-- /#om-maximenu-[menu name] -->   
<?php endif; ?>


