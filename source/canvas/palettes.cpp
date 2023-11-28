/**
 * \file
 * \brief Contains ToolPalette and FilterPalette implementation
*/


#include "canvas/palettes.hpp"
#include "canvas/tool.hpp"
#include "canvas/filter.hpp"


// ============================================================================


ToolPalette::ToolPalette(ColorPalette &color_palette) :
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


CanvasTool *ToolPalette::getCurrentTool() {
    return tools[current_tool];
}


void ToolPalette::setCurrentTool(size_t index) {
    if (index < TOOLS_SIZE) {
        tools[current_tool]->onCancel();
        current_tool = index;
    }
}


ToolPalette::~ToolPalette() {
    for (size_t i = 0; i < tools.size(); i++)
        delete tools[i];
}


void ToolPalette::setActiveCanvas(Canvas &canvas) {
    for (size_t i = 0; i < tools.size(); i++)
        tools[i]->setActiveCanvas(canvas);
}


void ToolPalette::setColorPalette(ColorPalette &color_palette) {
    for (size_t i = 0; i < tools.size(); i++)
        tools[i]->setColorPalette(color_palette);
}


// ============================================================================


class PaletteAction : public ButtonAction {
protected:
    ToolPalette &palette;
    int tool_id;

public:
    PaletteAction(ToolPalette &palette_, int tool_id_) : palette(palette_), tool_id(tool_id_) {}


    void operator () () override {
        palette.setCurrentTool(tool_id);
    }


    ButtonAction *clone() override {
        return new PaletteAction(palette, tool_id);
    }
};


// ============================================================================


void ToolPaletteView::updateToolButtons() {
    size_t current_tool = palette->getCurrentIndex();
    ActionButton *current_button = (ActionButton*) buttons.findWidget(current_tool + Widget::AUTO_ID + 1);

    if (group->getPressed() != current_button) group->setPressed(current_button);
}


#define ADD_TOOL_BUTTON(TOOL_ID, TOOL_TEXTURE_ID, POSITION)     \
do {                                                            \
    TextureIconButton *btn =  new TextureIconButton(            \
        Widget::AUTO_ID + TOOL_ID + 1,                          \
        LazyLayoutBox(POSITION, Vec2d()),                       \
        new PaletteAction(*palette, TOOL_ID),                   \
        asset[PaletteViewAsset::NORMAL_TEXTURE],                \
        asset[PaletteViewAsset::NORMAL_TEXTURE],                \
        asset[PaletteViewAsset::SELECTED_TEXTURE],              \
        asset[TOOL_TEXTURE_ID]                                  \
    );                                                          \
    btn->setButtonGroup(group);                                 \
    buttons.addChild(btn);                                      \
} while(0)


ToolPaletteView::ToolPaletteView(
    size_t id_, const LayoutBox &layout_,
    ToolPalette *palette_, const PaletteViewAsset &asset_
) :
    Widget(id_, layout_), 
    buttons(1, LazyLayoutBox(Vec2d(), layout->getSize()), false),
    palette(palette_), asset(asset_), group(nullptr)
{
    buttons.setParent(this);

    group = new ButtonGroup();

    ADD_TOOL_BUTTON(ToolPalette::PENCIL_TOOL,   PaletteViewAsset::PENCIL_TEXTURE,   Vec2d());
    ADD_TOOL_BUTTON(ToolPalette::RECT_TOOL,     PaletteViewAsset::RECT_TEXTURE,     Vec2d(94, 0));
    ADD_TOOL_BUTTON(ToolPalette::LINE_TOOL,     PaletteViewAsset::LINE_TEXTURE,     Vec2d(0, 94));
    ADD_TOOL_BUTTON(ToolPalette::ERASER_TOOL,   PaletteViewAsset::ERASER_TEXTURE,   Vec2d(94, 94));
    ADD_TOOL_BUTTON(ToolPalette::COLOR_PICKER,  PaletteViewAsset::PICKER_TEXTURE,   Vec2d(0, 188));
    ADD_TOOL_BUTTON(ToolPalette::BUCKET_TOOL,   PaletteViewAsset::BUCKET_TEXTURE,   Vec2d(94, 188));
    ADD_TOOL_BUTTON(ToolPalette::POLYGON_TOOL,  PaletteViewAsset::POLYGON_TEXTURE,  Vec2d(0, 282));
    ADD_TOOL_BUTTON(ToolPalette::TEXT_TOOL,     PaletteViewAsset::TEXT_TEXTURE,     Vec2d(94, 282));

    updateToolButtons();
}


#undef ADD_TOOL_BUTTON


void ToolPaletteView::draw(RenderTarget &result, TransformStack &stack) {
    TransformApplier add_transform(stack, getTransform());

    updateToolButtons();

    buttons.draw(result, stack);
}


void ToolPaletteView::onEvent(const Event &event, EHC &ehc) {
    Widget::onEvent(event, ehc);
    if (ehc.stopped) return;

    TransformApplier canvas_transform(ehc.stack, getTransform());
    buttons.onEvent(event, ehc);
}


void ToolPaletteView::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    // No modifiers pressed
    if (event.alt || event.ctrl || event.shift) return;

    switch(event.key_id) {
        case P: palette->setCurrentTool(ToolPalette::PENCIL_TOOL); break;
        case R: palette->setCurrentTool(ToolPalette::RECT_TOOL); break;
        case L: palette->setCurrentTool(ToolPalette::LINE_TOOL); break;
        case E: palette->setCurrentTool(ToolPalette::ERASER_TOOL); break;
        case C: palette->setCurrentTool(ToolPalette::COLOR_PICKER); break;
        case F: palette->setCurrentTool(ToolPalette::BUCKET_TOOL); break;
        case O: palette->setCurrentTool(ToolPalette::POLYGON_TOOL); break;
        case T: palette->setCurrentTool(ToolPalette::TEXT_TOOL); break;
        default: return;
    }

    ehc.stopped = true;
}


ToolPaletteView::~ToolPaletteView() {
    if (group) delete group;
}


// ============================================================================


FilterPalette::FilterPalette() : filters(FILTERS_SIZE, nullptr), last_filter(0) {
    filters[LIGHTEN_FILTER] = new IntensityFilter(20);
    filters[DARKEN_FILTER] = new IntensityFilter(-20);
    filters[MONOCHROME_FILTER] = new MonochromeFilter();
    filters[NEGATIVE_FILTER] = new NegativeFilter();
}


CanvasFilter *FilterPalette::getLastFilter() { return filters[last_filter]; }


void FilterPalette::setLastFilter(FILTERS filter_id) { last_filter = filter_id; }


CanvasFilter *FilterPalette::getFilter(FILTERS filter_id) { return filters[filter_id]; }


size_t FilterPalette::getFilterCount() const { return FILTERS_SIZE; }


FilterPalette::~FilterPalette() {
    for (size_t i = 0; i < filters.size(); i++)
        delete filters[i];
}
