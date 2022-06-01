#include "text-renderer.hpp"
#include<asset-loader.hpp>
namespace our{
    void TextRenderer::renderText(TextRenderCommand command){
        if(!command.font) {
            std::cerr<<"couldn't find font to render text"<<std::endl;
            return ;
        }
        pipelineState.setup();
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(windowSize.x), 0.0f, static_cast<float>(windowSize.y));
     
        shader->use();
        shader->set("projection", projection);
        shader->set("textColor", command.color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // iterate through all characters
        std::string::const_iterator c;
        for (c = command.text.begin(); c != command.text.end(); c++)
        {
            our::Character ch = command.font->characters[*c];

            float xpos = command.position.x + ch.bearing.x * command.scale;
            float ypos = command.position.y - (ch.size.y - ch.bearing.y) * command.scale;

            float w = ch.size.x * command.scale;
            float h = ch.size.y * command.scale;
            // update VBO for each character
            float vertices[6][4] = {
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos, ypos, 0.0f, 1.0f},
                {xpos + w, ypos, 1.0f, 1.0f},

                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos + w, ypos, 1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}};
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            command.position.x += (ch.advance >> 6) * command.scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TextRenderer::initialize(glm::ivec2 windowSize,ShaderProgram * shader)
    {
        this->windowSize=windowSize;
        this->shader = shader;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        pipelineState.faceCulling.enabled = true;
        pipelineState.blending.enabled = true;
        pipelineState.blending.sourceFactor = GL_SRC_ALPHA;
        pipelineState.blending.destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
    }

    void TextRenderer::render(const std::vector<TextRenderCommand> &commands){
        if (!shader)
        {
            std::cerr << "couldn't find text shader" << std::endl;
            return;
        }
        for (auto command :commands){
            renderText(command);
        }
    }
}