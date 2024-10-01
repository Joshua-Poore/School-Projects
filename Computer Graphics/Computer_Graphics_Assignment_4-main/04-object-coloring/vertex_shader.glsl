
/*
 * The change to the vertex shader from the last module is the introduction of the uniform
 * variable transform and using transform to change the position value from the buffer.
 */

#version 430 core

in  vec4 b_position;
in   vec4 b_color;


/*
 * The transform is set from the driver by code in the display function.
 */
uniform  mat4 u_transform;

out vec4 s_color;
out vec4 position;


void
main()
{
    s_color = b_color;
    /*
     * The * operator implements matrix multiplication in GLSL.
     * The output position is the transformed position.
     *
     * Note that this does not change bPosition.
     */
    gl_Position = u_transform * b_position;
    position = gl_Position;
    position = b_position;
}
