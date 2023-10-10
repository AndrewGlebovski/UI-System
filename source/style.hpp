/**
 * \file
 * \brief Contains style classes
*/


/// Contains texture button style
class ButtonStyle {
public:
    sf::Color text_color;       ///< Text color
    unsigned font_size;         ///< Font size
    const sf::Font &font;       ///< Font


    ButtonStyle(
        const sf::Color &text_color_, 
        unsigned font_size_, sf::Font &font_
    ) :
        text_color(text_color_), 
        font_size(font_size_), font(font_) 
    {}
};


/// Controls how window looks
class WindowStyle {
public:
    sf::Color title_color;      ///< Title color
    Vector2D title_offset;      ///< Title offset from top-left corner
    unsigned font_size;         ///< Title font size
    const sf::Font &font;       ///< Title font
    const WindowAsset &asset;   ///< Textures used in window
    float outline;              ///< Describes size of window resizing buttons
    Vector2D tl_offset;         ///< Window inner area offset from top-left corner
    Vector2D br_offset;         ///< Window inner area offset from bottom-right corner


    WindowStyle(
        const sf::Color &title_color_, const Vector2D &title_offset_,
        unsigned font_size_, const sf::Font &font_,
        const WindowAsset &asset_, float outline_, 
        const Vector2D &tl_offset_, const Vector2D &br_offset_
    ) :
        title_color(title_color_), title_offset(title_offset_),
        font_size(font_size_), font(font_),
        asset(asset_), outline(outline_),
        tl_offset(tl_offset_), br_offset(br_offset_)
    {}
};


/// Contains scroll bar style
class ScrollBarStyle {
public:
    sf::Color frame_color;          ///< Frame color
    vec_t frame_outline;            ///< Frame outline thickness
    sf::Color background_color;     ///< Background color
    sf::Color scroller_color;       ///< Scroller
    vec_t scroller_factor;          ///< Scroller height = factor * scrollbar.size

    ScrollBarStyle(
        const sf::Color &frame_color_, vec_t frame_outline_,
        const sf::Color &background_color_,
        const sf::Color &scroller_color_, vec_t scroller_factor_
    ) :
        frame_color(frame_color_), frame_outline(frame_outline_),
        background_color(background_color_),
        scroller_color(scroller_color_), scroller_factor(scroller_factor_) 
    {}
};


/// Controls how clock looks
class ClockStyle {
public:
    sf::Color text_color;       ///< Text color
    unsigned font_size;         ///< Font size
    const sf::Font &font;       ///< Font


    ClockStyle(
        const sf::Color &text_color_, 
        unsigned font_size_, sf::Font &font_
    ) :
        text_color(text_color_), 
        font_size(font_size_), font(font_) 
    {}
};
