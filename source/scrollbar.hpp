/**
 * \file
 * \brief Contains scrollbar and scrollbar action classes and protypes of their functions 
*/


/// Triggers when scroller moves
class ScrollAction {
public:
    /**
     * \brief Calls when scroller moves
     * \param [in]  param   Real number from [0;1] respreseting scroller position
    */
    virtual void operator () (vec_t param) = 0;


    /**
     * \brief Default destructor
    */
    virtual ~ScrollAction() = default;
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


/// Base class for scroll bars
class ScrollBar : public Widget {
protected:
    ScrollAction *action;               ///< Triggers when scroller moves
    ScrollBarStyle style;               ///< Style
    sf::RectangleShape scroller;        ///< Rectangle that represents scroller
    bool is_moving;                     ///< Scroller is moving right now
    Vector2D mouse_prev;                ///< Previous mouse click position


    virtual void scrollTo(const Vector2D &mouse) = 0;

public:
    ScrollBar(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );


    ScrollBar(const ScrollBar &scrollbar) = delete;
    ScrollBar &operator = (const ScrollBar& scrollbar) = delete;


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;


    virtual ~ScrollBar();
};


/// Can be scrolled up and down
class VScrollBar : public ScrollBar {
protected:
    virtual void scrollTo(const Vector2D &mouse) override;

public:
    VScrollBar(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );


    virtual int onParentResize() override;
};


/// Can be scrolled left and right
class HScrollBar : public ScrollBar {
protected:
    virtual void scrollTo(const Vector2D &mouse) override;

public:
    HScrollBar(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );


    virtual int onParentResize() override;
};
