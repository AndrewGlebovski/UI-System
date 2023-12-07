/**
 * \file
 * \brief Contains implementation of TestTool
*/


#include "plugins/test_tool.hpp"
#include "common/utils.hpp"


// ============================================================================


extern "C" plug::Plugin *loadPlugin() {
    return new TestTool();
}


// ============================================================================


TestTool::TestTool() :
    my_data(), prev_position() {}


void TestTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    switch (state.state) {
        case plug::State::Pressed:
            is_drawing = true;
            prev_position = mouse;
            break;
        case plug::State::Released:
            is_drawing = false;
        default:
            break;
    }
}


void TestTool::onMove(const plug::Vec2d &mouse) {
    if (is_drawing) {
        plug::VertexArray array(plug::Lines, 2);

        array[0] = plug::Vertex(prev_position, color_palette->getFGColor());
        array[1] = plug::Vertex(mouse, color_palette->getFGColor());

        prev_position = mouse;
        canvas->draw(array);
    }
}


const plug::PluginData *TestTool::getPluginData() const {
    return static_cast<const plug::PluginData*>(&my_data);
}
