<?php
// $Id$

/**
 * @file annotation_user.tpl.php
 *
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $annotation_user: user that is annotated.
 * - $annotation: annotation object
 * - $delete_button: Node body or teaser depending on $teaser flag.
 * - $annotator: user that annotated
 * - $annotator_name: name of the annotator
 * - $annotator_link: link to the annotator profile
 * - $content_name: name of the annotated user
 * - $content_link: link to the annotated user profile
 * - $annotated_by: format with annotator
 * - $annotation_date: date it was annotated
 *
 * Use hook_preprocess_annotation_user() to add additional variables.
 *
 */

?>

<div class="annotation annotation-<?php print $annotation->aid; ?>">
  <div><?php print $content_link; ?></div>
  <div><?php print $annotated_by; ?></div>
  <div><?php print $annotation_date; ?></div>
  <div><?php print $delete_button; ?></div>
</div>