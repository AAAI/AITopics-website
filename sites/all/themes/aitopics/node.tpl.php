<?php

/**
 * @file
 * Default theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: An array of node items. Use render($content) to print them all,
 *   or print a subset such as render($content['field_example']). Use
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $user_picture: The node author's picture from user-picture.tpl.php.
 * - $date: Formatted creation date. Preprocess functions can reformat it by
 *   calling format_date() with the desired parameters on the $created variable.
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $display_submitted: Whether submission information should be displayed.
 * - $submitted: Submission information created from $name and $date during
 *   template_preprocess_node().
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - node: The current template type, i.e., "theming hook".
 *   - node-[type]: The current node type. For example, if the node is a
 *     "Blog entry" it would result in "node-blog". Note that the machine
 *     name will often be in a short form of the human readable label.
 *   - node-teaser: Nodes in teaser form.
 *   - node-preview: Nodes in preview mode.
 *   The following are controlled through the node publishing options.
 *   - node-promoted: Nodes promoted to the front page.
 *   - node-sticky: Nodes ordered above other non-sticky nodes in teaser
 *     listings.
 *   - node-unpublished: Unpublished nodes visible only to administrators.
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $view_mode: View mode, e.g. 'full', 'teaser'...
 * - $teaser: Flag for the teaser state (shortcut for $view_mode == 'teaser').
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * Field variables: for each field instance attached to the node a corresponding
 * variable is defined, e.g. $node->body becomes $body. When needing to access
 * a field's raw values, developers/themers are strongly encouraged to use these
 * variables. Otherwise they will have to explicitly specify the desired field
 * language, e.g. $node->body['en'], thus overriding any language negotiation
 * rule that was previously applied.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 * @see template_process()
 */
?>

<?php $item_term = FALSE;
      if(isset($field_item_type) && is_array($field_item_type) && array_key_exists('und', $field_item_type)) {
         $item_term = taxonomy_term_load($field_item_type['und'][0]['tid']);
      }
      if($item_term === FALSE && isset($field_item_type)) {
         $item_term = taxonomy_term_load($field_item_type[0]['tid']);
      }
      $item_type = ""; $item_type_str = "";
      if(is_object($item_term)) { $item_type = strtolower($item_term->name); $item_type_str = $item_term->name; }
      if($node->type == 'page') { $item_type = 'page'; $item_type_str = 'Topic overview'; }
      if($node->type == 'misc_page') { $item_type = 'misc_page'; $item_type_str = 'Misc page'; }
?>

<?php if($item_type_str == 'Link' || $item_type_str == 'News' || $item_type_str == 'Podcast' || $item_type_str == 'Publication' || $item_type_str == 'Video' || $item_type_str == 'Topic overview' || $item_type_str == 'Misc page'): ?>

<?php $req = request_path(); ?>

<?php if($teaser && (substr($req, 0, 5) == "topic" || substr($req, 0, 5) == "links" || substr($req, 0, 11) == "publication")): ?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <div class="nodetype nodetype-<?php echo $item_type; ?>">

    <?php if($status == 0) { echo '<div class="messages warning">This item is unpublished</div>'; } ?>

    <div class="nodetype-name"><?php if($item_type_str == 'News') { echo "AI in the News"; } else { echo $item_type_str; } ?></div>

    <?php print render($title_prefix); ?>
    <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><?php if(!empty($field_original_link)) { display_link($field_original_link['und'][0], $title); } else { echo "<a href=\"$node_url\">$title</a>"; } ?></h2>
    <?php endif; ?>
    <?php print render($title_suffix); ?>

    <div class="summary">
      <?php if(!empty($field_representative_image)) {
              $image = field_get_items('node', $node, 'field_representative_image');
              print str_replace("/>", "align=\"left\"/>", render(field_view_value('node', $node, 'field_representative_image', $image[0], 'teaser')));
            }
      ?>
      <?php $summary = field_get_items('node', $node, 'body');
            print render(field_view_value('node', $node, 'body', $summary[0], 'teaser')); ?>
      <?php if(!empty($field_original_link)) {
            echo "<p>";
            display_link($field_original_link['und'][0], 'Link to external resource');
            echo "</p>";
      } ?>
   </div>

    <div class="metadata intopic">
      <?php if(!empty($field_publication_date)): ?>
        <?php $pubdate = field_get_items('node', $node, 'field_publication_date');
              print render(field_view_value('node', $node, 'field_publication_date', $pubdate[0], 'teaser')); ?>.
      <?php endif; ?>
      <?php
        $metadata = array();
        if(!empty($field_authors) && preg_match('/\w/', $field_authors['und'][0]['value'])) {
          array_push($metadata, "By ".$field_authors['und'][0]['value']);
        }
        if(!empty($field_source) && preg_match('/\w/', $field_source['und'][0]['value'])) {
          array_push($metadata, $field_source['und'][0]['value']);
        }
        if(!empty($field_minutes) && preg_match('/\w/', $field_minutes['und'][0]['value'])) {
          array_push($metadata, $field_minutes['und'][0]['value']." min");
        }
        if(empty($field_publication_date) && !empty($field_publication_year_int)) {
          array_push($metadata, $field_publication_year_int['und'][0]['value']);
        }
        print implode(", ", $metadata);
        if(!empty($metadata)) { print " &mdash; "; }
      ?>
      <a href="<?php print $node_url; ?>">Read more...</a>
    </div>

  </div>

