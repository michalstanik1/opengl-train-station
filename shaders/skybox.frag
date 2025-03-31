#version 450

// ----------------------------------------------------------------------------
// Input Variables
// ----------------------------------------------------------------------------

// The intpolated position from the vertex shader.
layout(location = 0) in vec3 fs_position;

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
    //rotate skybox 90 degrees
    mat3 rot = mat3(0, 0, 1, 0, 1, 0, -1, 0, 0);
    //swap Y-Z because Z is the up-axis, not Y
    vec4 frag_color = texture(cube_texture, fs_position.xzy * rot);
    vec4 fog_color = vec4(0.7);
    float fog_z1 = 100 * max(-0.5 + 1.5 - length(fs_position - vec3(1.0, 0.0, 0.0)), 0.0);
    float fog_z2 = 100 * max(-0.5 + 1.5 - length(fs_position - vec3(-1.0, 0.0, 0.0)), 0.0);
    float fog_z = max(fog_z1, fog_z2);
    float fog_density = 0.02;
    float fog_factor = exp2(-fog_density * fog_density * fog_z * fog_z * 1.44);
    fog_factor = clamp(fog_factor, 0.1, 1.0);
    final_color = mix(fog_color, frag_color, fog_factor);
}
