/**
 * \file
 * \brief Contains LineEdit class and protypes of his functions 
*/


/// Controls how LineEdit looks like
struct LineEditStyle {
    const sf::Font *font;       ///< Font
    unsigned font_size;         ///< Font size
    sf::Color font_color;       ///< Font color
    sf::Color background_color; ///< Background color
    sf::Color cursor_color;     ///< Cursor color
    sf::Color border_color;     ///< Border color
    float border_thickness;     ///< Border thickness


    LineEditStyle(
        const sf::Font &font_,
        unsigned font_size_,
        const sf::Color &font_color_,
        const sf::Color &background_color_,
        const sf::Color &cursor_color_,
        const sf::Color &border_color_,
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
protected:
    std::string str;            ///< Char buffer
    LineEditStyle style;        ///< Style
    bool is_typing;             ///< This widget is in user focus
    size_t max_length;          ///< Max amount characters in string
    bool shift_pressed;         ///< Shift key is pressed
    bool is_cursor_hidden;      ///< Cursor is hidden at this moment
    float blink_time;           ///< Time passed since last switch between cursor blink states
    size_t cursor_pos;          ///< Cursor position
    sf::RenderTexture *visible_rect;     ///< Visible part of the text
    float visible_rect_x;                ///< Visible rect offset from text begin


    /**
     * \brief Resets blink time to zero and sets cursor visible
    */
    void setCursorVisible();

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
    virtual bool isCorrectKey(int key_id) const;

    /**
     * \brief Converts key_id from enum to ASCII character
    */
    virtual char convertKey(int key_id) const;

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
    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override;

    virtual EVENT_STATUS onMouseButtonDown(const Vec2d &mouse, int button_id, TransformStack &stack) override;
    virtual EVENT_STATUS onKeyDown(int key_id) override;
    virtual EVENT_STATUS onKeyUp(int key_id) override;
    virtual EVENT_STATUS onTimer(float delta_time) override;

    /**
     * \brief Delete visible rect
    */
    virtual ~LineEdit() override;
};
