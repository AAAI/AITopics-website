<?php

/**
 * @file
 * Handler for 'finder_plugin_display' display.
 */
class finder_plugin_display extends views_plugin_display {

  function execute() {
    return $this->view->render($this->display->id);
  }

  function render() {
    return !empty($this->view->result) || !empty($this->view->style_plugin->definition['even empty']) ? $this->view->style_plugin->render($this->view->result) : '';
  }

  function query() {
    finder_build_display_query($this);
  }

  function render_empty() {
    return parent::render_empty();
  }

}