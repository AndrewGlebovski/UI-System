/**
 * \file
 * \brief Contains canvas view and canvas group implementation
*/


#include "canvas/canvas_view.hpp"
#include "canvas/palettes/palette_manager.hpp"


// ============================================================================


CanvasView::CanvasView(size_t id_, const plug::LayoutBox &layout_) :
    Widget(id_, layout_),
    canvas(),
    texture_offset(plug::Vec2d(0, 0)),
    filename("")
{
    CANVAS_GROUP.addCanvas(this);
}


bool CanvasView::createImage(size_t width, size_t height) {
    canvas.setSize(plug::Vec2d(width, height));
    filename = "";
    texture_offset = plug::Vec2d();

    plug::Texture texture(width, height);
    for (size_t i = 0; i < width * height; i++)
        texture.data[i] = COLOR_PALETTE.getBGColor();
    
    TextureShape(texture).draw(canvas, plug::Vec2d(), canvas.getSize());

    return true;
}


bool CanvasView::openImage(const char *filename_) {
    sf::Image image;
    if (image.loadFromFile(filename_)) {
        createImage(image.getSize().x, image.getSize().y);

        const uint8_t *src = image.getPixelsPtr();

        plug::Texture texture(image.getSize().x, image.getSize().y);
        
        for (size_t i = 0; i < texture.width * texture.height; i++)
            texture.data[i] = plug::Color(src[i * 4], src[i * 4 + 1], src[i * 4 + 2], src[i * 4 + 3]);

        TextureShape(texture).draw(canvas, plug::Vec2d(), getPlugVector(image.getSize()));

        filename = filename_;
        return true;
    }

    return false;
}


void CanvasView::saveImage() {
    ASSERT(isImageOpen(), "File was not specified!\n");
    saveImageAs(filename.data());
}


void CanvasView::saveImageAs(const char *filename_) {
    sf::Image image;
    image.create(canvas.getSize().x, canvas.getSize().y, reinterpret_cast<const uint8_t*>(canvas.getTexture().data));
    image.saveToFile(filename_);
    filename = filename_;
}


const char *CanvasView::getFilename() const {
    return (isImageOpen()) ? filename.data() : nullptr;
}


bool CanvasView::isImageOpen() const {
    return filename.length() > 0;
}


plug::Vec2d CanvasView::getTextureSize() const {
    return canvas.getSize();
}


plug::Canvas &CanvasView::getCanvas() {
    return canvas;
}


bool CanvasView::isActive() const {
    return (this == CANVAS_GROUP.getActive());
}


const plug::Vec2d &CanvasView::getTextureOffset() const { return texture_offset; }


void CanvasView::setTextureOffset(const plug::Vec2d &texture_offset_) {
    texture_offset = texture_offset_;
}


void CanvasView::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
    plug::Vec2d global_position = stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(stack, layout->getSize());

    plug::Vec2d size = canvas.getSize();
    if (size.x > global_size.x) size.x = global_size.x;
    if (size.y > global_size.y) size.y = global_size.y;

    plug::VertexArray array(plug::TriangleFan, 4);

    array[0] = plug::Vertex(plug::Vec2d(global_position), plug::Color(), plug::Vec2d(texture_offset));
    array[1] = plug::Vertex(plug::Vec2d(global_position.x, global_position.y + size.y), plug::Color(), plug::Vec2d(texture_offset.x, texture_offset.y + size.y));
    array[2] = plug::Vertex(plug::Vec2d(global_position + size), plug::Color(), texture_offset + size);
    array[3] = plug::Vertex(plug::Vec2d(global_position.x + size.x, global_position.y), plug::Color(), plug::Vec2d(texture_offset.x + size.x, texture_offset.y));

    result.draw(array, canvas.getTexture());

    if (isActive() && TOOL_PALETTE.getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(stack, getTransform());
        TransformApplier texture_transform(stack, plug::Transform(texture_offset * -1));
        TOOL_PALETTE.getCurrentTool()->getWidget()->draw(stack, result);
    }
}


void CanvasView::onEvent(const plug::Event &event, plug::EHC &ehc) {
    if (ehc.overlapped) return;

    Widget::onEvent(event, ehc);

    if (isActive() && TOOL_PALETTE.getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(ehc.stack, getTransform());
        TransformApplier texture_transform(ehc.stack, plug::Transform(texture_offset * -1));
        TOOL_PALETTE.getCurrentTool()->getWidget()->onEvent(event, ehc);
    }
}


