#include "glWrapper.hpp"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

int main(){
    glWrap::Window window("Window", {1000, 1000});
    window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window.m_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    glWrap::Camera camera;
    camera.SetFOV(90.0f);
    camera.m_transform.rot = {0.0f, 0.0f, -90.0f};
    camera.m_transform.pos = {0.0f, 0.0f, 3.0f};

    std::map<std::string, glWrap::Model> meshes;

    window.LoadFile(meshes, "../assets/Cube.gltf");

    for (auto& mesh : meshes){
        DEV_LOG("Model ", mesh.first);
    }

    glWrap::Texture2D texture("../assets/IdleMan.png", false, GL_NEAREST, GL_RGB);
    // glWrap::Texture2D texture2("../assets/RunningMan.png", false, GL_NEAREST, GL_RGB);

    glWrap::Shader shader("../assets/vertex.glsl", "../assets/fragment.glsl");
    shader.Use();
    shader.SetTexture("texture1", &texture);

    glWrap::Copy copy;

    copy.SetModel(&meshes.at("Cube.0"));

    double WalkSensitivity = 0.05f;
    double MouseSensitivity = 0.1f;

    while (!window.IsRequestedClose()){

        if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) window.SetRequestedClose(true);

        if (window.IsKeyHeld(GLFW_KEY_S)) camera.AddPosition(-camera.GetForwardVector() * glm::vec3(WalkSensitivity));
        if (window.IsKeyHeld(GLFW_KEY_W)) camera.AddPosition(camera.GetForwardVector() * glm::vec3(WalkSensitivity));
        if (window.IsKeyHeld(GLFW_KEY_D)) camera.AddPosition(camera.GetRightVector() * glm::vec3(WalkSensitivity));
        if (window.IsKeyHeld(GLFW_KEY_A)) camera.AddPosition(-camera.GetRightVector() * glm::vec3(WalkSensitivity));

        camera.AddRotation({0.0f, 0.0f, window.GetDeltaMousePos().x * MouseSensitivity});
        camera.AddRotation({0.0f, window.GetDeltaMousePos().y * MouseSensitivity, 0.0f});

        shader.SetMatrix4("model", copy.GetTransformMatrix());
        shader.SetMatrix4("view", camera.GetView());
        shader.SetMatrix4("projection", camera.GetProjection(window.GetSize()));

        shader.Update();

        meshes.at("Cube.0").m_meshes[0].Draw();

        // window.Draw(copy);

        window.Swap();
    }

    return 0;
}