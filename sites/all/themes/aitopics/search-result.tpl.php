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

if(!empty($result['fields']['sm_vid_Item_Type'])) {
  $item_type_str = $result['fields']['sm_vid_Item_Type'][0];
  $item_type = strtolower($item_type_str);
} else {
  $item_type = 'page';
  $item_type_str = 'Topic overview';
}

$rep_image = array();
if(!empty($result['fields']['ss_field_representative_image'])) {
  $rep_image = json_decode($result['fields']['ss_field_representative_image'], TRUE);
}

$primary_link = array();
if(!empty($result['fields']['ss_field_original_link'])) {
  $primary_link = json_decode($result['fields']['ss_field_original_link'], TRUE);
}

$topics = array();
if(!empty($result['fields']['im_field_topics'])) {
  foreach($result['fields']['im_field_topics'] as $tid) {
    array_push($topics, $tid);
  }
}
?>

<li class="<?php print $classes; ?>"<?php print $attributes; ?>>

  <div class="nodetype nodetype-<?php echo strtolower($item_type); ?>">

    <div class="nodetype-name"><?php if($item_type_str == 'News') { echo "AI in the News"; } else { echo $item_type_str; } ?></div>

    <?php print render($title_prefix); ?>
    <h2<?php print $title_attributes; ?>><?php if(!empty($primary_link)) { display_link($primary_link, $title); } else { echo "<a href=\"/node/".$result['fields']['entity_id']."\">$title</a>"; } ?></h2>
    <?php print render($title_suffix); ?>

    <div class="summary search-snippet-info">
      <?php
        if(!empty($rep_image)) {
            print "<img typeof=\"foaf:Image\" src=\"http://aitopics.org/sites/default/files/styles/thumbnail/public/".substr($rep_image['uri'], 8)."\" align=\"left\">\n";
        }
      ?>
      <?php if ($snippet): ?>
      <p class="search-snippet"<?php print $content_attributes; ?>>... <?php print $snippet; ?></p>
      <?php endif; ?>
    </div>

    <table class="metadata">
    <tr>
    <td>
    <div class="metadata notintopic">
      <?php
        $metadata = array();
        if(!empty($result['snippets']['tos_field_authors'])) {
          array_push($metadata, $result['snippets']['tos_field_authors'][0]);
        } elseif(!empty($result['fields']['tos_field_authors'])) {
          array_push($metadata, $result['fields']['tos_field_authors']);
        }

        if(!empty($result['snippets']['tos_field_source'])) {
          array_push($metadata, $result['snippets']['tos_field_source'][0]);
        } elseif(!empty($result['fields']['tos_field_source'])) {
          array_push($metadata, $result['fields']['tos_field_source']);
        }

        if(!empty($result['fields']['its_field_minutes'])) {
          array_push($metadata, $result['fields']['its_field_minutes']." min");
        }
        if(empty($result['fields']['dm_field_publication_date']) &&
           !empty($result['fields']['its_field_publication_year_int'])) {
          array_push($metadata, $result['fields']['its_field_publication_year_int']);
        }
        if(!empty($result['fields']['dm_field_publication_date'])) {
          $timestamp = strtotime($result['fields']['dm_field_publication_date'][0]);
          array_push($metadata, format_date($timestamp, 'custom', 'M j Y'));
        }
        print implode("<br/>", $metadata);
        if(!empty($metadata)) { print "<br/>"; }
      ?>

      <?php if(user_access('administer nodes')): ?>
      &mdash; <a href="<?php print "/node/".$result['fields']['entity_id']; ?>">View</a> | <a href="<?php print "/node/".$result['fields']['entity_id']."/edit"; ?>">Edit...</a>
      <?php endif; ?>
    </div>
    </td>
    <td>
    <div class="topics">
      <?php render_topics_subtopics_tids($topics, ($item_type == 'page'),
        html_entity_decode($title), array()); ?>
    </div>
    </td>
    </tr>
    </table>
  </div>
</li>
