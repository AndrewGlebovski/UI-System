/**
 * \file
 * \brief Contains AssetClasses and protypes of their functions 
*/


/// Holds textures required for window
class WindowAsset {
private:
    sf::Texture *textures;      ///< Textures required for window

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


    /**
     * \brief Loads window textures from the given directory
    */
    WindowAsset(const char *rootpath);


    WindowAsset(const WindowAsset &arg);


    WindowAsset &operator = (const WindowAsset &arg);


    /**
     * \brief Returns texture by its id
    */
    const sf::Texture &operator [] (TEXTURE_ID id) const;


    ~WindowAsset();
};
