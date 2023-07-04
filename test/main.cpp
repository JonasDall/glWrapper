#include "glWrapper.hpp"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << ' ' << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

int main(){

    glWrap::Window window("Window", {1000, 1000});
    window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window.m_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    glWrap::Camera camera;
    camera.SetFOV(90.0f);
    camera.SetRotation({0.0f, 0.0f, -90.0f});
    camera.SetPosition({0.0f, 0.0f, 3.0f});
    window.m_ActiveCamera = &camera;

    std::map<std::string, glWrap::Mesh> meshes;

    window.LoadMesh(meshes, "../assets/smiley.gltf");

    glWrap::Texture2D texture("../assets/IdleMan.png", false, GL_NEAREST, GL_RGB, "texture1");

    glWrap::Shader shader("../assets/vertex.glsl", "../assets/fragment.glsl");
    shader.AddTexture(&texture);

    glWrap::Instance instance;
    glWrap::Instance instance2;
    instance.SetMesh(&meshes.at("Cube.0"));
    // instance2.SetMesh(&meshes.at("Triangle.001.0"));
    instance.SetShader(&shader, 0);
    // instance2.SetShader(&shader, 0);

    camera.SetTarget(instance.GetPosition());
    camera.UseTarget(true);

    double WalkSensitivity = 0.01f;
    double MouseSensitivity = 0.1f;

    while (!window.IsRequestedClose()){

        if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) window.setRequestedClose(true);

        // if (window.IsKeyHeld(GLFW_KEY_S)) camera.AddPosition(-camera.GetForwardVector() * glm::vec3(WalkSensitivity));
        // if (window.IsKeyHeld(GLFW_KEY_W)) camera.AddPosition(camera.GetForwardVector() * glm::vec3(WalkSensitivity));
        // if (window.IsKeyHeld(GLFW_KEY_D)) camera.AddPosition(camera.GetRightVector() * glm::vec3(WalkSensitivity));
        // if (window.IsKeyHeld(GLFW_KEY_A)) camera.AddPosition(-camera.GetRightVector() * glm::vec3(WalkSensitivity));
        // 
        // camera.AddRotation({0.0f, 0.0f, window.GetDeltaMousePos().x * MouseSensitivity});
        // camera.AddRotation({0.0f, window.GetDeltaMousePos().y * MouseSensitivity, 0.0f});

        // if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) WalkSensitivity = 0.1f;
        // if (window.IsKeyReleased(GLFW_KEY_LEFT_SHIFT)) WalkSensitivity = 0.01f;
        // if (window.IsKeyPressed(GLFW_KEY_G)) camera.SetPerspective( !camera.IsPerspective() );

        camera.SetPosition(glm::vec3(cos(glfwGetTime()) * 3, 1.f, sin(glfwGetTime()) * 3 ));

        window.Draw(instance);
        // window.Draw(instance2);

        window.Swap();
    }

    return 0;
}