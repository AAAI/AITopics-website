<?php
// $Id$

/**
 * @file
 * OM Maximenu Effects 
 *
 * @author: Daniel Honrade http://drupal.org/user/351112
 *
 */
 
/**
 * Mouse actions
 *
 */
function om_maximenu_action($action = 'hover', $menu = '', $displace = 0, $style = '') {

  $displace_menu = ($displace == 1) ? "$('#om-maximenu-" . $menu . " li.om-leaf .om-maximenu-content').css('position', 'relative').addClass('om-maximenu-displace');": '';

  if ($action == 'click_fast') {
    $click_action = "
        $(this).parent().siblings().children('.om-maximenu-content').hide();        
        $(this).siblings('.om-maximenu-content').toggle().pause();";
  }
  elseif ($action == 'click_slow') {
    $click_action = "
        $(this).parent().siblings().children('.om-maximenu-content').slideUp('slow');        
        $(this).siblings('.om-maximenu-content').toggle('slow').pause();";        
  }
  else {
    $click_action = "";  
  }
  
  $click = "
    jQuery(document).ready(function($){"
      . $displace_menu . "     
      $('#om-maximenu-" . $menu . " li.om-leaf .om-maximenu-content').removeClass('om-maximenu-content-nofade');
      $('#om-maximenu-" . $menu . " li.om-leaf .om-link').click(function() {
				if($(this).parent().hasClass('open')) {
				  $(this).parent().removeClass('open');
				}
				else {
				  $('#om-maximenu-" . $menu . " li.om-leaf').removeClass('open');
				  $(this).parent().addClass('open');
				}	
        " . $click_action . "       
        return false;
      });
    }); 
    ";
                  
  $hover_fade = "
    jQuery(document).ready(function($){"
      . $displace_menu . "     
      $('#om-maximenu-" . $menu . " li.om-leaf .om-maximenu-content').removeClass('om-maximenu-content-nofade');
      //$('#om-maximenu-" . $menu . " li.om-leaf').hover(omFadeIn,omFadeOut); //native jquery
      $('#om-maximenu-" . $menu . " li.om-leaf').hoverIntent({
        over: omFadeIn,
        timeout: 1000,
        out: omFadeOut
      }); 
      function omFadeIn(){ $('.om-maximenu-content.closed', this).fadeIn();}
      function omFadeOut(){ $('.om-maximenu-content.closed', this).fadeOut();}  
    });
    ";

  $hover = "jQuery(document).ready(function($){" . $displace_menu . " });";

  if (($action == 'hover') || ($action == 'click_fast')) {
    $tabbed_action = "
        $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content .om-tabbed-content').addClass('om-tabbed-content-hide');              
        $('#om-maximenu-" . $menu . " #' + tabbedContentId).removeClass('om-tabbed-content-hide');";
  }
  elseif (($action == 'hover_fade') || ($action == 'click_slow')) {
    $tabbed_action = "
        $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content .om-tabbed-content').addClass('om-tabbed-content-hide').hide();              
        $('#om-maximenu-" . $menu . " #' + tabbedContentId).fadeIn('slow').removeClass('om-tabbed-content-hide');";    
  }
  else {
    $tabbed_action = "";  
  }

  $tabbed_hover_js = "
    jQuery(document).ready(function($){
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf:first-child').addClass('active');          
      $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content div:first-child').removeClass('om-tabbed-content-hide'); 
               
      $('#om-maximenu-" . $menu . " .om-leaf .om-link').hover(
        function () {
          var leafId = $(this).parent().attr('id');
          var tabbedContentId = leafId.replace('leaf', 'tabbed-content');
          $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
          $(this).parent().addClass('active');       
             
          " . $tabbed_action . " 
        }, function () {
          //do nothing
      });
    });  
  ";
    
  $tabbed_click_js = "
    jQuery(document).ready(function($){
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf:first-child').addClass('active');          
      $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content div:first-child').removeClass('om-tabbed-content-hide'); 
               
      $('#om-maximenu-" . $menu . " .om-leaf .om-link').click(function () {
        var leafId = $(this).parent().attr('id');
        var tabbedContentId = leafId.replace('leaf', 'tabbed-content');
        $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
        $(this).parent().addClass('active');          
                      
        " . $tabbed_action . "      
      });
    });  
  ";  
  
  $flow = '';  
  if ($style == 'scrollv')    $flow = "$('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content-inner').css('height', totalHeight + 'px');"; 
  if ($style == 'scrollh')    $flow = "$('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content-inner').css('width', totalWidth + 'px');"; 
  if ($action == 'hover')      $speed = 1000;
  if ($action == 'hover_fade') $speed = 2000;
  if ($action == 'click_fast') $speed = 750;
  if ($action == 'click_slow') $speed = 1500;   

  if (($action == 'hover') || ($action == 'hover_fade')) $modal_action = 'hover';
  if (($action == 'click_fast') || ($action == 'click_slow')) $modal_action = 'click';
  
  $style_modal_js = "
    jQuery(document).ready(function($) {	
   
	    $('#om-maximenu-" . $menu . " .om-link[rel=om-maximenu-modal]')." . $modal_action . "(function(e) {
	      // prevent default link behavior
	      e.preventDefault();
		
	      // get anchor id
	      var id = $(this).attr('href');

	      // mask height and width
	      var om_maskHeight = $(document).height();
	      var om_maskWidth = $(window).width();
	
	      $('#om-maximenu-mask').css({'width': om_maskWidth, 'height': om_maskHeight});
		
	      // fade effect	
	      $('#om-maximenu-mask').fadeIn(" . $speed . ");	
	      $('#om-maximenu-mask').fadeTo('slow',0.75);	
	
	      // om-maximenu-modal-content height and width
	      var windowHeight = $(window).height();
	      var windowWidth = $(window).width();
              
	      // set om-maximenu-modal-content to center
	      $(id).css('top',  windowHeight/2-$(id).height()/2);
	      $(id).css('left', windowWidth/2-$(id).width()/2);
	
	      // fade effect to window
	      $(id).fadeIn(" . $speed . "); 
	    }, function() {
	      // do nothing
	    });
	
	    // if close button is clicked
	    $('.om-maximenu-modal-content .om-maximenu-close').click(function() {
		    $('#om-maximenu-mask').hide();
		    $('.om-maximenu-modal-content').hide();
	    });		
	
	    // if om-maximenu-mask is clicked
	    $('#om-maximenu-mask').click(function() {
		    $(this).hide();
		    $('.om-maximenu-modal-content').hide();
	    });			
    });
  ";
    
  $tabbed_scroll_hover_js = "
    jQuery(document).ready(function($){
      var contentNum = $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content-inner').children().size();
      var height = $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content').height();
      var width = $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content').width();
      var totalHeight = height * contentNum;
      var totalWidth = width * contentNum;

      $('#om-maximenu-" . $menu . " .om-tabbed-content').css('height', height + 'px');
      $('#om-maximenu-" . $menu . " .om-tabbed-content').css('width', width + 'px');
      " . $flow . "
                        
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf:first-child').addClass('active');          
      $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content .om-tabbed-content').removeClass('om-tabbed-content-hide'); 
               
      $('#om-maximenu-" . $menu . " .om-leaf .om-link').hover(
        function () {
          var leafId = $(this).parent().attr('id');
          var tabbedContentId = leafId.replace('leaf', 'tabbed-content');
                        
          $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
          $(this).parent().addClass('active');          

          $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content').scrollTo($('#' + tabbedContentId), " . $speed . ");
        }, function () {
          // do nothing
      });
    });  
  ";

  $tabbed_scroll_click_js = "
    jQuery(document).ready(function($){
      var contentNum = $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content-inner').children().size();
      var height = $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content').height();
      var width = $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content').width();
      var totalHeight = height * contentNum;
      var totalWidth = width * contentNum;

      $('#om-maximenu-" . $menu . " .om-tabbed-content').css('height', height + 'px');
      $('#om-maximenu-" . $menu . " .om-tabbed-content').css('width', width + 'px');
      " . $flow . "
                        
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
      $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf:first-child').addClass('active');          
      $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content .om-tabbed-content').removeClass('om-tabbed-content-hide'); 
               
      $('#om-maximenu-" . $menu . " .om-leaf .om-link').click(function () {
        var leafId = $(this).parent().attr('id');
        var tabbedContentId = leafId.replace('leaf', 'tabbed-content');
                        
        $('#om-maximenu-" . $menu . " ul.om-menu li.om-leaf').removeClass('active');          
        $(this).parent().addClass('active');          

        $('#om-maximenu-" . $menu . " .om-maximenu-tabbed-content').scrollTo($('#' + tabbedContentId), " . $speed . ");
      });
    });  
  "; 

  $style_accordion_js = "
    jQuery(document).ready(function ($) {
      var height = $('#om-maximenu-" . $menu . "').height();
      var width = $('#om-maximenu-" . $menu . "').width();

      $('#om-maximenu-" . $menu . " dl.easy-accordion').css('width', width + 'px').css('height', height + 'px');
      $('#om-maximenu-" . $menu . " dl.easy-accordion>dt').removeClass('active');  
      $('#om-maximenu-" . $menu . " dl.easy-accordion>dd').removeClass('active');  

      $('#om-maximenu-" . $menu . " dl.easy-accordion>dt:first-child').addClass('active');  
      $('#om-maximenu-" . $menu . " dl.easy-accordion>dd:first-child').addClass('active');  
                         
      $('#om-maximenu-" . $menu . "').easyAccordion({
        autoStart: true,
        slideInterval: 5000
        //slideNum: false
      });
    });  
  ";

  $style_roundabout_js = "jQuery(document).ready(function($) { $('#om-menu-" . $menu . "').roundabout(); });";
          
  if (($action == 'click_fast') || ($action == 'click_slow')) { 
    if (!empty($style)) {
      if ($style == 'normal') { 
        drupal_add_js($tabbed_click_js, "inline"); 
      }
      elseif ($style == 'accordion') {
        drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.easyAccordion.js'); 
        drupal_add_js($style_accordion_js, "inline");       
      }
      elseif ($style == 'roundabout') {
        drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.roundabout.min.js'); 
        drupal_add_js($style_roundabout_js, "inline");       
      }   
      elseif ($style == 'modal') {
        drupal_add_js($style_modal_js, "inline");       
      }      
      else {
        drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.scrollTo.min.js'); 
        drupal_add_js($tabbed_scroll_click_js, "inline");   
      } 
    }
    else {
      drupal_add_js($click, "inline"); 
    }    
  }
  else {
    if (!empty($style)) {
      if ($style == 'normal') { 
        drupal_add_js($tabbed_hover_js, "inline"); 
      }
      elseif ($style == 'accordion') {
        drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.easyAccordion.js'); 
        drupal_add_js($style_accordion_js, "inline");       
      }
      elseif ($style == 'roundabout') {
        drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.roundabout.min.js'); 
        drupal_add_js($style_roundabout_js, "inline");       
      }   
      elseif ($style == 'modal') {
        drupal_add_js($style_modal_js, "inline");       
      }      
      else {
        drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.scrollTo.min.js'); 
        drupal_add_js($tabbed_scroll_hover_js, "inline");   
      } 
    }
    else {     
      if ($action == 'hover_fade') {
        drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.hoverIntent.minified.js'); 
        drupal_add_js($hover_fade, "inline"); 
      }
      else {
        if ($displace == 1) drupal_add_js($hover, "inline"); 
      }  
    }
  } 
}


