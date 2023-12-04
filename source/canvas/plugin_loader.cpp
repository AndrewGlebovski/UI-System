/**
 * \file
 * \brief Contains implementation of PluginLoader
*/


#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "common/assert.hpp"
#include "canvas/plugin_loader.hpp"
#include "canvas/palettes/palette_manager.hpp"


// ============================================================================


#define LOAD_PLUGIN_SYM "loadPlugin"    ///< Name of the dl function that creates plugin instance

const size_t MAX_PATH = 256;            ///< Max length of path to root dir and plugin file name


// ============================================================================


PluginLoader::PluginLoader(
    const char *root_dir_,
    Menu &menu_,
    size_t menu_button_id_
) :
    root_dir(root_dir_),
    menu(menu_),
    menu_button_id(menu_button_id_)
{
    scan();
}


void PluginLoader::scan() {
    DIR *dir = NULL;
    struct dirent *dir_info = NULL;

    dir = opendir(root_dir);
    
    if (!dir) {
        printf("Failed to open plugin folder!\n");
        return;
    }

    char filename[MAX_PATH * 2] = "";

    while ((dir_info = readdir(dir)) != NULL) {
        sprintf(filename, "%s/%s", root_dir, dir_info->d_name);

        Plugin *plugin = loadPlugin(filename);

        if (!plugin) continue;

        if (!loadTool(plugin) && !loadFilter(plugin)) {
            printf("Plugin does not support any of interfaces!\n");
            delete plugin;
        }
    }

    ASSERT(closedir(dir) != -1, "Failed to close plugin folder!\n");
}


Plugin *PluginLoader::loadPlugin(const char *path) {
    ASSERT(path, "Path is nullptr!\n");

    struct stat file_info = {};

    ASSERT(stat(path, &file_info) != -1, "Failed to get file info!\n");

    // Skip '/.' and '/..' directories
    if (!S_ISREG(file_info.st_mode)) return nullptr;

    void *handle = dlopen(path, RTLD_NOW | RTLD_NODELETE);

    if (!handle) {
        printf("Failed to open %s: %s\n", path, dlerror());
        return nullptr;
    }

    load_plugin_t load_plugin = reinterpret_cast<load_plugin_t>(dlsym(handle, LOAD_PLUGIN_SYM));

    if (!load_plugin) {
        printf("Failed to get loadPlugin symbol address!\n");
        return nullptr;
    }

    // RTLD_NODELETE flag leaves library in memory after close
    ASSERT(dlclose(handle) != -1, "Failed to close dl!\n");

    Plugin *plugin = (*load_plugin)();

    if (!plugin)
        printf("Failed to get plugin instance!\n");

    return plugin;
}


bool PluginLoader::loadTool(Plugin *plugin) {
    Tool *tool = static_cast<Tool*>(plugin->tryGetInterface(PluginGuid::Tool));
    
    if (tool) {
        // Set tool color palette and active canvas
        tool->setActiveCanvas(CANVAS_GROUP.getActive()->getCanvas());
        tool->setColorPalette(COLOR_PALETTE);

        // Add tool to palette view
        TOOL_PALETTE.addTool(*tool);

        return true;
    }
    
    return false;
}


bool PluginLoader::loadFilter(Plugin *plugin) {
    Filter *filter = static_cast<Filter*>(plugin->tryGetInterface(PluginGuid::Filter));

    if (filter) {
        FILTER_PALETTE.addFilter(*filter);

        menu.addButton(
            menu_button_id,
            filter->getPluginData()->getName(),
            new FilterAction(FILTER_PALETTE.getFilterCount() - 1)
        );

        return true;
    }
    
    return false;
}
