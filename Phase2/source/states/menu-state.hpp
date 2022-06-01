#pragma once

#include <application.hpp>
#include <asset-loader.hpp>
#include <../common/systems/text-renderer.cpp>

enum SelectedOption
{
    START = 0,
    QUIT = 1
};
// This state shows how to use the ECS framework and deserialization.
class MenuState : public our::State
{

    bool isGameStarted = false;
    int selectedOption = 0;
    bool lastPressUp = false;
    bool lastPressDown = false;

    our::TextRenderer textRenderer;
    our::Font *font;
    our::ShaderProgram *shader;
    glm::vec3 selectedTextColor = {1.0, 1.0, 1.0};
    glm::vec3 unselectedTextColor = {0.5, 0.5, 0.5};
    glm::vec3 bgColor = {0.2, 0.2, 0.2};
    std::string startGameText = "Start Game!";
    std::string resumeGameText = "Resume";
    std::string quitGameText = "Quit";

    void deserialize(const nlohmann::json &config)
    {
        if (config.contains("selectedTextColor"))
        {
            std::vector<float> v = config["selectedTextColor"];
            selectedTextColor = glm::vec3(v[0], v[1], v[2]);
        }
        if (config.contains("unselectedTextColor"))
        {
            std::vector<float> v = config["unselectedTextColor"];
            unselectedTextColor = glm::vec3(v[0], v[1], v[2]);
        }
        if (config.contains("bgColor"))
        {
            std::vector<float> v = config["bgColor"];
            bgColor = glm::vec3(v[0], v[1], v[2]);
        }
        startGameText = config.value("startGameText", startGameText);
        resumeGameText = config.value("resumeGameText", resumeGameText);
        quitGameText = config.value("quitGameText", quitGameText);

        font = our::AssetLoader<our::Font>::get(config.value("font", "default"));
        shader = our::AssetLoader<our::ShaderProgram>::get(config.value("shader", "text"));
    }
    
    void select(){
        if(selectedOption==SelectedOption::START){
            getApp()->changeState("game");
            isGameStarted=true;
        }
        if(selectedOption==SelectedOption::QUIT){
            glfwSetWindowShouldClose(getApp()->getWindow(),true);
        }
    }

    void onInitialize() override
    {

        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            auto &assets = config["assets"];
            // in the menu we only need the shaders and the fonts
            // since we will only need to render text
            our::AssetLoader<our::Font>::deserialize(assets["fonts"]);
            our::AssetLoader<our::ShaderProgram>::deserialize(assets["shaders"]);
        }
        deserialize(config["menu"]);
        textRenderer.initialize(getApp()->getWindowSize(), shader);
    }
    void updateSelection()
    {
        auto keyboard = getApp()->getKeyboard();
        if (keyboard.isPressed(GLFW_KEY_ENTER) || keyboard.isPressed(GLFW_KEY_KP_ENTER))
            select();
        
        if (keyboard.isPressed(GLFW_KEY_UP) && (keyboard.isPressed(GLFW_KEY_UP) != lastPressUp))
        {
            selectedOption -= 1;
        }
        else if (keyboard.isPressed(GLFW_KEY_DOWN) && (keyboard.isPressed(GLFW_KEY_DOWN) != lastPressDown))
        {
            selectedOption += 1;
        }
        lastPressUp = keyboard.isPressed(GLFW_KEY_UP);
        lastPressDown = keyboard.isPressed(GLFW_KEY_DOWN);

        selectedOption %= 2;
        if (selectedOption < 0)
            selectedOption += 2;
    }
    void onDraw(double deltaTime) override
    {
        glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 center = getApp()->getWindowSize() / 2;
        updateSelection();
        std::vector<our::TextRenderCommand>
            commands = {
                our::TextRenderCommand(
                    font,
                    isGameStarted ? resumeGameText : startGameText,
                    1.5f,
                    center - glm::vec2(100, -10),
                    selectedOption == SelectedOption::START ? selectedTextColor : unselectedTextColor),
                our::TextRenderCommand(
                    font,
                    quitGameText,
                    1.0f,
                    center - glm::vec2(100, 50),
                    selectedOption == SelectedOption::QUIT ? selectedTextColor : unselectedTextColor),
            };
        textRenderer.render(commands);
    }

};
