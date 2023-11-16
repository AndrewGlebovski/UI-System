/**
 * \file
 * \brief Contains sources of canvas and palettes functions
*/


#include <SFML/Graphics.hpp>
#include "canvas/canvas.hpp"
#include "canvas/tool.hpp"
#include "canvas/filter.hpp"


// ============================================================================


ToolPalette::ToolPalette() : tools(TOOLS_SIZE, nullptr), current_tool(PENCIL_TOOL), current_color(sf::Color::Red) {
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


const sf::Color &ToolPalette::getCurrentColor() const {
    return current_color;
}


void ToolPalette::setCurrentColor(const sf::Color &color)  {
    current_color = color;
}


ToolPalette::~ToolPalette() {
    for (size_t i = 0; i < tools.size(); i++)
        delete tools[i];
}


// ============================================================================


class PaletteAction : public ButtonAction {
protected:
    ToolPalette &palette;
    int tool_id;

public:
    PaletteAction(ToolPalette &palette_, int tool_id_) : palette(palette_), tool_id(tool_id_) {}


    virtual void operator () () override {
        palette.setCurrentTool(tool_id);
    }


    virtual ButtonAction *clone() override {
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


void ToolPaletteView::draw(sf::RenderTarget &result, TransformStack &stack) {
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


// ============================================================================


size_t CanvasGroup::getIndex(Canvas *canvas) const {
    for (size_t i = 0; i < canvases.size(); i++)
        if (canvases[i] == canvas) return i;
    
    return canvases.size();
}


CanvasGroup::CanvasGroup() : canvases(), active(0) {}


void CanvasGroup::setActive(Canvas *new_active) {
    size_t index = getIndex(new_active);
    if (index < canvases.size()) active = index;
}


Canvas *CanvasGroup::getActive() {
    if (canvases.size() == 0) return nullptr;
    return canvases[active];
}


void CanvasGroup::addCanvas(Canvas *new_canvas) {
    if (!isInGroup(new_canvas)) {
        canvases.push_back(new_canvas);
        setActive(new_canvas);
    }
}


void CanvasGroup::removeCanvas(Canvas *canvas) {
    size_t index = getIndex(canvas);
    if (index < canvases.size()) {
        canvases.remove(index);

        if (canvases.size()) {
            if (index < active) {
                active--;
                setActive(canvases[active]);
            }
            else if (index == active) {
                active = 0;
                setActive(canvases[active]);
            }
        }
        else active = 0;
    }
}


bool CanvasGroup::isInGroup(Canvas *canvas) const {
    return (getIndex(canvas) < canvases.size()); 
}


// ============================================================================


Canvas::Canvas(
    size_t id_, const LayoutBox &layout_,
    ToolPalette &palette_, CanvasGroup &group_
) :
    Widget(id_, layout_),
    texture(), texture_offset(Vec2d(0, 0)),
    palette(&palette_), last_position(), group(&group_), filter_mask(),
    filename("")
{
    group->addCanvas(this);
}


void Canvas::clearCanvas() {
    texture.clear(CANVAS_BACKGROUND);
}


bool Canvas::createImage(size_t width, size_t height) {
    ASSERT(texture.create(width, height), "Failed to create texture!\n");
    clearCanvas();

    filter_mask.initMask(width, height);
    filter_mask.fill(true);

    filename = "";
    return true;
}


bool Canvas::openImage(const char *filename_) {
    sf::Texture image;
    if (image.loadFromFile(filename_)) {
        createImage(image.getSize().x, image.getSize().y);

        sf::Sprite tool_sprite(image);
        tool_sprite.setPosition(Vec2d());

        texture.draw(tool_sprite);

        filename = filename_;
        return true;
    }

    return false;
}


void Canvas::saveImage() {
    ASSERT(isImageOpen(), "File was not specified!\n");
    saveImageAs(filename.data());
}


void Canvas::saveImageAs(const char *filename_) {
    sf::Image image = texture.getTexture().copyToImage();
    image.saveToFile(filename_);
    filename = filename_;
}


const char *Canvas::getFilename() const {
    return (isImageOpen()) ? filename.data() : nullptr;
}


bool Canvas::isImageOpen() const {
    return filename.length() > 0;
}


Vec2d Canvas::getTextureSize() const {
    return Vec2d(texture.getSize().x, texture.getSize().y);
}


sf::RenderTexture &Canvas::getTexture() { return texture; }


ToolPalette *Canvas::getPalette() {
    ASSERT(palette, "Canvas must have palette!\n");
    return palette;
}


SelectionMask &Canvas::getSelectionMask() { return filter_mask; }


bool Canvas::isActive() const {
    ASSERT(group, "Canvas must be assigned to group!\n");
    return (this == group->getActive());
}


const Vec2d &Canvas::getTextureOffset() const { return texture_offset; }


void Canvas::setTextureOffset(const Vec2d &texture_offset_) {
    texture_offset = texture_offset_;
}


void Canvas::draw(sf::RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    texture.display();

    sf::Sprite tool_sprite(texture.getTexture());
    sf::Vector2i offset_(texture_offset.x, texture_offset.y);
    sf::Vector2i size_(texture.getSize().x, texture.getSize().y);
    if (size_.x > global_size.x) size_.x = global_size.x;
    if (size_.y > global_size.y) size_.y = global_size.y;
    tool_sprite.setTextureRect(sf::IntRect(offset_, size_));
    tool_sprite.setPosition(global_position);

    result.draw(tool_sprite);

    if (isActive() && palette->getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(stack, getTransform());
        TransformApplier texture_transform(stack, Transform(-texture_offset));
        palette->getCurrentTool()->getWidget()->draw(result, stack);
    }
}


void Canvas::onEvent(const Event &event, EHC &ehc) {
    if (ehc.overlapped) return;

    Widget::onEvent(event, ehc);

    if (isActive() && palette->getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(ehc.stack, getTransform());
        TransformApplier texture_transform(ehc.stack, Transform(-texture_offset));
        palette->getCurrentTool()->getWidget()->onEvent(event, ehc);
    }
}


void Canvas::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());

    last_position = event.pos;

    palette->getCurrentTool()->onMove(
        event.pos - global_position + texture_offset,
        *this
    );

    ehc.overlapped = true;
}


void Canvas::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    if (event.button_id != MouseLeft) return;

    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, last_position)) {
        ASSERT(group, "Canvas must be assigned to group!\n");
        group->setActive(this);

        palette->getCurrentTool()->onMainButton(
            CanvasTool::PRESSED, 
            event.pos - global_position + texture_offset,
            *this
        );
        
        ehc.stopped = true;
    }
}


