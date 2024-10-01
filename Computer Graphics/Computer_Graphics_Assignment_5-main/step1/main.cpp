

#include <ctime>
#include "GLM/gtc/type_ptr.hpp"
#include <glad/gl.h>

#include "learnopengl/shader_4722.h"
#include "cs4722/artifact.h"
#include "cs4722/view.h"
#include "cs4722/shape.h"
#include "cs4722/buffer_utilities.h"
#include "cs4722/window.h"

//#define LOG_FLUP 1

static cs4722::view *the_view;
static Shader *shader;
static std::vector<cs4722::artifact*> artf_list;

static cs4722::height_field_fnl *terrain_height_field;
static glm::vec3 terrain_scale = glm::vec3(100, 10, 100);
static glm::vec3 terrain_translate = glm::vec3(0,0,0);
static bool movement_restricted = true;
static double max_height = .9 * terrain_scale.y + terrain_translate.y;

void init()
{
    the_view = new cs4722::view();

    shader = new Shader("vertex_shader.glsl","fragment_shader.glsl");
    shader->use();

    glEnable(GL_DEPTH_TEST);

    terrain_height_field = new cs4722::height_field_fnl(200, 200);
    terrain_height_field->frequency = 1.5;
    terrain_height_field->seed = 12345;
//    terrain_height_field->seed = clock();
    terrain_height_field->fractal_type = FastNoiseLite::FractalType_FBm;
    terrain_height_field->noise_type = FastNoiseLite::NoiseType_OpenSimplex2;
    terrain_height_field->number_of_octaves = 4;
//    terrain_height_field->ping_pong_strength = 7;
    terrain_height_field->generate();
    cs4722::shape* b = new cs4722::terrain(terrain_height_field);

    auto* artf = new cs4722::artifact();
    artf->the_shape = b;

    artf->world_transform.scale = terrain_scale;
    artf->world_transform.translate = terrain_translate;
    artf_list.push_back(artf);

    terrain_height_field->scale = terrain_scale;
    terrain_height_field->translate = terrain_translate;
    auto lp = terrain_height_field->low_point();
    the_view->camera_position = terrain_height_field->low_point() + glm::vec4(0,1,0,0);

    cs4722::init_buffers(shader->ID, artf_list, "b_position", "b_color");
}



void render()
{
    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect, the_view->perspective_near);
    auto vp_transform = projection_transform * view_transform;


    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;
    last_time = time;


    for (auto artf : artf_list) {

        artf->animate(time, delta_time);

        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();
        auto transform = vp_transform * model_transform;
        shader->setMat4("u_transform", transform);

        
        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start, artf->the_shape->buffer_size);
       

    }
}


static float lr_scale = .1f;
static float fb_scale = .1f;
static float ud_scale = .1f;

static float lr_pan_scale = .01f;
static float ud_pan_scale = lr_pan_scale;




void general_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /*
    * These variables are used to hold the window height and width and position to use when
    *  exiting full screen.
    */
    static int old_width, old_height, old_xpos, old_ypos;
    auto position0 = the_view->camera_position;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch(key) {
            case GLFW_KEY_A:
            case GLFW_KEY_LEFT:
                the_view->camera_position += lr_scale * the_view->camera_left;
                break;
            case GLFW_KEY_D:
            case GLFW_KEY_RIGHT:
                the_view->camera_position -= lr_scale * the_view->camera_left;
                break;
            case GLFW_KEY_W:
                the_view->camera_position += fb_scale * the_view->camera_forward;
                break;
            case GLFW_KEY_S:
                the_view->camera_position -= fb_scale * the_view->camera_forward;
                break;
            case GLFW_KEY_UP:
                the_view->camera_position += ud_scale * the_view->camera_up;
                break;
            case GLFW_KEY_DOWN:
                the_view->camera_position -= ud_scale * the_view->camera_up;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_F11:
                // only a key press, not repeats as that could cause chaotic window/full-screen switching
                if(action == GLFW_PRESS) {
                    if (!glfwGetWindowMonitor(window)) {
                        glfwGetWindowPos(window, &old_xpos, &old_ypos);
                        glfwGetWindowSize(window, &old_width, &old_height);
                        auto monitor = glfwGetPrimaryMonitor();
                        const auto* mode = glfwGetVideoMode(monitor);
                        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                    }
                    else  {
                        glfwSetWindowMonitor(window, nullptr, old_xpos, old_ypos, old_width, old_height, 0);
                    }
                }
                break;
            default: ;
                // nothing to do on other keys
        }
    }

    if(movement_restricted) {
        // code restricting the movement goes in here.
        float y = terrain_height_field->altitude(the_view->camera_position.x, the_view->camera_position.z) + 1;
        the_view->camera_position.y = y;
    }
#ifdef LOG_FLUP
    the_view->display_parameters();
#endif
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
    int widthfb, heightfb;
    glfwGetFramebufferSize(window, &widthfb, &heightfb);
    glViewport(0, 0, widthfb, heightfb);
    view->perspective_aspect =
        (static_cast<float>(widthfb) / static_cast<float>(heightfb));

}



void move_callback(GLFWwindow* window, double xpos, double ypos)
{
    static auto last_x = 0.0f;
    static auto last_y = 0.0f;
    auto* view = static_cast<cs4722::view*>(glfwGetWindowUserPointer(window));
     auto dx = xpos - last_x;
     auto dy = ypos - last_y;
     last_x = xpos;
     last_y = ypos;


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        auto const rot_lr = glm::mat3(glm::rotate(glm::identity<glm::mat4>(),
            static_cast<float>(-dx * lr_pan_scale), glm::vec3(0, 1, 0)));
        view->camera_left = rot_lr * view->camera_left;
        view->camera_forward = rot_lr * view->camera_forward;
        view->camera_up = rot_lr * view->camera_up;

        auto const rot_ud = glm::mat3(glm::rotate(glm::identity<glm::mat4>(),
            static_cast<float>(dy * ud_pan_scale), view->camera_left));
        view->camera_up = rot_ud * view->camera_up;
        view->camera_forward = rot_ud * view->camera_forward;
    }
#ifdef LOG_FLUP
    the_view->display_parameters();
#endif
}


int
main(int argc, char** argv)
{
    glfwInit();

    cs4722::set_opengl_43();
    GLFWwindow *window = cs4722::setup_window_9_16_9("Terrain Transit");

    gladLoadGL(glfwGetProcAddress);

    init();
    the_view->perspective_aspect = cs4722::get_aspect_ratio(window);

    glfwSetWindowUserPointer(window, the_view);
    glfwSetKeyCallback(window, general_key_callback);
    glfwSetCursorPosCallback(window, move_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

	
    while (!glfwWindowShouldClose(window))
    {
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray50.as_float_array());
        glClear(GL_DEPTH_BUFFER_BIT);
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
