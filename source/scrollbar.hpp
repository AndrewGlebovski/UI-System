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


/// Can be scrolled up and down
class VScrollBar : public BaseUI {
protected:
    ScrollAction *action;               ///< Triggers when scroller moves
    ScrollBarStyle style;               ///< Style
    sf::RectangleShape scroller;        ///< Rectangle that represents scroller
    bool is_moving;                     ///< Scroller is moving right now
    Vector2D mouse_prev;                ///< Previous mouse click position

public:
    VScrollBar(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );


    VScrollBar(const VScrollBar &scrollbar) = delete;


    VScrollBar &operator = (const VScrollBar& scrollbar) = delete;


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;


    ~VScrollBar();
};


/// Can be scrolled left and right
class HScrollBar : public BaseUI {
protected:
    ScrollAction *action;               ///< Triggers when scroller moves
    ScrollBarStyle style;               ///< Style
    sf::RectangleShape scroller;        ///< Rectangle that represents scroller
    bool is_moving;                     ///< Scroller is moving right now
    Vector2D mouse_prev;                ///< Previous mouse click position

public:
    HScrollBar(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );


    HScrollBar(const HScrollBar &scrollbar) = delete;


    HScrollBar &operator = (const HScrollBar& scrollbar) = delete;


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;


    ~HScrollBar();
};
