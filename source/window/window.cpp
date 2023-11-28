/**
 * \file
 * \brief Contains sources of window class functions
*/


#include "window/window.hpp"


const float MIN_OUTLINE = 0.0001f;                          ///< If outline is smaller then window can not be resized
const Vec2d CLOSE_OFFSET = Vec2d(-45, 12);                  ///< Window close button offset from window top-right corner
const Vec2d EXPAND_OFFSET = Vec2d(-85, 12);                 ///< Window expand button offset from window top-right corner
const size_t CLOSE_BUTTON_ID = Widget::AUTO_ID + 1;         ///< Window append button ID
const size_t EXPAND_BUTTON_ID = Widget::AUTO_ID + 2;        ///< Window close button ID
const size_t BUTTONS_ID = Widget::AUTO_ID + 3;              ///< Window buttons container ID
const size_t CONTAINER_ID = Widget::AUTO_ID + 4;            ///< Window container ID


// ============================================================================


class MoveLayoutBox;


/// Invisible button for moving windows
class MoveButton : public BaseButton {
public:
    friend MoveLayoutBox;

    MoveButton(size_t id_, Window &window_);

    /**
     * \brief Draws cyan rectangle for debug purposes
    */
    void draw(RenderTarget &result, TransformStack &stack) override;

protected:
    Window &window;         ///< Window to move
    Vec2d prev_mouse;       ///< Previous mouse click position
    bool is_moving;         ///< If moving is active

    void onMouseMove(const MouseMoveEvent &event, EHC &ehc) override;
    void onMousePressed(const MousePressedEvent &event, EHC &ehc) override;
    void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) override;
};


// ============================================================================


class MoveLayoutBox : public BoundLayoutBox {
private:
    MoveButton &btn;

public:
    MoveLayoutBox(MoveButton &btn_);

    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    virtual LayoutBox *clone() const override;
};


// ============================================================================


class ResizeLayoutBox;


/// Invisible button for resizing windows
class ResizeButton : public BaseButton {
public:
    friend ResizeLayoutBox;

    enum RESIZE_DIRECTION {
        LEFT,
        TOP,
        BOTTOM,
        RIGHT,
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };

    ResizeButton(size_t id_, Window &window_, RESIZE_DIRECTION resize_dir_);

    /**
     * \brief Draws red rectangle for debug purposes
    */
    void draw(RenderTarget &result, TransformStack &stack) override;

protected:
    Window &window;         ///< Window to move
    Vec2d prev_mouse;       ///< Previous mouse click position
    bool is_moving;         ///< If moving is active
    const int resize_dir;   ///< Resize direction

    void onMouseMove(const MouseMoveEvent &event, EHC &ehc) override;
    void onMousePressed(const MousePressedEvent &event, EHC &ehc) override;
    void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) override;
};


// ============================================================================


class ResizeLayoutBox : public BoundLayoutBox {
private:
    ResizeButton &btn;

public:
    ResizeLayoutBox(ResizeButton &btn_);

    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    virtual LayoutBox *clone() const override;
};


// ============================================================================


class ContainerLayoutBox : public BoundLayoutBox {
private:
    Window &window;

public:
    ContainerLayoutBox(Window &window_);

    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    virtual LayoutBox *clone() const override;
};


// ============================================================================


/// Sets widget status as DELETE
class CloseAction : public ButtonAction {
private:
    Widget &widget;

public:
    CloseAction(Widget &widget_) : widget(widget_) {}


    void operator () () override {
        widget.setStatus(Widget::DELETE);
    }


    ButtonAction *clone() override {
        return new CloseAction(widget);
    }
};


// ============================================================================


/// Maximizes widget size
class ExpandAction : public ButtonAction {
private:
    Window &window;         ///< Window to expand
    Vec2d prev_pos;         ///< Window position before expansion
    Vec2d prev_size;        ///< Window size before expansion

public:
    ExpandAction(Window &window_) :
        window(window_),
        prev_pos(window.getLayoutBox().getPosition()),
        prev_size(window.getLayoutBox().getSize())
    {}

    void operator () () override {
        Vec2d prev_pos_ = window.getLayoutBox().getPosition();
        Vec2d prev_size_ = window.getLayoutBox().getSize();

        window.setPosition(Vec2d());
        if (window.setSize(Vec2d(SCREEN_W, SCREEN_H))) {
            prev_pos = prev_pos_;
            prev_size = prev_size_;
        }
        else {
            window.setSize(prev_size);
            window.setPosition(prev_pos);
        }
    }

