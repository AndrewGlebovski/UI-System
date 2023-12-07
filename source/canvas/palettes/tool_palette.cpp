/**
 * \file
 * \brief Contains implementation of ToolPalette and ToolPaletteView
*/


#include "canvas/palettes/tool_palette.hpp"
#include "canvas/tools/tools.hpp"
#include "window/window.hpp"
#include "canvas/palettes/palette_manager.hpp"
#include "canvas/canvas_view.hpp"
#include "common/utils.hpp"


// ============================================================================


const plug::Vec2d TOOL_ICON_SIZE = plug::Vec2d(94, 94);         ///< Tool icon size


// ============================================================================


ToolPalette::ToolPalette(plug::ColorPalette &color_palette) :
    tools(TOOLS_SIZE, nullptr), current_tool(PENCIL_TOOL)
{
    tools[PENCIL_TOOL] = new PencilTool();
    tools[RECT_TOOL] = new RectTool();
    tools[LINE_TOOL] = new LineTool();
    tools[ERASER_TOOL] = new EraserTool();
    tools[COLOR_PICKER] = new ColorPicker();
    tools[BUCKET_TOOL] = new BucketTool();
    tools[POLYGON_TOOL] = new PolygonTool();
    tools[TEXT_TOOL] = new TextTool();
}


plug::Tool *ToolPalette::getTool(size_t index) {
    ASSERT(index < tools.size(), "Index is out of range!\n");

    return tools[index];
}


size_t ToolPalette::getCurrentIndex() const {
    return current_tool;
}


plug::Tool *ToolPalette::getCurrentTool() {
    return getTool(getCurrentIndex());
}


void ToolPalette::setCurrentTool(size_t index) {
    if (index < tools.size()) {
        getCurrentTool()->onCancel();
        current_tool = index;
    }
}


void ToolPalette::setActiveCanvas(plug::Canvas &canvas) {
    for (size_t i = 0; i < tools.size(); i++)
        tools[i]->setActiveCanvas(canvas);
}


void ToolPalette::setColorPalette(plug::ColorPalette &color_palette) {
    for (size_t i = 0; i < tools.size(); i++)
        tools[i]->setColorPalette(color_palette);
}


void ToolPalette::addTool(plug::Tool &tool) {
    // Set tool color palette and active canvas
    tool.setActiveCanvas(CANVAS_GROUP.getActive()->getCanvas());
    tool.setColorPalette(COLOR_PALETTE);

    tools.push_back(&tool);
}


size_t ToolPalette::getToolCount() const {
    return tools.size();
}


ToolPalette::~ToolPalette() {
    // Delete predefined tools
    for (size_t i = 0; i < TOOLS_SIZE; i++)
        delete tools[i];
    
    // Release additional tools
    for (size_t i = TOOLS_SIZE; i < tools.size(); i++)
        tools[i]->release();
}


// ============================================================================


class PaletteAction : public ButtonAction {
public:
    PaletteAction(size_t tool_id_) :
        tool_id(tool_id_) {}

    void operator () () override {
        TOOL_PALETTE.setCurrentTool(tool_id);
    }

    ButtonAction *clone() override {
        return new PaletteAction(tool_id);
    }

protected:
    size_t tool_id;
};


// ============================================================================


#define ADD_TOOL_BUTTON(TOOL_ID, TOOL_TEXTURE_ID, POSITION)     \
do {                                                            \
    TextureIconButton *btn =  new TextureIconButton(            \
        Widget::AUTO_ID + TOOL_ID + 1,                          \
        LazyLayoutBox(POSITION, plug::Vec2d()),                       \
        new PaletteAction(TOOL_ID),                             \
        asset[PaletteViewAsset::NORMAL_TEXTURE],                \
        asset[PaletteViewAsset::NORMAL_TEXTURE],                \
        asset[PaletteViewAsset::SELECTED_TEXTURE],              \
        asset[TOOL_TEXTURE_ID]                                  \
    );                                                          \
    btn->setButtonGroup(group);                                 \
    buttons.addChild(btn);                                      \
} while(0)


ToolPaletteView::ToolPaletteView(
    size_t id_, const plug::LayoutBox &layout_,
    const PaletteViewAsset &asset_
) :
    Widget(id_, layout_), 
    buttons(1, AnchorLayoutBox(plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H), plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H)), false),
    asset(asset_), group(nullptr),
    icons()
{
    buttons.setParent(this);

    group = new ButtonGroup();

    ADD_TOOL_BUTTON(ToolPalette::PENCIL_TOOL,   PaletteViewAsset::PENCIL_TEXTURE,   plug::Vec2d());
    ADD_TOOL_BUTTON(ToolPalette::RECT_TOOL,     PaletteViewAsset::RECT_TEXTURE,     plug::Vec2d(94, 0));
    ADD_TOOL_BUTTON(ToolPalette::LINE_TOOL,     PaletteViewAsset::LINE_TEXTURE,     plug::Vec2d(0, 94));
    ADD_TOOL_BUTTON(ToolPalette::ERASER_TOOL,   PaletteViewAsset::ERASER_TEXTURE,   plug::Vec2d(94, 94));
    ADD_TOOL_BUTTON(ToolPalette::COLOR_PICKER,  PaletteViewAsset::PICKER_TEXTURE,   plug::Vec2d(0, 188));
    ADD_TOOL_BUTTON(ToolPalette::BUCKET_TOOL,   PaletteViewAsset::BUCKET_TEXTURE,   plug::Vec2d(94, 188));
    ADD_TOOL_BUTTON(ToolPalette::POLYGON_TOOL,  PaletteViewAsset::POLYGON_TEXTURE,  plug::Vec2d(0, 282));
    ADD_TOOL_BUTTON(ToolPalette::TEXT_TOOL,     PaletteViewAsset::TEXT_TEXTURE,     plug::Vec2d(94, 282));

    updateCurrentButton();
}


