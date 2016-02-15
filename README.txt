To create setting type enable the setting_entities_admin_ui module and clear site cache

Go to /admin/structure/settings-entity-types and add a settings type
Now go to /admin/content/setting-entities/add to add an actual settings entity

In order to use the settings within your code there are two functions:


* setting_entities_get_active_settings()
Will return an array of all active entities keyed with the settings type


* setting_entities_get_active_type_settings($type, $load = TRUE)
This will load the active settings entity instance within the given type
The type is the entity type machine name.


This module is aimed for developers who need an easy way to create and manage their settings and then provide usable interface for the client to use.

Get the setting entity with the setting_entities_get_active_type_settings() and use $entity->wrapper()->field_my_field->value(); to get the value you are interesed in. Or just use Entity API without wrappers. Learn more about entity metadata wrappers


Usage example

$setting = setting_entities_get_active_type_settings('my_custom_settings');
$my_setting_value = $setting->wrapper()->field_my_custom_setting_field->value();
print render($my_setting_value);

Note that there can only be one active entity per type. You can add more entities to the type but only one of them can be active at a time. The entity instance being currently active is configurable for client.


It provides 2 submodules - "admin ui" and "client ui". In order to add setting entity types you need to enable the admin UI submodule. The client UI submodule provides a nice interface for the end user to fill. It can be found under "Settings dashboard" menu link under "Configuration".
