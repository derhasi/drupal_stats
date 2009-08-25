

Drupal.behaviors.annotation_marker = function (context) {
	var am = $("div[rel^='annomark']");
	am.addClass('annotation-marker');
	var specs = am.attr('rel').match(/\[(.*?)\]/)[1].split('|');
	var selector = specs[0];
	
  //Add container
	$(selector + ':not(.am-container-processed)').prepend("<div class='am-container></div>").addClass('am-container-processed');
	$(selector + ' .am-container').width('100%').height('100%').css('position','absolute').append(am);
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
  
	am.html(x + ":" + y);
	am.css('left', x + 'px');
	am.css('top', y + 'px');
	if (w) {
		am.css('width', w + 'px');
	}
	if (h) {
		am.css('height', h + 'px');
	}
	
  // Draggable annotation marker
	var dam = $("div.draggable[rel^='annomark']:not(.draggable-processed)");
	am.draggable({ containment: selector});
	am.addClass('draggable-processed');

};