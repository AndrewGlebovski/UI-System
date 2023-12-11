/**
 * \file
 * \brief Contains implementation of PluginLoader
*/


#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
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
    size_t menu_button_id_,
    Window &window_
) :
    root_dir(root_dir_),
    menu(menu_),
    menu_button_id(menu_button_id_),
    window(window_)
{
    scan();
}


void PluginLoader::scan() {
    DIR *dir = NULL;
    struct dirent *dir_info = NULL;

    dir = opendir(root_dir);
    
    if (!dir) {
        printf("Failed to open root folder!\n");
        return;
    }

    char plugin_dir[MAX_PATH * 2] = "";

    while ((dir_info = readdir(dir)) != NULL) {
        sprintf(plugin_dir, "%s/%s", root_dir, dir_info->d_name);
        if (isDir(plugin_dir)) scanPluginDir(plugin_dir);
    }

    ASSERT(closedir(dir) != -1, "Failed to close root folder!\n");
}


bool PluginLoader::scanPluginDir(const char *path) {
    ASSERT(path, "Path is nullptr!\n");
    
    DIR *dir = NULL;
    struct dirent *dir_info = NULL;

    dir = opendir(path);
    
    if (!dir) {
        printf("Failed to open plugin folder!\n");
        return false;
    }

    char plugin_file[MAX_PATH * 2] = "";

    while ((dir_info = readdir(dir)) != NULL) {
        sprintf(plugin_file, "%s/%s", path, dir_info->d_name);
        
        if (!isPluginFile(plugin_file)) continue;

        plug::Plugin *plugin = loadPlugin(plugin_file);

        if (!plugin) continue;

        if (!loadTool(plugin) && !loadFilter(plugin)) {
            printf("Plugin does not support any of interfaces!\n");
            plugin->release();
            return false;
        }

        plugin->release();
        break;
    }

    ASSERT(closedir(dir) != -1, "Failed to close plugin folder!\n");
    return true;
}


bool PluginLoader::isDir(const char *path) const {
    struct stat file_info = {};

    ASSERT(stat(path, &file_info) != -1, "Failed to get file info!\n");

    return S_ISDIR(file_info.st_mode);
}


bool PluginLoader::isPluginFile(const char *path) const {
    struct stat file_info = {};

    ASSERT(stat(path, &file_info) != -1, "Failed to get file info!\n");

    // File type check
    if (!S_ISREG(file_info.st_mode)) return false;

    // Extension check
    size_t len = strlen(path);
    if (len <= 3) return false;

    if (path[len - 1] != 'o') return false;
    if (path[len - 2] != 's') return false;
    if (path[len - 3] != '.') return false;

    return true;
}


plug::Plugin *PluginLoader::loadPlugin(const char *path) {
    ASSERT(path, "Path is nullptr!\n");

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

    plug::Plugin *plugin = (*load_plugin)();

    if (!plugin) {
        printf("Failed to get plugin instance from %s!\n", path);
    }

    return plugin;
}


bool PluginLoader::loadTool(plug::Plugin *plugin) {
    plug::Tool *tool = static_cast<plug::Tool*>(
        plugin->tryGetInterface(static_cast<size_t>(plug::PluginGuid::Tool))
    );
    
    if (tool) {
        // Add tool to palette view
        TOOL_PALETTE.addTool(*tool);

        return true;
    }
    
    return false;
}


bool PluginLoader::loadFilter(plug::Plugin *plugin) {
    plug::Filter *filter = static_cast<plug::Filter*>(
        plugin->tryGetInterface(static_cast<size_t>(plug::PluginGuid::Filter))
    );

    if (filter) {
        FILTER_PALETTE.addFilter(*filter);

        menu.addButton(
            menu_button_id,
            filter->getPluginData()->getName(),
            new FilterAction(window, FILTER_PALETTE.getFilterCount() - 1)
        );

        return true;
    }
    
    return false;
}
