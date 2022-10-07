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
#define M_PI 3.14159265358f
#define M_PI_4 0.62831853071f

struct Lines_Vector {
    float verticies[VERT_CAP];
    unsigned int size;
};

void vec_push(Lines_Vector *vec, float x, float y, float dx, float dy)
{
    assert(vec->size < VERT_CAP);
    
    vec->verticies[vec->size + 0] = x;
    vec->verticies[vec->size + 1] = y;
    vec->verticies[vec->size + 2] = dx;
    vec->verticies[vec->size + 3] = dy;
    
    vec->size += 4;
}

void tree(float x, float y, float len, float angle, unsigned int curr_depth, Lines_Vector *vec)
{
    if (curr_depth == 0 || len < 1.0f) return;
    
    float dx = x + (len * cosf(angle));
    float dy = y + (len * sinf(angle));
    vec_push(vec, x, y, dx, dy);

    float dlen = len * 0.75f;
    tree(dx, dy, dlen, angle - M_PI_4, curr_depth - 1, vec);
    tree(dx, dy, dlen, angle + M_PI_4, curr_depth - 1, vec);
}

GLFWwindow* create_window(unsigned int width, unsigned int height, const char* title)
{
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

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
    glOrtho(0.0, WIDTH, HEIGHT, 0.0, -1.0, 1.0);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    Lines_Vector lines = {0};
    tree(640.0f, 720.0f, 150.0f, -M_PI * 0.5f, DEPTH, &lines);

    glBufferData(GL_ARRAY_BUFFER, sizeof(lines.verticies), lines.verticies, GL_STATIC_DRAW);
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

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
