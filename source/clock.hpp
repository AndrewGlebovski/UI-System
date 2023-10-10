class Clock : public BaseUI {
private:
    size_t daytime;
    ClockStyle style;

public:
    Clock(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        const ClockStyle &style_
    ) :
        BaseUI(position_, size_, z_index_, parent_),
        daytime(0), style(style_)
    {
        time_t raw_time = time(NULL);
        struct tm *time_info = localtime(&raw_time);
        daytime = time_info->tm_hour * 3600 + time_info->tm_min * 60 + time_info->tm_sec;
    }


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override {
        TransformApplier(transforms, position);

        size_t hours = (daytime / 3600) % 24;
        size_t minutes = (daytime / 60) % 60;
        size_t seconds = daytime % 60;

        char str[] = "00:00:00";
        sprintf(str, "%02lu:%02lu:%02lu", hours, minutes, seconds);

        sf::Text text(str, style.font, style.font_size);
        text.setFillColor(style.text_color);

        sf::FloatRect text_rect = text.getLocalBounds();
        Vector2D text_offset((size.x - text_rect.width) / 2, (size.y - text_rect.height) / 2);

        text.setPosition(transforms[0] + text_offset);

        result.draw(text);
    }


    virtual int onTimer(float delta_daytime) override {
        daytime++;
        daytime %= 24 * 60 * 60;

        return UNHANDLED;
    }
};
