#include "glWrapper.hpp"

int main(){
    bool shouldLoop{ true };

    glWrap::Loader loader;

    loader.Load("../assets/Cubes.gltf");
    loader.Load("../assets/Cubes.gltf");
    loader.Load("../assets/Cubes.gltf");

    loader.ListMeshes();

    glWrap::Window* window = loader.AddWindow("Test", glm::ivec2{500, 500}, true);

    while (shouldLoop){


        loader.Update();
    }

    return 0;
}