</div>

<?php elseif($teaser): ?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <div class="nodetype nodetype-<?php echo $item_type; ?>">

    <?php if($status == 0) { echo '<div class="messages warning">This item is unpublished</div>'; } ?>

    <div class="nodetype-name"><?php if($item_type_str == 'News') { echo "AI in the News"; } else { echo $item_type_str; } ?></div>

    <?php print render($title_prefix); ?>
    <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><?php if(!empty($field_original_link)) { display_link($field_original_link['und'][0], $title); } else { echo "<a href=\"$node_url\">$title</a>"; } ?></h2>
    <?php endif; ?>
    <?php print render($title_suffix); ?>


    <div class="summary">
      <?php if(!empty($field_representative_image)) {
              $image = field_get_items('node', $node, 'field_representative_image');
              $out = field_view_value('node', $node, 'field_representative_image', $image[0], 'teaser');
              print str_replace("/>", "align=\"left\"/>", render($out));
            }
      ?>
      <?php $summary = field_get_items('node', $node, 'body');
            $out = field_view_value('node', $node, 'body', $summary[0], 'teaser');
            print render($out);
      ?>
      <?php if(!empty($field_original_link)) {
            echo "<p>";
            display_link($field_original_link['und'][0], 'Link to external resource');
            echo "</p>";
      } ?>

    </div>

    <table class="metadata">
    <tr>
    <td>
    <div class="metadata notintopic">
      <?php
        $metadata = array();
        if(!empty($field_authors) && preg_match('/\w/', $field_authors['und'][0]['value'])) {
          array_push($metadata, "By ".$field_authors['und'][0]['value']);
        }
        if(!empty($field_source) && preg_match('/\w/', $field_source['und'][0]['value'])) {
          array_push($metadata, $field_source['und'][0]['value']);
        }
        if(!empty($field_minutes) && preg_match('/\w/', $field_minutes['und'][0]['value'])) {
          array_push($metadata, $field_minutes['und'][0]['value']." min");
        }
        if(empty($field_publication_date) && !empty($field_publication_year_int)) {
          array_push($metadata, $field_publication_year_int['und'][0]['value']);
        }
        if(!empty($field_publication_date)) {
          $date = field_get_items('node', $node, 'field_publication_date');
          array_push($metadata, render(field_view_value('node', $node, 'field_publication_date', $date[0], 'teaser')));
        }
        print implode("<br/>", $metadata);
        if(!empty($metadata)) { print "<br/>"; }
      ?>
      <a href="<?php print $node_url; ?>">Read more...</a>
    </div>
    </td>
    <td>
    <div class="topics">
      <?php render_topics_subtopics($node); ?>
    </div>
    </td>
    </tr>
    </table>

  </div>

</div>


