jQuery.ajax({
    url: "/check_redirect.php",
    success: function(data) {
        if(data == "true") {
            jQuery("#redirect-notice").html("<div class=\"messages status redirect\">You have been redirected to the new AITopics.org</div>");
        }
    }
});
