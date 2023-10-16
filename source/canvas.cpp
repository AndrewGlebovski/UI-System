/**
 * \file
 * \brief Contains sources of canvas and his tools classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "configs.hpp"
#include "key-id.hpp"
#include "asset.hpp"
#include "style.hpp"
#include "ui-base.hpp"
#include "button.hpp"
#include "scrollbar.hpp"
#include "ui-system.hpp"
#include "canvas.hpp"


PencilTool::PencilTool() : prev_position() {}


void PencilTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED:
            is_drawing = true;
            prev_position = mouse;
            break;
        case REALEASED:
            is_drawing = false;
        default:
            break;
    }
}


void PencilTool::onMove(const Vector2D &mouse, Canvas &canvas) {
    if (is_drawing) {
        sf::Vertex line[] = {
            sf::Vertex(prev_position, canvas.getPalette()->getCurrentColor()),
            sf::Vertex(mouse, canvas.getPalette()->getCurrentColor()),
        };

        prev_position = mouse;
        canvas.getTexture().draw(line, 2, sf::Lines);
    }
}


void PencilTool::onConfirm(const Vector2D &mouse, Canvas &canvas) { is_drawing = false; }


void PencilTool::onCancel(const Vector2D &mouse, Canvas &canvas) { is_drawing = false; }


Palette::Palette() : tools(), current_tool(0), current_color(sf::Color::Red) {
    tools.push_back(new PencilTool());
}


CanvasTool *Palette::getCurrentTool() {
    return tools[current_tool];
}


void Palette::setCurrentTool(size_t index) {
    // NEED TO CANCEL CURRENT TOOL BEFORE SWITCHING
    current_tool = index;
}


const sf::Color &Palette::getCurrentColor() const {
    return current_color;
}


void Palette::setCurrentColor(const sf::Color &color)  {
    current_color = color;
}


Palette::~Palette() {
    for (size_t i = 0; i < tools.getSize(); i++)
        delete tools[i];
}


class PaletteAction : public ButtonAction {
protected:
    Palette &palette;
    int tool_id;

public:
    PaletteAction(Palette &palette_, int tool_id_) : palette(palette_), tool_id(tool_id_) {}


    void operator () () {
        palette.setCurrentTool(tool_id);
    }
};


PaletteView::PaletteView(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    Palette *palette_, const PaletteViewAsset &asset_
) : 
    BaseUI(position_, size_, z_index_, parent_), 
    buttons(Vector2D(), size, 0, this), palette(palette_), asset(asset_)
{
    buttons.addElement(new TextureButton(
        Vector2D(),
        0,
        nullptr,
        new PaletteAction(*palette, 0),
        asset[PaletteViewAsset::PENCIL],
        asset[PaletteViewAsset::PENCIL]
    ));

    buttons.addElement(new TextureButton(
        Vector2D(94, 0),
        0,
        nullptr,
        new PaletteAction(*palette, 1),
        asset[PaletteViewAsset::RECT],
        asset[PaletteViewAsset::RECT]
    ));
}


void PaletteView::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    buttons.draw(result, transforms);
}


int PaletteView::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    return buttons.onMouseMove(mouse_x, mouse_y, transforms);
}


int PaletteView::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    return buttons.onMouseButtonDown(mouse_x, mouse_y, button_id, transforms);
}


int PaletteView::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    return buttons.onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);
}


void Canvas::clear_canvas() {
    texture.clear(sf::Color::White);
}


Canvas::Canvas(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    const char *image_path, Palette *palette_
) :
    BaseUI(position_, size_, z_index_, parent_),
    texture(), texture_offset(Vector2D(0, 0)),
    palette(palette_), last_position()
{
    Vector2D texture_size = size;

    if (image_path) {
        sf::Texture image;
        if (image.loadFromFile(image_path)) {
            if (texture_size.x < image.getSize().x) texture_size.x = image.getSize().x;
            if (texture_size.y < image.getSize().y) texture_size.y = image.getSize().y;

            texture.create(texture_size.x, texture_size.y);
            clear_canvas();

            sf::Sprite tool_sprite(image);
            tool_sprite.setPosition(Vector2D());

            texture.draw(tool_sprite);

            return;
        }
    }

    texture.create(size.x, size.y);
    clear_canvas();
}


Vector2D Canvas::getTextureSize() const {
    return Vector2D(texture.getSize().x, texture.getSize().y);
}


sf::RenderTexture &Canvas::getTexture() {
    return texture;
}


Palette *Canvas::getPalette() {
    return palette;
}


void Canvas::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    texture.display();

    sf::Sprite tool_sprite(texture.getTexture());
    sf::Vector2i offset_(texture_offset.x, texture_offset.y);
    sf::Vector2i size_(size.x, size.y);
    tool_sprite.setTextureRect(sf::IntRect(offset_, size_));
    tool_sprite.setPosition(transforms[0]);

    result.draw(tool_sprite);

    if (palette->getCurrentTool()->getWidget())
        palette->getCurrentTool()->getWidget()->draw(result, transforms);
}


int Canvas::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    last_position = Vector2D(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, last_position)) {
        palette->getCurrentTool()->onMove(
            last_position - transforms[0] + texture_offset,
            *this
        );

        if (palette->getCurrentTool()->getWidget())
            palette->getCurrentTool()->getWidget()->onMouseMove(mouse_x, mouse_y, transforms);

        return HANDLED;
    }

    return UNHANDLED;
}


int Canvas::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, position);

    last_position = Vector2D(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, last_position)) {
        palette->getCurrentTool()->onMainButton(
            CanvasTool::PRESSED, 
            last_position - transforms[0] + texture_offset,
            *this
        );

        if (palette->getCurrentTool()->getWidget())
            palette->getCurrentTool()->getWidget()->onMouseButtonDown(mouse_x, mouse_y, button_id, transforms);

        return HANDLED;
    }

    return UNHANDLED;
}


int Canvas::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, position);

    last_position = Vector2D(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, last_position)) {
        palette->getCurrentTool()->onMainButton(
            CanvasTool::REALEASED, 
            last_position - transforms[0] + texture_offset,
            *this
        );

        if (palette->getCurrentTool()->getWidget())
            palette->getCurrentTool()->getWidget()->onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);

        return HANDLED;
    }

    return UNHANDLED;
}


int Canvas::onParentResize() {
    Vector2D new_size = texture.getSize();
    if (new_size.x > parent->size.x - 30) new_size.x = parent->size.x - 30;
    if (new_size.y > parent->size.y - 30) new_size.y = parent->size.y - 30;

    setSize(new_size);

    return UNHANDLED;
}


int Canvas::onKeyDown(int key_id) {
    switch (key_id) {
        case Escape: 
            palette->getCurrentTool()->onCancel(last_position, *this); return HANDLED;
        case LShift:
        case RShift:
            palette->getCurrentTool()->onModifier1(CanvasTool::PRESSED, last_position, *this); return HANDLED;
        case LControl:
        case RControl:
            palette->getCurrentTool()->onModifier2(CanvasTool::PRESSED, last_position, *this); return HANDLED;
        case LAlt:
        case RAlt:
            palette->getCurrentTool()->onModifier3(CanvasTool::PRESSED, last_position, *this); return HANDLED;
        default:
            if (palette->getCurrentTool()->getWidget())
                return palette->getCurrentTool()->getWidget()->onKeyDown(key_id);
            return UNHANDLED;
    }
}


#pragma GCC diagnostic pop
