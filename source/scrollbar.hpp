/**
 * \file
 * \brief Contains scrollbar and scrollbar action classes and protypes of their functions 
*/


class ScrollAction {
public:
    virtual void operator () (vec_t param) = 0;


    virtual ~ScrollAction() = default;
};


/// Allows to move scroller
class VScrollBar : public BaseUI {
protected:
    ScrollAction *action;
    ScrollBarStyle style;
    sf::RectangleShape scroller;
    bool is_moving;
    Vector2D mouse_prev;

public:
    VScrollBar(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
};


/// Allows to move scroller
class HScrollBar : public BaseUI {
protected:
    ScrollAction *action;
    ScrollBarStyle style;
    sf::RectangleShape scroller;
    bool is_moving;
    Vector2D mouse_prev;

public:
    HScrollBar(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
};
