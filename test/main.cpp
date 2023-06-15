#include "glWrapper.hpp"

int main(){
    glWrap::Loader loader("Test", {500, 500}, true, {0.1f, 0.1f, 0.1f, 1.0f});
    loader.Load("../assets/Triangle.gltf");

    // glWrap::Shader shader("../assets/vertex.glsl", "../assets/fragment.glsl");
    // glWrap::Shader shader2("../assets/vertex.glsl", "../assets/fragment2.glsl");

    glWrap::Instance* instance = loader.AddInstance("Triangle0");
    // instance->SetShader(&shader, 0);

    glWrap::Instance* instance2 = loader.AddInstance("Triangle.0010");
    // instance2->SetShader(&shader2, 0);

    glWrap::Camera camera;

    loader.SetActiveCamera(&camera);

    while (!loader.WindowRequestedClose() ){
        
        std::cout << (loader.isKeyHeld(GLFW_KEY_E) && loader.isKeyHeld(GLFW_KEY_F) ) << '\n';

        loader.Update();
    }

    return 0;
}