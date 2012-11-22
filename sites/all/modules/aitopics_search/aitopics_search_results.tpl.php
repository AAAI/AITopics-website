<i>Searched for "<?php echo $query; ?>"</i>

<?php if(!empty($custom_results['redirects'])): ?>
<h2>Did you mean?</h2>
<ul>
<?php foreach($custom_results['redirects'] as $redirect): ?>
<li><?php echo "<b>$redirect</b>"; ?></li>
<?php endforeach; ?>
</ul>
<?php endif; ?>

<?php if(FALSE !== strpos($custom_results['topics'], "<h2")): ?>
<h2>Matching topic overviews</h2>

<?php print $custom_results['topics']; ?>

<?php endif; ?>


<?php if(FALSE !== strpos($custom_results['authors'], "<table")): ?>
<h2>Results by matching author</h2>

<?php print $custom_results['authors']; ?>

<?php endif; ?>

<?php if(FALSE !== strpos($custom_results['title'], "<table")): ?>
<h2>Results by matching title</h2>

<?php print $custom_results['title']; ?>

<?php endif; ?>

<?php
if(!empty($solr_results)) { 
      print "<h2>Full-text search results</h2>";
      print theme('search_results', array('results' => $solr_results));
}
?>
