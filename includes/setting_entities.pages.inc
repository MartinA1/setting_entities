<?php

/**
 * Settings entity view callback.
 */
function settings_entity_view($settings_entity) {
  drupal_set_title(entity_label('settings_entity', $settings_entity));
  return entity_view('settings_entity', array(entity_id('settings_entity', $settings_entity) => $settings_entity), 'full');
}
