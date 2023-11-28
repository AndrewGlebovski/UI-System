/**
 * \file
 * \brief Contains BasicTool and BasicFilter implementation
*/


#include "canvas/plugin.hpp"


// ============================================================================


BasicTool::BasicTool() :
    is_drawing(false), canvas(nullptr), color_palette(nullptr), ref_count(0) {}


void BasicTool::setColorPalette(ColorPalette &color_palette_) { color_palette = &color_palette_; }


void BasicTool::setActiveCanvas(Canvas &canvas_) { onCancel(); canvas = &canvas_; }


void BasicTool::onSecondaryButton(ControlState state, const Vec2d &mouse)  {}


void BasicTool::onModifier1(ControlState state) {}


void BasicTool::onModifier2(ControlState state) {}


void BasicTool::onModifier3(ControlState state) {}


void BasicTool::onMove(const Vec2d &mouse) {}


void BasicTool::onConfirm() { is_drawing = false; }


void BasicTool::onCancel() { is_drawing = false; }


Widget *BasicTool::getWidget() { return nullptr; }


Plugin *BasicTool::tryGetInterface(PluginGuid guid) {
    Plugin *result = nullptr;

    if (guid == PluginGuid::Plugin)
        result = static_cast<Plugin*>(this);
    else if (guid == PluginGuid::Tool)
        result = static_cast<Tool*>(this);

    if (result) result->addRef();

    return result;
}


void BasicTool::addRef() {
    ref_count++;
}


void BasicTool::release() {
    ref_count--;
    if (ref_count == 0) delete this;
}


const PluginData *BasicTool::getPluginData() const { return nullptr; }


// ============================================================================


BasicFilter::BasicFilter() : ref_count(0) {}


Widget *BasicFilter::getWidget() { return nullptr; }


Plugin *BasicFilter::tryGetInterface(PluginGuid guid) {
    Plugin *result = nullptr;

    if (guid == PluginGuid::Plugin)
        result = static_cast<Plugin*>(this);
    else if (guid == PluginGuid::Filter)
        result = static_cast<Filter*>(this);

    if (result) result->addRef();

    return result;
}


void BasicFilter::addRef() {
    ref_count++;
}


void BasicFilter::release() {
    ref_count--;
    if (ref_count == 0) delete this;
}


const PluginData *BasicFilter::getPluginData() const { return nullptr; }
