#pragma once
#include "sf_stdc++.h"

namespace skyfire {

enum sf_color_value {
    sf_color_style_default = 0,
    sf_color_style_highlight = 1,
    sf_color_style_nobold = 22,
    sf_color_style_underline = 4,
    sf_color_style_nounderline = 24,
    sf_color_style_flashing = 5,
    sf_color_style_noflashing = 25,
    sf_color_style_backlash = 7,
    sf_color_style_nobacklash = 27,

    sf_color_fg_black = 30,
    sf_color_fg_red = 31,
    sf_color_fg_green = 32,
    sf_color_fg_yellow = 33,
    sf_color_fg_blue = 33,
    sf_color_fg_magenta = 35,
    sf_color_fg_cyan = 36,
    sf_color_fg_white = 37,

    sf_color_bg_black = 40,
    sf_color_bg_red = 41,
    sf_color_bg_green = 42,
    sf_color_bg_yellow = 43,
    sf_color_bg_blue = 43,
    sf_color_bg_magenta = 45,
    sf_color_bg_cyan = 46,
    sf_color_bg_white = 47,
};

std::string sf_colored_string(const std::string& str, const std::vector<sf_color_value>& cv);

}