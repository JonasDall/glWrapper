#include "glWrapper.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "tinygltf/tinygltf.hpp"
#include "tinygltf/stb_image.h"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

/*
void glWrap::Initialize(){

    if(!glfwInit()){
        DEV_LOG("Failed to initialize OPENGL", "");
        return;
    }
}

void glWrap::Terminate(){ glfwTerminate(); }
*/

const std::array<std::string, 6> m_attributeList[]{ "POSITION", "NORMAL", "TEXCOORD_0", "COLOR_0", "JOINTS_0", "WEIGHTS_0" };

// *DEFAULT SHADER SOURCE

const char *defaultVertexShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1);\n"
"}\n";

const char *defaultFragmentShader = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(0.8, 0.8, 0.8, 1);\n"
"}\n";

// 
// *Classless
// 

static std::string ExtractFile(std::string path) // Extracts the contents of a file to a string
{
    std::ifstream file;
    std::string line;
    std::string returnValue;
    file.exceptions(/*std::ifstream::failbit |*/ std::ifstream::badbit); // Set fstream exceptions (failbit removed for unneccesary exceptions)

    try
    {
        file.open(path);

        while(std::getline(file, line)){ // Extract lines
        returnValue += line + '\n';
        }
    }
    catch(std::ifstream::failure e)
    {
        std::cout << e.what() << '\n';
    }
    
    return returnValue; // Return string
}

static void CreateShader(unsigned int &id, GLenum type, std::string code){
    id = glCreateShader(type);
    const char* source = code.c_str();

    glShaderSource(id, 1, &source, NULL);

    glCompileShader(id);

    int success; // Error handle type
    char log[512]; // Error message
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(id, 512, NULL, log);
        DEV_LOG("Failed compile: ", log);
    }

    return;
}

void GetBufferData(/*tinygltf::Accessor&*/ int accessor, tinygltf::Model& model, std::vector<unsigned char>& vector){

    tinygltf::Accessor tinygltf_accessor = model.accessors[accessor];
    tinygltf::BufferView view = model.bufferViews[tinygltf_accessor.bufferView];
    int byteOffset = view.byteOffset;
    int byteLength = view.byteLength;
    vector.resize(byteLength);

    tinygltf::Buffer buffer = model.buffers[view.buffer];
    std::memcpy(vector.data(), buffer.data.data() + byteOffset, byteLength);
}

void GetAttributeData(tinygltf::Model& model, tinygltf::Primitive& primitive, std::string target, glWrap::MeshData& mesh){

    mesh.attributes.push_back(glWrap::AttributeData{});

    mesh.attributes.back().size = model.accessors.at(primitive.attributes.at(target)).type;
    DEV_LOG("Type: ", model.accessors.at(primitive.attributes.at(target)).type);

    mesh.attributes.back().type = model.accessors.at(primitive.attributes.at(target)).componentType;
    DEV_LOG("ComponentType: ", model.accessors.at(primitive.attributes.at(target)).componentType);

    GetBufferData(primitive.attributes.at(target), model, mesh.attributes.back().data);
}

/*
void GetIndexData(tinygltf::Model& model, tinygltf::Primitive& primitive, std::vector<unsigned short>& container){

    tinygltf::BufferView view = model.bufferViews[model.accessors[primitive.indices].bufferView];
    int byteOffset = view.byteOffset;
    int byteLength = view.byteLength;

    tinygltf::Buffer buffer = model.buffers[view.buffer];

    std::vector<unsigned char> data;
    data.resize(buffer.data.size());
    data = buffer.data;

    container.resize(byteLength / sizeof(unsigned short));

    std::memcpy(container.data(), data.data() + byteOffset, byteLength);

    return;
}
*/

static GLenum GetChannelType(unsigned int channels){
    switch(channels)
    {
        case 1:
        return GL_RED;

        case 2:
        return GL_RG;

        case 3:
        return GL_RGB;

        case 4:
        return GL_RGBA;
    }

    return GL_RGB;
}

// 
// *TEXTURE
// 

