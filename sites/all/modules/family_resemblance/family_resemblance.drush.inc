<?php

function family_resemblance_drush_command() {
  $items = array();

  $items['fr-export'] = array(
    'description' => 'Export items for training',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    );
  return $items;
}

function drush_family_resemblance_fr_export() {
  $topic_vid = taxonomy_vocabulary_machine_name_load('topics')->vid;
  $topic_tree = taxonomy_get_tree($topic_vid);
  $topics = array();
  foreach($topic_tree as $t) {
    $topics[$t->tid] = array('name' => $t->name, 'parent' => $t->parents[0]);
  }

  $arffs = array();
  foreach(array_keys($topics) as $tid) {
    $f = drupal_realpath(drupal_tempnam('temporary://', 'fr_'.$tid.'_'));
    $fh = fopen($f, 'w');
    $arffs[$tid] = array('filename' => $f, 'handle' => $fh);

    fwrite($fh, "@relation $tid\n");
    fwrite($fh, "@attribute title string\n");
    fwrite($fh, "@attribute body string\n");
    fwrite($fh, "@attribute class {yes,no}\n");
    fwrite($fh, "@data\n");
  }

  $nodes = array();

  $result = db_query("SELECT n.nid FROM {node} n WHERE n.type = :type AND n.status = 1",
            array(':type' => 'item'));

  $item_no = 0;
  print $result->rowCount()." items...\n";
  foreach($result as $record) {
    if($item_no % 100 == 0) {
      print "Item ".$item_no."\n";
    }
    $item_no++;

    $node = node_load($record->nid);
    $node_topics = array();
    if(array_key_exists('und', $node->field_topics)) {
      for($i = 0; $i < count($node->field_topics['und']); $i++) {
        $node_topics []= $node->field_topics['und'][$i]['tid'];
      }
    }
    if(empty($node_topics)) { continue; }

    /* $source = ''; */
    /* if(array_key_exists('und', $node->field_source)) { */
    /*   $source = $node->field_source['und'][0]['value']; */
    /* } */

    /* $source = preg_replace('/\W/', ' ', $source); */
    /* $source = preg_replace('/\s+/', ' ', $source); */

    $title = preg_replace('/\W/', ' ', $node->title);
    $title = preg_replace('/\s+/', ' ', $title);
    $body = preg_replace('/\W/', ' ', strip_tags($node->body['und'][0][value]));
    $body = preg_replace('/\s+/', ' ', $body);
    foreach(array_keys($topics) as $tid) {
      $label = 'no';
      if(in_array($tid, $node_topics)) {
        $label = 'yes';
      }
      fwrite($arffs[$tid]['handle'], "'$title','$body',$label\n");
    }
  }
  foreach($arffs as $tid => $arff) {
    fclose($arff['handle']);

    print $arff['filename']."\t$tid\t".$topics[$tid]['name']."\n";
  }

  $f = drupal_realpath(drupal_tempnam('temporary://', 'topics_'));
  $fh = fopen($f, 'w');
  foreach($arffs as $tid => $arff) {
      $t = taxonomy_term_load($tid);
      $uri = entity_uri('taxonomy_term', $t);
      fwrite($fh, "$tid,'".$topics[$tid]['name']."','".$uri['path']."'\n");
  }
  fclose($fh);
}
