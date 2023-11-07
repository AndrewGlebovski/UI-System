/**
 * \file
 * \brief Contains sources of LineEdit class functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "key-id.hpp"
#include "configs.hpp"
#include "widget.hpp"
#include "line-edit.hpp"


// ============================================================================


LineEdit::LineEdit(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    const LineEditStyle &style_, size_t max_length_
) :
    Widget(id_, transform_, size_, z_index_, parent_),
    str(""), style(style_), is_typing(false), max_length(max_length_),
    shift_pressed(false), is_cursor_hidden(false), blink_time(0), cursor_pos(0),
    visible_rect(nullptr), visible_rect_x(0)
{
    ASSERT((visible_rect = new sf::RenderTexture()), "Failed to allocate texture");
    ASSERT(visible_rect->create(size.x - TEXT_OFFSET, size.y - TEXT_OFFSET), "Failed to create texture!\n");
}


void LineEdit::setCursorVisible() {
    is_cursor_hidden = false;
    blink_time = 0;
}


bool LineEdit::isCorrectKey(int key_id) const {
    if (key_id >= A && key_id <= Z) return true;
    if (key_id >= Num0 && key_id <= Num9) return true;
    if (!shift_pressed && key_id == Slash) return true;
    if (!shift_pressed && key_id == Period) return true;
    if (shift_pressed && key_id == Hyphen) return true;
    return false;
}


char LineEdit::convertKey(int key_id) const {
    if (key_id >= Num0 && key_id <= Num9) return key_id - Num0 + '0';
    if (key_id == Hyphen) return '_';
    if (key_id == Period) return '.';
    if (key_id == Slash) return '/';
    if (shift_pressed) return key_id + 'A';
    return key_id + 'a';
}


const char *LineEdit::getString() const {
    return str.data();
}


void LineEdit::setString(const char *new_str) {
    str.assign(new_str);
    cursor_pos = 0;
}


const LineEditStyle &LineEdit::getStyle() const {
    return style;
}


void LineEdit::setStyle(const LineEditStyle &new_style) {
    style = new_style;
}


void LineEdit::setKeyboardFocus(bool is_focused) {
    if (is_focused) {
        setCursorVisible();
        is_typing = true;
    }
    else {
        is_typing = false;
    }
}


void LineEdit::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    sf::RectangleShape rect(size);
    rect.setFillColor(style.background_color);
    rect.setPosition(transforms.front().offset);
    rect.setOutlineThickness(style.border_thickness);
    rect.setOutlineColor(style.border_color);
    result.draw(rect);

    sf::Text text(str.data(), style.getFont(), style.font_size);
    text.setPosition(0, 0);
    text.setFillColor(style.font_color);

    Vector2D char_rel_pos = text.findCharacterPos(cursor_pos);

    float cursor_x = 0;

    // Move visible rect to left
    if (char_rel_pos.x < visible_rect_x) {
        visible_rect_x = char_rel_pos.x;
        cursor_x = TEXT_OFFSET;
    }
    // Move visible rect to right
    else if (char_rel_pos.x > visible_rect_x + size.x - TEXT_OFFSET) {
        visible_rect_x = char_rel_pos.x - (size.x - TEXT_OFFSET);
        cursor_x = size.x;
    }
    // Don't move visible rect
    else {
        cursor_x = char_rel_pos.x - visible_rect_x + TEXT_OFFSET;
    }
    
    text.setPosition(-visible_rect_x, 0);

    visible_rect->clear(sf::Color(0));
    visible_rect->draw(text);
    visible_rect->display();

    sf::Sprite tool_sprite(visible_rect->getTexture());
    tool_sprite.setPosition(transforms.front().offset + Vector2D(TEXT_OFFSET, TEXT_OFFSET));
    result.draw(tool_sprite);

    if (is_typing && !is_cursor_hidden) {
        sf::RectangleShape cursor(Vector2D(CURSOR_WIDTH, size.y + CURSOR_OFFSET * 2));
        cursor.setFillColor(style.cursor_color);
        cursor.setPosition(transforms.front().offset + Vector2D(cursor_x, -CURSOR_OFFSET));
        result.draw(cursor);
    }
}


int LineEdit::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideRect(transforms.front().offset, size, Vector2D(mouse_x, mouse_y))) {
        setKeyboardFocus(true);
        return HANDLED;
    }

    setKeyboardFocus(false);
    return UNHANDLED;
}


int LineEdit::onKeyDown(int key_id) {
    switch (key_id) {
        case Escape: 
        case Enter: 
            setKeyboardFocus(false); return HANDLED;
        case Left:
            if (cursor_pos > 0) cursor_pos--;
            setCursorVisible();
            return HANDLED;
        case Right:
            if (cursor_pos < str.length()) cursor_pos++;
            setCursorVisible();
            return HANDLED;
        case Backspace:
            if (is_typing && str.length() > 0 && cursor_pos > 0) {
                str.erase(str.begin() + cursor_pos - 1);
                cursor_pos--;
                setCursorVisible();
            }
            return (is_typing) ? HANDLED : UNHANDLED;
        case Delete:
            if (is_typing && str.length() > 0 && cursor_pos < str.length()) {
                str.erase(str.begin() + cursor_pos);
                setCursorVisible();
            }
            return (is_typing) ? HANDLED : UNHANDLED;
        case LShift:
        case RShift:
            shift_pressed = true; return UNHANDLED;
        default:
            if (is_typing && isCorrectKey(key_id) && str.length() < max_length) {
                if (cursor_pos == str.length())
                    str.push_back(convertKey(key_id));
                else
                    str.insert(str.begin() + cursor_pos, convertKey(key_id));
                
                cursor_pos++;
                setCursorVisible();
            }
            return (is_typing) ? HANDLED : UNHANDLED;
    }
}


int LineEdit::onKeyUp(int key_id) {
    switch (key_id) {
        case LShift:
        case RShift:
            shift_pressed = false; return UNHANDLED;
        default:
            return UNHANDLED;
    }
}


int LineEdit::onTimer(float delta_time) {
    blink_time += delta_time;

    if (blink_time > CURSOR_BLINK_TIME) {
        is_cursor_hidden = !is_cursor_hidden;
        blink_time = 0;
    }

    return UNHANDLED;
}


LineEdit::~LineEdit() {
    delete visible_rect;
}


// ============================================================================


#pragma GCC diagnostic pop
