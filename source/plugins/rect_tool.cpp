/**
 * \file
 * \brief Contains TestRectTool implementation
*/


#include "plugins/rect_tool.hpp"


// ============================================================================


extern "C" plug::Plugin *loadPlugin() {
    return new TestRectTool();
}


// ============================================================================


class TestRectPreview : public Widget {
private:
    TestRectTool &tool;

public:
    TestRectPreview(TestRectTool &tool_) :
        Widget(1, LazyLayoutBox()), tool(tool_)
    {
        printf("Rect tool preview created!\n");
    }
    

    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override {
        plug::Vec2d global_position = stack.apply(layout->getPosition());
        plug::Vec2d global_size = applySize(stack, layout->getSize());

        RectShape rect(global_position, global_size, hex2Color(0));
        rect.setBorder(1, Blue);
        rect.draw(result);
    }
};


TestRectTool::TestRectTool() :
    draw_start(), rect_preview(nullptr), my_data()
{
    rect_preview = new TestRectPreview(*this);
    printf("Rect tool created!\n");
}


RectShape TestRectTool::createRect(const plug::Vec2d &p1, const plug::Vec2d &p2) const {
    plug::Vec2d position = p1;
    plug::Vec2d size = p2 - p1;
    if (position.x > p2.x) position.x = p2.x;
    if (position.y > p2.y) position.y = p2.y;
    if (size.x < 0) size.x *= -1;
    if (size.y < 0) size.y *= -1;

    return RectShape(position, size, plug::Color());
}


void TestRectTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    switch (state.state) {
        case plug::State::Pressed:
            printf("Main button pressed!\n");
            is_drawing = true;
            draw_start = mouse;
            rect_preview->getLayoutBox().setPosition(mouse);
            rect_preview->getLayoutBox().setSize(plug::Vec2d());
            break;
        case plug::State::Released:
            printf("Main button released!\n");
            onConfirm();
        default:
            break;
    }
}


void TestRectTool::onMove(const plug::Vec2d &mouse) {
    RectShape rect = createRect(draw_start, mouse);
    rect_preview->getLayoutBox().setPosition(rect.getPosition());
    rect_preview->getLayoutBox().setSize(rect.getSize());
}


void TestRectTool::onConfirm() {
    if (is_drawing) {
        printf("Rect tool confirmed!\n");

        RectShape rect(
            rect_preview->getLayoutBox().getPosition(),
            rect_preview->getLayoutBox().getSize(),
            hex2Color(0)
        );
        rect.setBorder(1, color_palette->getFGColor());
        rect.draw(*canvas);

        is_drawing = false;
    }
}


plug::Widget *TestRectTool::getWidget() {
    return (is_drawing) ? rect_preview : nullptr;
}


TestRectTool::~TestRectTool() {
    printf("Rect tool destroyed!\n");
    delete rect_preview;
}


const plug::PluginData *TestRectTool::getPluginData() const {
    return static_cast<const plug::PluginData*>(&my_data);
}
