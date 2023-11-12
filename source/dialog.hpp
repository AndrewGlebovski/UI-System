/**
 * \file
 * \brief Contains dialogs classes and protypes of their functions 
*/


/// Base class for all dialogs
class Dialog : public Window {
protected:
    /**
     * \brief Dialog contrusctor that copies window constructor
     * \note Position and size consider title bar and frame
    */
    Dialog(
        size_t id_, const LayoutBox &layout_,
        const std::string &title_, const WindowStyle &style_
    );

public:
    virtual EVENT_STATUS onMouseMove(const Vec2d &mouse, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vec2d &mouse, int button_id, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vec2d &mouse, int button_id, TransformStack &stack) override;
    virtual EVENT_STATUS onKeyUp(int key_id) override;
    virtual EVENT_STATUS onKeyDown(int key_id) override;
};


/// Base class for all dialog actions
class DialogAction : public ButtonAction {
protected:
    Dialog *dialog;

public:
    DialogAction() : dialog(nullptr) {}

    DialogAction(const DialogAction&) = default;
    DialogAction &operator = (const DialogAction&) = default;

    virtual void operator () () override = 0;

    virtual DialogAction *clone() override = 0;

    void setDialog(Dialog &dialog_) { dialog = &dialog_; }
};


/// Asks user for confirming his actions
class ConfirmDialog : public Dialog {
public:
    /**
     * \brief Constructs dialog window for confirming choice from actions and style
     * \note Actions must set window status as DELETE on exit
    */
    ConfirmDialog(
        size_t id_, const LayoutBox &layout_,
        const std::string &title_,
        DialogAction *ok_action_,
        DialogAction *cancel_action_,
        const WindowStyle &window_style_,
        const RectButtonStyle &button_style_
    );
};


struct FileDialogStyle {
    WindowStyle window;

    FileDialogStyle(const WindowStyle &window_style_) :
        window(window_style_) {}
};


/// Allows user to enter filename
class SelectFileDialog : public Dialog {
public:
    /**
     * \brief Constructs dialog window for selecting file from actions and styles
     * \note Actions must set window status as DELETE on exit
    */
    SelectFileDialog(
        size_t id_, const LayoutBox &layout_,
        const std::string &title_,
        DialogAction *select_action_,
        DialogAction *cancel_action_,
        const FileDialogStyle &style_
    );

    /**
     * \brief Returns filename entered by user
    */
    const char *getFilename();
};