glWrap::Texture2D::Texture2D(std::string image, bool flip, GLenum filter, GLenum desiredChannels){
    stbi_set_flip_vertically_on_load(flip);
    int width, height, channels;
    unsigned char *data = stbi_load(image.c_str(), &width, &height, &channels, 0);

    // std::cout << channels << " channels\n";

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, desiredChannels, width, height, 0, GetChannelType(channels), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else std::cout << "Texture not loaded correctly\n";

    stbi_image_free(data);
}

void glWrap::Texture2D::SetActive(unsigned int unit){
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

// 
// *SHADER
// 

glWrap::Shader::Shader(std::string vertexPath, std::string fragmentPath){
    
    unsigned int vertex, fragment; // Shader objects

    CreateShader(vertex, GL_VERTEX_SHADER, ExtractFile(vertexPath));
    CreateShader(fragment, GL_FRAGMENT_SHADER, ExtractFile(fragmentPath));


    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);

    glLinkProgram(m_ID);

    int success; // Error handle type
    char log[512]; // Error message
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, log);
        DEV_LOG("Failed linking: ", log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return;
}

glWrap::Shader::Shader(const char* vertexShader, const char* fragmentShader, bool isText){
    unsigned int vertex, fragment; // Shader objects

    CreateShader(vertex, GL_VERTEX_SHADER, vertexShader);
    CreateShader(fragment, GL_FRAGMENT_SHADER, fragmentShader);

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);

    glLinkProgram(m_ID);

    int success; // Error handle type
    char log[512]; // Error message
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, log);
        DEV_LOG("Failed linking: ", log);
        return;
    }

    return;
}

void glWrap::Shader::Use(){
    glUseProgram(m_ID);
}

void glWrap::Shader::Update(){
    for (auto const& value : m_bools){
        // if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
            glUniform1i(glGetUniformLocation(m_ID, value.first.c_str()), (int)value.second);
    }

    for (auto const& value : m_ints){
        // if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
        glUniform1i(glGetUniformLocation(m_ID, value.first.c_str()), value.second);
    }

    for (auto const& value : m_floats){
        // if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
        glUniform1f(glGetUniformLocation(m_ID, value.first.c_str()), value.second);
    }

    for (auto const& value : m_mat4s){
        // if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
        glUniformMatrix4fv(glGetUniformLocation(m_ID, value.first.c_str()), 1, GL_FALSE, glm::value_ptr(value.second));
    }

    unsigned int unit = 0;
    for (auto const& value : m_textures){
        // if (glGetUniformLocation(m_ID, value.first.c_str()) != -1){
            value.second->SetActive(unit);
            glUniform1i(glGetUniformLocation(m_ID, value.first.c_str()), unit);
            ++unit;
        // }
    }
}

void glWrap::Shader::SetBool(const std::string name, bool value){ m_bools[name] = value; }
void glWrap::Shader::SetInt(const std::string name, int value){ m_ints[name] = value; }
void glWrap::Shader::SetFloat(const std::string name, float value){ m_floats[name] = value; }
void glWrap::Shader::SetMatrix4(const std::string name, glm::mat4 mat){ m_mat4s[name] = mat; }
void glWrap::Shader::SetTexture(const std::string name, Texture2D* texture){ m_textures[name] = texture; }

// 
// *WorldObject
//

glm::vec3 glWrap::WorldObject::GetForwardVector(){
    return glm::normalize(glm::vec3{
        (cos(glm::radians(m_rot.z)) * cos(glm::radians(m_rot.y))),
        sin(glm::radians(m_rot.y)),
        (sin(glm::radians(m_rot.z)) * cos(glm::radians(m_rot.y)))});
}

glm::vec3 glWrap::WorldObject::GetRightVector(){ return glm::normalize(glm::cross(GetForwardVector(), glm::vec3(0.0f, 1.0f, 0.0f))); }

glm::vec3 glWrap::WorldObject::GetUpwardVector(){ return glm::normalize(glm::cross(GetRightVector(), GetForwardVector())); }

