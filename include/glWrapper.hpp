#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include <memory>

#include "gl/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "tinygltf/tinygltf.hpp"

namespace glWrap
{
    struct Vertex{
        glm::vec3 pos;
        glm::vec3 nor;
        glm::vec2 tex;
    };

    class Shader
    {
        public:
        unsigned int m_ID;
        
        Shader(std::string vertexPath, std::string fragmentPath);
        void Use();

        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const;
        void SetFloat(const std::string &name, float value) const;
        void SetMatrix4(const std::string &name, glm::mat4 mat) const;
    };

    class Texture2D
    {
        public:
        unsigned int m_ID;

        /** @brief Texture2D Constructor 
         *@param[in] image String path to image location on disk
         *@param[in] flip If image should be vertically flipped
         *@param[in] filter Select pixel interpolation: GL_LINEAR or GL_NEAREST
         *@param[in] desiredChannels Select texture channels: GL_RED, GL_RG, GL_RGB or GL_RGBA
         */
        Texture2D(std::string image, bool flip, GLenum filter, GLenum desiredChannels);

        /** @brief Description
         *@param[in] unit GL Texture Unit
         */
        void SetActive(unsigned int unit);
    };

    class Camera{
        
    };
   
    class Primitive{
        public:

        std::vector<Vertex>         m_vertices;
        std::vector<unsigned short> m_indices;
        unsigned int                m_material;

        GLuint                      m_VBO,
                                    m_VAO,
                                    m_EBO;

        Primitive() = default;
        void Draw();
    };

    class Mesh{
        public:
        std::vector<Primitive> m_primitives;

        Mesh() = default;
        void Draw();
    };

    bool loadModel(std::string path, std::vector<Mesh>& meshes);
}