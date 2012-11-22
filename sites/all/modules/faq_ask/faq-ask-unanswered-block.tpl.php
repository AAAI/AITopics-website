<?php

/**
 * @file
 * Template file for the FAQ page if set to show/hide categorized answers when
 * the question is clicked.
 */

/**
 * Available variables:
 *
 * $header_title
 *   The category title.
 * $description
 *   The current page's description.
 * $term_image
 *   The HTML for the category image. This is empty if the taxonomy image module
 *   is not enabled or there is no image associated with the term.
 * $display_faq_count
 *   Boolean value controlling whether or not the number of faqs in a category
 *   should be displayed.
 * $nodes
 *   An array of nodes to be displayed.
 *   Each node stored in the $nodes array has the following information:
 *     $node['question'] is the question text.
 *
 * $links
 * 
 * $message
 *   
 */

?>
  <!-- list questions (in title link) and answers (in body) -->
  <?php if (count($items)): ?>
   <ul class="faq-ask-unanswered-questions">
  <?php foreach ($items as $i => $item): ?>
      <li class="faq-ask-question">
      <?php print $item; ?>
      </li> <!-- Close div: faq-question-answer -->
    <?php endforeach; ?>
    </ul> <!-- Close div: faq-question-answer -->
    <?php if (isset($links)): ?>
      <?php print $links; ?>
    <?php endif; ?>
    <?php if (isset($message)): ?>
      <?php print $message; ?>
    <?php endif; ?>
  <?php endif; ?>
