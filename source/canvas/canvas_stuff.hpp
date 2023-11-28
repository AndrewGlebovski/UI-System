/**
 * \file
 * \brief Contains classes that work with canvas
*/


#ifndef _CANVAS_STUFF_H_
#define _CANVAS_STUFF_H_


#include "canvas/canvas_view.hpp"
#include "window/dialog.hpp"
#include "basic/scrollbar.hpp"


/**
 * \brief Opens picture on canvas in new subwindow with scrollbars
 * \note If image fails to open, then nullptr will be returned
*/
Widget *openPicture(
    const char *filename,
    ToolPalette &tool_palette,
    ColorPalette &color_palette,
    CanvasGroup &group,
    WindowStyle &window_style,
    ScrollBarStyle &scrollbar_style
);


/// Moves canvas texture in vertical direction
class VScrollCanvas : public ScrollAction {
protected:
    CanvasView &canvas;

public:
    VScrollCanvas(CanvasView &canvas_);

    virtual void operator () (vec_t param) override;
};


/// Moves canvas texture in horizontal direction
class HScrollCanvas : public ScrollAction {
protected:
    CanvasView &canvas;

public:
    HScrollCanvas(CanvasView &canvas_);

    virtual void operator () (vec_t param) override;
};


/// Supports hot keys for applying filters
class FilterHotkey : public Widget {
public:
    FilterHotkey(Widget *parent_, FilterPalette &palette_, CanvasGroup &group_);

protected:
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) override;

private:
    FilterPalette &palette;
    CanvasGroup &group;
};


/// Applies specified filter to the active canvas
class FilterAction : public ButtonAction {
private:
    FilterPalette::FILTERS filter_id;
    FilterPalette &palette;
    CanvasGroup &group;

public:
    FilterAction(FilterPalette::FILTERS filter_id_, FilterPalette &palette_, CanvasGroup &group_);


    virtual void operator () () override;


    virtual FilterAction *clone() override;
};


/// Uses openPicture() to open image
class OpenFileAction : public DialogAction {
public:
    OpenFileAction(
        Window &window_,
        ToolPalette &tool_palette_,
        ColorPalette &color_palette_,
        CanvasGroup &group_,
        WindowStyle &window_style_,
        ScrollBarStyle &scrollbar_style_
    );

    virtual void operator () () override;

    virtual OpenFileAction *clone() override;

private:
    Window &window;
    ToolPalette &tool_palette;
    ColorPalette &color_palette;
    CanvasGroup &group;
    WindowStyle &window_style;
    ScrollBarStyle &scrollbar_style;
};


/// Saves active canvas texture to image
class SaveAsFileAction : public DialogAction {
public:
    SaveAsFileAction(CanvasGroup &group_);

    virtual void operator () () override;

    virtual SaveAsFileAction *clone() override;

private:
    CanvasGroup &group;
};


/// Deletes dialog
class CancelAction : public DialogAction {
public:
    virtual void operator () () override;

    virtual CancelAction *clone() override;
};


/// Creates dialog for opening file
class CreateOpenFileDialog : public ButtonAction {
public:
    CreateOpenFileDialog(
        Window &window_,
        ToolPalette &tool_palette_,
        ColorPalette &color_palette,
        CanvasGroup &group_,
        FileDialogStyle &dialog_style_,
        ScrollBarStyle &scrollbar_style_
    );

    virtual void operator () () override;

    virtual CreateOpenFileDialog *clone() override;

private:
    Window &window;
    ToolPalette &tool_palette;
    ColorPalette &color_palette;
    CanvasGroup &group;
    FileDialogStyle &dialog_style;
    ScrollBarStyle &scrollbar_style;
};


/// Creates dialog for saving file
class CreateSaveAsFileDialog : public ButtonAction {
public:
    CreateSaveAsFileDialog(
        Window &window_,
        CanvasGroup &group_,
        FileDialogStyle &dialog_style_
    );

    virtual void operator () () override;

    virtual CreateSaveAsFileDialog *clone() override;

private:
    Window &window;
    CanvasGroup &group;
    FileDialogStyle &dialog_style;
};


/// If active canvas has opened image, saves it to the same path
class SaveFileAction : public ButtonAction {
public:
    SaveFileAction(CanvasGroup &group_);

    virtual void operator () () override;

    virtual SaveAsFileAction *clone() override;

private:
    CanvasGroup &group;
};


#endif
