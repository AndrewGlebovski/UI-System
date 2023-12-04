/**
 * \file
 * \brief This file contains import constant values
*/


#ifndef _CONFIGS_H_
#define _CONFIGS_H_


#include "common/vector.hpp"


const int SCREEN_W = 1920;                      ///< Screen width in pixels
const int SCREEN_H = 1080;                      ///< Screen height in pixels

// PREDEFINED VALUES FOR WINDOW STYLE

const size_t WINDOW_TITLE_COLOR = 0xffffffff;
const Vec2d WINDOW_TITLE_OFFSET(20, 20);
const int WINDOW_FONT_SIZE = 20;
const float WINDOW_OUTLINE = 10;
const Vec2d WINDOW_TL_OFFSET(15, 60);
const Vec2d WINDOW_BR_OFFSET(15, 15);

// PREDEFINED VALUES FOR SCROLLBAR STYLE

const size_t SCROLLBAR_FRAME_COLOR = 0x000000ff;
const float SCROLLBAR_FRAME_OUTLINE = -1;
const size_t SCROLLBAR_BACKGROUND_COLOR = 0xffffffff;
const size_t SCROLLBAR_SCROLLER_COLOR = 0x000000ff;
const float SCROLLBAR_SCROLLER_FACTOR = 0.1f;

// PREDEFINED VALUES FOR LINE EDIT

const float CURSOR_WIDTH = 2.5;             ///< Cursor width
const float CURSOR_BLINK_TIME = 0.75f;      ///< Time required to passed between cursor blink states
const float TEXT_OFFSET = 5;                ///< Text offset from the LineEdit top-left corner
const float CURSOR_OFFSET = 2;              ///< Cursor position offset from LineEdit top

/// Path to window textures root directory
#define WINDOW_ASSET_DIR "assets/textures/window"

/// Path to palette view textures root directory
#define PALETTE_ASSET_DIR "assets/textures/tools"

/// Path to root directory for plugins
#define PLUGIN_DIR "Plugins"

/// Path to font file
#define FONT_FILE "assets/MainFont.ttf"

/// Path to log file
#define LOG_FILE "log/log.txt"

#endif
