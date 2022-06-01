#include <iostream>
#include <map>
#include <vector>
#include<glad/gl.h>
#include<glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#define SMALL_FONT_SIZE 14
#define LARGE_FONT_SIZE 48

namespace our{

    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character
    {
        unsigned int textureID; // ID handle of the glyph texture
        glm::ivec2 size;        // Size of glyph
        glm::ivec2 bearing;     // Offset from baseline to left/top of glyph
        unsigned int advance;   // Horizontal offset to advance to next glyph
    };

    class Font{
        void loadSize(FT_Face face,int fontSize){
            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, fontSize);

            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer);
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)};
                characters.insert(std::pair<char, Character>(c, character));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        public:
            std::map<GLchar, Character> characters;
            Font(std::string font_name,int smallFontSize=SMALL_FONT_SIZE,int largeFontSize=LARGE_FONT_SIZE)
            {
                // FreeType
                // --------
                FT_Library ft;
                // All functions return a value different than 0 whenever an error occurred
                if (FT_Init_FreeType(&ft))
                {
                    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
                    return;
                }

                // load font as face
                FT_Face face;
                if (FT_New_Face(ft, font_name.c_str(), 0, &face))
                {
                    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
                    return;
                }
                else
                {
                    loadSize(face,largeFontSize);
                }
                // destroy FreeType once we're finished
                FT_Done_Face(face);
                FT_Done_FreeType(ft);
            }
    };
};
