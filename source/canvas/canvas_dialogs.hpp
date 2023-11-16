/**
 * \file
 * \brief Contains dialog actions classes for opening and closing files
*/


#ifndef _CANVAS_DIALOGS_H_
#define _CANVAS_DIALOGS_H_


#include "canvas/canvas.hpp"
#include "window/dialog.hpp"
#include "basic/scrollbar.hpp"


/**
 * \brief Opens picture on canvas in new subwindow with scrollbars
 * \note If image fails to open, then nullptr will be returned
*/
Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style);


/// Uses openPicture() to open image
class OpenFileAction : public DialogAction {
public:
    OpenFileAction(
        Window &window_,
        ToolPalette &palette_,
        CanvasGroup &group_,
        WindowStyle &window_style_,
        ScrollBarStyle &scrollbar_style_
    );

    virtual void operator () () override;

    virtual OpenFileAction *clone() override;

private:
    Window &window;
    ToolPalette &palette;
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
        ToolPalette &palette_,
        CanvasGroup &group_,
        FileDialogStyle &dialog_style_,
        ScrollBarStyle &scrollbar_style_
    );

    virtual void operator () () override;

    virtual CreateOpenFileDialog *clone() override;

private:
    Window &window;
    ToolPalette &palette;
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
