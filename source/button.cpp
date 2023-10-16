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


RectButton::RectButton(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    ButtonAction *action_,
    const sf::String &text_, const ButtonStyle &style_,
    const sf::Color &normal_, const sf::Color &hover_
) :
    ActionButton(position_, size_, z_index_, parent_, action_),
    text(text_), style(style_),
    normal_color(normal_), hover_color(hover_)
{}


void RectButton::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    sf::Text btn_text(text, style.font, style.font_size);
    sf::FloatRect text_rect = btn_text.getLocalBounds();
    Vector2D text_offset((size.x - text_rect.width) / 2, (size.y - text_rect.height) / 2);

    btn_text.setPosition(transforms[0] + text_offset);

    sf::RectangleShape btn_rect(size);

    switch(status) {
        case BUTTON_NORMAL: btn_rect.setFillColor(normal_color); break;
        case BUTTON_HOVER: btn_rect.setFillColor(hover_color); break;
        default: ASSERT(0, "Invalid button status!\n");
    }

    btn_rect.setPosition(transforms[0]);

    result.draw(btn_rect);
    result.draw(btn_text);
}


int RectButton::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    if (isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y))) {
        status = BUTTON_HOVER;
        return HANDLED;
    }
    
    status = BUTTON_NORMAL;
    return UNHANDLED;
}


int RectButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    if (!isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y))) return UNHANDLED;

    (*action)();
    return HANDLED;
}


TextureButton::TextureButton(
    const Vector2D &position_, int z_index_, BaseUI *parent_,
    ButtonAction *action_,
    const sf::Texture &normal_, const sf::Texture &hover_
) :
    ActionButton(position_, Vector2D(), z_index_, parent_, action_),
    normal(normal_), hover(hover_)
{
    size = Vector2D(normal.getSize().x, normal.getSize().y);
}


int TextureButton::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    if (isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y))) {
        status = BUTTON_HOVER;
        return HANDLED;
    }
    
    status = BUTTON_NORMAL;
    return UNHANDLED;
}


int TextureButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    if (!isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y))) return UNHANDLED;

    (*action)();
    return HANDLED;
}


void TextureButton::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    sf::Sprite btn_sprite;

    switch(status) {
        case BUTTON_NORMAL: btn_sprite.setTexture(normal); break;
        case BUTTON_HOVER: btn_sprite.setTexture(hover); break;
        default: ASSERT(0, "Invalid button status!\n");
    }
    
    btn_sprite.setPosition(transforms[0]);

    result.draw(btn_sprite);
}


#pragma GCC diagnostic pop
