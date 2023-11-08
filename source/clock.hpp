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


/// Shows current time
class Clock : public Widget {
private:
    size_t daytime;
    ClockStyle style;
    float time_passed;

public:
    Clock(
        size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_,
        const ClockStyle &style_
    ) :
        Widget(id_, transform_, size_, z_index_, parent_),
        daytime(0), style(style_), time_passed(0)
    {
        time_t raw_time = time(NULL);
        struct tm *time_info = localtime(&raw_time);
        daytime = time_info->tm_hour * 3600 + time_info->tm_min * 60 + time_info->tm_sec;
    }


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override {
        TransformApplier(transforms, transform);

        size_t hours = (daytime / 3600) % 24;
        size_t minutes = (daytime / 60) % 60;
        size_t seconds = daytime % 60;

        char str[] = "00:00:00";
        sprintf(str, "%02lu:%02lu:%02lu", hours, minutes, seconds);

        sf::Text text(str, style.font, style.font_size);
        text.setFillColor(style.text_color);

        sf::FloatRect text_rect = text.getLocalBounds();
        Vec2d text_offset((size.x - text_rect.width) / 2, (size.y - text_rect.height) / 2);

        text.setPosition(transforms.front().offset + text_offset);

        result.draw(text);
    }


    virtual EVENT_STATUS onTimer(float delta_time) override {
        time_passed += delta_time;

        if (time_passed > 1) {
            time_passed = 0;
            daytime++;
            daytime %= 24 * 60 * 60;
        }

        return UNHANDLED;
    }
};
