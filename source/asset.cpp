/**
 * \file
 * \brief Contains sources of AssetClasses functions
*/


#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "asset.hpp"


const size_t WINDOW_ASSETS_SIZE = 9;
const size_t MAX_PATH = 256;


WindowAsset::WindowAsset(const char *rootpath) : textures(nullptr) {
    static const char *PATHS_TO_ASSETS[] = {
        "title",
        "top_left",
        "left",
        "bottom_left",
        "bottom",
        "bottom_right",
        "right",
        "top_right",
        "center"
    };

    textures = new sf::Texture[WINDOW_ASSETS_SIZE];
    assert(textures);

    char *path = (char *) calloc(strlen(rootpath) + MAX_PATH, 1);
    assert(path);

    for (size_t i = 0; i < WINDOW_ASSETS_SIZE; i++) {
        sprintf(path, "%s/%s.png", rootpath, PATHS_TO_ASSETS[i]);
        assert(textures[i].loadFromFile(path));
    }

    free(path);

    // SET REPEAT FLAG

    textures[TITLE].setRepeated(true);
    textures[FRAME_CENTER].setRepeated(true);
    textures[FRAME_B].setRepeated(true);
    textures[FRAME_L].setRepeated(true);
    textures[FRAME_R].setRepeated(true);
}


WindowAsset::WindowAsset(const WindowAsset &arg) : textures(nullptr) {
    assert(arg.textures);

    textures = new sf::Texture[WINDOW_ASSETS_SIZE];
    assert(textures);

    for (size_t i = 0; i < WINDOW_ASSETS_SIZE; i++)
        textures[i] = arg.textures[i];
}


WindowAsset &WindowAsset::operator = (const WindowAsset &arg) {
    assert(textures);
    assert(arg.textures);

    if (this != &arg) {
        for (size_t i = 0; i < WINDOW_ASSETS_SIZE; i++)
            textures[i] = arg.textures[i];
    }
    return *this;
}


const sf::Texture &WindowAsset::operator [] (TEXTURE_ID id) const {
    assert(textures);

    return textures[id];
}


WindowAsset::~WindowAsset() {
    assert(textures);
    
    delete[] textures;
}
