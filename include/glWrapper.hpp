#pragma once

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <algorithm>

#include "glad.h"
#include "glfw3.h"
#include "glm.hpp"
// #include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/type_ptr.hpp"
// #include "tinygltf/tinygltf.hpp"
// #include "tinygltf/stb_image.h"

struct GLFWwindow;

namespace glWrap
{

    struct AttributeData{
        std::vector<float> data;
        unsigned int size{};
    };

    struct MeshData{
        std::vector<AttributeData> attributes;
        std::vector<unsigned short> indices;
    };

    struct ModelData{
        std::vector<MeshData> meshes;
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

    class Shader
    {
    private:
        unsigned int m_ID;

        std::map<std::string, bool>         m_bools;
        std::map<std::string, int>          m_ints;
        std::map<std::string, float>        m_floats;
        std::map<std::string, glm::mat4>    m_mat4s;
        std::map<std::string, Texture2D*>   m_textures;

    public:
        Shader(std::string vertexPath, std::string fragmentPath);
        Shader(const char* vertexShader, const char* fragmentShader, bool isText);

        void Use();
        void Update();

        void SetBool(const std::string name, bool value);
        void SetInt(const std::string name, int value);
        void SetFloat(const std::string name, float value);
        void SetMatrix4(const std::string name, glm::mat4 mat);
        void SetTexture(const std::string name, Texture2D* texture);
    };

    class WorldObject{
    public:
        glm::vec3 m_pos{};
        glm::vec3 m_rot{};
        glm::vec3 m_scl{1.f, 1.f, 1.f};

        glm::vec3 GetForwardVector();
        glm::vec3 GetUpwardVector();
        glm::vec3 GetRightVector();
        glm::mat4 GetTransformMatrix();

        void AddPosition(glm::vec3 position);
        void AddRotation(glm::vec3 rotation);
        void AddScale(glm::vec3 scale);
    };

    class Camera : public WorldObject{
    private:
        float       m_FOV{90};
        glm::vec2   m_clip{0.1f, 1000.f};
        bool        m_perspective{true};
        glm::vec3*   m_target{};

    public:
        float GetFOV();
        glm::mat4 GetView();
        glm::mat4 GetProjection(glm::vec2 aspect);
        bool IsPerspective();

        void SetTarget(glm::vec3* target);
        void SetFOV(float FOV);
        void AddFOV(float FOV);
        void SetPerspective(bool isTrue);
    };

    class Mesh{
    public:
        std::vector<GLuint> m_buffers;
        unsigned int        m_indexAmount;
        GLuint m_VAO{}, m_EBO{};

        Mesh();
        // void SetMeshData(MeshData& mesh);
        void SetAttributeData(std::vector<float>& data, unsigned int size, unsigned int layout, unsigned int divisor, GLenum drawtype);
        void SetAttributeData(std::vector<float>& data, unsigned int size, unsigned int layout);

        void SetIndexData(std::vector<unsigned short>& indices);
        void Draw();
        void Draw(unsigned int count);
    };

    class Model{
    public:
        std::vector<Mesh> m_meshes;

        void SetModelData(ModelData& model);
        void SetModelAttribute(std::vector<float>& data, unsigned int size, unsigned int layout, unsigned int divisor, GLenum drawtype);
        void Draw();
        void Draw(unsigned int count);
    };

    class Animation{

    };

    class Bone{
    public:
        std::string m_name;
        glm::vec3 m_localPos;
        glm::vec4 m_localRot;
        glm::vec3 m_localScl;
        std::vector<unsigned int> m_parent;
        std::vector<unsigned int> m_children;
    };

    class Skeleton{
        std::vector<Bone> m_bones;
    };

    class Window{
    private:
        static void keyCall(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void frameCall(GLFWwindow* window, int width, int height);
        // static void mousePosCall(GLFWwindow* window, double xpos, double ypos);

        GLFWwindow*                         m_window;
        std::string                         m_name;
        double                              m_lastFrameTime;
        double                              m_deltaTime;
        bool                                m_firstFrame{true};
        bool                                m_windowRequestClose;
        static std::vector<unsigned int>    m_pressedKeys;
        static std::vector<unsigned int>    m_releasedKeys;
        static std::vector<unsigned int>    m_repeatKeys;
        static glm::dvec2                   m_lastMousePos;
        static glm::dvec2                   m_deltaMousePos;
        static glm::ivec2                   m_size;

    public:
        glm::vec4                           m_color{0.0f, 0.0f, 0.0f, 1.0f};

        // Window() = default;
        Window(std::string name, glm::ivec2 size);
        // Window(std::string name, glm::ivec2 size, GLFWwindow* context);
        void Swap();
        // void Draw(Copy& copy);
        float GetDeltaTime();
        void LoadGLTF(std::map<std::string, ModelData>& modelContainer, std::map<std::string, Skeleton>& skeletonContainer, std::string file);
        ~Window();

        bool IsKeyPressed(unsigned int key);
        bool IsKeyReleased(unsigned int key);
        bool IsKeyRepeat(unsigned int key);
        bool IsKeyHeld(unsigned int key);
        bool IsRequestedClose();
        glm::dvec2 GetMousePos();
        glm::dvec2 GetDeltaMousePos();
        glm::ivec2 GetSize();

        void SetRequestedClose(bool);
        void SetInputMode(unsigned int mode, unsigned int value);

        GLFWwindow* GetContext();
    };
}