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


/// Contains window style
class WindowStyle {
public:
    sf::Color frame_color;      ///< Window frame color
    sf::Color title_color;      ///< Window title color
    float frame_outline;        ///< Frame outline thickness
    float title_bar_height;     ///< Window title bar height
    unsigned font_size;         ///< Title font size
    const sf::Font &font;       ///< Title font


    WindowStyle(
        const sf::Color &frame_color_, const sf::Color &title_color_, 
        float frame_outline_, float title_bar_height_, 
        unsigned font_size_, const sf::Font &font_
    ) :
        frame_color(frame_color_), title_color(title_color_),
        frame_outline(frame_outline_), title_bar_height(title_bar_height_),
        font_size(font_size_), font(font_) 
    {}
};


/// Contains scroll bar style
class ScrollBarStyle {
public:
    sf::Color frame_color;
    vec_t frame_outline;
    sf::Color background_color;
    sf::Color scroller_color;
    vec_t scroller_factor;


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
