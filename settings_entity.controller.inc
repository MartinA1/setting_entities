<?php


class SettingsEntityController extends EntityAPIController {

  public function create(array $values = array()) {
    global $user;
    $values += array(
      'title' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'uid' => $user->uid,
      'active' => 0,
    );
    return parent::create($values);
  }

  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $wrapper = entity_metadata_wrapper('settings_entity', $entity);
    $content['author'] = array('#markup' => t('Created by: !author', array('!author' => $wrapper->uid->name->value(array('sanitize' => TRUE)))));

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }
}

 class SettingsEntityTypeController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
     $values += array(
       'label' => '',
     );
     return parent::create($values);
   }

   /**
    * Save Setting Entity Type.
    */
   public function save($entity, DatabaseTransaction $transaction = NULL) {
     parent::save($entity, $transaction);
     // Rebuild menu registry. We do not call menu_rebuild directly, but set
     // variable that indicates rebuild in the end.
     // @see http://drupal.org/node/1399618
     variable_set('menu_rebuild_needed', TRUE);
   }
 }

/**
 * UI controller for Settings Entity Type.
 */
class SettingsEntityTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Setting types.';
    return $items;
  }
}

/**
 * Settings Entity class.
 */
class SettingsEntity extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'admin/content/setting-entities/' . $this->identifier());
  }
}

/**
 * Settings Entity Type class.
 */
class SettingsEntityType extends Entity {
  public $type;
  public $label;
  public $weight = 0;

  public function __construct($values = array()) {
    parent::__construct($values, 'settings_entity_type');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}
