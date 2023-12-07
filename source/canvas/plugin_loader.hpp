/**
 * \file
 * \brief Contains PluginLoader interface
*/


#ifndef _PLUGIN_LOADER_H_
#define _PLUGIN_LOADER_H_


#include "canvas/canvas_stuff.hpp"
#include "canvas/plugin.hpp"


/// Loads plugins from root folder to ToolPalette and FilterPalette
class PluginLoader {
public:
    PluginLoader(
        const char *root_dir_,
        Menu &menu_,
        size_t menu_button_id_,
        Window &window_
    );

private:
    /**
     * \brief Scans root folder for plugin directories
    */
    void scan();

    /**
     * \brief Scans directory for .so file and loads plugin
     * \return True if plugin loaded successfully
    */
    bool scanPluginDir(const char *path);

    /**
     * \brief True if file at path is directory
    */
    bool isDir(const char *path) const;

    /**
     * \brief True if file at path has .so extension
    */
    bool isPluginFile(const char *path) const;

    /**
     * \brief Loads plugin dl and returns plugin instance
    */
    plug::Plugin *loadPlugin(const char *path);

    /**
     * \brief Tries to get tool interface from plugin and add it to palette view
     * \return True if tool interface is supported
    */
    bool loadTool(plug::Plugin *plugin);

    /**
     * \brief Tries to get filter interface from plugin and add it to menu
     * \return True if filter interface is supported
    */
    bool loadFilter(plug::Plugin *plugin);

    const char *root_dir;           ///< Path to root folder
    Menu &menu;                     ///< Menu to add new filter button
    size_t menu_button_id;          ///< Submenu id to add new filter
    Window &window;                 ///< Window to add dialog to
};


#endif
