/**
 * \file
 * \brief Contains implementation of PaletteManager
*/


#include "canvas/palettes/palette_manager.hpp"


// ============================================================================


plug::ColorPalette &PaletteManager::getColorPalette() {
    ASSERT(color_palette, "plug::Color palette is nullptr!\n");
    return *color_palette;
}


ToolPalette &PaletteManager::getToolPalette() {
    ASSERT(tool_palette, "Tool palette is nullptr!\n");
    return *tool_palette;
}

FilterPalette &PaletteManager::getFilterPalette() {
    ASSERT(filter_palette, "plug::Filter palette is nullptr!\n");
    return *filter_palette;
}


void PaletteManager::setColorPalette(plug::ColorPalette *color_palette_) {
    ASSERT(color_palette_, "plug::Color palette is nullptr!\n");
    color_palette = color_palette_;

    if (tool_palette) getToolPalette().setColorPalette(getColorPalette());
}


void PaletteManager::setToolPalette(ToolPalette *tool_palette_) {
    ASSERT(tool_palette_, "Tool palette is nullptr!\n");
    tool_palette = tool_palette_;

    if (color_palette) getToolPalette().setColorPalette(getColorPalette());
}


void PaletteManager::setFilterPalette(FilterPalette *filter_palette_) {
    ASSERT(filter_palette_, "plug::Filter palette is nullptr!\n");
    filter_palette = filter_palette_;
}


PaletteManager &PaletteManager::getInstance() {
    static PaletteManager palette_manager;
    return palette_manager;
}


PaletteManager::~PaletteManager() {
    ASSERT(color_palette, "plug::Color palette is nullptr!\n");
    delete color_palette;

    ASSERT(tool_palette, "Tool palette is nullptr!\n");
    delete tool_palette;

    ASSERT(filter_palette, "plug::Filter palette is nullptr!\n");
    delete filter_palette;
}


PaletteManager::PaletteManager() :
    color_palette(nullptr), tool_palette(nullptr), filter_palette(nullptr) {}
