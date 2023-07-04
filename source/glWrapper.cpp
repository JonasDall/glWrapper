#include "glWrapper.hpp"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

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

std::vector<float> GetAttributeData(tinygltf::Model& model, tinygltf::Primitive& primitive, std::string target){

    tinygltf::BufferView view = model.bufferViews[model.accessors[primitive.attributes.at(target)].bufferView];
    int byteOffset = view.byteOffset;
    int byteLength = view.byteLength;

    tinygltf::Buffer buffer = model.buffers[view.buffer];

    std::vector<unsigned char> data;
    data.resize(buffer.data.size());
    data = buffer.data;

    std::vector<float> attributeData;
    attributeData.resize(byteLength / sizeof(float));

    std::memcpy(attributeData.data(), data.data() + byteOffset, byteLength);

    return attributeData;
}

std::vector<unsigned short> GetIndexData(tinygltf::Model& model, tinygltf::Primitive& primitive){

    tinygltf::BufferView view = model.bufferViews[model.accessors[primitive.indices].bufferView];
    int byteOffset = view.byteOffset;
    int byteLength = view.byteLength;

    tinygltf::Buffer buffer = model.buffers[view.buffer];

    std::vector<unsigned char> data;
    data.resize(buffer.data.size());
    data = buffer.data;

    std::vector<unsigned short> indices;
    indices.resize(byteLength / sizeof(unsigned short));

    std::memcpy(indices.data(), data.data() + byteOffset, byteLength);

    return indices;
}

void CreateGlObjects(glWrap::Primitive &primitive){

    // DEV_LOG("Starting", "");
    glGenVertexArrays(1, &primitive.m_VAO);
    // DEV_LOG("VAO", primitive.m_VAO);
    glGenBuffers(1, &primitive.m_VBO);
    // DEV_LOG("VBO", primitive.m_VBO);
    glGenBuffers(1, &primitive.m_EBO);
    // DEV_LOG("EBO", primitive.m_EBO);
    // DEV_LOG("Cont", "");

    glBindVertexArray(primitive.m_VAO);
    // DEV_LOG("Binding VAO", "");

    glBindBuffer(GL_ARRAY_BUFFER, primitive.m_VBO);
    glBufferData(GL_ARRAY_BUFFER, primitive.m_vertices.size() * sizeof(glWrap::Vertex), primitive.m_vertices.data(), GL_STATIC_DRAW);
    // DEV_LOG("Vertex size", primitive.m_vertices.size());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.m_indices.size() * sizeof(GL_UNSIGNED_SHORT), primitive.m_indices.data(), GL_STATIC_DRAW);
    // DEV_LOG("Index size", primitive.m_indices.size());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // DEV_LOG("Attrib arrays generated", "");

    glBindVertexArray(0);

    // DEV_LOG("DONE", "");
}

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