#undef ADD_TOOL_BUTTON


void ToolPaletteView::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
    TransformApplier add_transform(stack, getTransform());

    updateButtons();

    updateCurrentButton();

    buttons.draw(stack, result);
}


void ToolPaletteView::onEvent(const plug::Event &event, plug::EHC &ehc) {
    Widget::onEvent(event, ehc);
    if (ehc.stopped) return;

    TransformApplier canvas_transform(ehc.stack, getTransform());
    buttons.onEvent(event, ehc);
}


void ToolPaletteView::onKeyboardPressed(const plug::KeyboardPressedEvent &event, plug::EHC &ehc) {
    // No modifiers pressed
    if (event.alt || event.ctrl || event.shift) return;

    switch(event.key_id) {
        case plug::KeyCode::P: TOOL_PALETTE.setCurrentTool(ToolPalette::PENCIL_TOOL); break;
        case plug::KeyCode::R: TOOL_PALETTE.setCurrentTool(ToolPalette::RECT_TOOL); break;
        case plug::KeyCode::L: TOOL_PALETTE.setCurrentTool(ToolPalette::LINE_TOOL); break;
        case plug::KeyCode::E: TOOL_PALETTE.setCurrentTool(ToolPalette::ERASER_TOOL); break;
        case plug::KeyCode::C: TOOL_PALETTE.setCurrentTool(ToolPalette::COLOR_PICKER); break;
        case plug::KeyCode::F: TOOL_PALETTE.setCurrentTool(ToolPalette::BUCKET_TOOL); break;
        case plug::KeyCode::O: TOOL_PALETTE.setCurrentTool(ToolPalette::POLYGON_TOOL); break;
        case plug::KeyCode::T: TOOL_PALETTE.setCurrentTool(ToolPalette::TEXT_TOOL); break;
        default: return;
    }

    ehc.stopped = true;
}


void ToolPaletteView::onParentUpdate(const plug::LayoutBox &parent_layout) {
    getLayoutBox().onParentUpdate(parent_layout);
    buttons.onParentUpdate(getLayoutBox());
}


void ToolPaletteView::updateCurrentButton() {
    size_t curr_tool = TOOL_PALETTE.getCurrentIndex();

    ActionButton *curr_btn = static_cast<ActionButton*>(
        buttons.findWidget(curr_tool + Widget::AUTO_ID + 1)
    );

    if (group->getPressed() != curr_btn) group->setPressed(curr_btn);
}


void ToolPaletteView::updateButtons() {
    if (buttons.getChildCount() == TOOL_PALETTE.getToolCount()) return;

    for (size_t i = buttons.getChildCount(); i < TOOL_PALETTE.getToolCount(); i++)
        addTool(i);
}


void ToolPaletteView::addTool(size_t tool_id) {
    plug::Tool &new_tool = *TOOL_PALETTE.getTool(tool_id);

    const char *texture_path = new_tool.getPluginData()->getTexturePath();

    plug::Texture *new_icon = nullptr;
    loadTexture(&new_icon, texture_path);

    if (new_icon) {
        icons.push_back(new_icon);
    }
    else {
        if (new_tool.getPluginData()->getName())
            printf("%s icon not found!\n", new_tool.getPluginData()->getName());
        else
            printf("Tool icon not found!\n");
    }

    Widget *prev_button = buttons.findWidget(Widget::AUTO_ID + tool_id);

    plug::Vec2d position = prev_button->getLayoutBox().getPosition();

    if (isEqual(position.x, 0)) position.x = TOOL_ICON_SIZE.x;
    else {
        position = plug::Vec2d(0, position.y + TOOL_ICON_SIZE.y);

        // Change parent window size to fit new tools
        Window *parent_window = static_cast<Window*>(getParent()->getParent());
        plug::Vec2d parent_size = parent_window->getLayoutBox().getSize();
        parent_window->setSize(parent_size + plug::Vec2d(0, TOOL_ICON_SIZE.y));
    }

    TextureIconButton *btn = new TextureIconButton(
        Widget::AUTO_ID + tool_id + 1,
        LazyLayoutBox(position, plug::Vec2d()),
        new PaletteAction(tool_id),
        asset[PaletteViewAsset::NORMAL_TEXTURE],
        asset[PaletteViewAsset::NORMAL_TEXTURE],
        asset[PaletteViewAsset::SELECTED_TEXTURE],
        (new_icon) ? *new_icon : asset[PaletteViewAsset::ERASER_TEXTURE]
    );

    btn->setButtonGroup(group);

    buttons.addChild(btn);
}


ToolPaletteView::~ToolPaletteView() {
    ASSERT(group, "Group is nullptr!\n");
    delete group;

    // Delete icons for additional tools
    for (size_t i = 0; i < icons.size(); i++) {
        ASSERT(icons[i], "Icon is nullptr!\n");
        delete icons[i];
    }
}