/**
 * Dock jquery plugin
 *
 */
function om_maximenu_dock($menu = '') {
  $add_js = "jQuery(document).ready(function($){ $('#om-menu-" . $menu . " .om-link img').resizeOnApproach(); });";
  drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.resizeOnApproach.min.js'); 
  drupal_add_js($add_js, "inline");
}


/**
 * Animated background slider
 *
 */
function om_maximenu_bg_slider($menu = NULL) {
  $bg_slide_js = "jQuery(document).ready(function($){ $('ul#om-menu-" . $menu . "').lavaLamp({ target: '.om-leaf', container: 'li', speed: 700 }); })";
  drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.lavalamp.min.js');   
  drupal_add_js($bg_slide_js, "inline");  
}


/**
 * Animated Link slider
 *
 */
function om_maximenu_link_slider($menu = NULL) {
  $link_slide_js = "
    jQuery(document).ready(function($) {
      linkHeight = $('#om-menu-" . $menu . " .om-leaf').height();
  
      $('#om-menu-" . $menu . " .om-leaf').css('height', linkHeight + 'px');
      $('#om-menu-" . $menu . " .om-leaf .om-link').css('overflow', 'hidden').css('height', 0);      
	    $('#om-menu-" . $menu . " .om-leaf').prepend('<span class=\"om-link-slider\"></span>'); 

	    $('#om-menu-" . $menu . " .om-leaf').each(function() { 
		    var textLink = $(this).find('.om-link').text(); 
		    $(this).find('span.om-link-slider').show().text(textLink); 
	    }); 

	    $('#om-menu-" . $menu . " .om-leaf').hover(
	      function() { 
	        $(this).find('span.om-link-slider').stop().animate({ lineHeight: 0, height: 0 }, 250);
	        $(this).find('.om-link').stop().animate({ height: linkHeight }, 250); 
	      },
	      function() { 
	        $(this).find('span.om-link-slider').stop().animate({ lineHeight: linkHeight, height: linkHeight }, 250);
	        $(this).find('.om-link').stop().animate({ height: 0 }, 250);
	    });
    });
  ";
  drupal_add_js($link_slide_js, "inline");
}


