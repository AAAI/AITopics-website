<blockquote>Searched for: <strong><?php echo $query; ?></strong> in topics, authors, titles, and full-text.</blockquote>

<?php if(!empty($custom_results['redirects'])): ?>
<h2>Did you mean?</h2>
<ul>
<?php foreach($custom_results['redirects'] as $redirect): ?>
<li><?php echo "<b>$redirect</b>"; ?></li>
<?php endforeach; ?>
</ul>
<?php endif; ?>

<?php if(FALSE !== strpos($custom_results['topics'], "<h2")): ?>

<?php print $custom_results['topics']; ?>

<?php endif; ?>


<?php if(FALSE !== strpos($custom_results['title_authors'], "<table")): ?>

<?php print $custom_results['title_authors']; ?>

<?php endif; ?>

<?php
if(!empty($solr_results)) { 
      print "<h2>Full-text search results</h2>";
      print theme('search_results', array('results' => $solr_results));
}
?>
