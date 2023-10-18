/**
 * \file
 * \brief Contains AssetClasses and protypes of their functions 
*/


/// Base class for all assets
class Asset {
protected:
    sf::Texture *textures;      ///< Textures required for window
    size_t count;               ///< Amount of textures


    /**
     * \brief Contructs empty asset
    */
    Asset();


    /**
     * \brief Init texture buffer and copies assets
    */
    Asset(const Asset &arg);


    /**
     * \brief Loads textures from files located in the rootpath directory
    */
    void loadTextures(const char *rootpath, const char *files[], size_t assets_count);


    /**
     * \brief Copies assets
    */
    Asset &operator = (const Asset &arg);


    /**
     * \brief Returns texture by its id
    */
    const sf::Texture &getTexture(int id) const;


    /**
     * \brief Free textures
    */
    ~Asset();
};


/// Holds textures required for window
class WindowAsset : public Asset {
public:
    enum TEXTURE_ID {
        TITLE,                  ///< Title texture (will be repeated)
        FRAME_TL,               ///< Top-left corner texture
        FRAME_L,                ///< Left side texture (will be repeated)
        FRAME_BL,               ///< Bottom-right corner texture
        FRAME_B,                ///< Bottom texture (will be repeated)
        FRAME_BR,               ///< Bottom-right corner texture
        FRAME_R,                ///< Right side texture (will be repeated)
        FRAME_TR,               ///< Top-right corner texture
        FRAME_CENTER            ///< Center texture (will be repeated)
    };


    WindowAsset(const char *rootpath);


    const sf::Texture &operator [] (TEXTURE_ID id) const;
};


/// Holds textures required for palette
class PaletteViewAsset : public Asset {
public:
    enum TEXTURE_ID {
        BUCKET_TEXTURE,
        ERASER_TEXTURE,
        LINE_TEXTURE,
        PENCIL_TEXTURE,
        PICKER_TEXTURE,
        RECT_TEXTURE,
        POLYGON_TEXTURE,
        NORMAL_TEXTURE,
        SELECTED_TEXTURE
    };


    PaletteViewAsset(const char *rootpath);


    const sf::Texture &operator [] (TEXTURE_ID id) const;
};
