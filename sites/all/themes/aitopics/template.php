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

function display_related_topics($tids) {
  $related = array();
  foreach($tids as $tid) {
    $term = taxonomy_term_load($tid);
    $uri = entity_uri('taxonomy_term', $term);
    array_push($related, l($term->name, $uri['path'], $uri['options']));
  }
  print implode(", ", $related);
}

function render_topics_subtopics_tids($tids, $topic_overview, $page_name, $related_tids) {
  $topics = array();
  $topics_with_children = array();
  foreach($tids as $tid) {
    try {
      $term = taxonomy_term_load($tid);
      $child_uri = entity_uri('taxonomy_term', $term);
      
      $parents = taxonomy_get_parents($tid);
      if(empty($parents) && (!$topic_overview || $term->name != $page_name)) {
        $topics[$term->name] = l($term->name, $child_uri['path'], $child_uri['options']);
      } else {
        foreach($parents as $parent) {
          $topics_with_children []= $parent->name;
          $uri = entity_uri('taxonomy_term', $parent);
          $link = l($parent->name, $uri['path'], $uri['options']);
          if(!$topic_overview || $term->name != $page_name) {
            $link .= " > ";
            $link .= l($term->name, $child_uri['path'], $child_uri['options']);
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
    <img src="/sites/all/themes/aitopics/icons/topic-meta.png" width="35" height="35" align="right" />
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
    
    if(!empty($related_tids)) {
      print '<br/><span class="related-topics">Related topics:</span> ';
      display_related_topics($related_tids);
    }
?>
    </td>
    </tr>
    </table>
    </div>
<?php
  }
}

function render_topics_subtopics($node) {
  if(isset($node) && property_exists($node, 'field_topics') && array_key_exists('und', $node->field_topics)) {
    $tids = array();
    for($i = 0; $i < count($node->field_topics['und']); $i++) {
      array_push($tids, $node->field_topics['und'][$i]['tid']);
    }
    $topic_overview = ($node->type == 'page');
    $related_tids = array();
    if(property_exists($node, 'field_related_topics') && array_key_exists('und', $node->field_related_topics)) {
      foreach($node->field_related_topics['und'] as $topic) {
        array_push($related_tids, $topic['tid']);
      }
    }
    render_topics_subtopics_tids($tids, $topic_overview, $node->title, $related_tids);
  }
}

function render_panel($node) {
  if(isset($node) && property_exists($node, 'field_link_category') && array_key_exists('und', $node->field_link_category) && !empty($node->field_link_category['und'])) {
    $term = $node->field_link_category['und'][0]['taxonomy_term'];
    $uri = entity_uri('taxonomy_term', $term);
    return "Found in ".l($term->name, $uri['path'], $uri['options'])."";
  } else {
    return "";
  }
}

function render_collection($node) {
  if(isset($node) && property_exists($node, 'field_collections') && array_key_exists('und', $node->field_collections) && !empty($node->field_collections['und'])) {
    $term = $node->field_collections['und'][0]['taxonomy_term'];
    $uri = entity_uri('taxonomy_term', $term);
    return "Part of the ".l($term->name, $uri['path'], $uri['options'])." collection";
  } else {
    return "";
  }
}

function render_persons_of_interest($node) {
  if(isset($node) && property_exists($node, 'field_persons_of_interest') && array_key_exists('und', $node->field_persons_of_interest) && !empty($node->field_persons_of_interest['und'])) {
    $persons = array();
    for($i = 0; $i < count($node->field_persons_of_interest['und']); $i++) {
      try {
        $tid = $node->field_persons_of_interest['und'][$i]['tid'];
        $term = taxonomy_term_load($tid);
        $uri = entity_uri('taxonomy_term', $term);

        $persons[$term->name] = l($term->name, $uri['path'], $uri['options']);
      }
      catch(Exception $e) { }
    }
    ksort($persons);
    if(!empty($persons)) {
      ?>
      <div class="persons">
      <table>
      <tr>
      <td style="vertical-align: middle; width: 35px;">
        <img src="/sites/all/themes/aitopics/icons/person.png" width="35" height="29" align="left" />
      </td>
      <td style="vertical-align: middle;">
      <?php
      $c = 0;
      foreach($persons as $person => $link) {
        print $link;
        $c++;
        if($c != count($persons)) { print "<br/>"; }
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

        $tags[$term->name] = l($term->name, $uri['path'], $uri['options']);
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
        <img src="/sites/all/themes/aitopics/icons/tags.png" width="35" height="29" align="left" />
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

function display_recommendations($links) {
    print '<ul class="recommended-links">';
    for($i = 0; $i < count($links); $i++) {
        print '<li>';
        display_link($links[$i], $links[$i]['title']);
        if(preg_match("!/news/!", $links[$i]['url'])) {
            print " (News)";
        } else if(preg_match("!/topic/!", $links[$i]['url'])) {
            print " (Topic overview)";
        } else if(preg_match("!/publication/!", $links[$i]['url'])) {
            print " (Publication)";
        } else if(preg_match("!/link/!", $links[$i]['url'])) {
            print " (Link)";
        } else if(preg_match("!/video/!", $links[$i]['url'])) {
            print " (Video)";
        } else if(preg_match("!/podcast/!", $links[$i]['url'])) {
            print " (Podcast)";
        }
        print '</li>';
    }
    print '</ul>';
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

function aitopics_preprocess_views_view_summary(&$vars) {
  if($vars['view']->name == 'persons_of_interest') {
    $items = array();
    foreach($vars['rows'] as $result){
      // remove links that aren't from the right vocabulary;
      // we detect this by checking for a number in the link rather than a term name
      if(!preg_match("!/\d+$!", $result->url)) {
        $items[] = $result;
      }
    }

    $vars['rows'] = $items;
  }
}

function dsm($arr) {
  drupal_set_message('<pre>'.print_r($arr, 1).'</pre>');
}