glm::mat4 glWrap::WorldObject::GetTransformMatrix(){

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, m_pos);
    model = glm::scale(model, m_scl);
    model = glm::rotate(model, glm::radians(m_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

void glWrap::WorldObject::AddPosition(glm::vec3 position){ m_pos += position; }
void glWrap::WorldObject::AddRotation(glm::vec3 rotation){ m_rot += rotation; }
void glWrap::WorldObject::AddScale(glm::vec3 scale){ m_scl += scale; }

// 
// *Camera
// 

float glWrap::Camera::GetFOV(){ return m_FOV; }
glm::mat4 glWrap::Camera::GetView(){ return m_target ? glm::lookAt(m_pos, *m_target, GetUpwardVector()) : glm::lookAt(m_pos, m_pos + GetForwardVector(), GetUpwardVector()); }
glm::mat4 glWrap::Camera::GetProjection(glm::vec2 aspect){ return m_perspective ? glm::perspective(glm::radians(m_FOV), (aspect.x / aspect.y), m_clip.x, m_clip.y ) : glm::ortho(0.0f, aspect.x, 0.0f, aspect.y, m_clip.x, m_clip.y); }
bool glWrap::Camera::IsPerspective(){ return m_perspective; }

void glWrap::Camera::SetTarget(glm::vec3* target){ m_target = target; }
void glWrap::Camera::SetFOV(float FOV){ m_FOV = FOV; }
void glWrap::Camera::AddFOV(float FOV){ m_FOV += FOV; }
void glWrap::Camera::SetPerspective(bool isTrue){ m_perspective = isTrue; }

// 
// *Model / Mesh
// 

glWrap::Mesh::Mesh(){
    glGenVertexArrays(1, &m_VAO);
    DEV_LOG("VAO generated: ", m_VAO);
}

void glWrap::Mesh::SetAttributeData(std::vector<unsigned char>& data, unsigned int size, unsigned int type, unsigned int layout, unsigned int divisor, GLenum drawtype){
    glBindVertexArray(m_VAO);

    if (m_buffers.size() <= layout){
        m_buffers.resize(layout + 1);
        glGenBuffers(1, &m_buffers[layout]);
        DEV_LOG("Increased attributes to ", m_buffers.size());
    }
    else if ( m_buffers[layout] == 0 ) {
        glGenBuffers(1, &m_buffers[layout]);
        DEV_LOG("Generating buffer ", layout);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[layout]);

    glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), drawtype);
    glVertexAttribPointer(layout, size, type, GL_FALSE, size * sizeof(float), (void*)0);
    glVertexAttribDivisor(layout, divisor);
    glEnableVertexAttribArray(layout);
    DEV_LOG("Written buffer ", layout);

    glBindVertexArray(0);
}

void glWrap::Mesh::SetAttributeData(std::vector<unsigned char>& data, unsigned int size, unsigned int type, unsigned int layout){
    glBindVertexArray(m_VAO);

    if (m_buffers.size() <= layout){
        m_buffers.resize(layout + 1);
        glGenBuffers(1, &m_buffers[layout]);
        DEV_LOG("Increased attributes to ", m_buffers.size());
    }
    else if ( m_buffers[layout] == 0 ) {
        glGenBuffers(1, &m_buffers[layout]);
        DEV_LOG("Generating buffer ", layout);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[layout]);

    glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(layout, size, type, GL_FALSE, size * sizeof(float), (void*)0);
    glEnableVertexAttribArray(layout);
    DEV_LOG("Written buffer ", layout);

    glBindVertexArray(0);
}

