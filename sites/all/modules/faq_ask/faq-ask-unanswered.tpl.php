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
 *     $node['body'] is the answer text.
 *     $node['links'] represents the node links, e.g. "Read more".
 * $use_teaser
 *   Whether $node['body'] contains the full body or just the teaser text.
 * $container_class
 *   The class attribute of the element containing the sub-categories, either
 *   'faq-qa' or 'faq-qa-hide'. This is used by javascript to open/hide
 *   a category's faqs.
 */

$hdr = 'h2';

?><div class="faq-category-group">
  <!-- category header with title, link, image, description, and count of
  questions inside -->
  <div class="faq-qa-header">
   <<?php print $hdr; ?> class="faq-header">
   <?php print $term_image; ?>
   <?php print $header_title; ?>
   (<?php print $question_count; ?>)
   </<?php print $hdr; ?>>

  <?php if (!empty($description)): ?>
    <div class="faq-qa-description"><?php print $description ?></div>
  <?php endif; ?>
  <?php if (!empty($term_image)): ?>
    <div class="clear-block"></div>
  <?php endif; ?>
  </div> <!-- Close div: faq-qa-header -->

  <!-- list questions (in title link) and answers (in body) -->
  <div class="faq-dl-hide-answer">
  <?php if (count($nodes)): ?>
    <?php foreach ($nodes as $i => $node): ?>
      <div class="faq-question-answer">
      <div class="faq-question faq-dt-hide-answer">
      <?php print $node['question']; ?>
      <?php if (isset($node['links'])): ?>
        <?php print $node['links']; ?>
	  <?php endif; ?>
      </div> <!-- Close div: faq-question faq-dt-hide-answer -->
      </div> <!-- Close div: faq-question-answer -->
    <?php endforeach; ?>
  <?php endif; ?>
  </div> <!-- Close div: faq-dl-hide-answer -->

</div> <!-- Close div: faq-category-group -->
