<?php
// Footheme by Adaptivethemes.com, a starter sub-sub-theme.

/**
 * Rename each function and instance of "footheme" to match
 * your subthemes name, e.g. if you name your theme "footheme" then the function
 * name will be "footheme_preprocess_hook". Tip - you can search/replace
 * on "footheme".
 */

/**
 * Override or insert variables into the html templates.
 * Replace 'footheme' with your themes name, i.e. mytheme_preprocess_html()
 */
function footheme_preprocess_html(&$vars) {

  // Load the media queries styles
  // If you change the names of these files they must match here - these files are
  // in the /css/ directory of your subtheme - the names must be identical!
  $media_queries_css = array(
    'footheme.responsive.style.css',
    'footheme.responsive.gpanels.css'
    );
  load_subtheme_media_queries($media_queries_css, 'footheme'); // Replace 'footheme' with your themes name

  /**
   * Load IE specific stylesheets
   * AT automates adding IE stylesheets, simply add to the array using
   * the conditional comment as the key and the stylesheet name as the value.
   *
   * See our online help: http://adaptivethemes.com/documentation/working-with-internet-explorer
   *
   * For example to add a stylesheet for IE8 only use:
   *
   *  'IE 8' => 'ie-8.css',
   *
   * Your IE CSS file must be in the /css/ directory in your subtheme.
   */
  /* -- Delete this line to add a conditional stylesheet for IE 7 or less.
     $ie_files = array(
     'lte IE 7' => 'ie-lte-7.css',
     );
     load_subtheme_ie_styles($ie_files, 'footheme'); // Replace 'footheme' with your themes name
     // */

}

function render_topics_subtopics($node) {
  if(isset($node) && property_exists($node, 'field_topics') && array_key_exists('und', $node->field_topics)) {
    $topics = array();
    $topics_with_children = array();
    for($i = 0; $i < count($node->field_topics['und']); $i++) {
      try {
        $tid = $node->field_topics['und'][$i]['tid'];
        $term = taxonomy_term_load($tid);
        $child_uri = entity_uri('taxonomy_term', $term);

        $parents = taxonomy_get_parents($tid);
        if(empty($parents)) {
          if($node->type != 'page' || $term->name != $node->title) {
            $topics[$term->name] = l($term->name, $child_uri['path']);
          }
        } else {
          foreach($parents as $parent) {
            $topics_with_children []= $parent->name;
            $uri = entity_uri('taxonomy_term', $parent);
            $link = l($parent->name, $uri['path']);
            if($node->type != 'page' || $term->name != $node->title) {
              $link .= " > ";
              $link .= l($term->name, $child_uri['path']);
            }
            $topics[$term->name." > ".$parent->name] = $link;
          }
        }
      }
      catch(Exception $e) { }
    }
    asort($topics);
    if(!empty($topics)) {
      ?>
      <div class="topics">
      <table>
      <tr>
      <td style="vertical-align: middle; width: 35px;">
        <img src="/sites/all/themes/aitopics/icons/topic-meta.png" width=35 height=35 align="right" />
      </td>
      <td style="vertical-align: middle;">
      <?php
      $c = 0;
      foreach($topics as $topic => $link) {
        if(in_array($topic, $topics_with_children)) { continue; }
        print $link;
        $c++;
        if($c != count($topics)) { print "<br/>"; }
      }
      ?>
      </td>
      </tr>
      </table>
      </div>
      <?php
    }
  }
}

function render_tags($node) {
  if(isset($node) && property_exists($node, 'field_tags') && array_key_exists('und', $node->field_tags)) {
    $tags = array();
    for($i = 0; $i < count($node->field_tags['und']); $i++) {
      try {
        $tid = $node->field_tags['und'][$i]['tid'];
        $term = taxonomy_term_load($tid);
        $uri = entity_uri('taxonomy_term', $term);

        $tags[$term->name] = l($term->name, $uri['path']);
      }
      catch(Exception $e) { }
    }
    ksort($tags);
    if(!empty($tags)) {
      ?>
      <div class="tags">
      <table>
      <tr>
      <td style="vertical-align: middle; width: 35px;">
        <img src="/sites/all/themes/aitopics/icons/tags.png" width=35 height=29 align="left" />
      </td>
      <td style="vertical-align: middle;">
      <?php
      $c = 0;
      foreach($tags as $tag => $link) {
        print $link;
        $c++;
        if($c != count($tags)) { print ", "; }
      }
      ?>
      </td>
      </tr>
      </table>
      </div>
      <?php
    }
  }
}

function display_link($link, $title) {
  print "<a href=\"";
  print $link['url'];
  if(array_key_exists('query', $link) && is_array($link['query'])) {
    print "?";
    foreach($link['query'] as $field => $value) {
      print "$field=$value&";
    }
  } else if(array_key_exists('query', $link)) {
    print $link['query'];
  }
  if(array_key_exists('fragment', $link)) {
    print '#'.$link['fragment'];
  }
  print "\" title=\"$title\">$title</a>";
}

function display_links($primary_link, $extra_links) {
  print '<div class="external-links">';
  for($i = 0; $i < count($extra_links); $i++) {
    display_link($extra_links[$i], $extra_links[$i]['title']);
    if($i < count($extra_links)-1) { print "<br/>"; }
  }
  print '</div>';
}

/**   
 * Returns HTML for a date element formatted as a single date.
 */
function aitopics_date_display_single($variables) { 
  $date = $variables['date'];
  $timezone = $variables['timezone'];
  $attributes = $variables['attributes'];
        
  // Wrap the result with the attributes.
  return $date;
}

function aitopics_vertical_tabs($variables) {
  $element = $variables['element'];
  // Add required JavaScript and Stylesheet.
  drupal_add_library('system', 'drupal.vertical-tabs');
  drupal_add_js(path_to_theme() . '/js/jquery.scrollTo-1.4.3.1-min.js', array('weight' => 8));
  drupal_add_js(path_to_theme() . '/js/vertical-tabs.js', array('weight' => 9));

  $output = '<h2 class="element-invisible">' . t('Vertical Tabs') . '</h2>';
  $output .= '<div class="vertical-tabs-panes">' . $element['#children'] . '</div>';
  return $output;
}

