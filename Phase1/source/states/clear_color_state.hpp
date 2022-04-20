#pragma once

#include <application.hpp>

class ClearColorState: public our::State {
    // onInitialize() function is called once before the state starts
    void onInitialize() override {
        //TODO: Read the color from the configuration file and use it to set the clear color for the window
        //HINT: you can the configuration for the whole application using "getApp()->getConfig()"
        //To see how the clear color is written in the json files, see "config/blue-screen.json"
        //To know how read data from a nlohmann::json object, 
        //look at the following documentation: https://json.nlohmann.me/features/element_access/


        //This is to get a pointer to the previously created application
        //Then from the application returning the JSON file object
        //From JSON file executing a checked access to get scene then clear-color
        const auto config = getApp()->getConfig()["scene"]["clear-color"];

        //Creating an array for the values of colors and alpha acquired from the configration above
        float clearColor[4] =  {
            config["r"],
            config["g"],
            config["b"],
            config["a"],
        };

        //This function is used to determine the value of the colors and alpha used when clearing color buffers
        //Operand 1: specify the amount of red color used
        //Operand 2: specify the amount of green color used
        //Operand 3: specify the amount of blue color used
        //Operand 4: specify the opacity(alpha) value
        glClearColor(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
    }

    // onDraw(deltaTime) function is called every frame 
    void onDraw(double deltaTime) override {
        //At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame.
        glClear(GL_COLOR_BUFFER_BIT);
    }
};