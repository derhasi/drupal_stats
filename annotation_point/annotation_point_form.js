// $Id$

Drupal.behaviors.annotation_point_form = function (context) {
  var af = $('.annotation-point-form:not(.annotation-point-form-processed)');
  af.addClass('annotation-point-form-processed');
  var valfield = af.find('input.annotation-point-vars');
  if (valfield.val()) {
    var specs = valfield.val().split('|');
    var selector = specs[0];
    var posx = specs[1];
    var posy = specs[2];
    if ($(selector).css('position') == 'static') {
      $(selector).css('position', 'relative');
    }

    $(selector).append(af);

    // Set form position.
    af.css('position', 'absolute');
    af.css('top', posx);
    af.css('left', posy);

    //Make form draggable
    //if (jQuery.isFunction(af.draggable)) {
      af.draggable({ cursorAt: { left: 5 }, handle: af.find('.dragger'), containment: selector, opacity: 0.3 });
      af.addClass('draggable-processed');
    //}

    af.bind('dragstop', function(event, ui) {
      var vars = selector + '|' + $(this).css('left') + '|' + $(this).css('top');
      valfield.val(vars);
    });
  }

}
