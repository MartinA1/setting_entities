<?php

/**
 * @Implements hook_views_data_alter().
 */
function setting_entities_views_data_alter(&$data) {
  $data['settings_entity']['edit_settings_entity'] = array(
    'field' => array(
      'title' => t('Edit link'),
      'help' => t('Provide a simple link to edit the content.'),
      'handler' => 'views_handler_field_settings_entity_link_edit',
    ),
  );
}
