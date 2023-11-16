/**
 * \file
 * \brief Contains sources of dialog actions classes functions
*/


#include <SFML/Graphics.hpp>
#include "canvas/canvas_dialogs.hpp"


// ============================================================================


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
    // If canvas is correct we can create other stuff
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


// ============================================================================


OpenFileAction::OpenFileAction(
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


void OpenFileAction::operator () () {
    ASSERT(dialog, "Dialog is nullptr!\n");
    const char *filename = ((SelectFileDialog*)(dialog))->getFilename();

    Widget *subwindow = openPicture(filename, palette, group, window_style, scrollbar_style);
    
    if (subwindow) {
        window.addChild(subwindow);
        dialog->setStatus(Widget::DELETE);
    }
}


OpenFileAction *OpenFileAction::clone() {
    return new OpenFileAction(window, palette, group, window_style, scrollbar_style);
}


// ============================================================================


SaveAsFileAction::SaveAsFileAction(CanvasGroup &group_) :
    group(group_) {}


void SaveAsFileAction::operator () () {
    ASSERT(dialog, "Dialog is nullptr!\n");
    const char *filename = ((SelectFileDialog*)(dialog))->getFilename();

    ASSERT(group.getActive(), "No active canvas!\n");
    group.getActive()->saveImageAs(filename);

    dialog->setStatus(Widget::DELETE);
}


SaveAsFileAction *SaveAsFileAction::clone() {
    return new SaveAsFileAction(group);
}


// ============================================================================


void CancelAction::operator () () {
    ASSERT(dialog, "Dialog is nullptr!\n");
    dialog->setStatus(Widget::DELETE);
}


CancelAction *CancelAction::clone() {
    return new CancelAction();
}


// ============================================================================


CreateOpenFileDialog::CreateOpenFileDialog(
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


void CreateOpenFileDialog::operator () () {
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


CreateOpenFileDialog *CreateOpenFileDialog::clone() {
    return new CreateOpenFileDialog(
        window,
        palette,
        group,
        dialog_style,
        scrollbar_style
    );
}


// ============================================================================


CreateSaveAsFileDialog::CreateSaveAsFileDialog(
    Window &window_,
    CanvasGroup &group_,
    FileDialogStyle &dialog_style_
) :
    window(window_),
    group(group_),
    dialog_style(dialog_style_)
{}


void CreateSaveAsFileDialog::operator () () {
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


CreateSaveAsFileDialog *CreateSaveAsFileDialog::clone() {
    return new CreateSaveAsFileDialog(
        window,
        group,
        dialog_style
    );
}


// ============================================================================


SaveFileAction::SaveFileAction(CanvasGroup &group_) :
    group(group_) {}

void SaveFileAction::operator () () {
    Canvas *canvas = group.getActive();
    
    if (canvas && canvas->isImageOpen())
        canvas->saveImage();
}

SaveAsFileAction *SaveFileAction::clone() {
    return new SaveAsFileAction(group);
}
