/// Opens picture on canvas in new subwindow with scrollbars
Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style);


Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style) {
    Window *subwindow = new Window(
        Widget::AUTO_ID,
        Transform(Vector2D(300, 100)),
        Vector2D(800, 600),
        1,
        nullptr,
        (filename) ? filename : "Canvas",
        window_style
    );

    Canvas *canvas = new Canvas(
        Widget::AUTO_ID,
        Transform(),
        subwindow->getAreaSize() - Vector2D(30, 30),
        0,
        nullptr,
        palette,
        group
    );

    if (filename) canvas->openImage(filename);
    else canvas->createImage(1920, 1080);

    subwindow->addChild(canvas);

    subwindow->addChild(new VScrollBar(
        Widget::AUTO_ID,
        Transform(Vector2D(subwindow->getAreaSize().x - 20, 0)),
        Vector2D(20, subwindow->getAreaSize().y - 30),
        1,
        nullptr,
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    subwindow->addChild(new HScrollBar(
        Widget::AUTO_ID,
        Transform(Vector2D(0, subwindow->getAreaSize().y - 20)),
        Vector2D(subwindow->getAreaSize().x, 20),
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

        struct stat file_info = {};
        if (stat(filename, &file_info) == 0) {
            window.addChild(openPicture(filename, palette, group, window_style, scrollbar_style));
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
