<?php

/**
 * @file
 *  Extended watermark creation
 */

module_load_include('inc', 'imagecache_coloractions', 'watermark');


class dh_watermark extends watermark {
  /**
   * @param $alpha_percent_from
   *   the percentage point for the transparancy level:
   *   0 means fully opague, 100 complete transparent
   */
  function create_watermark($base, $overlay, $x_ins, $y_ins, $alpha = array(100, 100)) {
    // Alpha as array or object
    if (!is_array($alpha) && !is_object($alpha)) {
      $alpha = (object) array('from' => $alpha, 'to' => $alpha);
    }
    elseif (is_array($alpha)) {
      $alpha = (object) $alpha;
    }

    // Base base data
    $base = (object) array(
      'resource' => $base,
      'width' => imagesx($base),
      'height' => imagesy($base),
    );
    // Overlay base data
    $overlay = (object) array(
      'resource' => $overlay,
      'width' => imagesx($overlay),
      'height' => imagesx($overlay),
    );

    // the image to return
    $image = (object) array(
      'resource' => imagecreatetruecolor($base->width, $base->height),
      'width' => $base->width,
      'height' => $base->height,
    );
    imagesavealpha($image->resource, true);
    imagealphablending($image->resource, false);

    // Retrieve the current transparent index in gif images.
    $base_transparent_ix = imagecolortransparent($base->resource);
    if ($base_transparent_ix >= 0) {
      $base->transparent = imagecolorsforindex($base->resource, $base_transparent_ix);
      // Allocate the values to the new image and get new color.
      $base->transparent = imagecolorallocatealpha($image->resource, $base->transparent['red'], $base->transparent['green'], $base->transparent['blue'], $base->transparent['alpha']);
      imagecolortransparent($image->resource, $base->transparent);
      $image->transparent = $base->transparent;
    }

    $overlay_transparent_ix = imagecolortransparent($overlay->resource);
    if ($overlay_transparent_ix >= 0) {
      $overlay->transparent = imagecolorsforindex($overlay->resource, $overlay_transparent_ix);
    }

    // Run each line
    for ($y = 0; $y < $image->height; $y++) {

      // Run each pixel
      for ($x = 0; $x < $image->width; $x++) {

        // pixel BASE
        if(($x < $base->width) && ($y < $base->height)) {
          // try not to go out of bounds when looking for color.
          $base->pixel = imagecolorsforindex($base->resource, imagecolorat($base->resource, $x, $y));
        }
        else {
          $base->pixel = array('red' => 255, 'green' => 255, 'blue' => 255, 'alpha' => 127);
        }
        $overlay->pixel = array(
          'x' => $x - $x_ins,
          'y' => $y - $y_ins,
        );
        //Begin at x_ins and y_ins
        if ($overlay->pixel['x'] > 0 && $overlay->pixel['y'] > 0 &&
            $overlay->pixel['x'] < $overlay->width &&  $overlay->pixel['y'] < $overlay->height) {
          $color_ix = imagecolorat($overlay->resource, $overlay->pixel['x'], $overlay->pixel['y']);
          $overlay->pixel += imagecolorsforindex($watermark_img_obj, $color_ix);

          // Gif indexed color ?
          if ($color_ix == $overlay_transparent_ix) {
            $overlay->pixel['alpha'] = 127;
          }

          $watermark_alpha = round(((127 - $watermark_rbg['alpha']) / 127), 2);
          $watermark_alpha = $watermark_alpha * $alpha_level;

          $avg_red = $this->_get_new_color_value($main_rgb['red'], $watermark_rbg['red'], $main_rgb['alpha'], $watermark_alpha);
          $avg_green = $this->_get_new_color_value($main_rgb['green'], $watermark_rbg['green'], $main_rgb['alpha'], $watermark_alpha);
          $avg_blue = $this->_get_new_color_value($main_rgb['blue'], $watermark_rbg['blue'], $main_rgb['alpha'], $watermark_alpha);

          $new_alpha = $this->_get_new_alpha($main_rgb['alpha'], $watermark_alpha);
          $return_color = $this->_get_image_color($return_img, $avg_red, $avg_green, $avg_blue, $new_alpha);
        }
        else {
            $return_color = imagecolorat($base, $x, $y);
        }
        if ($return_color == $transparent_ix) {
          $return_color = $transparent;
        }
        imagesetpixel($image->resource, $x, $y, $return_color);
      }
    }
    return $return_img;
  }

