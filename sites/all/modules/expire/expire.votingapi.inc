<?php

/**
 * Implementation of hook_votingapi_insert().
 *
 * @param $votes
 *  array of votes
 */
function expire_votingapi_insert($votes) {
  _expire_votingapi($votes);
}

/**
 * Implementation of hook_votingapi_delete().
 *
 * @param $votes
 *  array of votes
 */
function expire_votingapi_delete($votes) {
  _expire_votingapi($votes);
}

/**
 * Common expiry logic for votingapi.
 */
function _expire_votingapi($votes) {
  foreach ($votes as $vote) {
    if ($vote['entity_type'] == 'comment') {
      $nid = db_query("SELECT nid FROM {comments} WHERE cid = :cid", array(':cid' => $vote['entity_id']))->fetchField();
      if (is_numeric($nid)) {
        $node = node_load($nid);
        if ($node) {
          expire_node($node);
        }
      }
    }
    if ($vote['entity_type'] == 'node') {
      $node = node_load($vote['entity_id']);
      if ($node) {
        expire_node($node);
      }
    }
  }
}


