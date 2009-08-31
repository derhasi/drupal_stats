// $Id$

Drupal.behaviors.annotation_point = function (context) {
  $('div.annotation-point:not(.annotation-point-processed)').each(function () {
    $(this).addClass('annotation-point-processed');
    var specs = $(this).attr('rel').split('|');
    var selector = specs[0];
    var posx = specs[1];
    var posy = specs[2];
    if ($(selector).css('position') == 'static') {
      $(selector).css('position', 'relative');
    }

    $(selector).append($(this));
    $(this).css('position', 'absolute');
    $(this).css('left', posx);
    $(this).css('top', posy);

    // Hide children, display them on hover.
    $(this).children().hide();
    $(this).bind("mouseenter", function(e) {
        $(this).children(':not(.force-visible)').show();
    });
    $(this).bind("mouseleave", function(e){
        $(this).children(':not(.force-visible)').hide();
    });
  });

}