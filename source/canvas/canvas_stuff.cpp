/**
 * \file
 * \brief Contains implementation of classes that work with canvas
*/


#include "canvas/canvas_stuff.hpp"
#include "canvas/palettes/palette_manager.hpp"


// ============================================================================


Widget *openPicture(
    const char *filename,
    WindowStyle &window_style,
    ScrollBarStyle &scrollbar_style
) {
    // First we create to check if file is correct image
    CanvasView *canvas = new CanvasView(
        Widget::AUTO_ID,
        AnchorLayoutBox(
            plug::Vec2d(),
            plug::Vec2d(SCREEN_W - 30, SCREEN_H - 30),
            plug::Vec2d(),
            plug::Vec2d(SCREEN_W - 30, SCREEN_H - 30)
        )
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
        BoundLayoutBox(plug::Vec2d(300, 100), plug::Vec2d(800, 600)),
        (filename) ? filename : "Canvas",
        window_style
    );

    subwindow->addChild(canvas);
    
    subwindow->addChild(new VScrollBar(
        Widget::AUTO_ID,
        AnchorLayoutBox(
            plug::Vec2d(-20, 0),
            plug::Vec2d(20, SCREEN_H - 30),
            plug::Vec2d(SCREEN_W, 0),
            plug::Vec2d(0, SCREEN_H - 30)
        ),
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    subwindow->addChild(new HScrollBar(
        Widget::AUTO_ID,
        AnchorLayoutBox(
            plug::Vec2d(0, -20),
            plug::Vec2d(SCREEN_W, 20),
            plug::Vec2d(0, SCREEN_H),
            plug::Vec2d(SCREEN_W, 0)
        ),
        new HScrollCanvas(*canvas),
        scrollbar_style
    ));
    
    return subwindow;
}


// ============================================================================


VScrollCanvas::VScrollCanvas(CanvasView &canvas_) : canvas(canvas_) {}


void VScrollCanvas::operator () (double param) {
    plug::Vec2d canvas_size = canvas.getLayoutBox().getSize();
    plug::Vec2d texture_offset = canvas.getTextureOffset();

    if (canvas.getTextureSize().y > canvas_size.y) {
        canvas.setTextureOffset(plug::Vec2d(
            texture_offset.x,
            param * (canvas.getTextureSize().y - canvas_size.y)
        ));
    }
}


// ============================================================================


HScrollCanvas::HScrollCanvas(CanvasView &canvas_) : canvas(canvas_) {}


void HScrollCanvas::operator () (double param) {
    plug::Vec2d canvas_size = canvas.getLayoutBox().getSize();
    plug::Vec2d texture_offset = canvas.getTextureOffset();

    if (canvas.getTextureSize().x > canvas_size.x) {
        canvas.setTextureOffset(plug::Vec2d(
            param * (canvas.getTextureSize().x - canvas_size.x),
            texture_offset.y
        ));
    }
}


// ============================================================================


FilterHotkey::FilterHotkey() :
    Widget(AUTO_ID, BoundLayoutBox()) {}


void FilterHotkey::onKeyboardPressed(const plug::KeyboardPressedEvent &event, plug::EHC &ehc) {
    switch (event.key_id) {
        case plug::KeyCode::F: 
            if (event.ctrl) {
                FILTER_PALETTE.getLastFilter()->applyFilter(CANVAS_GROUP.getActive()->getCanvas());
                break;
            }
            return;
        default: return;
    }

    ehc.stopped = true;
}


// ============================================================================


FilterAction::FilterAction(Window &window_, size_t filter_id_) : 
    window(window_), filter_id(filter_id_) {}


void FilterAction::operator () () {
    if (CANVAS_GROUP.getActive()) {
        plug::Filter *filter = FILTER_PALETTE.getFilter(filter_id);

        Widget *filter_widget = static_cast<Widget*>(filter->getWidget());

        if (filter_widget) {
            window.addChild(filter_widget);
        }
        else {
            filter->applyFilter(CANVAS_GROUP.getActive()->getCanvas());
            FILTER_PALETTE.setLastFilter(filter_id);
        }
    }
}


FilterAction *FilterAction::clone() {
    return new FilterAction(window, filter_id);
}


// ============================================================================


OpenFileAction::OpenFileAction(
    Window &window_,
    WindowStyle &window_style_,
    ScrollBarStyle &scrollbar_style_
) :
    window(window_),
    window_style(window_style_),
    scrollbar_style(scrollbar_style_)
{}


void OpenFileAction::operator () () {
    ASSERT(dialog, "Dialog is nullptr!\n");
    const char *filename = ((SelectFileDialog*)(dialog))->getFilename();

    Widget *subwindow = openPicture(filename, window_style, scrollbar_style);
    
    if (subwindow) {
        window.addChild(subwindow);
        dialog->setStatus(Widget::Status::Delete);
    }
}


OpenFileAction *OpenFileAction::clone() {
    return new OpenFileAction(window, window_style, scrollbar_style);
}


// ============================================================================


SaveAsFileAction::SaveAsFileAction() {}


void SaveAsFileAction::operator () () {
    ASSERT(dialog, "Dialog is nullptr!\n");
    const char *filename = ((SelectFileDialog*)(dialog))->getFilename();

    ASSERT(CANVAS_GROUP.getActive(), "No active canvas!\n");
    CANVAS_GROUP.getActive()->saveImageAs(filename);

    dialog->setStatus(Widget::Status::Delete);
}


SaveAsFileAction *SaveAsFileAction::clone() {
    return new SaveAsFileAction();
}


// ============================================================================


void CancelAction::operator () () {
    ASSERT(dialog, "Dialog is nullptr!\n");
    dialog->setStatus(Widget::Status::Delete);
}


CancelAction *CancelAction::clone() {
    return new CancelAction();
}


// ============================================================================


CreateOpenFileDialog::CreateOpenFileDialog(
    Window &window_,
    FileDialogStyle &dialog_style_,
    ScrollBarStyle &scrollbar_style_
) :
    window(window_),
    dialog_style(dialog_style_),
    scrollbar_style(scrollbar_style_)
{}


void CreateOpenFileDialog::operator () () {
    window.addChild(new SelectFileDialog(
        Widget::AUTO_ID,
        BoundLayoutBox(),
        "Open File",
        new OpenFileAction(window, dialog_style.window, scrollbar_style),
        new CancelAction(),
        dialog_style
    ));
}


CreateOpenFileDialog *CreateOpenFileDialog::clone() {
    return new CreateOpenFileDialog(window, dialog_style, scrollbar_style);
}


// ============================================================================


CreateSaveAsFileDialog::CreateSaveAsFileDialog(
    Window &window_,
    FileDialogStyle &dialog_style_
) :
    window(window_),
    dialog_style(dialog_style_)
{}


void CreateSaveAsFileDialog::operator () () {
    if (!CANVAS_GROUP.getActive()) return;

    window.addChild(new SelectFileDialog(
        Widget::AUTO_ID,
        BoundLayoutBox(),
        "Save As",
        new SaveAsFileAction(),
        new CancelAction(),
        dialog_style
    ));
}


CreateSaveAsFileDialog *CreateSaveAsFileDialog::clone() {
    return new CreateSaveAsFileDialog(window, dialog_style);
}


// ============================================================================


SaveFileAction::SaveFileAction() {}


void SaveFileAction::operator () () {
    CanvasView *canvas = CANVAS_GROUP.getActive();
    
    if (canvas && canvas->isImageOpen())
        canvas->saveImage();
}


SaveAsFileAction *SaveFileAction::clone() {
    return new SaveAsFileAction();
}
