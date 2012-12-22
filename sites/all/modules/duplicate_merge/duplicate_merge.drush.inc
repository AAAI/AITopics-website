<?php

function duplicate_merge_drush_command() {
  $items = array();

  $items['dup-merge'] = array(
    'description' => 'Duplicate merge',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    );
  return $items;
}

function drush_duplicate_merge_dup_merge() {
  $result = db_query("SELECT n.nid FROM {node} n WHERE n.type = :type",
            array(':type' => 'item'));

  $unique_items = array();

  $item_no = 0;
  print $result->rowCount()." items...\n";
  foreach($result as $record) {
    if($item_no % 100 == 0) {
      print "Item $item_no\n";
    }

    $node = node_load($record->nid);
    $title = $node->title;
    $link = "";
    if(array_key_exists('und', $node->field_original_link)) {
      $link = $node->field_original_link['und'][0]['url'];
    }
    $key = "$title-----$link";

    if(array_key_exists($key, $unique_items)) {
      array_push($unique_items[$key], $node);
    } else {
      $unique_items[$key] = array($node);
    }
    $item_no++;
  }

  foreach($unique_items as $key => $matched_items) {
    if(count($matched_items) > 1) {
      print (count($matched_items)-1)." duplicates: ".$matched_items[0]->title."\n";
      $cononical = $matched_items[0];
      $cononical_path = drupal_lookup_path('alias',"node/".$cononical->nid);
      foreach($matched_items as $item) {
        $path = drupal_lookup_path('alias',"node/".$item->nid);
        if(0 === strpos($cononical_path, $path) || $item->sticky) {
          $cononical = $item;
          $cononical_path = $path;
        }
      }
      print "\tCononical is ".$cononical->nid." $cononical_path\n";

      $topics = array();
      $cononical_topics = array();
      if(array_key_exists('und', $cononical->field_topics)) {
        $topics = $cononical->field_topics['und'];
        $cononical_topics = $topics;
      }

      foreach($matched_items as $item) {
        if($item->nid == $cononical->nid) { continue; }
        if(array_key_exists('und', $item->field_topics)) {
          array_merge($topics, $item->field_topics['und']);
        }
      }
      if($topics != $cononical_topics) {
        print_r($cononical_topics);
        print_r($topics);
      } else {
        // same topics, same primary link; delete all but the cononical
        $to_delete = array();
        foreach($matched_items as $item) {
          if($item->nid == $cononical->nid) { continue; }
          array_push($to_delete, $item->nid);
        }
        print "\tDeleting: ".implode(" ", $to_delete)."\n";
        node_delete_multiple($to_delete);
      }
    }
  }
}
