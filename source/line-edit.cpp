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
    shift_pressed(false), is_cursor_hidden(false), blink_time(0), cursor_pos(0)
{}


void LineEdit::setCursorVisible() {
    is_cursor_hidden = false;
    blink_time = 0;
}


bool LineEdit::isCorrectKey(int key_id) const {
    if (key_id >= A && key_id <= Z) return true;
    if (key_id >= Num0 && key_id <= Num9) return true;
    if (shift_pressed && key_id == Subtract) return true;

    return false;
}


char LineEdit::convertKey(int key_id) const {
    if (key_id >= Num0 && key_id <= Num9) return key_id - Num0 + '0';
    if (key_id == Subtract) return '_';
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


void LineEdit::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    sf::RectangleShape rect(size);
    rect.setFillColor(style.background_color);
    rect.setPosition(transforms.front().offset);
    rect.setOutlineThickness(style.border_thickness);
    rect.setOutlineColor(style.border_color);
    result.draw(rect);

    sf::Text text(str.data(), style.getFont(), style.font_size);
    text.setFillColor(style.font_color);
    text.setPosition(transforms.front().offset + Vector2D(TEXT_OFFSET, TEXT_OFFSET));
    result.draw(text);

    if (is_typing && !is_cursor_hidden) {
        sf::RectangleShape cursor(Vector2D(CURSOR_WIDTH, size.y + CURSOR_OFFSET * 2));
        cursor.setFillColor(style.cursor_color);
        cursor.setPosition(Vector2D(text.findCharacterPos(cursor_pos).x, transforms.front().offset.y - CURSOR_OFFSET));
        result.draw(cursor);
    }
}


int LineEdit::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideRect(transforms.front().offset, size, Vector2D(mouse_x, mouse_y))) {
        is_typing = true;
        return HANDLED;
    }

    is_typing = false;
    return UNHANDLED;
}


int LineEdit::onKeyDown(int key_id) {
    switch (key_id) {
        case Escape: 
        case Enter: 
            is_typing = false; return HANDLED;
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


// ============================================================================


#pragma GCC diagnostic pop
