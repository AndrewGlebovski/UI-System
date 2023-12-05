/**
 * \file
 * \brief Contains clock implementation
*/


#include <ctime>
#include "basic/clock.hpp"


// ============================================================================


Clock::Clock(
    size_t id_, const LayoutBox &layout_,
    const ClockStyle &style_
) :
    Widget(id_, layout_),
    daytime(0), style(style_), time_passed(0),
    text(sf::Text("00:00:00", style.font, style.font_size))
{
    time_t raw_time = time(NULL);
    struct tm *time_info = localtime(&raw_time);
    daytime = time_info->tm_hour * 3600 + time_info->tm_min * 60 + time_info->tm_sec;

    text.setColor(style.text_color);
}


void Clock::draw(TransformStack &stack, RenderTarget &result) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = applySize(stack, layout->getSize());

    size_t hours = (daytime / 3600) % 24;
    size_t minutes = (daytime / 60) % 60;
    size_t seconds = daytime % 60;

    char str[] = "00:00:00";
    sprintf(str, "%02lu:%02lu:%02lu", hours, minutes, seconds);

    text.setText(str);

    Vec2d text_size = applySize(stack, text.getTextureSize());

    text.draw(
        result,
        global_position + (global_size - text_size) / 2,
        text_size
    );
}


void Clock::onTick(const TickEvent &event, EHC &ehc) {
    time_passed += event.delta_time;

    if (time_passed > 1) {
        time_passed = 0;
        daytime++;
        daytime %= 24 * 60 * 60;
    }
}