<?php else: // full node view ?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <div class="nodetype">

    <?php if($status == 0) { echo '<div class="messages warning">This item is unpublished</div>'; } ?>

    <?php if($sticky == 1) { echo '<div class="messages status">This item is highlighted</div>'; } ?>

    <?php print render($title_prefix); ?>
    <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><?php if(!empty($field_original_link)) { display_link($field_original_link[0], $title); } else { echo "<a href=\"$node_url\">$title</a>"; } ?></h2>
    <?php endif; ?>
    <?php print render($title_suffix); ?>

    <?php if ($display_submitted): ?>
    <div class="submitted">
      <?php print $submitted; ?>
    </div>
    <?php endif; ?>

    <div class="summary">
      <?php if(!empty($field_representative_image)) {
              $image = field_get_items('node', $node, 'field_representative_image');
              print str_replace("/>", "align=\"right\"/>", render(field_view_value('node', $node, 'field_representative_image', $image[0])));
            }
      ?>
      <?php $summary = field_get_items('node', $node, 'body');
            $out = field_view_value('node', $node, 'body', $summary[0], 'full');
            print render($out); ?>
    </div>


    <?php if(!empty($field_embedding)) {
        echo '<div style="text-align: center; margin: 2em;">';
        echo $field_embedding[0]['value'];
        echo '</div>';
    } ?>

    <?php if(isset($field_original_link)) { display_links($field_original_link, $field_link); } ?>

    <div class="metadata fullview">
      <?php if(!empty($field_publication_date)): ?>
        <?php $pubdate = field_get_items('node', $node, 'field_publication_date');
              print render(field_view_value('node', $node, 'field_publication_date', $pubdate[0])); ?>
        <br/>
      <?php endif; ?>
      <?php if(!empty($field_authors)): ?>
        By <?php echo $field_authors[0]['value']; ?>
        <br/>
      <?php endif; ?>
      <?php if(!empty($field_source)): ?>
        <?php echo $field_source[0]['value']; ?>
        <br/>
      <?php endif; ?>
      <?php if(!empty($field_link_category)): ?>
        <?php $term = taxonomy_term_load($field_link_category[0]['tid']); echo $term->name; ?>
        <br/>
      <?php endif; ?>
      <?php if(!empty($field_minutes)): ?>
        <?php echo $field_minutes[0]['value']; ?> min
        <br/>
      <?php endif; ?>
      <?php if(empty($field_publication_date) && !empty($field_publication_year_int)): ?>
        <?php echo $field_publication_year_int[0]['value']; ?>
        <br/>
      <?php endif; ?>
      <?php if(!empty($field_collections)): ?>
        <?php $term = taxonomy_term_load($field_collections[0]['tid']); echo $term->name; ?>
        <br/>
      <?php endif; ?>
    </div>

<?php
if(!empty($field_next_clicks)) {
    print '<div class="recommendations">';
    print 'Readers who view this page also view:';
    display_recommendations($field_next_clicks);
    print '</div>';
}
?>
    <div style="clear: both;"></div>

<?php 
if(!empty($field_editors)) {
  print '<div class="editors">Topic editor';
  if(count($field_editors) > 1) { print 's'; }
  print ': ';
  print render(field_view_field('node', $node, 'field_editors', 'default'));
  print '</div>';
}
?>

    <div class="topics-container">
    <?php if(isset($node)) { render_topics_subtopics($node); } ?>
    </div>
    <div class="tags-container">
    <?php if(isset($node)) { render_tags($node); } ?>
    </div>

    <div style="clear: both;"></div>

    <?php if(isset($service_links_rendered)) { echo $service_links_rendered; } ?>

  </div>

</div>

<?php endif; // end full node ?>

<?php elseif($type == "task"): ?>

  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

    <div class="summary">
      <?php $summary = field_get_items('node', $node, 'body');
            print render(field_view_value('node', $node, 'body', $summary[0], 'full')); ?>
    </div>

    <div class="metadata">
      This task is <?php if($node->status == 1) { print "<b>Active</b>"; } else { print "Inactive"; } ?>.<br/>
      Priority: <?php print render(field_view_value('node', $node, 'field_priority', $field_priority[0])); ?><br/>
      Last updated: <?php print format_date($node->changed, 'short'); ?><br/>
      <?php print render(field_view_value('node', $node, 'field_responsible', $field_responsible[0])); ?> is responsible for this task.<br/>
   </div>

<?php else: // not an Item or Task ?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print $user_picture; ?>

  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

  <?php if ($display_submitted): ?>
    <div class="submitted">
      <?php print $submitted; ?>
    </div>
  <?php endif; ?>

  <div class="content"<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      print render($content);
    ?>
  </div>

  <?php print render($content['links']); ?>

  <?php print render($content['comments']); ?>

</div>

<?php endif; ?>
