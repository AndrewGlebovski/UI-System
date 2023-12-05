/**
 * \file
 * \brief Contains implementation of TestTool
*/


#include "plugins/test_tool.hpp"


// ============================================================================


extern "C" Plugin *loadPlugin() {
    return new TestTool();
}


// ============================================================================


TestTool::TestTool() :
    my_data(), prev_position() {}


void TestTool::onMainButton(const ControlState &state, const Vec2d &mouse) {
    switch (state.state) {
        case State::Pressed:
            is_drawing = true;
            prev_position = mouse;
            break;
        case State::Released:
            is_drawing = false;
        default:
            break;
    }
}


void TestTool::onMove(const Vec2d &mouse) {
    if (is_drawing) {
        VertexArray array(Lines, 2);

        array[0] = Vertex(prev_position, color_palette->getFGColor());
        array[1] = Vertex(mouse, color_palette->getFGColor());

        prev_position = mouse;
        canvas->draw(array);
    }
}


const PluginData *TestTool::getPluginData() const {
    return static_cast<const PluginData*>(&my_data);
}
