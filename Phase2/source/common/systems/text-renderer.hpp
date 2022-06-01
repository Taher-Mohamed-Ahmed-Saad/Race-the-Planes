#pragma once
#include <cmath>
#include "../ecs/world.hpp"
#include "../font/font.hpp"
#include "../shader/shader.hpp"
#include "../common/material/pipeline-state.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>


namespace our
{
    class TextRenderCommand
    {
        public:
            Font *font;
            std::string text;
            float scale;
            glm::vec2 position;
            glm::vec3 color;
            TextRenderCommand(Font *font,
                            std::string text,
                            float scale,
                            glm::vec2 position,
                            glm::vec3 color)
            {
                this->font = font;
                this->text = text;
                this->scale = scale;
                this->position = position;
                this->color = color;
            }
    };
    // the system responsible for rendering text objects
    // unlike forward renderer this renders the text in 2d space
    // it also have a single VAO used to draw any text by modifing it's place and changing the textures
    class TextRenderer
    {

        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;

        unsigned int VAO, VBO;
        PipelineState pipelineState;

        void renderText(TextRenderCommand command);

    public:
        // shader that will be used to render text
        ShaderProgram *shader;
        void initialize(glm::ivec2 windowSize,ShaderProgram* shader);
        void render(const std::vector<TextRenderCommand> &commands);
    };
}
