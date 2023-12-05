/**
 * \file
 * \brief Contains TestRectTool implementation
*/


#include "plugins/rect_tool.hpp"


// ============================================================================


extern "C" Plugin *loadPlugin() {
    return new TestRectTool();
}


// ============================================================================


class TestRectPreview : public Widget {
private:
    TestRectTool &tool;

public:
    TestRectPreview(TestRectTool &tool_) :
        Widget(1, LazyLayoutBox()), tool(tool_) {}
    

    virtual void draw(TransformStack &stack, RenderTarget &result) override {
        Vec2d global_position = stack.apply(layout->getPosition());
        Vec2d global_size = applySize(stack, layout->getSize());

        RectShape rect(global_position, global_size, Color(0));
        rect.setBorder(1, Blue);
        rect.draw(result);
    }
};


TestRectTool::TestRectTool() :
    draw_start(), rect_preview(nullptr), my_data()
{
    rect_preview = new TestRectPreview(*this);
}


RectShape TestRectTool::createRect(const Vec2d &p1, const Vec2d &p2) const {
    Vec2d position = p1;
    Vec2d size = p2 - p1;
    if (position.x > p2.x) position.x = p2.x;
    if (position.y > p2.y) position.y = p2.y;
    if (size.x < 0) size.x *= -1;
    if (size.y < 0) size.y *= -1;

    return RectShape(position, size, Color());
}


void TestRectTool::onMainButton(const ControlState &state, const Vec2d &mouse) {
    switch (state.state) {
        case State::Pressed:
            is_drawing = true;
            draw_start = mouse;
            rect_preview->getLayoutBox().setPosition(mouse);
            rect_preview->getLayoutBox().setSize(Vec2d());
            break;
        case State::Released:
            onConfirm();
        default:
            break;
    }
}


void TestRectTool::onMove(const Vec2d &mouse) {
    RectShape rect = createRect(draw_start, mouse);
    rect_preview->getLayoutBox().setPosition(rect.getPosition());
    rect_preview->getLayoutBox().setSize(rect.getSize());
}


void TestRectTool::onConfirm() {
    if (is_drawing) {
        RectShape rect(
            rect_preview->getLayoutBox().getPosition(),
            rect_preview->getLayoutBox().getSize(),
            Color(0)
        );
        rect.setBorder(1, color_palette->getFGColor());
        rect.draw(*canvas);

        is_drawing = false;
    }
}


WidgetInterface *TestRectTool::getWidget() {
    return (is_drawing) ? rect_preview : nullptr;
}


TestRectTool::~TestRectTool() {
    delete rect_preview;
}


const PluginData *TestRectTool::getPluginData() const {
    return static_cast<const PluginData*>(&my_data);
}