    ButtonAction *clone() override {
        return new ExpandAction(window);
    }
};


// ============================================================================


Window::Window(
    size_t id_, const LayoutBox &layout_,
    const std::string &title_,
    const WindowStyle &style_,
    bool can_resize,
    bool can_move,
    bool can_close
) :
    Widget(id_, layout_),
    style(style_),
    buttons(
        BUTTONS_ID,
        AnchorLayoutBox(
            Vec2d(),
            Vec2d(SCREEN_W, SCREEN_H),
            Vec2d(),
            Vec2d(SCREEN_W, SCREEN_H)
        ),
        false
    ),
    container(CONTAINER_ID, ContainerLayoutBox(*this)),
    menu(nullptr),
    title(sf::Text(title_, style.font, style.font_size))
{
    title.setColor(style.title_color);

    buttons.setParent(this);
    container.setParent(this);

    addButtons(can_resize, can_move, can_close);
}


void Window::addButtons(bool can_resize, bool can_move, bool can_close) {
    if (can_move) addMoveButton();
    if (can_close) addCloseButton();
    if (can_resize) {
        addExpandButton();
        addResizeButtons();
    }
}


void Window::addCloseButton() {
    TextureIconButton *close_btn = new TextureIconButton(
        CLOSE_BUTTON_ID,
        AnchorLayoutBox(
            CLOSE_OFFSET,
            Vec2d(),
            Vec2d(SCREEN_W, 0),
            Vec2d()
        ),
        new CloseAction(*this),
        style.asset[WindowAsset::BUTTON_NORMAL],
        style.asset[WindowAsset::BUTTON_HOVER],
        style.asset[WindowAsset::BUTTON_PRESSED],
        style.asset[WindowAsset::CLOSE_ICON]
    );

    close_btn->setZIndex(1);

    buttons.addChild(close_btn);
}


void Window::addExpandButton() {
    TextureIconButton *expand_btn = new TextureIconButton(
        EXPAND_BUTTON_ID,
        AnchorLayoutBox(
            EXPAND_OFFSET,
            Vec2d(),
            Vec2d(SCREEN_W, 0),
            Vec2d()
        ),
        new ExpandAction(*this),
        style.asset[WindowAsset::BUTTON_NORMAL],
        style.asset[WindowAsset::BUTTON_HOVER],
        style.asset[WindowAsset::BUTTON_PRESSED],
        style.asset[WindowAsset::EXPAND_ICON]
    );

    expand_btn->setZIndex(1);

    buttons.addChild(expand_btn);
}


void Window::addMoveButton() {
    buttons.addChild(new MoveButton(Widget::AUTO_ID, *this));
}


void Window::addResizeButtons() {
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::LEFT));
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::TOP));
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::BOTTOM));
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::RIGHT));
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::TOP_LEFT));
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::TOP_RIGHT));
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::BOTTOM_LEFT));
    buttons.addChild(new ResizeButton(Widget::AUTO_ID, *this, ResizeButton::BOTTOM_RIGHT));
}


Vec2d Window::getAreaPosition() const {
    Vec2d area_position = style.tl_offset;
    if (menu) area_position.y += menu->getLayoutBox().getSize().y;
    return area_position;
}


Vec2d Window::getAreaSize() const {
    Vec2d area_size = layout->getSize() - style.tl_offset - style.br_offset;
    if (menu) area_size.y -= menu->getLayoutBox().getSize().y;
    return area_size;
}


#define DRAW_TEXTURE(TEXTURE_ID, POSITION, TEXTURE_RECT_SIZE) \
    TextureShape(style.asset[TEXTURE_ID]).draw(result, POSITION + global_position, stack.apply_size(TEXTURE_RECT_SIZE))


