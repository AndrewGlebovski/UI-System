/**
 * \file
 * \brief Contains sources of button classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "asset.hpp"
#include "style.hpp"
#include "ui-base.hpp"
#include "button.hpp"


int ActionButton::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideButton(Vector2D(mouse_x, mouse_y) - transforms[0].offset)) {
        if (status != BUTTON_PRESSED)
            status = BUTTON_HOVER;

        return HANDLED;
    }
    
    status = BUTTON_NORMAL;
    return UNHANDLED;
}


int ActionButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (!isInsideButton(Vector2D(mouse_x, mouse_y) - transforms[0].offset)) return UNHANDLED;

    status = BUTTON_PRESSED;
    (*action)();
    return HANDLED;
}


int ActionButton::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideButton(Vector2D(mouse_x, mouse_y) - transforms[0].offset)) {
        status = BUTTON_HOVER;
        return HANDLED;
    }
    
    status = BUTTON_NORMAL;
    return UNHANDLED;
}


ActionButton::~ActionButton() {
    if (action) delete action;
}


RectButton::RectButton(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    ButtonAction *action_,
    const sf::String &text_, const ButtonStyle &style_,
    const sf::Color &normal_, const sf::Color &hover_, const sf::Color &pressed_
) :
    ActionButton(id_, transform_, size_, z_index_, parent_, action_),
    text(text_), style(style_),
    normal_color(normal_), hover_color(hover_), pressed_color(pressed_)
{}


bool RectButton::isInsideButton(const Vector2D &point) {
    return isInsideRect(Vector2D(), size, point);
}


void RectButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    sf::Text btn_text(text, style.font, style.font_size);
    sf::FloatRect text_rect = btn_text.getLocalBounds();
    Vector2D text_offset((size.x - text_rect.width) / 2, (size.y - text_rect.height) / 2);

    btn_text.setPosition(transforms[0].offset + text_offset);

    sf::RectangleShape btn_rect(size);

    switch(status) {
        case BUTTON_NORMAL: btn_rect.setFillColor(normal_color); break;
        case BUTTON_HOVER: btn_rect.setFillColor(hover_color); break;
        case BUTTON_PRESSED: btn_rect.setFillColor(pressed_color); break;
        default: ASSERT(0, "Invalid button status!\n");
    }

    btn_rect.setPosition(transforms[0].offset);

    result.draw(btn_rect);
    result.draw(btn_text);
}


TextureButton::TextureButton(
    size_t id_, const Transform &transform_, int z_index_, BaseUI *parent_,
    ButtonAction *action_,
    const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_
) :
    ActionButton(id_, transform_, Vector2D(), z_index_, parent_, action_),
    normal(normal_), hover(hover_), pressed(pressed_)
{
    size = Vector2D(normal.getSize().x, normal.getSize().y);
}


bool TextureButton::isInsideButton(const Vector2D &point) {
    return isInsideRect(Vector2D(), size, point);
}


void TextureButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    sf::Sprite btn_sprite;

    switch(status) {
        case BUTTON_NORMAL: btn_sprite.setTexture(normal); break;
        case BUTTON_HOVER: btn_sprite.setTexture(hover); break;
        case BUTTON_PRESSED: btn_sprite.setTexture(pressed); break;
        default: ASSERT(0, "Invalid button status!\n");
    }
    
    btn_sprite.setPosition(transforms[0].offset);

    result.draw(btn_sprite);
}


TextureIconButton::TextureIconButton(
    size_t id_, const Transform &transform_, int z_index_, BaseUI *parent_,
    ButtonAction *action_,
    const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_,
    const sf::Texture &icon_
) : 
    TextureButton(id_, transform_, z_index_, parent_, action_, normal_, hover_, pressed_),
    icon(icon_)
{}


void TextureIconButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TextureButton::draw(result, transforms);

    sf::Sprite icon_sprite(icon);
    icon_sprite.setPosition(transforms[0].offset + transform.offset);
    result.draw(icon_sprite);
}


#pragma GCC diagnostic pop
