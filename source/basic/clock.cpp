/**
 * \file
 * \brief Contains clock implementation
*/


#include <SFML/Graphics.hpp>
#include <ctime>
#include "basic/clock.hpp"


// ============================================================================


Clock::Clock(
    size_t id_, const LayoutBox &layout_,
    const ClockStyle &style_
) :
    Widget(id_, layout_),
    daytime(0), style(style_), time_passed(0)
{
    time_t raw_time = time(NULL);
    struct tm *time_info = localtime(&raw_time);
    daytime = time_info->tm_hour * 3600 + time_info->tm_min * 60 + time_info->tm_sec;
}


void Clock::draw(sf::RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    size_t hours = (daytime / 3600) % 24;
    size_t minutes = (daytime / 60) % 60;
    size_t seconds = daytime % 60;

    char str[] = "00:00:00";
    sprintf(str, "%02lu:%02lu:%02lu", hours, minutes, seconds);

    sf::Text text(str, style.font, style.font_size);
    text.setFillColor(style.text_color);

    sf::FloatRect text_rect = text.getLocalBounds();
    Vec2d text_offset((global_size.x - text_rect.width) / 2, (global_size.y - text_rect.height) / 2);

    text.setPosition(global_position + text_offset);

    result.draw(text);
}


void Clock::onTick(const TickEvent &event, EHC &ehc) {
    time_passed += event.delta_time;

    if (time_passed > 1) {
        time_passed = 0;
        daytime++;
        daytime %= 24 * 60 * 60;
    }
}
