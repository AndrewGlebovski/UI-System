/**
 * \file
 * \brief Contains interface for Plugin, Tool and Filter
*/


#ifndef _PLUGIN_H_
#define _PLUGIN_H_


#include "widget/widget.hpp"
#include "canvas/canvas.hpp"
#include "canvas/palettes/color_palette.hpp"


/// Inteface id
enum class PluginGuid {
    Plugin  = 0,
    Tool    = 1,
    Filter  = 2
};


/// Resource data for plugin
class PluginData {
public:
    virtual const char *getName() const = 0;

    virtual const char *getTexturePath() const = 0;

    virtual ~PluginData() = default;
};


/// Plugin interface
class Plugin {
public:
    virtual Plugin *tryGetInterface(PluginGuid guid) = 0;

    virtual void addRef() = 0;

    virtual void release() = 0;

    virtual const PluginData *getPluginData() const = 0;

    virtual ~Plugin() = default;
};


/// Plugin loader function type
typedef Plugin *(*load_plugin_t)();


/// Key state
enum class State {
    Pressed,
    Released,
};


/// Button state used by tool
struct ControlState {
    ControlState(State state_) : state(state_) {}

    State state;
};


/// Tool interface
class Tool : public Plugin {
public:
    virtual void setColorPalette(ColorPalette &color_palette_) = 0;
    
    virtual void setActiveCanvas(Canvas &canvas_) = 0;

    virtual void onMainButton(ControlState state, const Vec2d &mouse) = 0;
    
    virtual void onSecondaryButton(ControlState state, const Vec2d &mouse) = 0;
    
    virtual void onModifier1(ControlState state) = 0;
    
    virtual void onModifier2(ControlState state) = 0;
    
    virtual void onModifier3(ControlState state) = 0;
    
    virtual void onMove(const Vec2d &mouse) = 0;
    
    virtual void onConfirm() = 0;
    
    virtual void onCancel() = 0;
    
    virtual WidgetInterface *getWidget() = 0;
};


/// Filter interface
class Filter : public Plugin {
public:
    virtual void applyFilter(Canvas &canvas) const = 0;

    virtual WidgetInterface *getWidget() = 0;
};


/// Implements some functions from Tool interface
class BasicTool : public Tool {
public:
    BasicTool();

    BasicTool(const BasicTool&) = default;
    
    BasicTool &operator = (const BasicTool&) = default;

    virtual void setColorPalette(ColorPalette &color_palette_) override;
    
    virtual void setActiveCanvas(Canvas &canvas_) override;

    virtual void onSecondaryButton(ControlState state, const Vec2d &mouse) override;

    virtual void onModifier1(ControlState state) override;
    
    virtual void onModifier2(ControlState state) override;
    
    virtual void onModifier3(ControlState state) override;
    
    virtual void onMove(const Vec2d &mouse) override;
    
    virtual void onConfirm() override;
    
    virtual void onCancel() override;
    
    virtual WidgetInterface *getWidget() override;

    virtual Plugin *tryGetInterface(PluginGuid guid) override;

    virtual void addRef() override;

    virtual void release() override;

    virtual const PluginData *getPluginData() const override;

protected:
    bool is_drawing;                ///< Tool is in drawing mode
    Canvas *canvas;                 ///< Canvas to draw on
    ColorPalette *color_palette;    ///< Color palette
    size_t ref_count;               ///< Count reference to plugin
};


/// Implement some functions from Filter interface
class BasicFilter : public Filter {
public:
    BasicFilter();

    virtual WidgetInterface *getWidget() override;

    virtual Plugin *tryGetInterface(PluginGuid guid) override;

    virtual void addRef() override;

    virtual void release() override;

    virtual const PluginData *getPluginData() const override;

protected:
    size_t ref_count;               ///< Count reference to plugin
};


#endif