glWrap::Texture2D::Texture2D(std::string image, bool flip, GLenum filter, GLenum desiredChannels, std::string name) : m_name{name} {
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
    if(!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, log);
        DEV_LOG("Failed linking: ", log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return;
}

void glWrap::Shader::Use(){
    glUseProgram(m_ID);
}

void glWrap::Shader::Update(){
    for (auto const& value : m_bools){
        if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
            glUniform1i(glGetUniformLocation(m_ID, value.first.c_str()), (int)value.second);
    }

    for (auto const& value : m_ints){
        if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
        glUniform1i(glGetUniformLocation(m_ID, value.first.c_str()), value.second);
    }

    for (auto const& value : m_floats){
        if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
        glUniform1f(glGetUniformLocation(m_ID, value.first.c_str()), value.second);
    }

    for (auto const& value : m_mat4s){
        if (glGetUniformLocation(m_ID, value.first.c_str()) != -1)
        glUniformMatrix4fv(glGetUniformLocation(m_ID, value.first.c_str()), 1, GL_FALSE, glm::value_ptr(value.second));
    }

    for (int i{}; i < m_textures.size(); ++i){
        if (m_textures[i]){
            m_textures[i]->SetActive(i);

            if (glGetUniformLocation(m_ID, m_textures[i]->m_name.c_str()) != -1)
            glUniform1i(glGetUniformLocation(m_ID, m_textures[i]->m_name.c_str()), i);
        }
    }
}

void glWrap::Shader::SetBool(const std::string name, bool value){ m_bools[name] = value; }
void glWrap::Shader::SetInt(const std::string name, int value){ m_ints[name] = value; }
void glWrap::Shader::SetFloat(const std::string name, float value){ m_floats[name] = value; }
void glWrap::Shader::SetMatrix4(const std::string name, glm::mat4 mat){ m_mat4s[name] = mat; }

void glWrap::Shader::AddTexture(Texture2D* texture){
    m_textures.push_back(texture);
}

// 
// *WorldObject
//

glm::vec3 glWrap::WorldObject::GetForwardVector(){
    return glm::normalize(glm::vec3{
        (cos(glm::radians(m_transform.rot.z)) * cos(glm::radians(m_transform.rot.y))),
        sin(glm::radians(m_transform.rot.y)),
        (sin(glm::radians(m_transform.rot.z)) * cos(glm::radians(m_transform.rot.y)))});
}

glm::vec3 glWrap::WorldObject::GetRightVector(){ return glm::normalize(glm::cross(GetForwardVector(), glm::vec3(0.0f, 1.0f, 0.0f))); }

glm::vec3 glWrap::WorldObject::GetUpwardVector(){ return glm::normalize(glm::cross(GetRightVector(), GetForwardVector())); }

void glWrap::WorldObject::SetTransform(Transform transform){ m_transform = transform; }
void glWrap::WorldObject::SetPosition(glm::vec3 position){ m_transform.pos = position; }
void glWrap::WorldObject::SetRotation(glm::vec3 rotation){ m_transform.rot = rotation; }
void glWrap::WorldObject::SetScale(glm::vec3 scale){ m_transform.scl = scale; }

void glWrap::WorldObject::AddPosition(glm::vec3 position){ m_transform.pos += position; }
void glWrap::WorldObject::AddRotation(glm::vec3 rotation){ m_transform.rot += rotation; }
void glWrap::WorldObject::AddScale(glm::vec3 scale){ m_transform.scl += scale; }

// 
// *Camera
// 

float glWrap::Camera::GetFOV(){ return m_FOV; }
glm::mat4 glWrap::Camera::GetView(){ return m_target ? glm::lookAt(m_transform.pos, *m_target, GetUpwardVector()) : glm::lookAt(m_transform.pos, m_transform.pos + GetForwardVector(), GetUpwardVector()); }
glm::mat4 glWrap::Camera::GetProjection(glm::vec2 aspect){ return m_perspective ? glm::perspective(glm::radians(m_FOV), (aspect.x / aspect.y), m_clip.x, m_clip.y ) : glm::ortho(0.0f, aspect.x, 0.0f, aspect.y, m_clip.x, m_clip.y); }
bool glWrap::Camera::IsPerspective(){ return m_perspective; }

void glWrap::Camera::SetTarget(glm::vec3* target){ m_target = target; }
void glWrap::Camera::SetFOV(float FOV){ m_FOV = FOV; }
void glWrap::Camera::AddFOV(float FOV){ m_FOV += FOV; }
void glWrap::Camera::SetPerspective(bool isTrue){ m_perspective = isTrue; }

// 
// *Mesh / Primitive
// 

void glWrap::Primitive::Draw(){

    // DEV_LOG("Binding VAO: ", m_VAO);
    glBindVertexArray(m_VAO);

    // DEV_LOG("Drawing elements", "");
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);

    // DEV_LOG("Elements drawn", "");
}

// 
// *Instance
// 

void glWrap::Instance::SetMesh(Mesh* mesh){
    m_mesh = mesh;
    m_shaders.resize(mesh->m_primitives.size());
    // DEV_LOG("Shaders required: ", m_shaders.size());
}

void glWrap::Instance::SetShader(Shader* shader, int primitive){
    if (primitive < m_shaders.size()){
        m_shaders[primitive] = shader;
    }
    else {
        DEV_LOG("Wrong primitive index, max is: ", m_shaders.size() - 1);
    }
}

void glWrap::Instance::SetVisibility(bool visibility){ m_visible = visibility; }

glWrap::Mesh* glWrap::Instance::GetMesh(){ return m_mesh; }

glWrap::Shader* glWrap::Instance::GetShader(int primitive){
    if (primitive < m_shaders.size()){
        return m_shaders[primitive];
    }
    else {
        // DEV_LOG("Wrong primitive index, max is: ", m_shaders.size() - 1);
        return nullptr;
    }
}

bool glWrap::Instance::GetVisibility(){ return m_visible; }

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

    m_window = glfwCreateWindow(size.x, size.y, name.c_str(), NULL, NULL);

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