void Canvas::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    if (event.button_id != MouseLeft) return;

    Vec2d global_position = ehc.stack.apply(layout->getPosition());

    palette->getCurrentTool()->onMainButton(
        CanvasTool::REALEASED, 
        event.pos - global_position + texture_offset,
        *this
    );

    ehc.stopped = true;
}


void Canvas::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case Escape: 
            palette->getCurrentTool()->onCancel(); break;
        case Enter: 
            palette->getCurrentTool()->onConfirm(last_position, *this); break;
        case LShift:
        case RShift:
            palette->getCurrentTool()->onModifier1(CanvasTool::PRESSED, last_position, *this); break;
        case LControl:
        case RControl:
            palette->getCurrentTool()->onModifier2(CanvasTool::PRESSED, last_position, *this); break;
        case LAlt:
        case RAlt:
            palette->getCurrentTool()->onModifier3(CanvasTool::PRESSED, last_position, *this); break;
        default:
            break;
    }
}


void Canvas::onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case LShift:
        case RShift:
            palette->getCurrentTool()->onModifier1(CanvasTool::REALEASED, last_position, *this); break;
        case LControl:
        case RControl:
            palette->getCurrentTool()->onModifier2(CanvasTool::REALEASED, last_position, *this); break;
        case LAlt:
        case RAlt:
            palette->getCurrentTool()->onModifier3(CanvasTool::REALEASED, last_position, *this); break;
        default:
            break;
    }
}


Canvas::~Canvas() {
    ASSERT(group, "Canvas is not in group!\n");
    group->removeCanvas(this);
}