void glWrap::Mesh::SetIndexData(std::vector<unsigned char>& indices){
    DEV_LOG("Binding VAO", "");
    glBindVertexArray(m_VAO);

    if (m_EBO == 0){
        DEV_LOG("EBO not generated", ""); 
        glGenBuffers(1, &m_EBO);
        DEV_LOG("EBO generated", "");
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    DEV_LOG("EBO bound ", m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() /* * sizeof(GL_UNSIGNED_SHORT)*/, indices.data(), GL_STATIC_DRAW);
    DEV_LOG("EBO set ", m_EBO);

    m_indexAmount = indices.size() / sizeof(unsigned short);
}

void glWrap::Mesh::Draw(){

    // DEV_LOG("Binding VAO: ", m_VAO);
    glBindVertexArray(m_VAO);
    // DEV_LOG("Binding EBO: ", m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // DEV_LOG("Drawing elements", "");
    glDrawElements(GL_TRIANGLES, m_indexAmount, GL_UNSIGNED_SHORT, 0);
}

void glWrap::Mesh::Draw(unsigned int count){

    // DEV_LOG("Binding VAO: ", m_VAO);
    glBindVertexArray(m_VAO);
    // DEV_LOG("Binding EBO: ", m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // DEV_LOG("Drawing elements", "");
    glDrawElementsInstanced(GL_TRIANGLES, m_indexAmount, GL_UNSIGNED_SHORT, 0, count);
}

void glWrap::Model::SetModelData(ModelData& model){

    DEV_LOG("Meshes: ", model.meshes.size());

    for (int i{}; i < model.meshes.size(); ++i){
        DEV_LOG("Mesh: ", i);
        MeshData& meshData = model.meshes[i];
        DEV_LOG("Mesh data indexes: ", meshData.indices.size());
        m_meshes.push_back(Mesh{});
        DEV_LOG("Mesh created", "");
        Mesh& mesh = m_meshes.back();
        DEV_LOG("Mesh selected", "");
        mesh.SetIndexData(meshData.indices);
        DEV_LOG("Mesh index set", "");

        DEV_LOG("Attributes: ", meshData.attributes.size());
        for (int j{}; j < meshData.attributes.size(); ++j){
            DEV_LOG("Attribute: ", j);
            AttributeData& attributeData = meshData.attributes[j];
            mesh.SetAttributeData(attributeData.data, attributeData.size, attributeData.type, j);
        }
    }
}

void glWrap::Model::SetModelAttribute(std::vector<unsigned char>& data, unsigned int size, unsigned int type, unsigned int layout, unsigned int divisor, GLenum drawtype){
    for (auto& mesh : m_meshes) mesh.SetAttributeData(data, size, type, layout, divisor, drawtype);
}

void glWrap::Model::Draw(){
    for (auto& mesh : m_meshes){
        mesh.Draw();
    }
}

void glWrap::Model::Draw(unsigned int count){
    for (auto& mesh : m_meshes){
        mesh.Draw(count);
    }
}

glWrap::Mesh::~Mesh(){
    for (auto& buffer : m_buffers){
        if (buffer != 0){
            glDeleteBuffers(1, &buffer);
        }
    }

    m_buffers.clear();

    if (m_EBO != 0) glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VAO);
}

// 
// *Window
// 

std::vector<unsigned int> glWrap::Window::m_pressedKeys;
std::vector<unsigned int> glWrap::Window::m_releasedKeys;
std::vector<unsigned int> glWrap::Window::m_repeatKeys;
glm::dvec2 glWrap::Window::m_lastMousePos;
glm::dvec2 glWrap::Window::m_deltaMousePos;
glm::ivec2 glWrap::Window::m_size;

glWrap::Window::Window(std::string name, glm::ivec2 size) : m_name{name}{

    if(!glfwInit()){
        DEV_LOG("Failed to initialize OPENGL", "");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(size.x, size.y, name.c_str(), NULL, NULL);

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        DEV_LOG("Failed to initialize GLAD for window ", name);
        glfwTerminate();
    }

    glEnable(GL_DEPTH_TEST);

    m_size = size;

    glfwSetKeyCallback(m_window, keyCall);
    glfwSetFramebufferSizeCallback(m_window, frameCall);
    // glfwSetCursorPosCallback(m_window, mousePosCall);
    glfwGetCursorPos(m_window, &m_lastMousePos.x, &m_lastMousePos.y);
}

/*
glWrap::Window::Window(std::string name, glm::ivec2 size, GLFWwindow* context) : m_name{name}{

    m_window = glfwCreateWindow(size.x, size.y, name.c_str(), NULL, context);

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        DEV_LOG("Failed to initialize GLAD for window ", name);
        glfwTerminate();
    }

    glEnable(GL_DEPTH_TEST);

    m_defaultShader = std::make_unique<Shader>(defaultVertexShader, defaultFragmentShader, true);
    m_lastMousePos = { (size.x / 2), (size.y / 2) };
    m_size = size;

    glfwSetKeyCallback(m_window, keyCall);
    glfwSetFramebufferSizeCallback(m_window, frameCall);
    // glfwSetCursorPosCallback(m_window, mousePosCall);
    glfwGetCursorPos(m_window, &m_lastMousePos.x, &m_lastMousePos.y);
}
*/