/**
 * Animated link giggle
 *
 */
function om_maximenu_jiggle($menu = NULL) {  
  
  $link_giggle_js = "
    jQuery(document).ready(function($){ 
	    // creates the target paths
	    var list_elements = '#om-menu-" . $menu . "' + ' li.om-leaf';
	    var link_elements = list_elements + ' .om-link';
	    var pad_out = 25;
	    var pad_in = 15;
	    var time = 150;
	    var multiplier = .8;
	    
  	  // initiates the timer used for the sliding animation
  	  var timer = 0;
	
	    // creates the jiggle animation for all list elements 
	    $(list_elements).each(function(i){
  		  // margin left = - ([width of element] + [total vertical padding of element])
	  	  $(this).css('margin-left','-180px');
	  	  // updates timer
	  	  timer = (timer*multiplier + time);
	  	  $(this).animate({ marginLeft: '0' }, timer);
	  	  $(this).animate({ marginLeft: '15px' }, timer);
	  	  $(this).animate({ marginLeft: '0' }, timer);
	    });

  	  // creates the hover-jiggle effect for all link elements 		
  	  $(link_elements).each(function(i){
	  	  $(this).hover(
		      function(){
			      $(this).animate({ paddingLeft: pad_out }, 150);
		      },		
		      function(){
		      	$(this).animate({ paddingLeft: pad_in }, 150);
		    });
	    });
    });";
  drupal_add_js($link_giggle_js, "inline");  
}
    

