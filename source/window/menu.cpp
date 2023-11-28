/**
 * \file
 * \brief Contains sources of menu class functions
*/


#include <limits>
#include "window/menu.hpp"


const size_t INVALID_OPENED = std::numeric_limits<size_t>::max();   ///< No opened menu at the moment
const Vec2d ADD_SIZE(20, 20);                                       ///< Additional size to text rect


// ============================================================================


MenuButton::MenuButton(
    size_t id_, const LayoutBox &layout_,
    const std::string &text_, const RectButtonStyle &style_
) : 
    RectButton(AUTO_ID, layout_, nullptr, text_, style_), 
    buttons(), is_opened(false)
{}


void MenuButton::addButton(const std::string &text_, ButtonAction *action_) {
    ASSERT(action_, "Option button action is nullptr!\n");

    Widget *prev_btn = (buttons.size()) ? buttons.back() : nullptr;

    sf::Text btn_text(text_, style.font, style.font_size);
    Vec2d btn_size = Vec2d(btn_text.getLocalBounds().width + ADD_SIZE.x, getLayoutBox().getSize().y);
    Vec2d btn_pos = Vec2d();

    if (prev_btn) {
        btn_pos.y = prev_btn->getLayoutBox().getPosition().y + prev_btn->getLayoutBox().getSize().y;

        if (btn_size.x > prev_btn->getLayoutBox().getSize().x) {
            for (size_t i = 0; i < buttons.size(); i++)
                buttons[i]->getLayoutBox().setSize(btn_size);
        }
        else btn_size.x = prev_btn->getLayoutBox().getSize().x;
    }
    else btn_pos.y = getLayoutBox().getSize().y;

    RectButton *btn = new RectButton(
        AUTO_ID,
        LazyLayoutBox(btn_pos, btn_size),
        action_,
        text_,
        style
    );

    btn->setParent(this);

    buttons.push_back(btn);
}


void MenuButton::setOpened(bool is_opened_) {
    is_opened = is_opened_;
}


void MenuButton::draw(RenderTarget &result, TransformStack &stack) {
    RectButton::draw(result, stack);

    TransformApplier add_transform(stack, getTransform());

    if (is_opened) {
        for (size_t i = 0; i < buttons.size(); i++)
            buttons[i]->draw(result, stack);
    }
}


void MenuButton::onEvent(const Event &event, EHC &ehc) {
    RectButton::onEvent(event, ehc);
    if (ehc.stopped) return;

    if (!is_opened) return;

    TransformApplier add_transform(ehc.stack, getTransform());

    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i]->onEvent(event, ehc);
        if (ehc.stopped) break;
    }
}


MenuButton::~MenuButton() {
    for (size_t i = 0; i < buttons.size(); i++) {
        ASSERT(buttons[i], "Button is nullptr!\n");
        delete buttons[i];
    }
}


// ============================================================================


Menu::Menu(
    size_t id_, const LayoutBox &layout_,
    const RectButtonStyle &style_, Color background_
) :
    Widget(id_, layout_),
    buttons(), style(style_), background(background_) ,opened(INVALID_OPENED)
{
    sf::Text btn_text("Test", style.font, style.font_size);

    Vec2d auto_size(layout->getSize().x, btn_text.getLocalBounds().height + ADD_SIZE.y);

    layout->setSize(auto_size);
}


bool Menu::isMenuOpened() const {
    return (opened < INVALID_OPENED);
}


void Menu::openMenu(size_t menu_id) {
    ASSERT(menu_id < buttons.size(), "Invalid menu id!\n");

    if (opened == menu_id) {
        buttons[opened]->setOpened(false);
        opened = INVALID_OPENED;
    }
    else if (isMenuOpened()) {
        buttons[opened]->setOpened(false);
        buttons[menu_id]->setOpened(true);
        opened = menu_id;
    }
    else {
        buttons[menu_id]->setOpened(true);
        opened = menu_id;
    }
}


void Menu::addMenuButton(const std::string &text) {
    MenuButton *prev_btn = buttons.size() ? buttons.back() : nullptr;

    sf::Text btn_text(text, style.font, style.font_size);
    Vec2d btn_size(btn_text.getLocalBounds().width + ADD_SIZE.x, layout->getSize().y);
    Vec2d btn_pos;

    if (prev_btn)
        btn_pos.x = prev_btn->getLayoutBox().getPosition().x + prev_btn->getLayoutBox().getSize().x;

    MenuButton *btn = new MenuButton(
        AUTO_ID,
        LazyLayoutBox(btn_pos, btn_size),
        text,
        style
    );

    btn->setParent(this);

    buttons.push_back(btn);
}


void Menu::addButton(size_t menu_id, const std::string &text, ButtonAction *action) {
    ASSERT(menu_id < buttons.size(), "Invalid menu id!\n");
    ASSERT(action, "Option button action is nullptr!\n");
    buttons[menu_id]->addButton(text, action);
}


void Menu::draw(RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    RectShape rect(global_position, global_size, background);
#ifdef DEBUG_DRAW
    rect.setBorder(1, Cyan);
#endif
    rect.draw(result);

    TransformApplier add_transform(stack, getTransform());

    for (size_t i = 0; i < buttons.size(); i++)
        buttons[i]->draw(result, stack);
}


void Menu::onEvent(const Event &event, EHC &ehc) {
    TransformApplier add_transform(ehc.stack, getTransform());

    for (size_t i = 0; i < buttons.size(); i++) {
        buttons[i]->onEvent(event, ehc);
        if (ehc.stopped) {
            if (event.getType() == MousePressed) openMenu(i);
            break;
        }
    }
}


Menu::~Menu() {
    for (size_t i = 0; i < buttons.size(); i++) {
        ASSERT(buttons[i], "MenuButton is nullptr!\n");
        delete buttons[i];
    }
}
