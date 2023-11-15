/**
 * \brief Opens picture on canvas in new subwindow with scrollbars
 * \note If image fails to open, then nullptr will be returned
*/
Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style);


Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style) {
    // First we create to check if file is correct image
    Canvas *canvas = new Canvas(
        Widget::AUTO_ID,
        AnchorLayoutBox(
            Vec2d(),
            Vec2d(SCREEN_W - 30, SCREEN_H - 30),
            Vec2d(0, 0),
            Vec2d(SCREEN_W - 30, SCREEN_H - 30)
        ),
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
        BoundLayoutBox(Vec2d(300, 100), Vec2d(800, 600)),
        (filename) ? filename : "Canvas",
        window_style
    );

    subwindow->addChild(canvas);
    
    subwindow->addChild(new VScrollBar(
        Widget::AUTO_ID,
        AnchorLayoutBox(
            Vec2d(-20, 0),
            Vec2d(20, SCREEN_H - 30),
            Vec2d(SCREEN_W, 0),
            Vec2d(0, SCREEN_H - 30)
        ),
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    subwindow->addChild(new HScrollBar(
        Widget::AUTO_ID,
        AnchorLayoutBox(
            Vec2d(0, -20),
            Vec2d(SCREEN_W, 20),
            Vec2d(0, SCREEN_H),
            Vec2d(SCREEN_W, 0)
        ),
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
    FileDialogStyle &dialog_style;
    ScrollBarStyle &scrollbar_style;

public:
    CreateOpenFileDialog(
        Window &window_,
        ToolPalette &palette_,
        CanvasGroup &group_,
        FileDialogStyle &dialog_style_,
        ScrollBarStyle &scrollbar_style_
    ) :
        window(window_),
        palette(palette_),
        group(group_),
        dialog_style(dialog_style_),
        scrollbar_style(scrollbar_style_)
    {}

    virtual void operator () () override {
        window.addChild(new SelectFileDialog(
            Widget::AUTO_ID,
            BoundLayoutBox(),
            "Open File",
            new OpenFileAction(
                window,
                palette,
                group,
                dialog_style.window,
                scrollbar_style
            ),
            new CancelAction(),
            dialog_style
        ));
    }

    virtual CreateOpenFileDialog *clone() override {
        return new CreateOpenFileDialog(
            window,
            palette,
            group,
            dialog_style,
            scrollbar_style
        );
    }
};


class CreateSaveAsFileDialog : public ButtonAction {
private:
    Window &window;
    CanvasGroup &group;
    FileDialogStyle &dialog_style;

public:
    CreateSaveAsFileDialog(
        Window &window_,
        CanvasGroup &group_,
        FileDialogStyle &dialog_style_
    ) :
        window(window_),
        group(group_),
        dialog_style(dialog_style_)
    {}

    virtual void operator () () override {
        if (!group.getActive()) return;

        window.addChild(new SelectFileDialog(
            Widget::AUTO_ID,
            BoundLayoutBox(),
            "Save As",
            new SaveAsFileAction(group),
            new CancelAction(),
            dialog_style
        ));
    }

    virtual CreateSaveAsFileDialog *clone() override {
        return new CreateSaveAsFileDialog(
            window,
            group,
            dialog_style
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