/**
 * Animated floating menu
 *
 */
function om_maximenu_scroll($menu = NULL) {    

  $scroll_float_js = "
    jQuery(document).ready(function($){ 
      var topYloc = null;

      $(window).scroll(function () { 
	      var scrollTop = $(document).scrollTop();
	      scrollTop = parseInt(scrollTop);
	
	      var offset = topYloc+scrollTop+'px';  
	      $('#om-maximenu-" . $menu . "').animate({top:offset},{duration:500,queue:false});
      });

	    topYloc = parseInt($('#om-maximenu-" . $menu . "').css('top').substring(0,$('#om-maximenu-" . $menu . "').css('top').indexOf('px')));
    });";
  drupal_add_js($scroll_float_js, "inline");  
}


/**
 * Admin - hide/show options
 *
 */
function om_maximenu_admin_js() {
  
  $add_js = "
    jQuery(document).ready(function($){ 
      $('form#om-maximenu-admin select.om-maximenu-style').change(function() {
        var style_wrapper = $(this).parent().parent().parent().attr('id');  
        var style_value = $(this).val(); 

        if(style_value == '') {
          $('form#om-maximenu-admin fieldset#' + style_wrapper + ' .om-maximenu-nontabbed-options').show();          
        }
        else {
          $('form#om-maximenu-admin fieldset#' + style_wrapper + ' .om-maximenu-nontabbed-options').hide();        
        }
        if((style_value != 'roundabout') && (style_value != 'accordion')) {
          $('form#om-maximenu-admin fieldset#' + style_wrapper + ' .om-maximenu-other-options').show();          
        }
        else {
          $('form#om-maximenu-admin fieldset#' + style_wrapper + ' .om-maximenu-other-options').hide();        
        }        
      });

      $('form#om-maximenu-admin select.om-maximenu-output').change(function() {
        var parent_wrapper = $(this).parent().parent().parent().attr('id');  
        var output_value = $(this).val(); 
        if(output_value == 'block') {
          $('form#om-maximenu-admin fieldset#' + parent_wrapper + ' .om-maximenu-block-options').show();
          $('form#om-maximenu-admin fieldset#' + parent_wrapper + ' .om-maximenu-float-options').hide();          
        }
        if(output_value == 'float') {
          $('form#om-maximenu-admin fieldset#' + parent_wrapper + ' .om-maximenu-float-options').show();
          $('form#om-maximenu-admin fieldset#' + parent_wrapper + ' .om-maximenu-block-options').hide();
        }     
        if(output_value == 'main_menu') {
          $('form#om-maximenu-admin fieldset#' + parent_wrapper + ' .om-maximenu-float-options').hide();
          $('form#om-maximenu-admin fieldset#' + parent_wrapper + ' .om-maximenu-block-options').hide();
        } 
      });
      $('form#om-maximenu-admin select.om-maximenu-x-origin').change(function() {
        var option_wrapper = $(this).parent().parent().parent().attr('id');  
        var x_origin_value = $(this).val(); 
        //alert(option_wrapper);
        if(x_origin_value == 'middle') {
          $('form#om-maximenu-admin #' + option_wrapper + ' fieldset.om-maximenu-non-middle-options').hide();
        }
        else {
          $('form#om-maximenu-admin #' + option_wrapper + ' fieldset.om-maximenu-non-middle-options').show();
        }     
      });     
    }); ";
  
  drupal_add_js($add_js, "inline");
} 