  // The resulting alpha value
  function _get_new_alpha($alpha_main, $alpha_overlay) {
    return $alpha_overlay + (1 - $alpha_overlay) * $alpha_main;
  }
  // Color value of the given constellation
  function _get_new_color_value($value_main, $value_overlay, $alpha_main, $alpha_overlay) {
    $alpha = $this->_get_new_alpha($alpha_main, $alpha_overlay);
    $value = $value_overlay * $alpha_overlay + $value_main * ($alpha - $alpha_overlay);
    $value /= $alpha;
    return round($value);
  }


  function alpha_for_overlay($alpha, $x, $y, $ins_x, $ins_y, $base, $mode = 'default') {
    switch ($mode) {
      default:
        if ($ins_y > $y) return 1;
        $return = $alpha->from + ($alpha->to - $alpha->from) * ($y - $ins_y) / ($base->height - $ins_y);
        $return /= 100;
        break;
    }
    return $return;
  }

}

/**
 * Image variables with loop indicator
 */
class dh_image {
  var $resource;
  var $width;
  var $heigth;
  // current active pixel
  var $x;
  var $y;
  var $color_index;
  var $rgba;  // alpha: 0 to 127, opague to transparent
  var $alpha; // alpha as float value - 0 to 1, opague to transparent

  // constructor
  function __construct($resource, $x = 0, $y = 0, $width = 100, $height = 100) {
    // Existing resource
    if ($resource) {
      $this->resource = $resource;
    }
    // new blank image
    elseif($height > 0 && $width > 0) {
      $this->resource = imagecreatetruecolor($width, $height);
    }
    else {
      return FALSE;
    }
    $this->width = imagesx($resource);
    $this->heigth = imagesy($resource);
    $this->x = $x;
    $this->y = $y;
    $this->color();
  }

  // Control x-position
  function x_rel($step = 1) {
    if ($this->x + $step < $this->width && $this->x + $step > 0) {
      $this->x += $step;
      $this->color();
      return TRUE;
    }
    else {
      $this->_clear_pixel();
      return FALSE;
    }
  }

  // Control y-position
  function y_rel($step = 1) {
    if ($this->y + $step < $this->height && $this->y + $step > 0) {
      $this->y += $step;
      $this->color();
      return TRUE;
    }
    else {
      $this->_clear_pixel();
      return FALSE;
    }
  }

  // Clear the pixel variables
  function _clear_pixel() {
    // fully transparent black pixel
    $this->rgba = array('red' => 0, 'green' => 0, 'blue' => 0, 'alpha' => 127);
    $this->alpha = NULL;
    $this->color_index = NULL;
  }

  // Get color variables
  function color($refresh = FALSE) {
    if ($refresh || !isset($this->rgba)) {
      if ($this->x > 0 && $this->y > 0 && $this->x < $this->width && $this->y < $this->heigth) {
        $this->color_index = imagecolorat($this->resource, $x, $y);
        $this->rgba = imagecolorsforindex($this->resource, $this->color_index);
        $this->alpha = ($this->rgba['alpha'] / 127);
      }
      else {
        $this->_clear_pixel();
      }
    }
  }
}

class dh_overlay extends watermark {
  /**
   * @param $alpha_percent_from
   *   the percentage point for the transparancy level:
   *   0 means fully opague, 100 complete transparent
   */
  function create_overlay($base_resource, $overlay_resource, $x_ins, $y_ins, $alpha = array(100, 100)) {
    $base = new dh_image($base_resource);
    $overlay = new dh_image($overlay_resource);
    $image = new dh_image(NULL, 0, 0, $base->width, $base->heigth);
    //Alpha handling


  }
}
