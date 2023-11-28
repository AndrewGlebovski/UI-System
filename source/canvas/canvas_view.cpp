/**
 * \file
 * \brief Contains canvas view and canvas group implementation
*/


#include <cstring>
#include "canvas/canvas_view.hpp"


// ============================================================================


size_t CanvasGroup::getIndex(CanvasView *canvas) const {
    for (size_t i = 0; i < canvases.size(); i++)
        if (canvases[i] == canvas) return i;
    
    return canvases.size();
}


CanvasGroup::CanvasGroup() : canvases(), active(0) {}


void CanvasGroup::setActive(CanvasView *new_active) {
    size_t index = getIndex(new_active);
    if (index < canvases.size()) active = index;
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


// ============================================================================


CanvasView::CanvasView(
    size_t id_, const LayoutBox &layout_,
    ToolPalette &tool_palette_,
    ColorPalette &color_palette_,
    CanvasGroup &group_
) :
    Widget(id_, layout_),
    canvas(),
    texture_offset(Vec2d(0, 0)),
    tool_palette(&tool_palette_),
    color_palette(&color_palette_),
    group(&group_),
    filename("")
{
    group->addCanvas(this);

    tool_palette->setColorPalette(*color_palette);
    tool_palette->setActiveCanvas(canvas);
}


bool CanvasView::createImage(size_t width, size_t height) {
    canvas.setSize(Vec2d(width, height));
    filename = "";
    texture_offset = Vec2d();

    Texture texture(width, height);
    for (size_t i = 0; i < width * height; i++)
        texture.data[i] = CANVAS_BACKGROUND;
    
    TextureShape(texture).draw(canvas, Vec2d(), canvas.getSize());

    return true;
}


bool CanvasView::openImage(const char *filename_) {
    sf::Image image;
    if (image.loadFromFile(filename_)) {
        createImage(image.getSize().x, image.getSize().y);

        const uint8_t *src = image.getPixelsPtr();

        Texture texture(image.getSize().x, image.getSize().y);
        
        for (size_t i = 0; i < texture.width * texture.height; i++)
            texture.data[i] = Color(src[i * 4], src[i * 4 + 1], src[i * 4 + 2], src[i * 4 + 3]);

        TextureShape(texture).draw(canvas, Vec2d(), image.getSize());

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


Vec2d CanvasView::getTextureSize() const {
    return canvas.getSize();
}


Canvas &CanvasView::getCanvas() {
    return canvas;
}


bool CanvasView::isActive() const {
    ASSERT(group, "CanvasView must be assigned to group!\n");
    return (this == group->getActive());
}


const Vec2d &CanvasView::getTextureOffset() const { return texture_offset; }


void CanvasView::setTextureOffset(const Vec2d &texture_offset_) {
    texture_offset = texture_offset_;
}


void CanvasView::draw(RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    Vec2d size = canvas.getSize();
    if (size.x > global_size.x) size.x = global_size.x;
    if (size.y > global_size.y) size.y = global_size.y;

    VertexArray array(TriangleFan, 4);

    array[0] = Vertex(Vec2d(global_position), Color(), Vec2d(texture_offset));
    array[1] = Vertex(Vec2d(global_position.x, global_position.y + size.y), Color(), Vec2d(texture_offset.x, texture_offset.y + size.y));
    array[2] = Vertex(Vec2d(global_position + size), Color(), texture_offset + size);
    array[3] = Vertex(Vec2d(global_position.x + size.x, global_position.y), Color(), Vec2d(texture_offset.x + size.x, texture_offset.y));

    result.draw(array, canvas.getTexture());

    if (isActive() && tool_palette->getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(stack, getTransform());
        TransformApplier texture_transform(stack, Transform(-texture_offset));
        tool_palette->getCurrentTool()->getWidget()->draw(result, stack);
    }
}


void CanvasView::onEvent(const Event &event, EHC &ehc) {
    if (ehc.overlapped) return;

    Widget::onEvent(event, ehc);

    if (isActive() && tool_palette->getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(ehc.stack, getTransform());
        TransformApplier texture_transform(ehc.stack, Transform(-texture_offset));
        tool_palette->getCurrentTool()->getWidget()->onEvent(event, ehc);
    }
}


void CanvasView::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());

    tool_palette->getCurrentTool()->onMove(event.pos - global_position + texture_offset);

    ehc.overlapped = true;
}


void CanvasView::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    if (event.button_id != MouseLeft) return;

    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos)) {
        if (!isActive()) {
            ASSERT(group, "CanvasView must be assigned to group!\n");
            group->setActive(this);
            
            tool_palette->setActiveCanvas(canvas);
        }

        tool_palette->getCurrentTool()->onMainButton(
            State::Pressed, 
            event.pos - global_position + texture_offset
        );
        
        ehc.stopped = true;
    }
}


void CanvasView::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    if (event.button_id != MouseLeft) return;

    Vec2d global_position = ehc.stack.apply(layout->getPosition());

    tool_palette->getCurrentTool()->onMainButton(
        State::Released, 
        event.pos - global_position + texture_offset
    );

    ehc.stopped = true;
}


void CanvasView::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case Escape: 
            tool_palette->getCurrentTool()->onCancel(); break;
        case Enter: 
            tool_palette->getCurrentTool()->onConfirm(); break;
        case LShift:
        case RShift:
            tool_palette->getCurrentTool()->onModifier1(State::Pressed); break;
        case LControl:
        case RControl:
            tool_palette->getCurrentTool()->onModifier2(State::Pressed); break;
        case LAlt:
        case RAlt:
            tool_palette->getCurrentTool()->onModifier3(State::Pressed); break;
        default:
            break;
    }
}


void CanvasView::onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case LShift:
        case RShift:
            tool_palette->getCurrentTool()->onModifier1(State::Released); break;
        case LControl:
        case RControl:
            tool_palette->getCurrentTool()->onModifier2(State::Released); break;
        case LAlt:
        case RAlt:
            tool_palette->getCurrentTool()->onModifier3(State::Released); break;
        default:
            break;
    }
}


CanvasView::~CanvasView() {
    ASSERT(group, "CanvasView is not in group!\n");
    group->removeCanvas(this);
}
