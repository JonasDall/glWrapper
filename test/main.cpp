#include "glWrapper.hpp"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

int main(){

    glWrap::Window window("Window", {500, 500});

    glWrap::Camera camera;

    window.m_ActiveCamera = &camera;

    std::map<std::string, glWrap::Mesh> meshes;

    window.LoadMesh(meshes, "../assets/Triangle.gltf");

    for (auto& mesh : meshes){
        DEV_LOG("Mesh: ", mesh.first);
    }

    glWrap::Shader shader("../assets/vertex.glsl", "../assets/fragment.glsl");
    glWrap::Instance instance;
    glWrap::Instance instance2;
    instance.SetMesh(&meshes.at("Triangle.0"));
    instance2.SetMesh(&meshes.at("Triangle.001.0"));
    instance.SetShader(&shader, 0);
    // instance2.SetShader(&shader, 0);

    while (!window.WindowRequestedClose()){

        if (window.isKeyHeld(GLFW_KEY_SPACE)) instance.AddRotation({0.0f, 1.0f, 0.0f});

        window.Draw(instance);
        window.Draw(instance2);
        window.Swap();
    }

    return 0;
}