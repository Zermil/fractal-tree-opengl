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
#define VERT_CAP (1 << DEPTH)
#define IND_CAP (VERT_CAP - 1)
#define FRAC_PI 3.14159265358f
#define FRAC_PI_5 0.62831853071f
#define STACK_CAP 256

#define internal static

internal const char *vertex_shader =
    "#version 330\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform vec2 resolution;\n"
    "out vec2 oPos;\n"
    "void main() {\n"
    "  vec2 pos = (aPos / resolution) * 2.0 - 1.0;\n"
    "  gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);\n"
    "  oPos = vec2(pos.x, -pos.y);\n"
    "}";

internal const char *fragment_shader =
    "#version 330\n"
    "out vec4 frag_color;\n"
    "in vec2 oPos;\n"
    "void main() {\n"
    "  float r = clamp(oPos.y + 0.20, 0.0, 1.0);\n"
    "  float b = clamp(oPos.y - 0.30, 0.0, 1.0);\n"
    "  frag_color = vec4(r, 0.60, b, 1.0);\n"
    "}";

typedef struct {
    unsigned int data[STACK_CAP];
    unsigned int size;
} Stack;

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    unsigned int a;
    unsigned int b;
} Line;

typedef struct {
    Vec2 *vertices;
    unsigned int vert_size;

    Line *indices;
    unsigned int ind_size;
} Vert_Vec;

internal void stack_push(Stack *stack, unsigned int x)
{
    assert(stack->size < STACK_CAP);
    
    stack->data[stack->size] = x;
    stack->size += 1;
}

internal unsigned int stack_top(Stack *stack)
{
    assert(stack->size > 0);
    
    return(stack->data[stack->size - 1]);
}

internal void stack_pop(Stack *stack)
{
    assert(stack->size > 0);
    
    stack->size -= 1;
}

internal unsigned int append_vertex(Vert_Vec *vec, float x, float y)
{
    assert(vec->vert_size < VERT_CAP);

    unsigned int idx = vec->vert_size;
    
    vec->vertices[vec->vert_size] = {x, y};
    vec->vert_size += 1;

    return(idx);
}

internal void append_line(Vert_Vec *vec, unsigned int a, unsigned int b)
{
    assert(vec->ind_size < IND_CAP);
    
    vec->indices[vec->ind_size] = {a, b};
    vec->ind_size += 1;
}

internal void tree(float x, float y, float len, float angle, unsigned int curr_depth, Stack *stack, Vert_Vec *vec)
{
    if (curr_depth == 0 || len < 1.0f) return;

    float dx = x + (len * cosf(angle));
    float dy = y + (len * sinf(angle));
    float dlen = len * 0.75f;

    if (DEPTH == curr_depth) {
        unsigned int a = append_vertex(vec, x, y);
        unsigned int b = append_vertex(vec, dx, dy);

        append_line(vec, a, b);
        stack_push(stack, b);
    } else {
        unsigned int a = stack_top(stack);
        unsigned int b = append_vertex(vec, dx, dy);
        
        append_line(vec, a, b);
        stack_push(stack, b);
    }
    
    tree(dx, dy, dlen, angle - FRAC_PI_5, curr_depth - 1, stack, vec);
    tree(dx, dy, dlen, angle + FRAC_PI_5, curr_depth - 1, stack, vec);
    
    stack_pop(stack);
}

internal GLFWwindow* create_window(unsigned int width, unsigned int height, const char *title)
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
    Stack stack = {0};
    lines.vertices = (Vec2 *) malloc(VERT_CAP * sizeof(Vec2));
    lines.indices = (Line *) malloc(IND_CAP * sizeof(Line));

    if (lines.vertices == NULL || lines.indices == NULL) {
        fprintf(stderr, "Could not allocate enough memory for verticies, consider lowering the DEPTH!\n");
        exit(1);
    }
    
    tree(640.0f, 720.0f, 150.0f, -FRAC_PI * 0.5f, DEPTH, &stack, &lines);

    glBufferData(GL_ARRAY_BUFFER, VERT_CAP * sizeof(Vec2), lines.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (void*)offsetof(Vert_Vec, vertices));
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, IND_CAP * 2, GL_UNSIGNED_INT, lines.indices);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(lines.vertices);
    free(lines.indices);
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
