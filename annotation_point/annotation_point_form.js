// $Id$

Drupal.behaviors.annotation_point_form = function (context) {
  $('.annotation-point-form:not(.annotation-point-form-processed)').each(function () {
    $(this).addClass('annotation-point-form-processed');
    var valfield = $(this).find('.annotation-point-vars');
    if (valfield.val()) {
      var specs = valfield.val().split('|');
      var selector = specs[0];
      var posx = specs[1];
      var posy = specs[2];
      if ($(selector).css('position') == 'static') {
        $(selector).css('position', 'relative');
      }

      var a = $(this);
      $(selector).append($(this));
      $(this).hide();
      $(this).find('.adder').bind('click', function (e) {
        a.toggle();
        var t_new = $(this).attr('rel');
        var t_cur = $(this).children('a').html();
        //$(this).toggle();
        $(this).children('a').html(t_new);
        $(this).attr('rel', t_cur);
      }
      );
      $(selector).before($(this).find('.adder'));
      // Set form position.
      $(this).css('position', 'absolute');
      $(this).css('top', posx);
      $(this).css('left', posy);
      //alert(selector);

      $(this).children('div').addClass('inner');
      $(this).prepend($(this).find('.dragger'));

      //Make form draggable
      if (jQuery.isFunction($(this).draggable)) {
        $(this).draggable({ cursorAt: { left: 5 }, handle: $(this).children('div.dragger'), containment: selector, opacity: 0.3 });
        $(this).addClass('draggable-processed');
      }

      $(this).bind('dragstop', function(event, ui) {
        var vars = selector + '|' + $(this).css('left') + '|' + $(this).css('top');
        valfield.val(vars);
      });
    }
  }
  );

}
