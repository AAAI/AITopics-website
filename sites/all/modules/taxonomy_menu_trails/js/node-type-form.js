(function ($) {
  Drupal.behaviors.taxonomyMenuTrailsFieldsetSummaries = {
    attach: function (context) {
      $('fieldset#edit-taxonomy-menu-trails', context).drupalSetSummary(function (context) {
        var vals = [];

        // Term references for setting active trail
        var categories = [];
        $('#edit-taxonomy-menu-trails-instances input:checked', context).siblings('label').each(function() {
          categories.push('"' + $(this).text().replace(/\s+$/, '') + '"');
        });
        if (categories.length) {
          vals.push(Drupal.t("Terms from @categories", {'@categories': categories.join(', ')}));

          // Term selection method
          var method = $("#edit-taxonomy-menu-trails-selection-method option:selected", context).text();
          vals.push(Drupal.t('select @method', {'@method': method.toLowerCase()}));

          // Taxonomy Menu integration
          var no_menu = $('#edit-taxonomy-menu-trails-tm-integration', context).is(':checked');
          if (no_menu) {
            vals.push(Drupal.t("path from Taxonomy Menu"));
          }
        }
        else {
          vals.push(Drupal.t('Disabled'))
        }

        return vals.join(', ');
      });
    }
  };
})(jQuery);
