/**
 * \file
 * \brief Contains BasicTool and BasicFilter implementation
*/


#include "canvas/plugin.hpp"


// ============================================================================


BasicTool::BasicTool() :
    is_drawing(false), canvas(nullptr), color_palette(nullptr), ref_count(1) {}


void BasicTool::setColorPalette(plug::ColorPalette &color_palette_) { color_palette = &color_palette_; }


void BasicTool::setActiveCanvas(plug::Canvas &canvas_) { onCancel(); canvas = &canvas_; }


void BasicTool::onSecondaryButton(const plug::ControlState &state, const plug::Vec2d &mouse)  {}


void BasicTool::onModifier1(const plug::ControlState &state) {}


void BasicTool::onModifier2(const plug::ControlState &state) {}


void BasicTool::onModifier3(const plug::ControlState &state) {}


void BasicTool::onMove(const plug::Vec2d &mouse) {}


void BasicTool::onConfirm() { is_drawing = false; }


void BasicTool::onCancel() { is_drawing = false; }


plug::Widget *BasicTool::getWidget() { return nullptr; }


plug::Plugin *BasicTool::tryGetInterface(size_t guid) {
    plug::Plugin *result = nullptr;

    if (static_cast<plug::PluginGuid>(guid) == plug::PluginGuid::Plugin)
        result = static_cast<plug::Plugin*>(this);
    else if (static_cast<plug::PluginGuid>(guid) == plug::PluginGuid::Tool)
        result = static_cast<Tool*>(this);

    if (result) result->addReference();

    return result;
}


void BasicTool::addReference() {
    ref_count++;
}


void BasicTool::release() {
    ref_count--;
    if (ref_count == 0) delete this;
}


const plug::PluginData *BasicTool::getPluginData() const { return nullptr; }


// ============================================================================


BasicFilter::BasicFilter() : ref_count(1) {}


plug::Widget *BasicFilter::getWidget() { return nullptr; }


plug::Plugin *BasicFilter::tryGetInterface(size_t guid) {
    plug::Plugin *result = nullptr;

    if (static_cast<plug::PluginGuid>(guid) == plug::PluginGuid::Plugin)
        result = static_cast<plug::Plugin*>(this);
    else if (static_cast<plug::PluginGuid>(guid) == plug::PluginGuid::Filter)
        result = static_cast<plug::Filter*>(this);

    if (result) result->addReference();

    return result;
}


void BasicFilter::addReference() {
    ref_count++;
}


void BasicFilter::release() {
    ref_count--;
    if (ref_count == 0) delete this;
}


const plug::PluginData *BasicFilter::getPluginData() const { return nullptr; }
