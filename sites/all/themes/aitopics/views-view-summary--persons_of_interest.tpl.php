<?php

/**
 * @file
 * Default simple view template to display a list of summary lines.
 *
 * @ingroup views_templates
 */

$per_column = ceil(count($rows) / 3);

function item_column($rows) {
  global $options;
  print '<div class="item-list-column">';
  print '<ul class="views-summary persons-of-interest">';
  foreach($rows as $row) {
    print '<li><a href="'.$row->url.'">'.$row->link.'</a> ('.$row->count.')</li>';
  }
  print '</ul>';
  print '</div>';
}

$rows_chunked = array_chunk($rows, $per_column);
foreach($rows_chunked as $chunk) {
  item_column($chunk);
}

?>
