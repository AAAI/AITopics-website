<?php

/**
 * @file
 * Default theme implementation for displaying a single search result.
 *
 * This template renders a single search result and is collected into
 * search-results.tpl.php. This and the parent template are
 * dependent to one another sharing the markup for definition lists.
 *
 * Available variables:
 * - $url: URL of the result.
 * - $title: Title of the result.
 * - $snippet: A small preview of the result. Does not apply to user searches.
 * - $info: String of all the meta information ready for print. Does not apply
 *   to user searches.
 * - $info_split: Contains same data as $info, split into a keyed array.
 * - $module: The machine-readable name of the module (tab) being searched, such
 *   as "node" or "user".
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * Default keys within $info_split:
 * - $info_split['type']: Node type (or item type string supplied by module).
 * - $info_split['user']: Author of the node linked to users profile. Depends
 *   on permission.
 * - $info_split['date']: Last update of the node. Short formatted.
 * - $info_split['comment']: Number of comments output as "% comments", %
 *   being the count. Depends on comment.module.
 *
 * Other variables:
 * - $classes_array: Array of HTML class attribute values. It is flattened
 *   into a string within the variable $classes.
 * - $title_attributes_array: Array of HTML attributes for the title. It is
 *   flattened into a string within the variable $title_attributes.
 * - $content_attributes_array: Array of HTML attributes for the content. It is
 *   flattened into a string within the variable $content_attributes.
 *
 * Since $info_split is keyed, a direct print of the item is possible.
 * This array does not apply to user searches so it is recommended to check
 * for its existence before printing. The default keys of 'type', 'user' and
 * 'date' always exist for node searches. Modules may provide other data.
 * @code
 *   <?php if (isset($info_split['comment'])): ?>
 *     <span class="info-comment">
 *       <?php print $info_split['comment']; ?>
 *     </span>
 *   <?php endif; ?>
 * @endcode
 *
 * To check for all available data within $info_split, use the code below.
 * @code
 *   <?php print '<pre>'. check_plain(print_r($info_split, 1)) .'</pre>'; ?>
 * @endcode
 *
 * @see template_preprocess()
 * @see template_preprocess_search_result()
 * @see template_process()
 */
?>

<?php
$node = node_load($result['node']->entity_id);
$item_term = taxonomy_term_load($node->field_item_type['und'][0]['tid']);
$item_type = $item_term->name;
?>

<li class="<?php print $classes; ?>"<?php print $attributes; ?>>

  <div class="nodetype nodetype-<?php echo strtolower($item_type); ?>">

    <div class="nodetype-name"><?php if($item_type == 'news') { echo "AI in the News"; } else { echo $item_type; } ?></div>

    <?php print render($title_prefix); ?>
    <h2 class="title"<?php print $title_attributes; ?>><?php if(!empty($node->field_original_link)) { display_link($node->field_original_link['und'][0], $title); } else { echo "<a href=\"$url\">$title</a>"; } ?></h2>
    <?php print render($title_suffix); ?>

    <div class="summary search-snippet-info" <?php if(!empty($node->field_representative_image)) { print "style=\"min-height: ".$node->field_representative_image['und'][0]['height']."px;\""; } ?>>
      <?php if(!empty($node->field_representative_image)) {
              $image = field_get_items('node', $node, 'field_representative_image');
              print str_replace("/>", "align=\"left\"/>", render(field_view_value('node', $node, 'field_representative_image', $image[0])));
            }
      ?>
      <?php if ($snippet): ?>
      <p class="search-snippet"<?php print $content_attributes; ?>><?php print $snippet; ?></p>
      <?php endif; ?>
      <?php //print '<pre>'. check_plain(print_r($info_split, 1)) .'</pre>'; ?>
    </div>

    <div class="metadata">
      <?php if($item_type == 'News'): ?>
        <?php $pubdate = field_get_items('node', $node, 'field_publication_date');
              print render(field_view_value('node', $node, 'field_publication_date', $pubdate[0])); ?>
        <br/>
        <?php print $node->field_source['und'][0]['value']; ?>
      <?php endif; ?>
      <?php if($item_type == 'Publication' && !empty($node->field_authors)): ?>
        By <?php echo $node->field_authors['und'][0]['value']; ?>
        <br/>
      <?php endif; ?>
      <?php if($item_type == 'Publication' && !empty($node->field_source)): ?>
        <?php echo $node->field_source['und'][0]['value']; ?>
        <br/>
      <?php endif; ?>
      <?php if(!empty($node->field_link_category)): ?>
        <?php $term = taxonomy_term_load($node->field_link_category['und'][0]['tid']); echo $term->name; ?>
        <br/>
      <?php endif; ?>
      <br/>
      <?php if(($item_type == 'Podcast' || $item_type == 'Video') && !empty($node->field_minutes)): ?>
        <?php echo $node->field_minutes['und'][0]['value']; ?> min
        <br/>
      <?php endif; ?>
      <?php if(!empty($node->field_publication_year_int)): ?>
        <?php echo $node->field_publication_year_int['und'][0]['value']; ?>
        <br/>
      <?php endif; ?>
      <?php if(!empty($node->field_collections)): ?>
        <?php $term = taxonomy_term_load($node->field_collections['und'][0]['tid']); echo $term->name; ?>
        <br/>
      <?php endif; ?>
      <a href="<?php print $url; ?>"><strong>Read more...</strong></a>
    </div>

    <div class="topics">
      <?php render_topics_subtopics($node); ?>
    </div>

    <div class="clearfix"></div>
  </div>
</li>