/**
 * Make menus active when submenus are active
 *
 */
function om_maximenu_active_js($menu = NULL, $animated_bg = 0, $tabbed = NULL) {

  $active     = 'active-trail';
  $lava       = 'selectedLava';
  $class = $animated_bg ? $lava: $active;
  
  $active_js = "
    jQuery(function($) {
      $.each($('#om-menu-" . $menu . " .om-leaf'),function() {

        var active = $('.menu a', this).hasClass('active');
        var active_trail = $('.menu a', this).hasClass('active-trail');
        
        if ((active == true) || (active_trail == true)) {
          $('#om-menu-" . $menu . " .om-leaf').removeClass('" . $class . "');
          $(this).addClass('" . $class . "');
        }
      });
    }); ";

  $active_tabbed_js = "
    jQuery(function($) {
      $.each($('#om-menu-" . $menu . " .om-leaf'),function() {
        var leafId = $(this).attr('id');
        var tabbedContentId = leafId.replace('leaf', 'tabbed-content');
        var active_tabbed = $('.menu a', '.om-maximenu-tabbed-content #' + tabbedContentId).hasClass('active');

        var active = $('.menu a', this).hasClass('active');
        
        if ((active_tabbed == true) || (active == true)) {
          $('#om-menu-" . $menu . " .om-leaf').removeClass('" . $class . "');
          $(this).addClass('" . $class . "');
        }
      });
    }); ";
  
  if ($tabbed == NULL) {    
    drupal_add_js($active_js, "inline");
  }
  else {
    drupal_add_js($active_tabbed_js, "inline");  
  }
} 


