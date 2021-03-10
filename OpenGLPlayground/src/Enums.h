#pragma once

enum render_type {
    POINT = 0,
    LINE = 1,
    TRIANGLE = 2
};
enum culling_type {
    CW = 0,
    CCW = 1
};
enum shading_type {
    SMOOTH = 0,
    FLAT = 1,
};
enum depth_type {
    LESS = 0,
    ALWAYS = 1,
};
enum model_type {
    CUBE = 0,
    ROCK = 1,
    CYBORG = 2,
    TWO_CUBES = 3,
};

const std::vector<int> render_type_2_gl_type = {GL_POINT, GL_LINE, GL_FILL};
const std::vector<int> culling_type_2_gl_type = {GL_CW, GL_CCW};
const std::vector<int> depth_type_2_gl_type = {GL_LESS, GL_ALWAYS};
