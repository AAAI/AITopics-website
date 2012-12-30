<?php $item_term = FALSE;
      if(isset($node) && property_exists($node, 'field_item_type') && is_array($node->field_item_type) && array_key_exists('und', $node->field_item_type)) { $item_term = taxonomy_term_load($node->field_item_type['und'][0]['tid']); }
      if(isset($node) && $item_term === FALSE && isset($node->field_item_type)) { $item_term = taxonomy_term_load($node->field_item_type[0]['tid']); }
      $item_type = "";
      if(is_object($item_term)) { $item_type = $item_term->name; }
      if(isset($node) && $node->type == 'page') { $item_type = 'Page'; }
      if(isset($node) && $node->type == 'misc_page') { $item_type = 'Misc Page'; }
?>
<div class="texture-overlay">
  <div id="page" class="container">

    <header id="header" class="clearfix" role="banner">
      <div class="header-inner clearfix">
        <div id="branding">
          <?php if ($linked_site_logo): ?>
            <div id="logo"><?php print $linked_site_logo; ?></div>
          <?php endif; ?>
          <?php if ($site_name || $site_slogan): ?>
            <hgroup<?php if (!$site_slogan && $hide_site_name): ?> class="<?php print $visibility; ?>"<?php endif; ?>>
              <?php if ($site_name): ?>
                <h1 id="site-name"<?php if ($hide_site_name): ?> class="<?php print $visibility; ?>"<?php endif; ?>><?php print $site_name; ?></h1>
              <?php endif; ?>
              <?php if ($site_slogan): ?>
                <h2 id="site-slogan"><?php print $site_slogan; ?></h2>
              <?php endif; ?>
            </hgroup>
          <?php endif; ?>
        </div>
        <?php print render($page['header']); ?> <!-- /header region -->
      </div>
    </header> <!-- /header -->

    <?php print render($page['menu_bar']); ?> <!-- /menu bar -->

    <?php print $messages; ?> <!-- /message -->
    <?php print render($page['help']); ?> <!-- /help -->

    <?php if ($breadcrumb): ?>
      <nav id="breadcrumb"><?php print $breadcrumb; ?></nav> <!-- /breadcrumb -->
    <?php endif; ?>

    <?php print render($page['secondary_content']); ?> <!-- /secondary-content -->

    <!-- Three column 3x33 Gpanel -->
    <?php if ($page['three_33_first'] || $page['three_33_second'] || $page['three_33_third']): ?>
      <div class="three-3x33 gpanel clearfix">
        <?php print render($page['three_33_first']); ?>
        <?php print render($page['three_33_second']); ?>
        <?php print render($page['three_33_third']); ?>
      </div>
    <?php endif; ?>

    <div id="columns"><div class="columns-inner clearfix">
      <div id="content-column"><div class="content-inner">

        <?php print render($page['highlight']); ?> <!-- /highlight -->

        <?php $tag = $title ? 'section' : 'div'; ?>
        <<?php print $tag; ?> id="main-content" role="main">

          <div class="nodetype nodetype-<?php echo strtolower($item_type); ?>">

            <?php print render($title_prefix); ?>
            <?php if ($title || $primary_local_tasks || $secondary_local_tasks || $action_links = render($action_links)): ?>
            <?php if ($primary_local_tasks || $secondary_local_tasks || $action_links): ?>
            <div id="tasks" class="clearfix">
              <?php if ($primary_local_tasks): ?>
              <ul class="tabs primary clearfix"><?php print render($primary_local_tasks); ?></ul>
              <?php endif; ?>
              <?php if ($secondary_local_tasks): ?>
              <ul class="tabs secondary clearfix"><?php print render($secondary_local_tasks); ?></ul>
              <?php endif; ?>
              <?php if ($action_links = render($action_links)): ?>
              <ul class="action-links clearfix"><?php print $action_links; ?></ul>
              <?php endif; ?>
            </div>
            <?php endif; ?>
            
            <header class="clearfix">
              <?php if(isset($node) && $item_type != 'Misc Page' && $item_type != 'Page'): ?>
                <div class="nodetype-name">
                  <?php if($item_type == 'News') { echo "AI in the News"; } else { echo $item_type; } ?>
                </div>
              <?php endif; ?>
              <?php if ($title): ?>
                  <?php if(isset($node) && !empty($node->field_original_link) && !preg_match('/<em/', $title)): ?>
                      <h1 id="page-title"><?php display_link($node->field_original_link['und'][0], $title); ?></h1>
                  <?php else: ?>
                      <h1 id="page-title"><?php print $title; ?></h1>
                  <?php endif; ?>
              <?php endif; ?>
            </header>
            <?php endif; ?>
            <?php print render($title_suffix); ?>
            
            <?php print render($page['content']); ?> <!-- /content -->
            
            <?php print $feed_icons; ?> <!-- /feed icons -->

          </div>
            
        </<?php print $tag; ?>> <!-- /main-content -->

        <?php print render($page['content_aside']); ?> <!-- /content-aside -->

      </div></div> <!-- /content-column -->

      <?php print render($page['sidebar_first']); ?>
      <?php print render($page['sidebar_second']); ?>

    </div></div> <!-- /columns -->

    <?php print render($page['tertiary_content']); ?> <!-- /tertiary-content -->

    <!--
    <footer id="footer" role="contentinfo"><div id="footer-inner" class="clearfix">

    <?php if ($page['four_first'] || $page['four_second'] || $page['four_third'] || $page['four_fourth']): ?>
      <div class="four-4x25 gpanel clearfix">
        <?php if($page['four_first']) { print render($page['four_first']); } else { print "<div class=\"region region-four-first\">&nbsp;</div>"; } ?>
        <?php if($page['four_second']) { print render($page['four_second']); } else { print "<div class=\"region region-four-second\">&nbsp;</div>"; } ?>
        <?php if($page['four_third']) { print render($page['four_third']); } else { print "<div class=\"region region-four-third\">&nbsp;</div>"; } ?>
        <?php if($page['four_fourth']) { print render($page['four_fourth']); } else { print "<div class=\"region region-four-fourth\">&nbsp;</div>"; } ?>
      </div>
    <?php endif; ?>

    <?php print render($page['footer']); ?>
    </div></footer>
    -->

  </div> <!-- /page -->
</div> <!-- /texture overlay -->
