/**
 * \brief Opens picture on canvas in new subwindow with scrollbars
 * \note If image fails to open, then nullptr will be returned
*/
Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style);


Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style) {
    // First we create to check if file is correct image
    Canvas *canvas = new Canvas(
        Widget::AUTO_ID,
        Transform(),
        Vec2d(),
        0,
        nullptr,
        palette,
        group
    );

    if (filename) {
        if (!canvas->openImage(filename)) {
            delete canvas;
            return nullptr;
        }
    }
    else {
        if (!canvas->createImage(SCREEN_W, SCREEN_H)) {
            delete canvas;
            return nullptr;
        }
    }
    // If file is correct we can create other stuff
    Window *subwindow = new Window(
        Widget::AUTO_ID,
        Transform(Vec2d(300, 100)),
        Vec2d(800, 600),
        1,
        nullptr,
        (filename) ? filename : "Canvas",
        window_style
    );

    canvas->size = subwindow->getAreaSize() - Vec2d(30, 30);

    subwindow->addChild(canvas);

    subwindow->addChild(new VScrollBar(
        Widget::AUTO_ID,
        Transform(Vec2d(subwindow->getAreaSize().x - 20, 0)),
        Vec2d(20, subwindow->getAreaSize().y - 30),
        1,
        nullptr,
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    subwindow->addChild(new HScrollBar(
        Widget::AUTO_ID,
        Transform(Vec2d(0, subwindow->getAreaSize().y - 20)),
        Vec2d(subwindow->getAreaSize().x, 20),
        1,
        nullptr,
        new HScrollCanvas(*canvas),
        scrollbar_style
    ));

    return subwindow;
}


class OpenFileAction : public DialogAction {
private:
    Window &window;
    ToolPalette &palette;
    CanvasGroup &group;
    WindowStyle &window_style;
    ScrollBarStyle &scrollbar_style;

public:
    OpenFileAction(
        Window &window_,
        ToolPalette &palette_,
        CanvasGroup &group_,
        WindowStyle &window_style_,
        ScrollBarStyle &scrollbar_style_
    ) :
        window(window_),
        palette(palette_),
        group(group_),
        window_style(window_style_),
        scrollbar_style(scrollbar_style_)
    {}

    virtual void operator () () override {
        ASSERT(dialog, "Dialog is nullptr!\n");
        const char *filename = ((SelectFileDialog*)(dialog))->getFilename();

        Widget *subwindow = openPicture(filename, palette, group, window_style, scrollbar_style);
        
        if (subwindow) {
            window.addChild(subwindow);
            dialog->setStatus(Widget::DELETE);
        }
    }

    virtual OpenFileAction *clone() override {
        return new OpenFileAction(window, palette, group, window_style, scrollbar_style);
    }
};


class SaveAsFileAction : public DialogAction {
private:
    CanvasGroup &group;

public:
    SaveAsFileAction(CanvasGroup &group_) : group(group_) {}

    virtual void operator () () override {
        ASSERT(dialog, "Dialog is nullptr!\n");
        const char *filename = ((SelectFileDialog*)(dialog))->getFilename();

        ASSERT(group.getActive(), "No active canvas!\n");
        group.getActive()->saveImageAs(filename);

        dialog->setStatus(Widget::DELETE);
    }

    virtual SaveAsFileAction *clone() override {
        return new SaveAsFileAction(group);
    }
};


class CancelAction : public DialogAction {
public:
    virtual void operator () () override {
        ASSERT(dialog, "Dialog is nullptr!\n");
        dialog->setStatus(Widget::DELETE);
    }

    virtual CancelAction *clone() override {
        return new CancelAction();
    }
};


class CreateOpenFileDialog : public ButtonAction {
private:
    Window &window;
    ToolPalette &palette;
    CanvasGroup &group;
    WindowStyle &window_style;
    ScrollBarStyle &scrollbar_style;
    LineEditStyle &line_edit_style;
    ButtonStyle &button_style;

public:
    CreateOpenFileDialog(
        Window &window_,
        ToolPalette &palette_,
        CanvasGroup &group_,
        WindowStyle &window_style_,
        ScrollBarStyle &scrollbar_style_,
        LineEditStyle &line_edit_style_,
        ButtonStyle &button_style_
    ) :
        window(window_),
        palette(palette_),
        group(group_),
        window_style(window_style_),
        scrollbar_style(scrollbar_style_),
        line_edit_style(line_edit_style_),
        button_style(button_style_)
    {}

    virtual void operator () () override {
        window.addChild(new SelectFileDialog(
            Widget::AUTO_ID,
            Transform(),
            1000,
            nullptr,
            "Open File",
            new OpenFileAction(
                window,
                palette,
                group,
                window_style,
                scrollbar_style
            ),
            new CancelAction(),
            window_style,
            button_style,
            line_edit_style
        ));
    }

    virtual CreateOpenFileDialog *clone() override {
        return new CreateOpenFileDialog(
            window,
            palette,
            group,
            window_style,
            scrollbar_style,
            line_edit_style,
            button_style
        );
    }
};


class CreateSaveAsFileDialog : public ButtonAction {
private:
    Window &window;
    CanvasGroup &group;
    WindowStyle &window_style;
    LineEditStyle &line_edit_style;
    ButtonStyle &button_style;

public:
    CreateSaveAsFileDialog(
        Window &window_,
        CanvasGroup &group_,
        WindowStyle &window_style_,
        LineEditStyle &line_edit_style_,
        ButtonStyle &button_style_
    ) :
        window(window_),
        group(group_),
        window_style(window_style_),
        line_edit_style(line_edit_style_),
        button_style(button_style_)
    {}

    virtual void operator () () override {
        if (!group.getActive()) return;

        window.addChild(new SelectFileDialog(
            Widget::AUTO_ID,
            Transform(),
            1000,
            nullptr,
            "Save As",
            new SaveAsFileAction(group),
            new CancelAction(),
            window_style,
            button_style,
            line_edit_style
        ));
    }

    virtual CreateSaveAsFileDialog *clone() override {
        return new CreateSaveAsFileDialog(
            window,
            group,
            window_style,
            line_edit_style,
            button_style
        );
    }
};


class SaveFileAction : public ButtonAction {
private:
    CanvasGroup &group;

public:
    SaveFileAction(CanvasGroup &group_) : group(group_) {}

    virtual void operator () () override {
        Canvas *canvas = group.getActive();
        
        if (canvas && canvas->isImageOpen())
            canvas->saveImage();
    }

    virtual SaveAsFileAction *clone() override {
        return new SaveAsFileAction(group);
    }
};