void Window::draw(RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());

    Vec2d tl_size = Vec2d(style.asset[WindowAsset::FRAME_TL].width, style.asset[WindowAsset::FRAME_TL].height);
    Vec2d br_size = Vec2d(style.asset[WindowAsset::FRAME_BL].width, style.asset[WindowAsset::FRAME_BL].height);

    vec_t center_h = layout->getSize().y - tl_size.y - br_size.y;
    vec_t center_w = layout->getSize().x - tl_size.x - br_size.x;

    DRAW_TEXTURE(WindowAsset::FRAME_TL,     Vec2d(),                                            tl_size);
    DRAW_TEXTURE(WindowAsset::FRAME_L,      Vec2d(0, tl_size.y),                                Vec2d(tl_size.x, center_h));
    DRAW_TEXTURE(WindowAsset::FRAME_BL,     Vec2d(0, tl_size.y + center_h),                     Vec2d(tl_size.x, br_size.y));
    DRAW_TEXTURE(WindowAsset::FRAME_B,      Vec2d(tl_size.x, tl_size.y + center_h),             Vec2d(center_w, br_size.y));
    DRAW_TEXTURE(WindowAsset::FRAME_BR,     Vec2d(tl_size.x + center_w, tl_size.y + center_h),  br_size);
    DRAW_TEXTURE(WindowAsset::FRAME_R,      Vec2d(tl_size.x + center_w, tl_size.y),             Vec2d(br_size.x, center_h));
    DRAW_TEXTURE(WindowAsset::FRAME_TR,     Vec2d(tl_size.x + center_w, 0),                     Vec2d(br_size.x, tl_size.y));
    DRAW_TEXTURE(WindowAsset::TITLE,        Vec2d(tl_size.x, 0),                                Vec2d(center_w, tl_size.y));
    DRAW_TEXTURE(WindowAsset::FRAME_CENTER, tl_size,                                            Vec2d(center_w, center_h));

    title.draw(result, global_position + style.title_offset - title.getTextOffset(), stack.apply_size(title.getTextureSize()));

    TransformApplier add_transform(stack, getTransform());

    container.draw(result, stack);
    buttons.draw(result, stack);

    if (menu) menu->draw(result, stack);
}


#undef DRAW_TEXTURE


void Window::setMenu(Menu *menu_) {
    if (menu) delete menu;

    menu = menu_;
    menu->getLayoutBox().setPosition(style.tl_offset);
    menu->getLayoutBox().setSize(Vec2d(getAreaSize().x, menu->getLayoutBox().getSize().y));
    menu->setParent(this);

    container.onParentUpdate(*layout);
}


Menu *Window::getMenu() {
    return menu;
}


Widget *Window::findWidget(size_t widget_id) {
    Widget *result = container.findWidget(widget_id);
    if (result) return result;
    return Widget::findWidget(widget_id);
}


size_t Window::addChild(Widget *child) {
    return container.addChild(child);
}


void Window::removeChild(size_t child_id) {
    container.removeChild(child_id);
}


#define CHECK_EHC(CALL_FUNC)    \
do {                            \
    CALL_FUNC;                  \
    if (ehc.stopped) {          \
        ehc.stack.leave();      \
        return;                 \
    }                           \
} while(0)


void Window::onEvent(const Event &event, EHC &ehc) {
    ehc.stack.enter(getTransform());

    CHECK_EHC(buttons.onEvent(event, ehc));

    if (menu) CHECK_EHC(menu->onEvent(event, ehc));

    CHECK_EHC(container.onEvent(event, ehc));

    ehc.stack.leave();

    Widget::onEvent(event, ehc);
}


#undef CHECK_EHC


void Window::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos))
        ehc.overlapped = true;
}


void Window::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos))
        ehc.stopped = true;
}


bool Window::setPosition(const Vec2d &position_) {
    return layout->setPosition(position_);
}


bool Window::setSize(const Vec2d &size_) {
    if (!layout->setSize(size_)) return false;

    buttons.onParentUpdate(*layout);
    container.onParentUpdate(*layout);
    
    if (menu)
        menu->getLayoutBox().setSize(Vec2d(getAreaSize().x, menu->getLayoutBox().getSize().y));
    
    return true;
}


void Window::onParentUpdate(const LayoutBox &parent_layout) {
    layout->onParentUpdate(parent_layout);

    buttons.onParentUpdate(*layout);
    container.onParentUpdate(*layout);
    
    if (menu)
        menu->getLayoutBox().setSize(Vec2d(getAreaSize().x, menu->getLayoutBox().getSize().y));
}


void Window::checkChildren() {
    container.checkChildren();
}


Window::~Window() {
    if (menu) delete menu;
}


// ============================================================================


MainWindow::MainWindow(
    size_t id_, const LayoutBox &layout_,
    const std::string &title_, const WindowStyle &style_
) :
    Window(id_, layout_, title_, style_)
{}


