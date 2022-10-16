#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>

#define UNUSED(x) ((void)(x))
#define WIDTH 1280
#define HEIGHT 720
#define DEPTH 10
#define VERT_CAP (((1 << DEPTH) - 1) * 4)
#define FRAC_PI 3.14159265358f
#define FRAC_PI_5 0.62831853071f

static const char *vertex_shader =
    "#version 330\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform vec2 resolution;\n"
    "out vec2 oPos;\n"
    "void main() {\n"
    "  vec2 pos = (aPos / resolution) * 2.0 - 1.0;\n"
    "  gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);\n"
    "  oPos = vec2(pos.x, -pos.y);\n"
    "}";

static const char *fragment_shader =
    "#version 330\n"
    "out vec4 frag_color;\n"
    "in vec2 oPos;\n"
    "void main() {\n"
    "  float r = clamp(oPos.y + 0.20, 0.0, 1.0);\n"
    "  float b = clamp(oPos.y - 0.30, 0.0, 1.0);\n"
    "  frag_color = vec4(r, 0.60, b, 1.0);\n"
    "}";

typedef struct {
    float *verticies;
    unsigned int size;
} Vert_Vec;

void vec_push(Vert_Vec *vec, float x, float y, float dx, float dy)
{
    assert(vec->size + 3 < VERT_CAP);
    
    vec->verticies[vec->size + 0] = x;
    vec->verticies[vec->size + 1] = y;
    vec->verticies[vec->size + 2] = dx;
    vec->verticies[vec->size + 3] = dy;
    
    vec->size += 4;
}

void tree(float x, float y, float len, float angle, unsigned int curr_depth, Vert_Vec *vec)
{
    if (curr_depth == 0 || len < 1.0f) return;
    
    float dx = x + (len * cosf(angle));
    float dy = y + (len * sinf(angle));
    vec_push(vec, x, y, dx, dy);

    float dlen = len * 0.75f;
    tree(dx, dy, dlen, angle - FRAC_PI_5, curr_depth - 1, vec);
    tree(dx, dy, dlen, angle + FRAC_PI_5, curr_depth - 1, vec);
}

GLFWwindow* create_window(unsigned int width, unsigned int height, const char* title)
{
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    
    if (window == NULL) {
        fprintf(stderr, "[ERROR]: Could not create window!\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    unsigned int err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "[ERROR]: Could not initialize glew: %s\n", glewGetErrorString(err));
        glfwTerminate();
        exit(1);        
    }

    return(window);
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    GLFWwindow *window = create_window(WIDTH, HEIGHT, "Hello, Sailor!");

    // Shader
    unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &vertex_shader, NULL);
    glCompileShader(vert);

    glShaderSource(frag, 1, &fragment_shader, NULL);
    glCompileShader(frag);

    unsigned int shader_program = glCreateProgram();

    glAttachShader(shader_program, vert);
    glAttachShader(shader_program, frag);
    glLinkProgram(shader_program);
    
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    glUseProgram(shader_program);
    glUniform2f(glGetUniformLocation(shader_program, "resolution"), WIDTH, HEIGHT);
    
    // Render
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    Vert_Vec lines = {0};
    lines.verticies = (float *) malloc(VERT_CAP * sizeof(float));

    if (lines.verticies == NULL) {
        fprintf(stderr, "Could not allocate enough memory for verticies, consider lowering the DEPTH!\n");
        exit(1);
    }
    
    tree(640.0f, 720.0f, 150.0f, -FRAC_PI * 0.5f, DEPTH, &lines);

    glBufferData(GL_ARRAY_BUFFER, VERT_CAP * sizeof(float), lines.verticies, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, VERT_CAP);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(lines.verticies);
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
