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


// ============================================================================


const size_t BUTTON_NORMAL_COLOR = 0xd4d0c8ff;
const size_t BUTTON_HOVER_COLOR = 0x000080ff;
const size_t BUTTON_PRESSED_COLOR = 0x000080ff;

const unsigned BUTTON_FONT_SIZE = 20;
const size_t BUTTON_FONT_NORMAL_COLOR = 0x000000ff;
const size_t BUTTON_FONT_HOVER_COLOR = 0xffffffff;
const size_t BUTTON_FONT_PRESSED_COLOR = 0xffffffff;

const size_t LINE_EDIT_FONT_COLOR = 0x000000ff;
const unsigned LINE_EDIT_FONT_SIZE = 20;
const size_t LINE_EDIT_BACKGROUND_COLOR = 0xffffffff;
const size_t LINE_EDIT_CURSOR_COLOR = 0x000000ff;
const size_t LINE_EDIT_BORDER_COLOR = 0x000000ff;
const float LINE_EDIT_BORDER_THICKNESS = 1;

const size_t MAX_FILENAME_LENGTH = 256;

// ============================================================================


Dialog::Dialog(
    size_t id_, const LayoutBox &layout_,
    const std::string &title_, const WindowStyle &style_
) :
    Window(id_, layout_, title_, style_, false, true, false)
{}


EVENT_STATUS Dialog::onMouseMove(const Vec2d &mouse, TransformStack &stack) {
    Window::onMouseMove(mouse, stack);
    return HANDLED;
}


EVENT_STATUS Dialog::onMouseButtonUp(const Vec2d &mouse, int button_id, TransformStack &stack) {
    Window::onMouseButtonUp(mouse, button_id, stack);
    return HANDLED;
}


EVENT_STATUS Dialog::onMouseButtonDown(const Vec2d &mouse, int button_id, TransformStack &stack) {
    Window::onMouseButtonDown(mouse, button_id,stack);
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
    size_t id_, const LayoutBox &layout_,
    const std::string &title_,
    DialogAction *ok_action_,
    DialogAction *cancel_action_,
    const WindowStyle &window_style_,
    const RectButtonStyle &button_style_
) :
    Dialog(id_, layout_, title_, window_style_)
{
    layout->setSize(Vec2d(400, 200));

    ok_action_->setDialog(*this);
    cancel_action_->setDialog(*this);

    container.addChild(new RectButton(
        1,
        BasicLayoutBox(Vec2d(), Vec2d(100, 50)),
        ok_action_,
        "Ok",
        button_style_
    ));

    container.addChild(new RectButton(
        2,
        BasicLayoutBox(Vec2d(120, 0), Vec2d(100, 50)),
        cancel_action_,
        "Cancel",
        button_style_
    ));
}


// ============================================================================


SelectFileDialog::SelectFileDialog(
    size_t id_, const LayoutBox &layout_,
    const std::string &title_,
    DialogAction *select_action_,
    DialogAction *cancel_action_,
    const FileDialogStyle &style_
) :
    Dialog(id_, layout_, title_, style_.window)
{
    setSize(Vec2d(400, 200));

    select_action_->setDialog(*this);
    cancel_action_->setDialog(*this);

    RectButtonStyle button_style(
        sf::Color(BUTTON_NORMAL_COLOR),
        sf::Color(BUTTON_HOVER_COLOR),
        sf::Color(BUTTON_PRESSED_COLOR),
        style_.window.font,
        BUTTON_FONT_SIZE,
        sf::Color(BUTTON_FONT_NORMAL_COLOR),
        sf::Color(BUTTON_FONT_HOVER_COLOR),
        sf::Color(BUTTON_FONT_PRESSED_COLOR)
    );

    LineEditStyle line_edit_style(
        style_.window.font,
        LINE_EDIT_FONT_SIZE,
        sf::Color(LINE_EDIT_FONT_COLOR),
        sf::Color(LINE_EDIT_BACKGROUND_COLOR),
        sf::Color(LINE_EDIT_CURSOR_COLOR),
        sf::Color(LINE_EDIT_BORDER_COLOR),
        LINE_EDIT_BORDER_THICKNESS
    );

    container.addChild(new RectButton(
        1,
        BasicLayoutBox(Vec2d(0, 50), Vec2d(100, 50)),
        select_action_,
        "Select",
        button_style
    ));

    container.addChild(new RectButton(
        2,
        BasicLayoutBox(Vec2d(120, 50), Vec2d(100, 50)),
        cancel_action_,
        "Cancel",
        button_style
    ));

    LineEdit *line_edit = new LineEdit(
        3,
        BasicLayoutBox(Vec2d(), Vec2d(getAreaSize().x, 30)),
        line_edit_style,
        MAX_FILENAME_LENGTH
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
