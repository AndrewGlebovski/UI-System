/**
 * \file
 * \brief Contains LineEdit class and protypes of his functions 
*/


#ifndef _LINE_EDIT_H_
#define _LINE_EDIT_H_


#include "widget/widget.hpp"


/// Controls how LineEdit looks like
struct LineEditStyle {
    const sf::Font *font;       ///< Font
    unsigned font_size;         ///< Font size
    Color font_color;           ///< Font color
    Color background_color;     ///< Background color
    Color cursor_color;         ///< Cursor color
    Color border_color;         ///< Border color
    float border_thickness;     ///< Border thickness


    LineEditStyle(
        const sf::Font &font_,
        unsigned font_size_,
        Color font_color_,
        Color background_color_,
        Color cursor_color_,
        Color border_color_,
        float border_thickness_
    ) :
        font(&font_), font_size(font_size_), font_color(font_color_),
        background_color(background_color_),
        cursor_color(cursor_color_),
        border_color(border_color_),
        border_thickness(border_thickness_)
    {}


    LineEditStyle(const LineEditStyle &style) = default;
    LineEditStyle &operator = (const LineEditStyle &style) = default;


    const sf::Font &getFont() const { return *font; }
    void setFont(const sf::Font &new_font) { font = &new_font; }
};


/// Receives user input
class LineEdit : public Widget {
public:
    /**
     * \brief Constructs LineEdit from style and max length
     * \note Max length must be greater than zero
    */
    LineEdit(
        size_t id_, const LayoutBox &layout_,
        const LineEditStyle &style_, size_t max_length_
    );

    LineEdit(const LineEdit &line_edit) = delete;
    LineEdit &operator = (const LineEdit &line_edit) = delete;

    /**
     * \brief Checks if key can be added to string
    */
    virtual bool isCorrectKey(int key_id, bool shift) const;

    /**
     * \brief Converts key_id from enum to ASCII character
    */
    virtual char convertKey(int key_id, bool shift) const;

    /**
     * \brief Returns char buffer
    */
    const char *getString() const;

    /**
     * \brief Sets char buffer
    */
    void setString(const char *new_str);

    /**
     * \brief Returns widget style
    */
    const LineEditStyle &getStyle() const;

    /**
     * \brief Sets widget style
    */
    void setStyle(const LineEditStyle &new_style);

    /**
     * \brief If is_focused == true, this widget starts handling keyboard inputs
    */
    void setKeyboardFocus(bool is_focused);

    /**
     * \brief Draws buffer content and cursor
    */
    virtual void draw(TransformStack &stack, RenderTarget &result) override;

protected:
    /**
     * \brief Resets blink time to zero and sets cursor visible
    */
    void setCursorVisible();

    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) override;
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) override;
    virtual void onTick(const TickEvent &event, EHC &ehc) override;


    std::string str;            ///< Char buffer
    LineEditStyle style;        ///< Style
    bool is_typing;             ///< This widget is in user focus
    size_t max_length;          ///< Max amount characters in string
    bool shift_pressed;         ///< Shift key is pressed
    bool is_cursor_hidden;      ///< Cursor is hidden at this moment
    float blink_time;           ///< Time passed since last switch between cursor blink states
    size_t cursor_pos;          ///< Cursor position
    TextShape text;             ///< Text shape
};


#endif
