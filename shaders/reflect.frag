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

layout(location = 20) uniform int texture_flags;

// The intpolated position from the vertex shader.
layout(location = 0) in vec3 fs_position;
// The intpolated normal from the vertex shader.
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in flat int fs_instance_id;

layout(binding = 9) uniform samplerCube cube_texture;

// ----------------------------------------------------------------------------
// Output Variables
// ----------------------------------------------------------------------------

// The final output color.
layout(location = 0) out vec4 final_color;

// ----------------------------------------------------------------------------
// Main Method
// ----------------------------------------------------------------------------
void main() {
    final_color = vec4(1);
    vec3 color_sum = vec3(0.0);
    Object object = objects[fs_instance_id];
    mat3 rot = mat3(0, 0, 1, 0, 1, 0, -1, 0, 0);

    for (int i = 0; i < lights.length(); i++) {
        Light light = lights[i];
        vec3 light_vector = light.position.xyz - fs_position * light.position.w;
        vec3 L = normalize(light_vector);
        vec3 N = normalize(fs_normal);
        vec3 E = normalize(camera.position - fs_position);
        vec3 H = normalize(L + E);

        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0001);

        vec3 ambient = object.ambient_color.rgb * light.ambient_color.rgb;
        vec3 reflected = normalize(reflect(fs_position - camera.position, normalize(fs_normal)));
        vec3 diffuse = texture(cube_texture, reflected.xzy * rot).rgb;
        vec3 specular = object.specular_color.rgb * light.specular_color.rgb;

        vec3 color = ambient.rgb + NdotL * diffuse.rgb + pow(NdotH, object.specular_color.w) * specular;
        color /= (length(light_vector) * length(light_vector));

        color_sum += color;
    }

    color_sum = pow(color_sum, vec3(1.0 / 2.2)); // gamma correction
    
    vec3 fog_color = vec3(0.7);
    float fog_z = max(-50.0 + abs(fs_position.x), 0.0);
    float fog_density = 0.02;
    float fog_factor = exp2(-fog_density * fog_density * fog_z * fog_z * 1.44);
    fog_factor = clamp(fog_factor, 0.1, 1.0);
    color_sum = mix(fog_color, color_sum, fog_factor);

    final_color = vec4(color_sum, 1.0);
}