void MainWindow::onParentUpdate(const LayoutBox &parent_layout) {
    printf("Main window does not support parent update!\n");
    abort();
}


void MainWindow::parseEvent(const sf::Event &event, TransformStack &stack) {
    static bool shift = false, ctrl = false, alt = false;

    EHC ehc(stack, false, false);

    switch (event.type) {
        case sf::Event::KeyPressed: {
            switch (KEY_ID(event.key.code)) {
                case LShift:
                case RShift:
                    shift = true; break;
                case LControl:
                case RControl:
                    ctrl = true; break;
                case LAlt:
                case RAlt:
                    alt = true; break;
                default:
                    break;
            }

            KeyboardPressedEvent event_(KEY_ID(event.key.code), shift, ctrl, alt);

            onEvent(event_, ehc); break;
        }
        case sf::Event::KeyReleased: {
            switch (KEY_ID(event.key.code)) {
                case LShift:
                case RShift:
                    shift = false; break;
                case LControl:
                case RControl:
                    ctrl = false; break;
                case LAlt:
                case RAlt:
                    alt = false; break;
                default:
                    break;
            }

            KeyboardReleasedEvent event_(KEY_ID(event.key.code), shift, ctrl, alt);
            onEvent(event_, ehc); break;
        }
        case sf::Event::MouseButtonPressed: {
            MousePressedEvent event_(
                MOUSE_BUTTON_ID(event.mouseButton.button),
                shift, ctrl, alt,
                Vec2d(event.mouseButton.x, event.mouseButton.y)
            );
            onEvent(event_, ehc); break;
        }
        case sf::Event::MouseButtonReleased: {
            MouseReleasedEvent event_(
                MOUSE_BUTTON_ID(event.mouseButton.button),
                shift, ctrl, alt,
                Vec2d(event.mouseButton.x, event.mouseButton.y)
            );
            onEvent(event_, ehc); break;
        }
        case sf::Event::MouseMoved: {
            MouseMoveEvent event_(
                shift, ctrl, alt,
                Vec2d(event.mouseMove.x, event.mouseMove.y)
            );
            onEvent(event_, ehc); break;
        }
        default:
            return;
    }
}


// ============================================================================


MoveButton::MoveButton(
    size_t id_, Window &window_
) :
    BaseButton(id_, MoveLayoutBox(*this)),
    window(window_), prev_mouse(Vec2d()), is_moving(false)
{}


void MoveButton::draw(RenderTarget &result, TransformStack &stack) {
# ifdef DEBUG_DRAW
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    RectShape rect(global_position, global_size, Cyan);
    rect.draw(result);
#endif
}


void MoveButton::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    if (!is_moving) return;

    // WE CHANGED WINDOW POSITION SO CURRENT TRANSFORM WILL BE INCORRECT
    // BUT IT DOESN'T MATTER CAUSE EVENT IS HANDLED
    // AND FARTHER BROADCASTING IS NOT NEEDED

    Vec2d new_position = window.getLayoutBox().getPosition() + (event.pos - prev_mouse);
    prev_mouse = event.pos;

    window.setPosition(new_position);

    ehc.overlapped = true;
}


void MoveButton::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos)) {
        is_moving = true;
        prev_mouse = event.pos;
        ehc.stopped = true;
    }
}


void MoveButton::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    is_moving = false;
}


// ============================================================================


MoveLayoutBox::MoveLayoutBox(MoveButton &btn_) : btn(btn_) {}


void MoveLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {
    float outline = btn.window.getStyle().outline;
    Vec2d window_size = btn.window.getLayoutBox().getSize();

    position = Vec2d(1, 1) * outline;
    size = Vec2d(window_size.x - 2 * outline, btn.window.getStyle().tl_offset.y - outline);
}


LayoutBox *MoveLayoutBox::clone() const {
    return new MoveLayoutBox(*this);
}


// ============================================================================


ResizeButton::ResizeButton(
    size_t id_, Window &window_, RESIZE_DIRECTION resize_dir_
) :
    BaseButton(id_, ResizeLayoutBox(*this)),
    window(window_), prev_mouse(Vec2d()), is_moving(false), resize_dir(resize_dir_)
{}


