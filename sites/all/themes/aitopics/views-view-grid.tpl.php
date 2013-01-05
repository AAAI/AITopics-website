<?php
/**
 * @file views-view-grid.tpl.php
 * Default simple view template to display a rows in a grid.
 *
 * - $rows contains a nested array of rows. Each row contains an array of
 *   columns.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)) : ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>

<div class="grid-items">
<?php foreach ($rows as $columns): ?>
  <?php foreach ($columns as $counter => $item): ?>
    <?php if(count($rows) > 1): ?>
      <div class="grid-item grid-item-<?php if($counter % 2 == 0) { echo "left"; } else { echo "right"; } ?>">
    <?php else: ?>
      <div class="grid-item">
    <?php endif; ?>
    <?php print $item; ?>
    </div>
  <?php endforeach; ?>
<?php endforeach; ?>
</div>
<div style="clear:both"></div>