#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <algorithm>

#include "gl/glad.h"
#include "gl/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "tinygltf/tinygltf.hpp"

namespace glWrap
{
    struct Vertex{
        glm::vec3 pos{};
        glm::vec3 nor{};
        glm::vec2 tex{};
    };

    struct Transform{
        glm::vec3 pos{};
        glm::vec3 rot{};
        glm::vec3 scl{};
    };

    class Shader
    {
        public:
        unsigned int m_ID;
        
        Shader(std::string vertexPath, std::string fragmentPath);
        Shader(const char* vertexShader, const char* fragmentShader, bool isText);

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

    class WorldObject{
    protected:
        Transform   m_transform;

    public:
        Transform GetTransform();
        glm::vec3 GetPosition();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();
        glm::vec3 GetForwardVector();
        glm::vec3 GetUpwardVector();
        glm::vec3 GetDirection();

        void SetTransform(Transform transform);
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale);

        void AddPosition(glm::vec3 position);
        void AddRotation(glm::vec3 rotation);
        void AddScale(glm::vec3 scale);
    };

    class Camera : public WorldObject{
    private:
        float       m_FOV{90};
        glm::vec2   m_aspect{800, 600};
        glm::vec2   m_clip{0.1f, 1000.f};
        bool        m_perspective{true};

    public:
        float GetFOV();
        glm::mat4 GetView();
        glm::mat4 GetProjection();
        
        void SetFOV(float FOV);
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
    };

    class Instance : public WorldObject {
    private:
        Mesh*                   m_model;
        std::vector<Shader*>    m_shaders;
        bool                    m_remove;
        bool                    m_visible;

    public:
        Instance(Mesh* mesh);

        void SetMesh(Mesh* mesh);
        void SetShader(Shader* shader, int primitive);
        void SetTransform(Transform transform);
        void Draw(Shader* defaultShader, Camera* camera);
        void FlagRemoval();
        bool ToBeRemoved();
        void SetVisible(bool is);
    };

    /*
    class Window{
    public:
        std::string             m_name;
        GLFWwindow*             m_window;
        std::vector<Instance*>  m_instances;
        glm::vec4               m_color;

        // Window() = default;
        Window(std::string name, glm::ivec2 size, bool visible, glm::vec4 color);
        bool AddInstance(Instance* instance);
        bool RemoveInstance(Instance* instance);
        void DrawInstances();
        ~Window();
    };
    */

    class Loader{
    private:

        // static void framebuffer_size_callback(GLFWwindow* win, int width, int height);
        static void keyCall(GLFWwindow* window, int key, int scancode, int action, int mods);

        GLFWwindow*                 m_mainWindow;
        glm::vec4                   m_clearColor;

        std::map<std::string, Mesh> m_models;
        std::vector<Instance>       m_instances;
        Camera*                     m_Activecamera;
        std::unique_ptr<Shader>     m_defaultShader;
        static std::vector<unsigned int>   m_heldKeys;

        double                      m_lastFrameTime;
        double                      m_deltaTime;
        // std::map<std::string, std::unique_ptr<Window>> m_windows;

    public:

        Loader(std::string name, glm::ivec2 size, bool visible, glm::vec4 color);
        void Load(std::string path);
        // Window* AddWindow(std::string name, glm::ivec2 size, bool visible, glm::vec4 color);
        // bool RemoveWindow(std::string name);
        // Window* GetWindow(std::string name);
        // int GetWindowAmount();

        Instance* AddInstance(std::string model);
        Mesh* GetMesh(std::string name);
        void Update();
        float GetDeltaTime();

        bool isKeyPressed(unsigned int key);
        bool isKeyReleased(unsigned int key);
        bool isKeyHeld(unsigned int key);
        bool isKeyRepeat(unsigned int key);
        bool WindowRequestedClose();

        void SetActiveCamera(Camera* camera);

        ~Loader();
    };
}