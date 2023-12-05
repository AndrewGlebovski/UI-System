/**
 * \file
 * \brief Contains clock interface
*/


#ifndef _CLOCK_H_
#define _CLOCK_H_


#include "widget/widget.hpp"


/// Controls how clock looks
struct ClockStyle {
    Color text_color;       ///< Text color
    unsigned font_size;     ///< Font size
    const sf::Font &font;   ///< Font

    ClockStyle(
        Color text_color_, 
        unsigned font_size_, sf::Font &font_
    ) :
        text_color(text_color_), 
        font_size(font_size_), font(font_) 
    {}
};


/// Shows current time
class Clock : public Widget {
public:
    Clock(
        size_t id_, const LayoutBox &layout_,
        const ClockStyle &style_
    );

    virtual void draw(TransformStack &stack, RenderTarget &result) override;

protected:
    virtual void onTick(const TickEvent &event, EHC &ehc) override;

    size_t daytime;
    ClockStyle style;
    double time_passed;
    TextShape text;
};


#endif
