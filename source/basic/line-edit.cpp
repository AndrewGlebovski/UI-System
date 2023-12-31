/**
 * \file
 * \brief Contains sources of LineEdit class functions
*/


#include <SFML/Graphics.hpp>
#include "basic/line-edit.hpp"


// ============================================================================


LineEdit::LineEdit(
    size_t id_, const LayoutBox &layout_,
    const LineEditStyle &style_, size_t max_length_
) :
    Widget(id_, layout_),
    str(""), style(style_), is_typing(false), max_length(max_length_),
    shift_pressed(false), is_cursor_hidden(false), blink_time(0), cursor_pos(0),
    visible_rect(nullptr), visible_rect_x(0)
{
    ASSERT((visible_rect = new sf::RenderTexture()), "Failed to allocate texture");

    Vec2d size = layout->getSize();
    ASSERT(visible_rect->create(size.x - TEXT_OFFSET, size.y - TEXT_OFFSET), "Failed to create texture!\n");
}


void LineEdit::setCursorVisible() {
    is_cursor_hidden = false;
    blink_time = 0;
}


bool LineEdit::isCorrectKey(int key_id, bool shift) const {
    if (key_id >= A && key_id <= Z) return true;
    if (key_id >= Num0 && key_id <= Num9) return true;
    if (!shift && key_id == Slash) return true;
    if (!shift && key_id == Period) return true;
    if (shift && key_id == Hyphen) return true;
    return false;
}


char LineEdit::convertKey(int key_id, bool shift) const {
    if (key_id >= Num0 && key_id <= Num9) return key_id - Num0 + '0';
    if (key_id == Hyphen) return '_';
    if (key_id == Period) return '.';
    if (key_id == Slash) return '/';
    if (shift) return key_id + 'A';
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


void LineEdit::draw(sf::RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    sf::RectangleShape rect(global_size);
    rect.setFillColor(style.background_color);
    rect.setPosition(global_position);
    rect.setOutlineThickness(style.border_thickness);
    rect.setOutlineColor(style.border_color);
    result.draw(rect);

    sf::Text text(str.data(), style.getFont(), style.font_size);
    text.setPosition(0, 0);
    text.setFillColor(style.font_color);

    Vec2d char_rel_pos = text.findCharacterPos(cursor_pos);

    float cursor_x = 0;

    // Move visible rect to left
    if (char_rel_pos.x < visible_rect_x) {
        visible_rect_x = char_rel_pos.x;
        cursor_x = TEXT_OFFSET;
    }
    // Move visible rect to right
    else if (char_rel_pos.x > visible_rect_x + global_size.x - TEXT_OFFSET) {
        visible_rect_x = char_rel_pos.x - (global_size.x - TEXT_OFFSET);
        cursor_x = global_size.x;
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
    tool_sprite.setPosition(global_position + Vec2d(TEXT_OFFSET, TEXT_OFFSET));
    result.draw(tool_sprite);

    if (is_typing && !is_cursor_hidden) {
        sf::RectangleShape cursor(Vec2d(CURSOR_WIDTH, global_size.y + CURSOR_OFFSET * 2));
        cursor.setFillColor(style.cursor_color);
        cursor.setPosition(global_position + Vec2d(cursor_x, -CURSOR_OFFSET));
        result.draw(cursor);
    }
}


void LineEdit::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos)) {
        setKeyboardFocus(true);
        ehc.stopped = true;
        return;
    }

    setKeyboardFocus(false);
}


void LineEdit::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case Escape: 
        case Enter: 
            setKeyboardFocus(false);
            ehc.stopped = true;
            break;
        case Left:
            if (cursor_pos > 0) cursor_pos--;
            setCursorVisible();
            ehc.stopped = true;
            break;
        case Right:
            if (cursor_pos < str.length()) cursor_pos++;
            setCursorVisible();
            ehc.stopped = true;
            break;
        case Backspace:
            if (is_typing && str.length() > 0 && cursor_pos > 0) {
                str.erase(str.begin() + cursor_pos - 1);
                cursor_pos--;
                setCursorVisible();
            }
            ehc.stopped = is_typing;
            break;
        case Delete:
            if (is_typing && str.length() > 0 && cursor_pos < str.length()) {
                str.erase(str.begin() + cursor_pos);
                setCursorVisible();
            }
            ehc.stopped = is_typing;
            break;
        default:
            if (is_typing && isCorrectKey(event.key_id, event.shift) && str.length() < max_length) {
                if (cursor_pos == str.length())
                    str.push_back(convertKey(event.key_id, event.shift));
                else
                    str.insert(str.begin() + cursor_pos, convertKey(event.key_id, event.shift));
                
                cursor_pos++;
                setCursorVisible();
            }
            ehc.stopped = is_typing;
            break;
    }
}


void LineEdit::onTick(const TickEvent &event, EHC &ehc) {
    blink_time += event.delta_time;

    if (blink_time > CURSOR_BLINK_TIME) {
        is_cursor_hidden = !is_cursor_hidden;
        blink_time = 0;
    }
}


LineEdit::~LineEdit() {
    delete visible_rect;
}
