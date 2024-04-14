#pragma once
#include "../../../wayverb/JuceLibraryCode/JuceHeader.h"

#include <OpenGL/gl3.h>

#include "bindable.h"
#include "render_buffer.h"
#include "texture_object.h"

namespace mglu {

template <GLuint mode>
class fbo final : public bindable {
public:
    fbo()
            : bindable([](auto& i) { juce::gl::glGenFramebuffers(1, &i); },
                       [](auto i) { juce::gl::glDeleteFramebuffers(1, &i); }) {
    }

    void depthbuffer(const depth_buffer& buffer) const {
        juce::gl::glFramebufferRenderbuffer(juce::gl::GL_FRAMEBUFFER,
                                  juce::gl::GL_DEPTH_STENCIL_ATTACHMENT,
                                  juce::gl::GL_RENDERBUFFER,
                                  buffer.get_handle());
    }

    void texture(const texture_object& texture, GLenum attachment) const {
        juce::gl::glFramebufferTexture(
                             juce::gl::GL_FRAMEBUFFER, attachment, texture.get_handle(), 0);
    }

private:
    void do_bind(GLuint index) const override {
        juce::gl::glBindFramebuffer(mode, index);
    }
};

using read_draw_fbo = fbo<juce::gl::GL_FRAMEBUFFER>;
using read_fbo = fbo<juce::gl::GL_READ_FRAMEBUFFER>;
using draw_fbo = fbo<juce::gl::GL_DRAW_FRAMEBUFFER>;

}  // namespace mglu
