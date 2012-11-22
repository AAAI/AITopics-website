/**
 * @file
 * Adds draggable functionality to the table display of the view.
 */

(function ($) {
  Drupal.behaviors.draggableviewsAutosave = {
    attach: function(){
      if (typeof Drupal.tableDrag == 'undefined') {
        return;
      }
      for (var prop in Drupal.tableDrag){
        if (prop.substring(0, 14) == 'draggableviews'){
          var table = Drupal.tableDrag[prop];
          table.onDrop = function() {
            $('.tabledrag-changed-warning').hide();
            $(this.table).parent().find('#edit-actions input').triggerHandler('mousedown');
          }
          // Hide Save button.
          $('#' + prop).parent().find('#edit-actions input').hide();
        }
      }
    }
  }
})(jQuery);
