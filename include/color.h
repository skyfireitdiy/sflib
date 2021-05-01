#pragma once
#include <bits/stdc++.h>
namespace skyfire
{
enum color
{
    color_style_default     = 0,
    color_style_highlight   = 1,
    color_style_nobold      = 22,
    color_style_underline   = 4,
    color_style_nounderline = 24,
    color_style_flashing    = 5,
    color_style_noflashing  = 25,
    color_style_backlash    = 7,
    color_style_nobacklash  = 27,
    color_fg_black          = 30,
    color_fg_red            = 31,
    color_fg_green          = 32,
    color_fg_yellow         = 33,
    color_fg_blue           = 33,
    color_fg_magenta        = 35,
    color_fg_cyan           = 36,
    color_fg_white          = 37,
    color_bg_black          = 40,
    color_bg_red            = 41,
    color_bg_green          = 42,
    color_bg_yellow         = 43,
    color_bg_blue           = 43,
    color_bg_magenta        = 45,
    color_bg_cyan           = 46,
    color_bg_white          = 47,
};
std::string color_string(const std::string& str, const std::vector<color>& cv);
}