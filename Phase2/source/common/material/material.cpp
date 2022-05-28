#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our
{

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const
    {
        // TODO: (Req 6) Write this function
        // setup the pipeline state
        pipelineState.setup();

        // set the shader program to be used
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint
    void TintedMaterial::setup() const
    {
        // TODO: (Req 6) Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    void TexturedMaterial::setup() const
    {
        // TODO: (Req 6) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        texture->bind();
        sampler->bind(0);
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    // void MultiTexturedMaterial::setup() const
    // {
    //     // TODO: (Req 6) Write this function
    //     TintedMaterial::setup();
    //     shader->set("alphaThreshold", alphaThreshold);
    //     shader->set("tex_count", (int)textures.size());
    //     auto activeTexture=GL_TEXTURE0;
    //     for(int i=0;i<textures.size();i++){
    //         glActiveTexture(activeTexture+i);
    //         textures[i]->bind();
    //         samplers[i]->bind(0);
    //         shader->set("tex["+std::to_string(i)+"]", 0);
    //     }
    //     glActiveTexture(GL_TEXTURE0);
    // }

    // // This function read the material data from a json object
    // void MultiTexturedMaterial::deserialize(const nlohmann::json &data)
    // {
    //     TintedMaterial::deserialize(data);
    //     if (!data.is_object())
    //         return;
    //     alphaThreshold = data.value("alphaThreshold", 0.0f);
    //     for(int i=0;i<textures.size();i++){
    //         textures[i] = AssetLoader<Texture2D>::get(data.value("textures["+std::to_string(i)+"]", ""));
    //         samplers[i] = AssetLoader<Sampler>::get(data.value("sampler[" + std::to_string(i) + "]","default"));
    //     }
    // }

    void LitMaterial::setup() const
    {
        Material::setup();
        glActiveTexture(GL_TEXTURE0);
        if (albedo)
            albedo->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        albedoSampler->bind(0);
        shader->set("material.albedo", 0);

        glActiveTexture(GL_TEXTURE1);
        if (specular)
            specular->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        specularSampler->bind(1);
        shader->set("material.specular", 1);

        glActiveTexture(GL_TEXTURE2);
        if (roughness)
            roughness->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        roughnessSampler->bind(2);
        shader->set("material.roughness", 2);

        glActiveTexture(GL_TEXTURE3);
        if (ambientOcc)
            ambientOcc->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        ambientOccSampler->bind(3);
        shader->set("material.ambientOcc", 3);

        glActiveTexture(GL_TEXTURE4);
        if (emission)
            emission->bind();
        else
            glBindTexture(GL_TEXTURE_2D,0);
        emissionSampler->bind(4);
        shader->set("material.emissive", 4);

      
        glActiveTexture(GL_TEXTURE0);
    }
    void LitMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        if (data.contains("albedo_texture"))
            albedo = AssetLoader<Texture2D>::get(data.value("albedo_texture", ""));
        albedoSampler = AssetLoader<Sampler>::get(data.value("albedo_sampler", "default"));

        if (data.contains("specular_texture"))
            specular = AssetLoader<Texture2D>::get(data.value("specular_texture", ""));
        specularSampler = AssetLoader<Sampler>::get(data.value("specular_sampler", "default"));

        if (data.contains("roughness_texture"))
            roughness = AssetLoader<Texture2D>::get(data.value("roughness_texture", ""));
        roughnessSampler = AssetLoader<Sampler>::get(data.value("roughness_sampler", "default"));

        if (data.contains("ambient_occlusion_texture"))
            ambientOcc = AssetLoader<Texture2D>::get(data.value("ambient_occlusion_texture", ""));
        ambientOccSampler = AssetLoader<Sampler>::get(data.value("ambient_occlusion_sampler", "default"));

        if (data.contains("emission_texture"))
            emission = AssetLoader<Texture2D>::get(data.value("emission_texture", ""));
        emissionSampler = AssetLoader<Sampler>::get(data.value("emission_sampler", "default"));
    }
}