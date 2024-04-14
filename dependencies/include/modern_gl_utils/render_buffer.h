#pragma once
#include "../../../wayverb/JuceLibraryCode/JuceHeader.h"

#include "bindable.h"

#include "glm/glm.hpp"
#include <OpenGL/gl3.h>

namespace mglu {

template <GLuint mode>
class render_buffer final : public bindable {
public:
    render_buffer()
            : bindable([](auto& i) { juce::gl::glGenRenderbuffers(1, &i); },
                       [](auto i) { juce::gl::glDeleteRenderbuffers(i); }) {
    }

    void storage(glm::ivec2 size) const {
        juce::gl::glRenderbufferStorage(juce::gl::GL_RENDERBUFFER, mode, size.x, size.y);
    }

private:
    void do_bind(GLuint index) const {
        juce::gl::glBindRenderbuffer(juce::gl::GL_RENDERBUFFER, index);
    }
};

using depth_buffer = render_buffer<juce::gl::GL_DEPTH_STENCIL>;

}  // namespace mglu