/**
 * Scroll long menu if it won't fit the container
 *
 */
function om_maximenu_longmenu_js($menu = NULL, $longmenu = NULL) {

  $longmenu_hover_scroll_js = "
    jQuery(document).ready(function($){
      if ($('#om-maximenu-" . $menu . "').hasClass('om-maximenu-row')) {
        var wrapper = $('#om-maximenu-" . $menu . " .om-menu-ul-wrapper'), 
            wrapper_width = wrapper.width(),
            ul = $('#om-menu-" . $menu . "'), 
            ul_width = 0;
        
        $('#om-menu-" . $menu . " .om-leaf').each(function(index) {
          ul_width += parseInt($(this).width(), 10);
        });
        
        // this will only be triggered when links won't fit inside the wrapper
        if (ul_width > wrapper_width) {

          var li_first = ul.find('li:first-child').width(),
              li_last = ul.find('li:last-child').width();
            
          // scroll adjustment to end link
          ul.width(ul_width + 30 + 'px')
        
          wrapper.css({overflow: 'hidden'});
        
          wrapper.mousemove(function(e){
            var wrapper_offset = wrapper.offset();
            var mouse = (e.pageX - wrapper_offset.left) * (ul_width - wrapper_width) / wrapper_width;

            // scroll adjustments for both ends
            if(mouse < li_first) mouse = (e.pageX - wrapper_offset.left - li_first) * (ul_width - wrapper_width) / wrapper_width;
            if(mouse > li_first) mouse = (e.pageX - wrapper_offset.left) * (ul_width - wrapper_width + li_last + 15) / wrapper_width;

            wrapper.scrollLeft(mouse);
          });
        }
      }
      else if ($('#om-maximenu-" . $menu . "').hasClass('om-maximenu-column')) {
        var wrapper = $('#om-maximenu-" . $menu . " .om-menu-ul-wrapper'), 
            wrapper_height = wrapper.height(),
            ul = $('#om-menu-" . $menu . "'), 
            ul_height = 0;

        $('#om-menu-" . $menu . " .om-leaf').each(function(index) {
          ul_height += parseInt($(this).height(), 10);
        });

        // this will only be triggered when links won't fit inside the wrapper
        if (ul_height > wrapper_height) {
        
          var li_first = ul.find('li:first-child').height(),
              li_last = ul.find('li:last-child').height();

          // scroll adjustment to end link
          ul.height(ul_height + 30 + 'px')
                             
          wrapper.css({overflow: 'hidden'});

          wrapper.mousemove(function(e){
            var wrapper_offset = wrapper.offset();
            var mouse = (e.pageY - wrapper_offset.top) * (ul_height - wrapper_height) / wrapper_height;

            // scroll adjustments for both ends
            if(mouse < li_first) mouse = (e.pageY - wrapper_offset.top - 5) * (ul_height - wrapper_height) / wrapper_height;

            wrapper.scrollTop(mouse);
          });
        }
      }
    }); ";

  $longmenu_prev_next_scroll_js = "
    jQuery(document).ready(function($){
      if ($('#om-maximenu-" . $menu . "').hasClass('om-maximenu-row')) {
        var wrapper = $('#om-maximenu-" . $menu . " .om-menu-ul-wrapper'), 
            wrapper_width = wrapper.width(),
            ul = $('#om-menu-" . $menu . "'), 
            ul_width = 0;
        
        $('#om-menu-" . $menu . " .om-leaf').each(function(index) {
          ul_width += parseInt($(this).width(), 10);
        });
  
        // this will only be triggered when links won't fit inside the wrapper
        if (ul_width > wrapper_width) {

          var wrapper_height = 30,
              li_first = ul.find('li:first-child').width(),
              li_last = ul.find('li:last-child').width();
                    
          wrapper.height(wrapper_height + 'px');

          $('#om-menu-" . $menu . "-ul-wrapper').css('margin', '0 30px');        
          $('#om-menu-" . $menu . "').css('position', 'absolute');
        
          // scroll adjustment to end link
          ul.width(ul_width + 30 + 'px')
                
          wrapper.css({overflow: 'hidden'});
        
          $('#om-maximenu-" . $menu . "').prepend('<div class=\"om-menu-scroller om-menu-previous\"><</div>');
          $('#om-maximenu-" . $menu . "').append('<div class=\"om-menu-scroller om-menu-next\">></div>');
        
          $('#om-maximenu-" . $menu . "').serialScroll({
            target:'#om-menu-" . $menu . "-ul-wrapper',
            items:'li.om-leaf', 
            prev:'div.om-menu-previous',
            next:'div.om-menu-next',
            axis:'xy',
            navigation:'',
            duration: 250,
            force:true,
            onBefore:function( e, elem, \$pane, \$items, pos ){
              e.preventDefault();
              if( this.blur ) this.blur();
            },
            onAfter:function( elem ){
              //'this' is the element being scrolled (\$pane) not jqueryfied
            }
          });
        }
      }
      else if ($('#om-maximenu-" . $menu . "').hasClass('om-maximenu-column')) {
        var wrapper = $('#om-maximenu-" . $menu . " .om-menu-ul-wrapper'), 
            wrapper_height = wrapper.height(),
            ul = $('#om-menu-" . $menu . "'), 
            ul_height = 0;

        $('#om-menu-" . $menu . " .om-leaf').each(function(index) {
          ul_height += parseInt($(this).height(), 10);
        });
  
        // this will only be triggered when links won't fit inside the wrapper
        if (ul_height > wrapper_height) {

          var wrapper_width = wrapper.width(),
              li_first = ul.find('li:first-child').height(),
              li_last = ul.find('li:last-child').height();

          $('#om-menu-" . $menu . "').css('position', 'absolute');
                      
          // scroll adjustment to end link
          ul.height(ul_height + 30 + 'px');
                  
          wrapper.css({overflow: 'hidden'});

          $('#om-maximenu-" . $menu . "').prepend('<div class=\"om-menu-scroller om-menu-previous\">^</div>');
          //$('#om-maximenu-" . $menu . "').append('<div class=\"om-menu-scroller om-menu-next\">v</div>');
          $('<div class=\"om-menu-scroller om-menu-next\">v</div>').insertAfter(wrapper);
                    
          $('#om-maximenu-" . $menu . " .om-menu-scroller').width(wrapper_width);
          
          $('#om-maximenu-" . $menu . "').serialScroll({
            target:'#om-menu-" . $menu . "-ul-wrapper',
            items:'li.om-leaf', 
            prev:'div.om-menu-previous',
            next:'div.om-menu-next',
            axis:'xy',
            navigation:'',
            duration: 250,
            force:true,
            onBefore:function( e, elem, \$pane, \$items, pos ){
              e.preventDefault();
              if( this.blur ) this.blur();
            },
            onAfter:function( elem ){
              //'this' is the element being scrolled (\$pane) not jqueryfied
            }
          });
        }
      }
    }); ";
      
  if ($longmenu == 'hover') {
    drupal_add_js($longmenu_hover_scroll_js, "inline");
  }
  elseif ($longmenu == 'prev_next') {  
    drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.scrollTo.min.js');   
    drupal_add_js(OM_MAXIMENU_PATH . '/contrib/jquery.serialScroll.min.js');   
    drupal_add_js($longmenu_prev_next_scroll_js, "inline");
  }
}  


