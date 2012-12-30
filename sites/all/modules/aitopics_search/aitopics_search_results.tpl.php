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

<?php
if(!empty($solr_results)) { 
      print theme('search_results', array('results' => $solr_results));
}
?>
