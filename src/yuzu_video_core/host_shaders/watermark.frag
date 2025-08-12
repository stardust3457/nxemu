// SPDX-License-Identifier: GPL-2.0-or-later

#version 460 core

layout(location = 0) in vec2 tex_coord;
layout(location = 0) out vec4 frag_color;

layout(set = 0, binding = 0) uniform sampler2D watermark_texture;

layout(push_constant) uniform PushConstants {
    vec4 alpha_data;  // Use vec4 like yuzu does
};

void main() {
    vec4 tex_color = texture(watermark_texture, tex_coord);
    frag_color = vec4(tex_color.rgb, tex_color.a * alpha_data.x);
}