/**
 * \file
 * \brief Contains sources of dialogs classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "asset.hpp"
#include "configs.hpp"
#include "widget.hpp"
#include "container.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "window.hpp"
#include "line-edit.hpp"
#include "dialog.hpp"


const sf::Color BUTTON_NORMAL_COLOR(0xd4d0c8ff);
const sf::Color BUTTON_HOVER_COLOR(0x000080ff);
const sf::Color BUTTON_PRESSED_COLOR(0x000080ff);


// ============================================================================


Dialog::Dialog(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    const sf::String &title_, const WindowStyle &style_
) :
    Window(id_, transform_, size_, z_index_, parent_, title_, style_, false, true, false)
{}


EVENT_STATUS Dialog::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    Window::onMouseMove(mouse_x, mouse_y, transforms);
    return HANDLED;
}


EVENT_STATUS Dialog::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    Window::onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);
    return HANDLED;
}


EVENT_STATUS Dialog::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    Window::onMouseButtonDown(mouse_x, mouse_y, button_id,transforms);
    return HANDLED;
}


EVENT_STATUS Dialog::onKeyUp(int key_id) {
    Window::onKeyUp(key_id);
    return HANDLED;
}


EVENT_STATUS Dialog::onKeyDown(int key_id) {
    Window::onKeyDown(key_id);
    return HANDLED;
}


// ============================================================================


ConfirmDialog::ConfirmDialog(
    size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
    const std::string &title_,
    DialogAction *ok_action_,
    DialogAction *cancel_action_,
    const WindowStyle &window_style_,
    const ButtonStyle &button_style_
) :
    Dialog(id_, transform_, Vector2D(200, 200), z_index_, parent_, title_, window_style_)
{
    ok_action_->setDialog(*this);
    cancel_action_->setDialog(*this);

    container.addChild(new RectButton(
        1,
        Transform(),
        Vector2D(100, 50),
        0,
        nullptr,
        ok_action_,
        nullptr,
        "Ok",
        button_style_,
        BUTTON_NORMAL_COLOR,
        BUTTON_HOVER_COLOR,
        BUTTON_PRESSED_COLOR
    ));

    container.addChild(new RectButton(
        2,
        Transform(Vector2D(120, 0)),
        Vector2D(100, 50),
        0,
        nullptr,
        cancel_action_,
        nullptr,
        "Cancel",
        button_style_,
        BUTTON_NORMAL_COLOR,
        BUTTON_HOVER_COLOR,
        BUTTON_PRESSED_COLOR
    ));
}


// ============================================================================


SelectFileDialog::SelectFileDialog(
    size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
    const std::string &title_,
    DialogAction *select_action_,
    DialogAction *cancel_action_,
    const WindowStyle &window_style_,
    const ButtonStyle &button_style_,
    const LineEditStyle &line_edit_style_
) :
    Dialog(id_, transform_, Vector2D(400, 200), z_index_, parent_, title_, window_style_)
{
    select_action_->setDialog(*this);
    cancel_action_->setDialog(*this);

    container.addChild(new RectButton(
        1,
        Transform(Vector2D(0, 50)),
        Vector2D(100, 50),
        0,
        nullptr,
        select_action_,
        nullptr,
        "Select",
        button_style_,
        BUTTON_NORMAL_COLOR,
        BUTTON_HOVER_COLOR,
        BUTTON_PRESSED_COLOR
    ));

    container.addChild(new RectButton(
        2,
        Transform(Vector2D(120, 50)),
        Vector2D(100, 50),
        0,
        nullptr,
        cancel_action_,
        nullptr,
        "Cancel",
        button_style_,
        BUTTON_NORMAL_COLOR,
        BUTTON_HOVER_COLOR,
        BUTTON_PRESSED_COLOR
    ));

    LineEdit *line_edit = new LineEdit(
        3,
        Transform(),
        Vector2D(getAreaSize().x, 30),
        0,
        nullptr,
        line_edit_style_,
        256
    );

    line_edit->setKeyboardFocus(true);

    container.addChild(line_edit);
}


const char *SelectFileDialog::getFilename() {
    LineEdit *line_edit = (LineEdit*) container.findWidget(3);
    ASSERT(line_edit, "LineEdit not found!\n");

    return line_edit->getString();
}


// ============================================================================


#pragma GCC diagnostic pop
