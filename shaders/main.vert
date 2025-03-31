#version 450

// ----------------------------------------------------------------------------
// Input Variables
// ----------------------------------------------------------------------------

// The buffer with data about camera.
layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

// The buffer with data about light.
struct Light {
	vec4 position;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
};

layout(binding = 1, std430) buffer Lights {
	Light lights[];
};

// The buffer with data about object.
struct Object {
	mat4 model_matrix;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
};

layout(binding = 2, std430) buffer Objects {
	Object objects[];
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinate;

layout(location = 0) out vec3 fs_position;
layout(location = 1) out vec3 fs_normal;
layout(location = 2) out flat int fs_instance_id;
layout(location = 3) out vec2 fs_texture_coordinate;
layout(location = 4) out vec3 fs_model_position;

void main()
{
	Object object = objects[gl_InstanceID];
	fs_model_position = position;
	fs_position = vec3(object.model_matrix * vec4(position, 1.0));
	fs_normal = transpose(inverse(mat3(object.model_matrix))) * normal;
	fs_texture_coordinate = texture_coordinate;
	fs_instance_id = gl_InstanceID;
    gl_Position = camera.projection * camera.view * object.model_matrix * vec4(position, 1.0);
}
