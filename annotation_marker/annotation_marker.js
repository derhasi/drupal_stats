

Drupal.behaviors.annotation_marker = function (context) {
  var am = $("div[rel^='annotation-marker']");
  am.addClass('annotation-marker');
  var specs = am.attr('rel').match(/\[(.*?)\]/)[1].split('|');
  var selector = specs[0];
  //Add container
  $(selector + ':not(.am-container-processed)').prepend("<div class='am-container></div>").addClass('am-container-processed');
  $(selector + ' .am-container').width($(selector).width()).height($(selector).height()).css('position','absolute').append(am);
  am.addClass('am-appended');
  am.css('position','absolute');
  // Specify position
  // modify x and w when container width spec is given
  if (specs[3]) {
    var sw = $(selector).width();
    alert('sw' + sw);
    var x = (specs[1] * sw / specs[3]);
    if (specs[5]) {
      w = (specs[5] * sw / specs[3]);
    }
  }
  else {
    x = specs[1];
    w = specs[5];
  }
  // modify y and h when container height spec is given
  if (specs[4]) {
    var sh = $(selector).height();
    var y = (specs[2] * sh / specs[4]);
    if (specs[6]) {
      h = (specs[6] * sh / specs[4]);
    }
    alert('sh' + sh + ':y' + y + ':h' + h);
  }
  else {
    y = specs[2];
    h = specs[6];
  }

  //am.html(x + ":" + y);
  am.css('left', x + 'px');
  am.css('top', y + 'px');
  if (w) {
    am.css('width', w + 'px');
  }
  if (h) {
    am.css('height', h + 'px');
  }

  // Draggable annotation marker
  var sub_am = $("div.draggable[rel^='annotation-marker']:not(.draggable-processed)");
  if (jQuery.isFunction(sub_am.draggable)) {
    sub_am.draggable({ containment: selector});
    sub_am.addClass('draggable-processed');
  }

  // resizable annotation marker
  var sub_am = $("div.resizable[rel^='annotation-marker']:not(.resizable-processed)");
  if (jQuery.isFunction(sub_am.resizable)) {
    sub_am.resizable({ containment: selector});
    sub_am.addClass('resizable-processed');
  }

  // Input handling
  var sub_am = $("div.input[rel^='annotation-marker']:not(.input-processed)");
  sub_am.bind('dragstop', function(event, ui) {
    var id = "#" + $(this).attr("id");
    //alert("#" + ui.id() + '-posx');
    $(id + '-posx').val($(this).css('left'));
    $(id + '-posy').val($(this).css('top'));
    $(id + '-container-width').val($(selector).width());
    $(id + '-container-height').val($(selector).height());
    $(id + '-container-selector').val(selector);
  });
  sub_am.bind('resizestop', function(event, ui) {
    var id = "#" + $(this).attr("id");
    $(id + '-width').val($(this).css('width'));
    $(id + '-height').attr($(this).css('height'));
    $(id + '-container-width').val($(selector).css('width'));
    $(id + '-container-height').val($(selector).css('height'));
  });
  sub_am.addClass('input-processed');

  // Toggle
  var at = $('.annotation-toggle:not(.annotation-toggle-processed)');
  at.addClass('annotation-toggle-processed');
  at.bind('click', function(e) {
    var rel = $(this).attr('rel').split('|');
    var cur = $(this).html();
    $(rel[0]).toggle();
    $(this).html(rel[1]).attr('rel', rel[0] + '|' + cur);
  }
  );
};