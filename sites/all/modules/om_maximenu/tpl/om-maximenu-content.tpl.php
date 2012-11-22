<?php
/**
 * @file om_maximenu_content.tpl.php
 * Default theme implementation of om maximenu contents
 *
 * Available variables:
 * - $module: module name
 * - $delta: module delta
 * - $om_classes: from om tools if installed
 * - $title: block title
 * - $block_title: modified block title
 *
 * @see template_preprocess_om_maximenu_content()
 * 
 */
?> 

<div class="block block-<?php print $module; ?> block-<?php print $module . '-id-' . $delta . $om_classes; ?>">           
  <?php print ($title != '<none>') ? '<h3 class="title">' . $block_title . '</h3>': ''; ?>
  <?php $content = isset($block['content']) ? is_array($block['content']) ? drupal_render($block['content']): $block['content']: '' . ''; ?>
  <div class="content"><?php print $content; ?></div>
  <?php if (user_access('administer blocks')) print om_maximenu_block_edit($module, $delta, $title); ?>
</div><!-- /.block -->





