
Drupal.behaviors.annotation_cs = function (context) {
  var sets = Drupal.settings.annotation_cs_control;

  // Anchor for JS Buttons
  $(sets.selector).prepend('<a name="annotation-cs"></a>');

  // Default state ADD
  if (sets.adder_default == 'hide') {
    $(sets.selector).find('.annotation-point-form').show();
  }
  else {
    $(sets.selector).find('.annotation-point-form').hide();
  }

  // Default state VIEW
  if (sets.viewer_default == 'hide') {
    $(sets.selector).find('.annotation-point').show();
  }
  else {
    $(sets.selector).find('.annotation-point').hide();
  }

  // Event handling on click
  $('ul.annotation-cs-control:not(.annotation-cs-control-processed)').each(function () {
    var list = $(this);
    list.addClass('annotation-cs-control-processed');
    // ADDER
    list.find('.adder a').bind('click', function (e) {
      var rel = $(this).attr('rel');
      if (rel == 'show') {
        $(sets.selector).find('.annotation-point-form').show();
        $(this).attr('rel', 'hide');
        $(this).html(sets.adder_hide);
      }
      else {
        $(sets.selector).find('.annotation-point-form').hide();
        $(this).attr('rel', 'show');
        $(this).html(sets.adder_show);
      }
    });
    // VIEWER
    list.find('.viewer a').bind('click', function (e) {
      var rel = $(this).attr('rel');
      if (rel == 'show') {
        $(sets.selector).find('.annotation-point').show();
        $(this).attr('rel', 'hide');
        $(this).html(sets.viewer_hide);
      }
      else {
        $(sets.selector).find('.annotation-point').hide();
        $(this).attr('rel', 'show');
        $(this).html(sets.viewer_show);
      }
    });
  });
}