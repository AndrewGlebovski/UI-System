/**
 * \file
 * \brief Contains LineEdit class and protypes of his functions 
*/


/// Controls how LineEdit looks like
struct LineEditStyle {
    const sf::Font &font;       ///< Font
    unsigned font_size;         ///< Font size
    sf::Color font_color;       ///< Font color
    sf::Color background_color; ///< Background color
    sf::Color cursor_color;     ///< Cursor color
    float cursor_width;         ///< Cursor width
    float blink_time;           ///< Time required to passed between cursor blink states


    LineEditStyle(
        const sf::Font &font_,
        unsigned font_size_,
        const sf::Color &font_color_,
        const sf::Color &background_color_,
        const sf::Color &cursor_color_,
        float cursor_width_,
        float blink_time_
    ) :
        font(font_), font_size(font_size_), font_color(font_color_),
        background_color(background_color_),
        cursor_color(cursor_color_), cursor_width(cursor_width_),
        blink_time(blink_time_)
    {}
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

public:
    /**
     * \brief Constructs LineEdit from style and max length
     * \note Max length must be greater than zero
    */
    LineEdit(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
        const LineEditStyle &style_, size_t max_length_
    );

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
     * \brief Draws buffer content and cursor
    */
    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;

    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onKeyDown(int key_id) override;
    virtual int onKeyUp(int key_id) override;
    virtual int onTimer(float delta_time) override;
};
