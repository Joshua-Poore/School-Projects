

#include <iostream>

#include <glad/gl.h>

#include <GLM/glm.hpp>
#include <GLFW/glfw3.h>

#include "learnopengl/shader_4722.h"

#include "cs4722/x11.h"

static const auto  number_of_vertices = 3;

static const auto background_color = cs4722::x11::gray70;

static auto const verts_per_triangle = 3;

static Shader *shader;

glm::vec4 positions0[] = {
        glm::vec4(-.5, .8, 0, 1),
        glm::vec4(-.8, .2,0, 1),
        glm::vec4(-.2, .2, 0, 1)
};
auto size0 = 4 * 4 * verts_per_triangle;
auto offset0 = 0;
auto bOffset0 = 0;
auto color0 = cs4722::x11::medium_violet_red;

glm::vec4 positions1[] = {
        glm::vec4(.5, .8, 0, 1),
        glm::vec4(.2, .2,0, 1),
        glm::vec4(.8, .2, 0, 1)
};
auto size1 = 4 * 4 * verts_per_triangle;
auto offset1 = offset0 + verts_per_triangle;
auto bOffset1 = bOffset0 + size0;
auto color1 = cs4722::x11::slate_blue;


glm::vec4 positions2[] = {
        glm::vec4(.5, -.2, 0, 1),
        glm::vec4(.2, -.8,0, 1),
        glm::vec4(.8, -.8, 0, 1)
};
auto size2 = 4 * 4 * verts_per_triangle;
auto offset2 = offset1 + verts_per_triangle;
auto bOffset2 = bOffset1 + size1;
auto color2 = cs4722::x11::goldenrod1;

glm::vec4 positions3[] = {
        glm::vec4(-.5, -.8, 0, 1),
        glm::vec4(-.2, -.2,0, 1),
        glm::vec4(-.8, -.2, 0, 1)
};
auto size3 = 4 * 4 * verts_per_triangle;
auto offset3 = offset2 + verts_per_triangle;
auto bOffset3 = bOffset2 + size2;
auto color3 = cs4722::x11::brown;

glm::vec4 positions4[] = {
        glm::vec4(-.5, .2, 0, 1),
        glm::vec4(-.8, .8,0, 1),
        glm::vec4(-.2, .8, 0, 1)
};
auto size4 = 4 * 4 * verts_per_triangle;
auto offset4 = offset3 + verts_per_triangle;
auto bOffset4 = bOffset3 + size3;
auto color4 = cs4722::x11::slate_gray;


const auto number_of_shapes = 5;



void init(void)
{
    shader = new Shader("vertex_shader.glsl","fragment_shader.glsl");
    shader->use();


    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    const auto number_of_buffers = 1;
    GLuint  buffers[number_of_buffers];
    glGenBuffers(number_of_buffers, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * (number_of_shapes * verts_per_triangle),
                 nullptr,  GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, bOffset0, size0, positions0);
    glBufferSubData(GL_ARRAY_BUFFER, bOffset1, size1, positions1);
    glBufferSubData(GL_ARRAY_BUFFER, bOffset2, size2, positions2);
    glBufferSubData(GL_ARRAY_BUFFER, bOffset3, size3, positions3);
    glBufferSubData(GL_ARRAY_BUFFER, bOffset4, size4, positions4);

    GLuint b_position = glGetAttribLocation(shader->ID, "b_position");
    glVertexAttribPointer(b_position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(b_position);
}


void render(void)
{
    shader->setVec4("u_color", color4.as_float_array());
    glDrawArrays(GL_TRIANGLES, offset4, verts_per_triangle);

    shader->setVec4("u_color", color0.as_float_array());
    glDrawArrays(GL_TRIANGLES, offset0, verts_per_triangle);

    shader->setVec4("u_color", color1.as_float_array());
    glDrawArrays(GL_TRIANGLES, offset1, verts_per_triangle);

    shader->setVec4("u_color", color2.as_float_array());
    glDrawArrays(GL_TRIANGLES, offset2, verts_per_triangle);

    shader->setVec4("u_color", color3.as_float_array());
    glDrawArrays(GL_TRIANGLES, offset3, verts_per_triangle);
}


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int
main(int argc, char** argv)
{

    glfwSetErrorCallback(error_callback);
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    auto* primary = glfwGetPrimaryMonitor();
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
    std::cout << xpos << " " << ypos << " " << width << " " << height << std::endl;
    const auto size = glm::min(width, height);
    const auto ratio = .9f;
    const auto w_width = static_cast<int>(ratio * size);
    const auto w_height = w_width;
    const auto w_x = static_cast<int>(size * (1 - ratio) / 2);
    const auto w_y = w_x;
    auto* window = glfwCreateWindow(w_width, w_height, "Five Triangles", NULL, NULL);
    glfwSetWindowPos(window, w_x, w_y);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    init();

    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, background_color.as_float_array());
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
