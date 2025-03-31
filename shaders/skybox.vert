#version 450

// The buffer with data about camera.
layout(binding = 0, std140) uniform Camera {
	mat4 projection;
	mat4 view;
	vec3 position;
} camera;

// The position of the current vertex that is being processed.
layout(location = 0) in vec3 position;

// ----------------------------------------------------------------------------
// Output Variables
// ----------------------------------------------------------------------------
// The position forwarded to fragment shader.
layout(location = 0) out vec3 fs_position;

void main()
{
	fs_position = position;
	mat4 view = mat4(mat3(camera.view));
	//TODO clear camera translation from view matrix;
	gl_Position = (camera.projection * view * vec4(position, 1.0)).xyww;
}
