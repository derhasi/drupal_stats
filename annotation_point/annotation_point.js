// $Id$

Drupal.behaviors.annotation_point = function (context) {
  var ap = $('div.annotation-point:not(.annotation-point-processed)');
  ap.addClass('annotation-point-processed');
  var specs = ap.attr('rel').split('|');
  var selector = specs[0];
  var posx = specs[1];
  var posy = specs[2];
  if ($(selector).css('position') == 'static') {
    $(selector).css('position', 'relative');
  }

  $(selector).append(ap);
  ap.css('position', 'absolute');
  ap.css('left', posx);
  ap.css('top', posy);

  // Hide children, display them on hover.
  ap.children().hide();
  ap.bind("mouseenter mouseleave", function(e){
      $(this).children(':not(.force-visible)').toggle();
  });
}