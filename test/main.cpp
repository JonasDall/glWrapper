#include "glWrapper.hpp"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

int main(){

    glWrap::Engine engine;

    std::map<std::string, glWrap::Mesh> meshes;

    glWrap::LoadMesh(meshes, "../assets/Triangle.gltf");

    for (auto& mesh : meshes){
        std::cout << mesh.first << '\n';
    }

    glWrap::Shader shader("../assets/vertex.glsl", "../assets/fragment.glsl");

    glWrap::Instance instance;

    instance.SetMesh(&meshes.at("Triangle.0"));

    instance.SetShader(&shader, 0);

    glWrap::Camera camera;

    glWrap::Window window("Window", {500, 500}, {0.2f, 0.6f, 0.8f, 1.0f}, &camera, engine.GetContext());

    while (!window.WindowRequestedClose()){
        window.Draw(instance);
        window.Swap();

        engine.Update();
    }

    return 0;
}