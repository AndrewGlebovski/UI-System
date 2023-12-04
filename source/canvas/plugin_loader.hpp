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
        CanvasGroup &group_
    );

private:
    /**
     * \brief Scans root folder for plugins
    */
    void scan();

    /**
     * \brief Loads plugin dl and returns plugin instance
    */
    Plugin *loadPlugin(const char *path);

    /**
     * \brief Tries to get tool interface from plugin and add it to palette view
     * \return True if tool interface is supported
    */
    bool loadTool(Plugin *plugin);

    /**
     * \brief Tries to get filter interface from plugin and add it to menu
     * \return True if filter interface is supported
    */
    bool loadFilter(Plugin *plugin);

    const char *root_dir;           ///< Path to root folder
    Menu &menu;                     ///< Menu to add new filter button
    size_t menu_button_id;          ///< Submenu id to add new filter
    CanvasGroup &group;             ///< Canvas group
};


#endif
