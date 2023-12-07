/**
 * \file
 * \brief Contains interface for plug::Plugin, Tool and plug::Filter
*/


#ifndef _PLUGIN_H_
#define _PLUGIN_H_


#include "widget/widget.hpp"
#include "canvas/canvas.hpp"
#include "standart/Tool/Tool.h"
#include "standart/Filter.h"


/// plug::Plugin loader function type
typedef plug::Plugin *(*load_plugin_t)();


/// Implements some functions from Tool interface
class BasicTool : public plug::Tool {
public:
    BasicTool();

    BasicTool(const BasicTool&) = default;
    
    BasicTool &operator = (const BasicTool&) = default;

    virtual void setColorPalette(plug::ColorPalette &color_palette_) override;
    
    virtual void setActiveCanvas(plug::Canvas &canvas_) override;

    virtual void onSecondaryButton(const plug::ControlState &state, const plug::Vec2d &mouse) override;

    virtual void onModifier1(const plug::ControlState &state) override;
    
    virtual void onModifier2(const plug::ControlState &state) override;
    
    virtual void onModifier3(const plug::ControlState &state) override;
    
    virtual void onMove(const plug::Vec2d &mouse) override;
    
    virtual void onConfirm() override;
    
    virtual void onCancel() override;
    
    virtual plug::Widget *getWidget() override;

    virtual plug::Plugin *tryGetInterface(size_t guid) override;

    virtual void addReference() override;

    virtual void release() override;

    virtual const plug::PluginData *getPluginData() const override;

protected:
    bool is_drawing;                ///< Tool is in drawing mode
    plug::Canvas *canvas;                 ///< plug::Canvas to draw on
    plug::ColorPalette *color_palette;    ///< plug::Color palette
    size_t ref_count;               ///< Count reference to plugin
};


/// Implement some functions from plug::Filter interface
class BasicFilter : public plug::Filter {
public:
    BasicFilter();

    virtual plug::Widget *getWidget() override;

    virtual plug::Plugin *tryGetInterface(size_t guid) override;

    virtual void addReference() override;

    virtual void release() override;

    virtual const plug::PluginData *getPluginData() const override;

protected:
    size_t ref_count;               ///< Count reference to plugin
};


#endif
