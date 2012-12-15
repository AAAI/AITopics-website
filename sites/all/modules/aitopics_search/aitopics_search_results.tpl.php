<blockquote>
<p>Terms: <strong><?php echo $query; ?></strong></p>
<p>Searched topics, authors, titles, and full-text.
     <?php if(count($search_query_words) > 1): ?>
     Use quotes to group terms, e.g. "<?php print $search_query_words[0].' '.$search_query_words[1]; ?>"
     <?php endif; ?>
</p>
</blockquote>

<?php if(!empty($custom_results['redirects'])): ?>
<blockquote>
<h2>Where you looking for...</h2>
<ul>
<?php foreach($custom_results['redirects'] as $redirect): ?>
<li><?php echo "<b>$redirect</b>"; ?></li>
<?php endforeach; ?>
</ul>
</blockquote>
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