void ResizeButton::draw(RenderTarget &result, TransformStack &stack) {
# ifdef DEBUG_DRAW
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    RectShape rect(global_position, global_size, Red);
    rect.draw(result);
#endif
}


void ResizeButton::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    if (!is_moving) return;

    Vec2d shift = event.pos - prev_mouse;
    prev_mouse = event.pos;

    Vec2d prev_position = window.getLayoutBox().getPosition();
    Vec2d prev_size = window.getLayoutBox().getSize();

    switch (resize_dir) {
        case LEFT: 
            shift.y = 0;
            if (window.setPosition(prev_position + shift)) {
                shift = window.getLayoutBox().getPosition() - prev_position;
                window.setSize(prev_size - shift);
            }
            break;
        case TOP:
            shift.x = 0;
            if (window.setPosition(prev_position + shift)) {
                shift = window.getLayoutBox().getPosition() - prev_position;
                window.setSize(prev_size - shift);
            }
            break;
        case BOTTOM:
            shift.x = 0;
            window.setSize(prev_size + shift);
            break;
        case RIGHT:
            shift.y = 0;
            window.setSize(prev_size + shift);
            break;
        case TOP_LEFT:
            if (window.setPosition(prev_position + shift)) {
                shift = window.getLayoutBox().getPosition() - prev_position;
                window.setSize(prev_size - shift);
            }
            break;
        case TOP_RIGHT:
            if (window.setPosition(prev_position + Vec2d(0, shift.y))) {
                shift.y = window.getLayoutBox().getPosition().y - prev_position.y;
                window.setSize(prev_size + Vec2d(shift.x, -shift.y));
            }
            break;
        case BOTTOM_LEFT:
            if (window.setPosition(prev_position + Vec2d(shift.x, 0))) {
                shift.x = window.getLayoutBox().getPosition().x - prev_position.x;
                window.setSize(prev_size + Vec2d(-shift.x, shift.y));
            }
            break;
        case BOTTOM_RIGHT:
            window.setSize(prev_size + shift);
            break;
        default:
            printf("Unknown resize direction!\n"); abort();
    }

    ehc.overlapped = true;
}


void ResizeButton::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos)) {
        is_moving = true;
        prev_mouse = event.pos;
        ehc.stopped = true;
    }
}


void ResizeButton::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    is_moving = false;
}


// ============================================================================


ResizeLayoutBox::ResizeLayoutBox(ResizeButton &btn_) : btn(btn_) {}


void ResizeLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {
    float outline = btn.window.getStyle().outline;
    Vec2d window_size = btn.window.getLayoutBox().getSize();

    switch (btn.resize_dir) {
        case ResizeButton::LEFT:
            position = Vec2d(0, outline);
            size = Vec2d(outline, window_size.y - 2 * outline);
            break;
        case ResizeButton::TOP:
            position = Vec2d(outline, 0);
            size = Vec2d(window_size.x - 2 * outline, outline);
            break;
        case ResizeButton::BOTTOM:
            position = Vec2d(outline, window_size.y - outline);
            size = Vec2d(window_size.x - 2 * outline, outline);
            break;
        case ResizeButton::RIGHT:
            position = Vec2d(window_size.x - outline, outline);
            size = Vec2d(outline, window_size.y - 2 * outline);
            break;
        case ResizeButton::TOP_LEFT:
            position = Vec2d(0, 0);
            size = Vec2d(outline, outline);
            break;
        case ResizeButton::TOP_RIGHT:
            position = Vec2d(window_size.x - outline, 0);
            size = Vec2d(outline, outline);
            break;
        case ResizeButton::BOTTOM_LEFT:
            position = Vec2d(0, window_size.y - outline);
            size = Vec2d(outline, outline);
            break;
        case ResizeButton::BOTTOM_RIGHT:
            position = Vec2d(window_size.x - outline, window_size.y - outline);
            size = Vec2d(outline, outline);
            break;
        default:
            printf("Unknown resize direction!\n"); abort();
    }
}


LayoutBox *ResizeLayoutBox::clone() const {
    return new ResizeLayoutBox(*this);
}


// ============================================================================


ContainerLayoutBox::ContainerLayoutBox(Window &window_) :
    window(window_) {}


void ContainerLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {
    bound = window.getAreaSize();
    position = window.getAreaPosition();
    size = bound;
}


LayoutBox *ContainerLayoutBox::clone() const {
    return new ContainerLayoutBox(window);
}
