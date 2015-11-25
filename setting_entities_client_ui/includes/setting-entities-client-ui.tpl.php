<div id="setting-entities-client-ui">
  <div id="setting-entities-tabs">
    <ul>
      <?php foreach ($tabs as $tab): ?>
        <li <?php print drupal_attributes($tab['attributes']); ?>><strong><?php print l($tab['title'], $tab['path']); ?></strong></li>
      <?php endforeach; ?>
    </ul>
  </div>
  <div id="setting-entities-content">
    <?php print render($content_array); ?>
  </div>
</div>