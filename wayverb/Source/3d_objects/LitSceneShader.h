#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"
#include "modern_gl_utils/exceptions.h"
#include "modern_gl_utils/generic_shader.h"

#include "glm/gtc/type_ptr.hpp"

class LitSceneShader final {
public:
    LitSceneShader();

    void set_colour(const glm::vec3& c) const;

    auto get_attrib_location_v_position() const {
        return program.get_attrib_location("v_position");
    }

    auto get_attrib_location_v_color() const {
        return program.get_attrib_location("v_color");
    }

    void set_model_matrix(const glm::mat4& mat) const;
    void set_view_matrix(const glm::mat4& mat) const;
    void set_projection_matrix(const glm::mat4& mat) const;

    auto get_scoped() const { return program.get_scoped(); }

private:
    static const char* vertex_shader;
    static const char* fragment_shader;

    mglu::program program;
};
