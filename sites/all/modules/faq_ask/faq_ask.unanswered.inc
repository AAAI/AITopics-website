<?php
/**
* @file
* Theming preparation unanswered list functions for the FAQ-Ask module.
*/

/**
 * Create list of unanswered questions for display in block
 *
 * @param &$variables
 *   Array reference of arguments given to the theme() function.
 */
function template_preprocess_faq_ask_unanswered_block(&$variables) {

  $data = $variables['data'];
  $mode = $variables['mode'];
  $more_link = $variables['more_link'];

  $items = array();
  foreach ($data as $nid) {
    $items[] = _faq_ask_answerlink($nid, $mode);
  }

  $variables['items'] = $items;

  if ($more_link)
    $variables['links'] = l(t('more...'), 'faq_ask/unanswered', array('attributes' => array('class' => array('faq_ask_more_link'))));

  $variables['message'] = '<p class="faq_ask_expert_advice">' . filter_xss_admin(variable_get('faq_ask_asker_advice', _faq_ask_advice_default('asker'))) . '</p>';

}
/**
* Helper function to create a link to unanswered nodes using the toke verificationb
*
* @param unknown_type $n    Node, either node object or node id
* @param unknown_type $mode  'answer' or 'edit'
*
* @return  string  Link to the node with a token
*/
function _faq_ask_answerlink($n, $mode) {
  if (!is_object($n))
  $n = node_load($n);
  $nid = $n->nid;

  // Create token to enable instant answering of unanswered questions
  $token = _faq_ask_get_token('faq_ask/answer/' . $nid);
  $options = array( 'query' => array( 'token' => array( $token ) ));

  // Allow for edit mode in link to the unanswered questions if in edit mode
  if ($mode == 'edit') {
    return l($n->title, "faq_ask/edit/" . $nid, array( 'query' => array( 'ask' => TRUE ) ));
  }
  elseif ($mode == 'answer') {

    return l($n->title, "faq_ask/answer/" . $nid, array( 'query' => array( 'token' => $token )));
  }
}

/**
* Create a categorized list of nodes that are not answered.
*
* @param &$variables
*   Array reference of arguments given to the theme() function.
*/
function template_preprocess_faq_ask_unanswered(&$variables) {

  $data = $variables['data'];
  $tid = $variables['term'];    // Fetch the term from term id
  $term = taxonomy_term_load($tid);
  $class = $variables['class'];
  $mode = $variables['mode'];

  // Get number of questions, and account for hidden sub-categories.
  $count = count($data);

  // Get taxonomy image.
  $variables['term_image'] = '';
  if (module_exists('taxonomy_image')) {
    $variables['term_image'] = taxonomy_image_display($term->tid, array('class' => 'faq-tax-image'));
  }

  // Configure header.
  if (is_object($term)) {

    $variables['category_depth']   = (!empty($term->depth)?$term->depth:1);
    $variables['category_name']   = check_plain(faq_tt("taxonomy:term:$term->tid:name", $term->name));
    $variables['header_title']     = check_plain(faq_tt("taxonomy:term:$term->tid:name", $term->name));

    // Configure category description.
    $variables['description'] = check_markup(faq_tt("taxonomy:term:$term->tid:description", $term->description));
  }
  else {
    $variables['category_depth']   = 1;
    $variables['category_name']   = t('Uncategorized');
    $variables['header_title']     = t('Uncategorized');

    // Configure category description.
    $variables['description'] = t('Nodes that are not assigned to any category yet. This must be done when answering the question.');
  }

  // Configure class (faq-qa or faq-qa-hide).
  $variables['container_class'] = 'faq-qa';

  if (!count($data)) {
    $variables['question_count'] = $count;
    $variables['nodes'] = array();
    return;
  }

  $nodes = array();
  foreach ($data as $nid) {
    $node = node_load($nid);
    $node_var = array();
    $anchor = 't' . $tid . 'n' . $node->nid;
    faq_view_question($node_var, $node, '1', $anchor);
    node_build_content($node, 'teaser', $GLOBALS['language_content']->language);

    // Add "edit answer" link if they have the correct permissions.
    if (node_access('update', $node)) {
      $node->content['links']['node']['#links']['faq_edit_link'] = array(
        'title' => t('Edit answer'),
        'href' => "node/$node->nid/edit",
        'query' => drupal_get_destination(),
        'attributes' => array('title' => t('Edit answer')),
        );
    }

    // Add "answer question" link if they have the correct permissions.
    if (user_access('answer question')) {
      $token = _faq_ask_get_token('faq_ask/answer/' . $nid);
      $node->content['links']['node']['#links']['faq_ask_answer_link'] = array(
        'title' => t('Answer question'),
        'href' => 'faq_ask/' . $mode . '/' . $node->nid,
        'query' => array( 'token' => $token ),
        'attributes' => array( 'title' => t('Answer question')),
      );
    }

    $build = $node->content;
    // We don't need duplicate rendering info in node->content.
    unset($node->content);

      $build += array(
        '#theme' => 'node',
        '#node' => $node,
        '#view_mode' => 'teaser',
        '#language' => $GLOBALS['language_content']->language,
      );

    // Add contextual links for this node.
    $build['#contextual_links']['node'] = array('node', array($node->nid));

    // Allow modules to modify the structured node.
    $type = 'node';
    drupal_alter(array('node_view', 'entity_view'), $build, $type);

    $node_links = $build['links']['node']['#links'];
    unset($build['links']);
    unset($build['#theme']); // We don't want node title displayed.

    $node_var['body'] = drupal_render($build);
    $node_var['links'] = theme('links', array('links' => $node_links, 'attributes' => array('class' => 'links inline')));
    $nodes[] = $node_var;
  }
  $variables['nodes'] = $nodes;
  $variables['question_count'] = $count;
}