void glWrap::Window::Draw(Instance& instance){
    if (instance.GetMesh() && instance.GetVisibility() && m_ActiveCamera){

        for (int i{}; i < instance.GetMesh()->m_primitives.size(); ++i){

            if (m_currentShader != (instance.GetShader(i) ? instance.GetShader(i) : m_defaultShader.get())){
                m_currentShader = instance.GetShader(i) ? instance.GetShader(i) : m_defaultShader.get();
                m_currentShader->Use();
            }

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::rotate(model, glm::radians(instance.m_transform.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 view = glm::mat4(1.0f);

            view = m_ActiveCamera->GetView();

            glm::mat4 projection = glm::mat4(1.0f);

            projection = m_ActiveCamera->GetProjection((glm::vec2)m_size);

            m_currentShader->SetMatrix4("model", model);
            m_currentShader->SetMatrix4("view", view);
            m_currentShader->SetMatrix4("projection", projection);

            m_currentShader->Update();

            instance.GetMesh()->m_primitives[i].Draw();
        }
    }
}

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

void glWrap::Window::frameCall(GLFWwindow* win, int width, int height){
    glViewport(0, 0, width, height);
    m_size = {width, height};
}

void glWrap::Window::LoadMesh(std::map<std::string, Mesh>& container, std::string file){

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string error{};
    std::string warning{};

    if (!loader.LoadASCIIFromFile(&model, &error, &warning, file)){
        DEV_LOG("ASCII Error", error);
        DEV_LOG("ASCII Warning", warning);
    }

    /*
    else if (!Engine.LoadBinaryFromFile(&model, &error, &warning, path)){
        DEV_LOG("BINARY Error", error);
        DEV_LOG("BINARY Warning", warning);

        return;
    }
    */

    for(int i{}; i < model.meshes.size(); ++i){
        Mesh temp_mesh;

        int postfix{0};
        while (container.count(model.meshes[i].name + "." + std::to_string(postfix))){
            ++postfix;
        }

        for(int j{}; j < model.meshes[i].primitives.size(); ++j){
            temp_mesh.m_primitives.push_back(Primitive());

            Primitive& prim = temp_mesh.m_primitives.back();

            std::vector<float> position = GetAttributeData(model, model.meshes[i].primitives[j], "POSITION");
            std::vector<float> normal = GetAttributeData(model, model.meshes[i].primitives[j], "NORMAL");
            std::vector<float> texCoord = GetAttributeData(model, model.meshes[i].primitives[j], "TEXCOORD_0");

            int floats = position.size() + normal.size() + texCoord.size();
            int testfloats = position.size() + normal.size() + texCoord.size() - 3;

            std::vector<Vertex>& vertices = prim.m_vertices;

            vertices.resize(floats / 8);

            for (int x{}; x < vertices.size(); ++x){

                int posLoc = x * 3;
                int texLoc = x * 2;

                vertices[x].pos.x = position[0 + posLoc];
                vertices[x].pos.y = position[1 + posLoc];
                vertices[x].pos.z = position[2 + posLoc];
                vertices[x].nor.x = normal[0 + posLoc];
                vertices[x].nor.y = normal[1 + posLoc];
                vertices[x].nor.z = normal[2 + posLoc];
                vertices[x].tex.x = texCoord[0 + texLoc];
                vertices[x].tex.y = texCoord[1 + texLoc];
            }

            prim.m_indices = GetIndexData(model, model.meshes[i].primitives[j]);

            CreateGlObjects(prim);

            }
        container.insert({(model.meshes[i].name + "." + std::to_string(postfix)), temp_mesh});
    }
    return;
}

bool glWrap::Window::IsKeyPressed(unsigned int key){ return std::count(m_pressedKeys.begin(), m_pressedKeys.end(), key); }
bool glWrap::Window::IsKeyReleased(unsigned int key){ return std::count(m_releasedKeys.begin(), m_releasedKeys.end(), key); }
bool glWrap::Window::IsKeyRepeat(unsigned int key){ return std::count(m_repeatKeys.begin(), m_repeatKeys.end(), key); }

bool glWrap::Window::IsKeyHeld(unsigned int key){ return glfwGetKey(m_window, key) == GLFW_PRESS; }
bool glWrap::Window::IsRequestedClose(){ return glfwWindowShouldClose(m_window); }

glm::dvec2 glWrap::Window::GetMousePos(){
    glm::dvec2 pos;
    glfwGetCursorPos(m_window, &pos.x, &pos.y);
    return pos;
}

glm::dvec2 glWrap::Window::GetDeltaMousePos(){ return m_deltaMousePos; }

void glWrap::Window::setRequestedClose(bool should){ glfwSetWindowShouldClose(m_window, should ? GLFW_TRUE : GLFW_FALSE); }
void glWrap::Window::setInputMode(unsigned int mode, unsigned int value){ glfwSetInputMode(m_window, mode, value); }

glWrap::Window::~Window(){
    glfwTerminate();
}