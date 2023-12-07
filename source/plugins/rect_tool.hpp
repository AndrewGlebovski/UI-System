/**
 * \file
 * \brief Contains TestRectTool interface
*/


#ifndef _RECT_TOOL_H_
#define _RECT_TOOL_H_


#include "canvas/plugin.hpp"


extern "C" plug::Plugin *loadPlugin();


class TestRectToolData : public plug::PluginData {
    virtual const char *getName() const override {
        return "Rect Tool";
    }

    virtual const char *getTexturePath() const override {
        return "Plugins/RectTool/rect.png";
    }
};


class TestRectTool : public BasicTool {
public:
    TestRectTool();

    TestRectTool(const TestRectTool &rect_tool) = delete;
    
    TestRectTool &operator = (const TestRectTool &rect_tool) = delete;

    virtual void onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) override;
    
    virtual void onMove(const plug::Vec2d &mouse) override;
    
    virtual void onConfirm() override;
    
    virtual plug::Widget *getWidget() override;

    virtual const plug::PluginData *getPluginData() const override;

    virtual ~TestRectTool() override;

protected:
    RectShape createRect(const plug::Vec2d &p1, const plug::Vec2d &p2) const;

    plug::Vec2d draw_start;               ///< Previous mouse click position
    plug::Widget *rect_preview;  ///< Widget that draws preview of the rectangle
    TestRectToolData my_data;
};


#endif
