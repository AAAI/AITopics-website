<?php

/**
 * @file
 * Handler for 'finder_plugin_style' style.
 */
class finder_plugin_style extends views_plugin_style {

  function render() {
    return finder_build_style_render($this);
  }

}