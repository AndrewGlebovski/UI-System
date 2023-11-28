/**
 * \file
 * \brief Contains sources of AssetClasses functions
*/


#include <cstdlib>
#include <cstring>
#include "common/assert.hpp"
#include "common/asset.hpp"


const size_t MAX_PATH = 256;


Asset::Asset() : textures() {}


Asset::Asset(const Asset &arg) : textures(arg.textures.size(), nullptr) {
    for (size_t i = 0; i < textures.size(); i++)
        textures[i] = new Texture(*arg.textures[i]);
}


void Asset::loadTextures(const char *rootpath, const char *files[], size_t files_size) {
    textures.resize(files_size, nullptr);

    char *path = (char *) calloc(strlen(rootpath) + MAX_PATH, 1);
    ASSERT(path, "Failed to allocate buffer!\n");

    for (size_t i = 0; i < files_size; i++) {
        sprintf(path, "%s/%s.png", rootpath, files[i]);
        loadTexture(&textures[i], path);
    }

    free(path);
}


Asset &Asset::operator = (const Asset &arg) {
    if (this != &arg) {
        for (size_t i = 0; i < textures.size(); i++)
            delete textures[i];

        textures.resize(arg.textures.size(), nullptr);

        for (size_t i = 0; i < textures.size(); i++)
            textures[i] = arg.textures[i];
    }

    return *this;
}


const Texture &Asset::getTexture(int id) const {
    ASSERT(0 <= id && size_t(id) < textures.size(), "Index is out of range!\n");

    return *(textures[id]);
}


Asset::~Asset() {
    for (size_t i = 0; i < textures.size(); i++)
        delete textures[i];
}


WindowAsset::WindowAsset(const char *rootpath) {
    static const char *FILES[] = {
        "title",
        "top_left",
        "left",
        "bottom_left",
        "bottom",
        "bottom_right",
        "right",
        "top_right",
        "center",
        "button_normal",
        "button_hover",
        "button_pressed",
        "expand_icon",
        "close_icon"
    };

    loadTextures(rootpath, FILES, sizeof(FILES) / 8);
}


const Texture &WindowAsset::operator [] (TEXTURE_ID id) const {
    return getTexture(id);
}


PaletteViewAsset::PaletteViewAsset(const char *rootpath) {
    static const char *FILES[] = {
        "bucket",
        "eraser",
        "line",
        "pencil",
        "picker",
        "rect",
        "polygon",
        "normal",
        "selected",
        "text"
    };

    loadTextures(rootpath, FILES, sizeof(FILES) / 8);
}


const Texture &PaletteViewAsset::operator [] (TEXTURE_ID id) const {
    return getTexture(id);
}