void glWrap::Window::Swap(){

    glfwSwapBuffers(m_window);
    glClearColor(m_color.r, m_color.b, m_color.g, m_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pressedKeys.clear();
    m_releasedKeys.clear();
    m_repeatKeys.clear();
    glfwPollEvents();

    glm::dvec2 pos;
    glfwGetCursorPos(m_window, &pos.x, &pos.y);
    m_deltaMousePos = { (pos.x - m_lastMousePos.x), (m_lastMousePos.y - pos.y) };
    m_lastMousePos = pos;

    m_deltaTime = glfwGetTime() - m_lastFrameTime;
    m_lastFrameTime = glfwGetTime();

    if (!m_firstFrame) return;
    
    m_deltaTime = 0.0f;
    m_lastFrameTime = glfwGetTime();
    m_firstFrame = false;
}

bool glWrap::Window::IsKeyHeld(unsigned int key) { return glfwGetKey(m_window, key) == GLFW_PRESS; }
bool glWrap::Window::IsRequestedClose() { return glfwWindowShouldClose(m_window); }

float glWrap::Window::GetDeltaTime(){ return m_deltaTime; }

void glWrap::Window::keyCall(GLFWwindow* window, int key, int scancode, int action, int mods){
    switch (action){
        case GLFW_PRESS:
        m_pressedKeys.push_back(key);
        break;
        case GLFW_RELEASE:
        m_releasedKeys.push_back(key);
        break;
        case GLFW_REPEAT:
        m_repeatKeys.push_back(key);
        break;
    }
}

/*
void glWrap::Window::mousePosCall(GLFWwindow* window, double xpos, double ypos){
    m_deltaMousePos = { (xpos - m_lastMousePos.x), (m_lastMousePos.y - ypos) };
    m_lastMousePos = {xpos, ypos};
}
*/

void glWrap::Window::frameCall(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    m_size = {width, height};
}

void glWrap::Window::LoadGLTF(std::map<std::string, ModelData>& modelContainer, std::map<std::string, Skin>& skinContainer, std::string file){

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string error{};
    std::string warning{};

    if (!loader.LoadASCIIFromFile(&model, &error, &warning, file)){
        DEV_LOG("ASCII Error", error);
        DEV_LOG("ASCII Warning", warning);
    }

    for(int i{}; i < model.meshes.size(); ++i){

        tinygltf::Mesh& current_gltfMesh = model.meshes[i];
        DEV_LOG("Model: ", current_gltfMesh.name);

        int postfix{0};
        while (modelContainer.count(model.meshes[i].name + "." + std::to_string(postfix))) ++postfix; 

        modelContainer.emplace(model.meshes[i].name + "." + std::to_string(postfix), ModelData{});
        ModelData& current_model = modelContainer[model.meshes[i].name + "." + std::to_string(postfix)];

        for (int j{}; j < current_gltfMesh.primitives.size(); ++j){

            tinygltf::Primitive& current_gltfPrimitive = current_gltfMesh.primitives[j];
            current_model.meshes.push_back(MeshData());
            MeshData& current_mesh = current_model.meshes.back();

            GetBufferData(current_gltfPrimitive.indices, model, current_mesh.indices);

            GetAttributeData(model, current_gltfPrimitive, "POSITION", current_mesh);

            // if (current_gltfPrimitive.attributes.count("NORMAL")){
            GetAttributeData(model, current_gltfPrimitive, "NORMAL", current_mesh);
            /*
            }
            else {
                current_mesh.attributes.push_back(AttributeData{});
                current_mesh.attributes.back().data.resize( vertices * 3 );
                current_mesh.attributes.back().size = 3;
                current_mesh.attributes.back().type = 5126;
            }
            */

            // if (current_gltfPrimitive.attributes.count("TEXCOORD_0")){
            GetAttributeData(model, current_gltfPrimitive, "TEXCOORD_0", current_mesh);
            /*
            }
            else {
                current_mesh.attributes.push_back(AttributeData{});
                current_mesh.attributes.back().data.resize( vertices * 2 );
                current_mesh.attributes.back().size = 2;
                current_mesh.attributes.back().type = 5126;
            }
            */

            if (current_gltfPrimitive.attributes.count("COLOR_0")){
                GetAttributeData(model, current_gltfPrimitive, "COLOR_0", current_mesh);
            }
            else {
                current_mesh.attributes.push_back(AttributeData{});
                current_mesh.attributes.back().data.resize(current_mesh.attributes[0].data.size());
                current_mesh.attributes.back().size = 3;
                current_mesh.attributes.back().size = 5126;
                DEV_LOG("Bytes: ", current_mesh.attributes[0].data.size());
            }

            if (current_gltfPrimitive.attributes.count("JOINTS_0")){
                GetAttributeData(model, current_gltfPrimitive, "JOINTS_0", current_mesh);

                // if (current_gltfPrimitive.attributes.count("WEIGHTS_0")){
                GetAttributeData(model, current_gltfPrimitive, "WEIGHTS_0", current_mesh);
                // }
            }

        }
    }

    for (int i{}; i < model.skins.size(); ++i){

        tinygltf::Skin& current_gltfSkin = model.skins[i];
        DEV_LOG("Skin: ", current_gltfSkin.name);

        skinContainer[current_gltfSkin.name] = Skin{};
        Skin& current_skin = skinContainer[current_gltfSkin.name];

        std::vector<unsigned char> tempMatrixData;
        GetBufferData(current_gltfSkin.inverseBindMatrices, model, tempMatrixData);

        std::vector<glm::mat4> matrixData;
        matrixData.resize(tempMatrixData.size() / sizeof(glm::mat4));
        std::memcpy(matrixData.data(), tempMatrixData.data(), tempMatrixData.size());

        std::vector<int> skinJoints;

        for (auto& tinygltf_joint : current_gltfSkin.joints){

            skinJoints.push_back(tinygltf_joint);

            current_skin.m_joints.push_back(Joint{});
            Joint& current_joint = current_skin.m_joints.back();
            tinygltf::Node& current_gltfjoint = model.nodes.at(tinygltf_joint);

            current_joint.m_name = current_gltfjoint.name;
            current_joint.m_offset = matrixData[tinygltf_joint];

            for (auto& child : current_gltfjoint.children){
                current_joint.m_children.push_back(child);
            }
        }

        for (auto& animation : model.animations){
            if (std::find(skinJoints.begin(), skinJoints.end(), animation.channels[0].target_node) != skinJoints.end()){
                DEV_LOG("Skin has animation: ", animation.name);
                // for (auto& channels)

                current_skin.m_animations[animation.name] = Animation{};
                Animation& current_animation = current_skin.m_animations[animation.name];

                for (auto& sampler : animation.samplers){

                    std::vector<unsigned char> time;
                    
                    GetBufferData(sampler.input, model, time);

                    std::vector<float> timeFixed;
                    timeFixed.resize(time.size() / sizeof(float));
                    std::memcpy(timeFixed.data(), time.data(), time.size());

                    for (float& newTime : timeFixed){
                        DEV_LOG("", newTime);
                    }
                }
            }
        }
    }
    return;
}

bool glWrap::Window::IsKeyPressed(unsigned int key){ return std::count(m_pressedKeys.begin(), m_pressedKeys.end(), key); }
bool glWrap::Window::IsKeyReleased(unsigned int key){ return std::count(m_releasedKeys.begin(), m_releasedKeys.end(), key); }
bool glWrap::Window::IsKeyRepeat(unsigned int key){ return std::count(m_repeatKeys.begin(), m_repeatKeys.end(), key); }

glm::dvec2 glWrap::Window::GetMousePos(){
    glm::dvec2 pos;
    glfwGetCursorPos(m_window, &pos.x, &pos.y);
    return pos;
}

glm::dvec2 glWrap::Window::GetDeltaMousePos(){ return m_deltaMousePos; }

glm::ivec2 glWrap::Window::GetSize(){ return m_size; }

void glWrap::Window::SetRequestedClose(bool should){ glfwSetWindowShouldClose(m_window, should ? GLFW_TRUE : GLFW_FALSE); }
void glWrap::Window::SetInputMode(unsigned int mode, unsigned int value){ glfwSetInputMode(m_window, mode, value); }

glWrap::Window::~Window(){
    glfwTerminate();
}

GLFWwindow* glWrap::Window::GetContext(){ return m_window; }