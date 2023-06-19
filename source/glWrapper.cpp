#include "glWrapper.hpp"

#define GW_DEBUG

#ifdef GW_DEBUG
    #define DEV_LOG(x, y) std::cout << x << y << '\n'
#else
    #define DEV_LOG(x, y)
#endif

// 
// *SHADER
//

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

void glWrap::Shader::SetBool(const std::string &name, bool value) const{
    if (glGetUniformLocation(m_ID, name.c_str()) != -1)
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void glWrap::Shader::SetInt(const std::string &name, int value) const{
    if (glGetUniformLocation(m_ID, name.c_str()) != -1)
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void glWrap::Shader::SetFloat(const std::string &name, float value) const{
    if (glGetUniformLocation(m_ID, name.c_str()) != -1)
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void glWrap::Shader::SetMatrix4(const std::string &name, glm::mat4 mat) const{
    if (glGetUniformLocation(m_ID, name.c_str()) != -1)
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// 
// *TEXTURE
// 

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
// *WorldObject
// 

glWrap::Transform glWrap::WorldObject::GetTransform() { return m_transform; }
glm::vec3 glWrap::WorldObject::GetPosition(){ return m_transform.pos; }
glm::vec3 glWrap::WorldObject::GetRotation() { return m_transform.rot; }
glm::vec3 glWrap::WorldObject::GetScale(){ return m_transform.scl; }
glm::vec3 glWrap::WorldObject::GetDirection(){
    return glm::vec3{
        (cos(glm::radians(m_transform.rot.z)) * cos(glm::radians(m_transform.rot.y))),
        sin(glm::radians(m_transform.rot.y)),
        (sin(glm::radians(m_transform.rot.z)) * cos(glm::radians(m_transform.rot.y)))};
}

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
// glm::mat4 glWrap::Camera::GetView(){ return glm::lookAt( m_transform.pos, m_transform.pos +  ); }
glm::mat4 glWrap::Camera::GetProjection(){
    if(m_perspective) return glm::perspective(glm::radians(m_FOV), (m_aspect.x / m_aspect.y ), m_clip.x, m_clip.y );
    else return glm::ortho(0.0f, m_aspect.x, 0.0f, m_aspect.y, m_clip.x, m_clip.y);
}

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

glWrap::Window::Window(std::string name, glm::ivec2 size, glm::vec4 color, Camera* camera, GLFWwindow* context) : m_name{name}, m_color{color}{

    m_ActiveCamera = camera;

    m_window = glfwCreateWindow(size.x, size.y, name.c_str(), NULL, context);

    glfwMakeContextCurrent(m_window);

    /*
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        DEV_LOG("Failed to initialize GLAD for window ", name);
        glfwTerminate();
    }
    */

    // glfwSetKeyCallback(m_window, keyCall);
}

void glWrap::Window::Swap(){
    glfwSwapBuffers(m_window);
    if (glfwGetCurrentContext() != m_window) glfwMakeContextCurrent(m_window);
    glClearColor(m_color.r, m_color.b, m_color.g, m_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void glWrap::Window::Draw(Instance& instance){
    if (instance.GetMesh() && instance.GetVisibility() && m_ActiveCamera){

        for (int i{}; i < instance.GetMesh()->m_primitives.size(); ++i){
            
            if (!instance.GetShader(i)) continue;

            if (glfwGetCurrentContext() != m_window) glfwMakeContextCurrent(m_window);

            Shader* currentShader = instance.GetShader(i);

            currentShader->Use();

            glm::mat4 model = glm::mat4(1.0f);

            // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            // model = glm::rotate(model, glm::radians(m_transform.rot.x), glm::vec3(1.0f, 0.0f, 0.0f));

            // model = glm::rotate(model, glm::radians(m_transform.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
            // model = glm::rotate(model, glm::radians(m_transform.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 view = glm::mat4(1.0f);

            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

            // view = glm::translate(view, m_transform.pos);

            glm::mat4 projection = glm::mat4(1.0f);

            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

            currentShader->SetMatrix4("model", model);
            currentShader->SetMatrix4("view", view);
            currentShader->SetMatrix4("projection", projection);

            instance.GetMesh()->m_primitives[i].Draw();
        }
    }
}

glWrap::Window::~Window(){
    glfwDestroyWindow(m_window);
}

/*
void glWrap::Window::keyCall(GLFWwindow* window, int key, int scancode, int action, int mods){
    switch (action){
        case GLFW_PRESS:
            m_heldKeys.push_back(key);
            break;

        case GLFW_RELEASE:
            for (int i{}; i < m_heldKeys.size(); ++i){
                if(m_heldKeys[i] == key){
                    m_heldKeys.erase(m_heldKeys.begin() + i);
                    break;
                }
            }
            break;

        case GLFW_REPEAT:
            break;
    }
}
*/

//
// *Engine
// 

glWrap::Engine::Engine(){
    
    if(!glfwInit()){
        DEV_LOG("Failed to initialize OPENGL", "");
        return;
    }
    
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_context = glfwCreateWindow(10, 10, ".", NULL, NULL);

    glfwMakeContextCurrent(m_context);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        DEV_LOG("Failed to initialize GLAD for default context", "");
        glfwTerminate();
    }

    m_defaultShader = std::make_unique<Shader>(defaultVertexShader, defaultFragmentShader, true);

    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
}

void glWrap::Engine::Update(){
    if (m_firstFrame){
        m_deltaTime = 0.0f;
        m_lastFrameTime = glfwGetTime();
        m_firstFrame = false;
        glfwPollEvents();
        return;
    }

    glfwPollEvents();
    m_deltaTime = glfwGetTime() - m_lastFrameTime;
    m_lastFrameTime = glfwGetTime();
}

float glWrap::Engine::GetDeltaTime(){ return m_deltaTime; }

glWrap::Shader* glWrap::Engine::GetDefaultShader(){ return m_defaultShader.get(); }

GLFWwindow* glWrap::Engine::GetContext(){ return m_context; }

glWrap::Engine::~Engine(){
    glfwTerminate();
}

void glWrap::LoadMesh(std::map<std::string, Mesh>& container, std::string file){

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

    // DEV_LOG("Meshes found: ", model.meshes.size());

    for(int i{}; i < model.meshes.size(); ++i){
        Mesh temp_mesh;

        int postfix{0};
        while (container.count(model.meshes[i].name + "." + std::to_string(postfix))){
            // DEV_LOG("Name exists already: ", (model.meshes[i].name + "." + std::to_string(postfix)));
            ++postfix;
        }

        // DEV_LOG("Created mesh: ", (model.meshes[i].name + "." + std::to_string(postfix)));

        for(int j{}; j < model.meshes[i].primitives.size(); ++j){

            temp_mesh.m_primitives.push_back(Primitive());
            // DEV_LOG("Pushed primitive: ", j);

            Primitive& prim = temp_mesh.m_primitives.back();

            std::vector<float> position = GetAttributeData(model, model.meshes[i].primitives[j], "POSITION");
            std::vector<float> normal = GetAttributeData(model, model.meshes[i].primitives[j], "NORMAL");
            std::vector<float> texCoord = GetAttributeData(model, model.meshes[i].primitives[j], "TEXCOORD_0");

            int floats = position.size() + normal.size() + texCoord.size();

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
                vertices[x].tex.x = texCoord[0 + posLoc];
                vertices[x].tex.y = texCoord[1 + posLoc];
            }

            prim.m_indices = GetIndexData(model, model.meshes[i].primitives[j]);

            // DEV_LOG("Generating GL objects", "");

            CreateGlObjects(prim);

            // DEV_LOG("GL objects generated", "");
            }
        container.insert({(model.meshes[i].name + "." + std::to_string(postfix)), temp_mesh});

        // DEV_LOG("Inserted mesh: ", (model.meshes[i].name + std::to_string(postfix)));
    }
    // DEV_LOG("Meshes created", "");

    return;
}