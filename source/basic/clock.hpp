/**
 * \file
 * \brief Contains clock interface
*/


#ifndef _CLOCK_H_
#define _CLOCK_H_


#include "widget/widget.hpp"


/// Controls how clock looks
struct ClockStyle {
    const sf::Font &font;       ///< Font
    unsigned font_size;         ///< Font size
    plug::Color text_color;     ///< Text color

    ClockStyle(
        sf::Font &font_,
        unsigned font_size_ = 20,
        plug::Color text_color_ = Black
    ) :
        font(font_),
        font_size(font_size_),
        text_color(text_color_)
    {}
};


/// Shows current time
class Clock : public Widget {
public:
    Clock(
        size_t id_, const plug::LayoutBox &layout_,
        const ClockStyle &style_
    );

    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;

protected:
    virtual void onTick(const plug::TickEvent &event, plug::EHC &ehc) override;

    size_t daytime;
    ClockStyle style;
    double time_passed;
    TextShape text;
};


#endif
