/**
 * \file
 * \brief Contains implementation of classes that work with canvas
*/


#include <SFML/Graphics.hpp>
#include "canvas/canvas_stuff.hpp"


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


VScrollCanvas::VScrollCanvas(Canvas &canvas_) : canvas(canvas_) {}


void VScrollCanvas::operator () (vec_t param) {
    Vec2d canvas_size = canvas.getLayoutBox().getSize();
    Vec2d texture_offset = canvas.getTextureOffset();

    if (canvas.getTextureSize().y > canvas_size.y) {
        canvas.setTextureOffset(Vec2d(
            texture_offset.x,
            param * (canvas.getTextureSize().y - canvas_size.y)
        ));
    }
}


// ============================================================================


HScrollCanvas::HScrollCanvas(Canvas &canvas_) : canvas(canvas_) {}

void HScrollCanvas::operator () (vec_t param) {
    Vec2d canvas_size = canvas.getLayoutBox().getSize();
    Vec2d texture_offset = canvas.getTextureOffset();

    if (canvas.getTextureSize().x > canvas_size.x) {
        canvas.setTextureOffset(Vec2d(
            param * (canvas.getTextureSize().x - canvas_size.x),
            texture_offset.y
        ));
    }
}


// ============================================================================


FilterHotkey::FilterHotkey(Widget *parent_, FilterPalette &palette_, CanvasGroup &group_) :
    Widget(AUTO_ID, BoundLayoutBox()),
    palette(palette_), group(group_)
{}


void FilterHotkey::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case F: 
            if (event.ctrl) {
                palette.getLastFilter()->applyFilter(*group.getActive());
                break;
            }
            return;
        default: return;
    }

    ehc.stopped = true;
}


// ============================================================================


FilterAction::FilterAction(FilterPalette::FILTERS filter_id_, FilterPalette &palette_, CanvasGroup &group_) : 
    filter_id(filter_id_), palette(palette_), group(group_) {}


void FilterAction::operator () () {
    if (group.getActive()) {
        palette.getFilter(filter_id)->applyFilter(*group.getActive());
        palette.setLastFilter(filter_id);
    }
}


FilterAction *FilterAction::clone() { return new FilterAction(filter_id, palette, group); }


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