void CanvasView::onMouseMove(const plug::MouseMoveEvent &event, plug::EHC &ehc) {
    plug::Vec2d global_position = ehc.stack.apply(layout->getPosition());

    TOOL_PALETTE.getCurrentTool()->onMove(event.pos - global_position + texture_offset);

    ehc.overlapped = true;
}


void CanvasView::onMousePressed(const plug::MousePressedEvent &event, plug::EHC &ehc) {
    if (event.button_id != plug::MouseButton::Left) return;

    plug::Vec2d global_position = ehc.stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(ehc.stack, layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos)) {
        if (!isActive()) CANVAS_GROUP.setActive(this);

        TOOL_PALETTE.getCurrentTool()->onMainButton(
            {plug::State::Pressed}, 
            event.pos - global_position + texture_offset
        );
        
        ehc.stopped = true;
    }
}


void CanvasView::onMouseReleased(const plug::MouseReleasedEvent &event, plug::EHC &ehc) {
    if (event.button_id != plug::MouseButton::Left) return;

    plug::Vec2d global_position = ehc.stack.apply(layout->getPosition());

    TOOL_PALETTE.getCurrentTool()->onMainButton(
        {plug::State::Released}, 
        event.pos - global_position + texture_offset
    );

    ehc.stopped = true;
}


void CanvasView::onKeyboardPressed(const plug::KeyboardPressedEvent &event, plug::EHC &ehc) {
    switch (event.key_id) {
        case plug::KeyCode::Escape: 
            TOOL_PALETTE.getCurrentTool()->onCancel(); break;
        case plug::KeyCode::Enter: 
            TOOL_PALETTE.getCurrentTool()->onConfirm(); break;
        case plug::KeyCode::LShift:
        case plug::KeyCode::RShift:
            TOOL_PALETTE.getCurrentTool()->onModifier1({plug::State::Pressed}); break;
        case plug::KeyCode::LControl:
        case plug::KeyCode::RControl:
            TOOL_PALETTE.getCurrentTool()->onModifier2({plug::State::Pressed}); break;
        case plug::KeyCode::LAlt:
        case plug::KeyCode::RAlt:
            TOOL_PALETTE.getCurrentTool()->onModifier3({plug::State::Pressed}); break;
        default:
            break;
    }
}


void CanvasView::onKeyboardReleased(const plug::KeyboardReleasedEvent &event, plug::EHC &ehc) {
    switch (event.key_id) {
        case plug::KeyCode::LShift:
        case plug::KeyCode::RShift:
            TOOL_PALETTE.getCurrentTool()->onModifier1({plug::State::Released}); break;
        case plug::KeyCode::LControl:
        case plug::KeyCode::RControl:
            TOOL_PALETTE.getCurrentTool()->onModifier2({plug::State::Released}); break;
        case plug::KeyCode::LAlt:
        case plug::KeyCode::RAlt:
            TOOL_PALETTE.getCurrentTool()->onModifier3({plug::State::Released}); break;
        default:
            break;
    }
}


CanvasView::~CanvasView() {
    CANVAS_GROUP.removeCanvas(this);
}


// ============================================================================


size_t CanvasGroup::getIndex(CanvasView *canvas) const {
    for (size_t i = 0; i < canvases.size(); i++)
        if (canvases[i] == canvas) return i;
    
    return canvases.size();
}


CanvasGroup::CanvasGroup() : canvases(), active(0) {}


void CanvasGroup::setActive(CanvasView *new_active) {
    size_t index = getIndex(new_active);
    if (index < canvases.size()) {
        active = index;
        TOOL_PALETTE.setActiveCanvas(new_active->getCanvas());
    }
}


CanvasView *CanvasGroup::getActive() {
    if (canvases.size() == 0) return nullptr;
    return canvases[active];
}


void CanvasGroup::addCanvas(CanvasView *new_canvas) {
    if (!isInGroup(new_canvas)) {
        canvases.push_back(new_canvas);
        setActive(new_canvas);
    }
}


void CanvasGroup::removeCanvas(CanvasView *canvas) {
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


bool CanvasGroup::isInGroup(CanvasView *canvas) const {
    return (getIndex(canvas) < canvases.size()); 
}


CanvasGroup &CanvasGroup::getInstance() {
    static CanvasGroup canvas_group;
    return canvas_group;
}
