#include "glWrapper.hpp"
// #include "al.h"
// #include "alc.h"
// #include "efx-creative.h"
// #include "efx.h"
// #include "xram.h"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

int main(){

    glWrap::Window window("Window", {1920, 1080});
    window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window.m_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    glWrap::Camera camera;
    camera.SetFOV(90.0f);
    camera.m_rot = {0.0f, 0.0f, -90.0f};
    camera.m_pos = {0.0f, 0.0f, 3.0f};

    glWrap::Texture2D texture("../assets/IdleMan.png", false, GL_NEAREST, GL_RGB);

    glWrap::Shader shader("../assets/vertex.glsl", "../assets/fragment.glsl");
    shader.Use();
    shader.SetTexture("texture1", &texture);

    glWrap::Model model;
    std::map<std::string, glWrap::Skin> skins;

    {
        std::map<std::string, glWrap::ModelData> models;
        window.LoadGLTF(models, skins, "../assets/Cube.gltf");

        for (auto& mesh : models){
            DEV_LOG("Model ", mesh.first);
        }

        model.SetModelData(models.at("Arm.0"));
    }

    glWrap::WorldObject object{};
    object.AddScale({5, 5, 5});

    double WalkSensitivity = 0.05f;
    double MouseSensitivity = 0.1f;

    while (!window.IsRequestedClose()){

        if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) window.SetRequestedClose(true);
        if (window.IsKeyPressed(GLFW_KEY_SPACE)) camera.SetPerspective(!camera.IsPerspective());

        if (window.IsKeyHeld(GLFW_KEY_S)) camera.AddPosition(-camera.GetForwardVector() * glm::vec3(WalkSensitivity));
        if (window.IsKeyHeld(GLFW_KEY_W)) camera.AddPosition(camera.GetForwardVector() * glm::vec3(WalkSensitivity));
        if (window.IsKeyHeld(GLFW_KEY_D)) camera.AddPosition(camera.GetRightVector() * glm::vec3(WalkSensitivity));
        if (window.IsKeyHeld(GLFW_KEY_A)) camera.AddPosition(-camera.GetRightVector() * glm::vec3(WalkSensitivity));

        camera.AddRotation({0.0f, 0.0f, window.GetDeltaMousePos().x * MouseSensitivity});
        camera.AddRotation({0.0f, window.GetDeltaMousePos().y * MouseSensitivity, 0.0f});

        shader.SetMatrix4("model", object.GetTransformMatrix());
        shader.SetMatrix4("model", object.GetTransformMatrix());
        shader.SetMatrix4("view", camera.GetView());
        shader.SetMatrix4("projection", camera.GetProjection(window.GetSize()));

        shader.Update();

        model.Draw();

        window.Swap();
    }

    return 0;
}