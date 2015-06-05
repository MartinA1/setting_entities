<?php

/**
 * Generates the Settings entity type editing form.
 */
function settings_entity_type_form($form, &$form_state, $settings_entity_type, $op = 'edit') {

  if ($op == 'clone') {
    $settings_entity_type->label .= ' (cloned)';
    $settings_entity_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $settings_entity_type->label,
    '#description' => t('The human-readable name of this setting type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($settings_entity_type->type) ? $settings_entity_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $settings_entity_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'settings_entity_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this setting type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings entity type'),
    '#weight' => 40,
  );

  if (!$settings_entity_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete setting type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('settings_entity_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing settings_entity_type
 */
function settings_entity_type_form_submit(&$form, &$form_state) {
  $settings_entity_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  settings_entity_type_save($settings_entity_type);

  // Redirect user back to list of settings entity types.
  $form_state['redirect'] = 'admin/structure/settings-entity-types';
}

function settings_entity_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/settings-entity-types/' . $form_state['settings_entity_type']->type . '/delete';
}

/**
 * Settings entity type delete form.
 */
function settings_entity_type_form_delete_confirm($form, &$form_state, $setting_type) {
  $form_state['settings_entity_type'] = $setting_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['settings_entity_type_id'] = array('#type' => 'value', '#value' => entity_id('settings_entity_type' ,$setting_type));
  return confirm_form($form,
    t('Are you sure you want to delete setting type %title?', array('%title' => entity_label('settings_entity_type', $setting_type))),
    'settings_entity/' . entity_id('settings_entity_type' ,$setting_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Settings entity type delete form submit handler.
 */
function settings_entity_type_form_delete_confirm_submit($form, &$form_state) {
  $setting_type = $form_state['settings_entity_type'];
  settings_entity_type_delete($setting_type);

  watchdog('settings_entity_type', '@type: deleted %title.', array('@type' => $setting_type->type, '%title' => $setting_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $setting_type->type, '%title' => $setting_type->label)));

  $form_state['redirect'] = 'admin/structure/settings-entity-types';
}

/**
 * Page to select settings entity type to add new entity.
 */
function settings_entity_admin_add_page() {
  $items = array();
  foreach (settings_entity_types() as $setting_type_key => $setting_type) {
    $items[] = l(entity_label('settings_entity_type', $setting_type), 'admin/content/setting-entities/add/' . $setting_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of setting to create.')));
}

/**
 * Add new settings entity page callback.
 */
function settings_entity_add($type) {
  $setting_type = settings_entity_types($type);

  $setting_entity = entity_create('settings_entity', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('settings_entity_type', $setting_type))));

  $output = drupal_get_form('settings_entity_form', $setting_entity);

  return $output;
}

/**
 * Settings entity Form.
 */
function settings_entity_form($form, &$form_state, $settings_entity) {
  $form_state['settings_entity'] = $settings_entity;

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Name'),
    '#default_value' => $settings_entity->title,
  );

  $form['active'] = array(
    '#title' => t('Active'),
    '#type' => 'checkbox',
    '#default_value' => $settings_entity->active,
    '#description' => t('Wheter this setting is active or not.'),
  );

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $settings_entity->uid,
  );

  field_attach_form('settings_entity', $settings_entity, $form, $form_state);

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings entity'),
    '#submit' => $submit + array('settings_entity_form_submit'),
  );

  // Show Delete button if we edit entity.
  $settings_entity_id = entity_id('settings_entity', $settings_entity);
  if (!empty($settings_entity_id) && settings_entity_access('delete', $settings_entity)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('settings_entity_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'settings_entity_form_validate';

  return $form;
}

/**
 * Settings entity validate handler.
 */
function settings_entity_form_validate($form, &$form_state) {
  $entity = $form_state['settings_entity'];
  if ($form_state['values']['active']) {

    // If we have an active settings entity in that bundle.
    $active = setting_entities_get_active_type_settings($entity->type);

    $message = 'An active <a href="@edit_url">entity</a> for bundle: "@bundle"
already exists. Remove the active state from an existing entity or don\'t save
this entity as an active one.';


    if ($active && $entity->identifier() != $active->identifier()) {
      $uri = $active->uri();
      form_error($form['actions'], t($message, array(
          '@edit_url' => url($uri['path'] . '/edit'),
          '@bundle' => $entity->type,
        )
      ));
    }

  }
}

/**
 * Settings entity submit handler.
 */
function settings_entity_form_submit($form, &$form_state) {
  $settings_entity = $form_state['settings_entity'];

  entity_form_submit_build_entity('settings_entity', $settings_entity, $form, $form_state);

  settings_entity_save($settings_entity);

  $form_state['redirect'] = setting_entities_get_entity_list_uri();

  drupal_set_message(t('Settings entity %title saved.', array('%title' => entity_label('settings_entity', $settings_entity))));
}

function settings_entity_form_submit_delete($form, &$form_state) {
  $settings_entity = $form_state['settings_entity'];
  $settings_entity_uri = entity_uri('settings_entity', $settings_entity);
  $form_state['redirect'] = $settings_entity_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function settings_entity_delete_form($form, &$form_state, $settings_entity) {
  $form_state['settings_entity'] = $settings_entity;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['settings_entity_type_id'] = array('#type' => 'value', '#value' => entity_id('settings_entity' ,$settings_entity));
  $settings_entity_uri = entity_uri('settings_entity', $settings_entity);
  return confirm_form($form,
    t('Are you sure you want to delete settings entity %title?', array('%title' => entity_label('settings_entity', $settings_entity))),
    $settings_entity_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function settings_entity_delete_form_submit($form, &$form_state) {
  $settings_entity = $form_state['settings_entity'];
  settings_entity_delete($settings_entity);

  drupal_set_message(t('Settings entity %title deleted.', array('%title' => entity_label('settings_entity', $settings_entity))));

  $form_state['redirect'] = '<front>';
}
