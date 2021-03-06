#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include <glm/gtx/euler_angles.hpp>
#include "../components/light.hpp"

namespace our {

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json& config){
        // First, we store the window size for later use
        this->windowSize = windowSize;
        deserialize(config);
        
        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));
            
            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram* skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();
            
            //TODO: (Req 9) Pick the correct pipeline state to draw the sky
            // Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            // We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState;
            skyPipelineState.depthTesting.enabled=true;         //// configure the pipeline state for the sky
            skyPipelineState.depthTesting.function=GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace=GL_FRONT;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D* skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky 
            Sampler* skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material 
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if(config.contains("postprocess")){
            //TODO: (Req 10) Create a framebuffer
            glGenFramebuffers(1,&postprocessFrameBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);

            //TODO: (Req 10) Create a color and a depth texture and attach them to the framebuffer
            // Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            // The depth format can be (Depth component with 24 bits).
            colorTarget=texture_utils::empty(GL_RGBA8,windowSize);
            depthTarget = texture_utils::empty(GL_DEPTH_COMPONENT32, windowSize);

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT , GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);

            //TODO: (Req 10) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);
            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler* postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram* postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy(){
        // Delete all objects related to the sky
        if(skyMaterial){
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if(postprocessMaterial){
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }
    std::vector<Entity*> getLights(World* world){
        std::vector<Entity *> entities;
        for (auto entity : world->getEntities())
        {
            // If this entity has a light renderer component
            if (auto light = entity->getComponent<Light>(); light)
            {
                entities.push_back(entity);
            }
        }
        return entities;
    }
    
 
    /*
        setup all the lights in the scene so that the shader get affected with it
        this is done by giving the shader all the required parameters about each light in the scene
    */
    void setupLighting(ShaderProgram *shader, std::vector<Entity *> lightEntities)
    {
        shader->set("light_count", (int)lightEntities.size());
        for (int i = 0; i < lightEntities.size(); i++)
        {

            std::string lightStr = "lights[" + std::to_string(i) + "]";
            auto M = lightEntities[i]->getLocalToWorldMatrix();
            /*
                assume default light direction going downwards
                so to get the direction of the light we  nedd to rotate the vector rotating downwards to point to the required direction
            */

            auto dir = glm::normalize(glm::vec3(M * glm::vec4(0, -1, 0, 0)));
            shader->set(lightStr + ".direction", dir);
            shader->set(lightStr + ".position", glm::vec3(M * glm::vec4(0, 0, 0, 1)));
            
            Light* light=lightEntities[i]->getComponent<Light>();
            shader->set(lightStr + ".type", (int)light->lightType);
            shader->set(lightStr + ".diffuse", light->diffuse);
            shader->set(lightStr + ".specular",light->specular);
            shader->set(lightStr + ".attenuation",light->attenuation);
            shader->set(lightStr + ".cone_angles", glm::vec2(glm::radians(light->cone_angles.x), glm::radians(light->cone_angles.y)));
        }
    }

    void ForwardRenderer::render(World* world){
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent* camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        for(auto entity : world->getEntities()){
            // If we hadn't found a camera yet, we look for a camera in this entity
            if(!camera) camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if(auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer){
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if(command.material->transparent){
                    transparentCommands.push_back(command);
                } else {
                // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if(camera == nullptr) return;

        //TODO: (Req 8) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        our::Entity* cameraOwner= camera->getOwner();
        glm::vec3 cameraForward = cameraOwner->getLocalToWorldMatrix()*glm::vec4(0,0,-1,1);

        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand& first, const RenderCommand& second){
            //TODO: (Req 8) Finish this function
            // the far object should be drawn first so that the near object replaces it   
            // HINT: the following return should return true "first" should be drawn before "second".

            float fz = glm::dot(first.center, cameraForward);
            float sz = glm::dot(second.center, cameraForward);
            return fz<sz;
        });

        //TODO: (Req 8) Get the camera ViewProjection matrix and store it in VP
        glm::mat4 VP = camera->getProjectionMatrix(windowSize)*camera->getViewMatrix();

        //TODO: (Req 8) Set the OpenGL viewport using windowSize
        /*
        *   x , y : Specify the lower left corner
        *   width : Specify viewport width
        *   height: Specify viewport height
        */
        glViewport(0,0,windowSize.x,windowSize.y);
        //TODO: (Req 8) Set the clear color to black and the clear depth to 1
        //set the clear color to black
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        //set the clear depth to 1
        glClearDepth(1);
        // TODO: (Req 8) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        // enable writing to frame buffer for both color & depth
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            //TODO: (Req 10) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        //TODO: (Req 8) Clear the color and depth buffers
        //clear the background to be(black) and clear the depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::vector<Entity*> lights = getLights(world);

        auto executeCommands = [&VP, &camera,&lights,this](std::vector<RenderCommand> commands)
        {
            for(RenderCommand command: commands)
            {
                // configure the commands then draw them
                ShaderProgram* shader=command.material->shader;
                Mesh* mesh=command.mesh;
                command.material->setup();

                shader->set("eye",glm::vec3(camera->getOwner()->getLocalToWorldMatrix()* glm::vec4(camera->eye,1)));
                shader->set("M", command.localToWorld);
                shader->set("VP", VP);
                shader->set("MIT", glm::transpose(glm::inverse(command.localToWorld)));
                
                setupLighting(shader,lights);
                shader->set("sky.top", this->topLight);
                shader->set("sky.middle", this->middleLight);
                shader->set("sky.bottom", this->topLight);

                mesh->draw();
            }
        };
        //TODO: (Req 8) Draw all the opaque commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        executeCommands(opaqueCommands);
        // If there is a sky material, draw the sky
        if(this->skyMaterial){
            //TODO: (Req 9) setup the sky material
            this->skyMaterial->setup();
            
            //TODO: (Req 9) Get the camera position
            glm::vec3 cameraPosition=cameraOwner->getLocalToWorldMatrix()*glm::vec4(camera->eye,1);


            //TODO: (Req 9) Create a model matrix for the sy such that it always follows the camera (sky sphere center = camera position)
            glm::mat4 identity(1.0f);
            glm::mat4 M= glm::translate(identity,cameraPosition);
            //  glm::mat4 M= identity;

            //TODO: (Req 9) We want the sky to be drawn behind everything (in NDC space, z=1)
            // We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            /**
             * 1 0  0  0
             * 0 1  0  0
             * 0 0 (0)(1)
             * 0 0 0   1
             * 
             * this will make z=w 
             * 
             */
            glm::mat4 alwaysBehindTransform = glm::mat4(
            //  Row1, Row2, Row3, Row4
                1.0f, 0.0f, 0.0f, 0.0f, // Column1
                0.0f, 1.0f, 0.0f, 0.0f, // Column2
                0.0f, 0.0f, 0.0f, 0.0f, // Column3
                0.0f, 0.0f, 1.0f, 1.0f  // Column4
            );
            //TODO: (Req 9) set the "transform" uniform
            skyMaterial->shader->set("M", M);
            skyMaterial->shader->set("VP", alwaysBehindTransform * VP);
            

            //TODO: (Req 9) draw the sky sphere
            skySphere->draw();
        }
        //TODO: (Req 8) Draw all the transparent commands
        // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        executeCommands(transparentCommands);
        // If there is a postprocess material, apply postprocessing
        if(postprocessMaterial){
            //TODO: (Req 10) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            //TODO: (Req 10) Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();

            glActiveTexture(GL_TEXTURE1);
            ShaderProgram* shader=postprocessMaterial->shader;
            depthTarget->bind();
            postprocessMaterial->sampler->bind(1);
            shader->set("depth_sampler", 1);
            shader->set("inverse_projection",glm::inverse(camera->getProjectionMatrix(windowSize)));
            glActiveTexture(GL_TEXTURE0);

            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES,0,3);
        }
    }

}