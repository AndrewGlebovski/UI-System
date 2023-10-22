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


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;


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
