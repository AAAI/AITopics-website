<?php

/**
 * @file
 * i18n_string.inc
 *
 * Provides i18n string functions for the finder ui.
 */

/**
 * Implements hook_locale().
 */
function finder_ui_locale($op = 'groups', $group = NULL) {
  switch ($op) {
    case 'groups':
      return array('finder' => t('Finder'));
    case 'refresh':
      $finders = finder_load_all();
      if (is_array($finders)) {
        foreach ($finders as $finder) {
          finder_save($finder);
        }
      }
  }
}

/**
 * Implements hook_finder_render().
 */
function finder_ui_finder_render($object) {
  if (!empty($object->settings['i18n_string'])) {
    finder_ui_i18n_string_translate_strings($object, $object->settings['i18n_string']);
  }
  if (!empty($object->elements)) {
    foreach ($object->elements as $element) {
      finder_ui_finder_render($element);
    }
  }
}

/**
 * Implements hook_finder_presave().
 */
function finder_ui_finder_presave($object) {
  $object->settings['i18n_string'] = finder_ui_i18n_string_map($object);
  finder_ui_i18n_string_save_strings($object, $object->settings['i18n_string']);
  if (!empty($object->elements)) {
    foreach ($object->elements as $element) {
      finder_ui_finder_presave($element);
    }
  }
}

/**
 * Implements hook_finder_delete().
 */
function finder_ui_finder_delete($object) {
  finder_ui_i18n_string_delete_strings($object, finder_ui_i18n_string_map($object));
  if (!empty($object->elements)) {
    foreach ($object->elements as $element) {
      finder_ui_finder_delete($element);
    }
  }
}

/**
 * Get a default map of which fields are translatable from the form.
 */
function finder_ui_i18n_string_map($object, $form = NULL, $id = NULL, $parents = array()) {
  $map = array();

  if (is_null($form)) {
    if (empty($object->name)) {
      finder_inc('element', 'finder_ui');
      if (!is_object($object->finder)) {
        $object->finder = finder_load($object->finder);
      }
      $data = finder_ui_element($object->finder, $object->id);
      // Uses a '.' instead of ':' to prevent collisions between properties and element ids.
      $id = 'finder:' . $object->finder->name . '.' . $object->id . ':';
      $map['title'] = $id . 'title';
    }
    else {
      finder_inc('finder', 'finder_ui');
      $data = finder_ui($object);
      $id = 'finder:' . $object->name . ':';
      $map['title'] = $id . 'title';
      $map['description'] = $id . 'description';
    }
    $form = array();
    foreach ($data['items'] as $item) {
      foreach ($item['#form'] as $form_key => $form_item) {
        if (empty($form[$form_key])) {
          $form[$form_key] = array();
        }
        $form[$form_key] = array_merge($form[$form_key], $form_item);
      }
    }
  }
  foreach (element_children($form) as $key) {
    $path = $parents;
    $path[] = $key;
    // Set #translatable to true on these form element types, unless it is already set.
    if (!isset($form[$key]['#translatable']) && !empty($form[$key]['#type']) && in_array($form[$key]['#type'], array('textfield', 'textarea', 'text_format'))) {
      $form[$key]['#translatable'] = TRUE;
    }
    if (!empty($form[$key]['#translatable'])) {
      $map[$key] = $id . implode('.', $path);
    }

    $children = finder_ui_i18n_string_map($object, $form[$key], $id, $path);
    if (!empty($children)) {
      if (empty($map[$key])) {
        $map[$key] = array();
      }
      foreach ($children as $child_key => $child) {
        $map[$key][$child_key] = $child;
      }
    }
  }

  return $map;
}

/**
 * Recursively save strings using the map.
 */
function finder_ui_i18n_string_save_strings($object, $map) {
  if (is_array($object)) {
    $object = clone (object)$object;
  }
  if (!empty($map)) {
    foreach ($map as $key => $value) {
      if (isset($object->$key) && !is_numeric($object->$key) && is_string($value)) {
        i18n_string_update($value, $object->$key);
      }
      if (is_array($value) && !empty($object->$key)) {
        finder_ui_i18n_string_save_strings($object->$key, $value);
      }
    }
  }
}

/**
 * Recursively translate strings using the map.
 */
function finder_ui_i18n_string_translate_strings(&$object, $map) {
  if (!empty($map)) {
    $was_array = FALSE;
    if (is_array($object)) {
      $was_array = TRUE;
      $object = (object)$object;
    }
    foreach ($map as $key => $value) {
      if (is_string($object->$key) && is_string($value)) {
        $object->$key = i18n_string($value, $object->$key);
      }
      elseif (is_array($value)) {
        finder_ui_i18n_string_translate_strings($object->$key, $value);
      }
    }
    if ($was_array) {
      $object = (array)$object;
    }
  }
}

/**
 * Recursively delete strings using the map.
 */
function finder_ui_i18n_string_delete_strings($object, $map) {
  $array = (array)$object;
  if (!empty($map)) {
    foreach ($map as $key => $value) {
      if (isset($array[$key]) && is_string($value)) {
        i18n_string_remove($value, $array[$key]);
      }
      if (is_array($value)) {
        finder_ui_i18n_string_delete_strings($array[$key], $value);
      }
    }
  }
}

