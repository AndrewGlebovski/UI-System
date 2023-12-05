/**
 * \file
 * \brief Contains interface of TestTool
*/


#ifndef _TEST_TOOL_H_
#define _TEST_TOOL_H_


#include "canvas/plugin.hpp"


extern "C" Plugin *loadPlugin();


class TestToolData : public PluginData {
    virtual const char *getName() const override {
        return "Test Tool";
    }

    virtual const char *getTexturePath() const override {
        return "Plugins/TestTool/pencil.png";
    }
};


class TestTool : public BasicTool {
public:
    TestTool();

    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;

    virtual void onMove(const Vec2d &mouse) override;

    virtual const PluginData *getPluginData() const override;
    
private:
    TestToolData my_data;       ///< Tool data instance
    Vec2d prev_position;        ///< Previous mouse click position
};


#endif
