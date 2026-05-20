#pragma once
#include <glad/glad.h>
#include <vector>

namespace starlight {

    class GLStateSentinel {
    public:
        GLStateSentinel() {
            glGetIntegerv(GL_CURRENT_PROGRAM, &m_program);
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_fbo);
            glGetIntegerv(GL_VIEWPORT, m_viewport);
            m_depthTest = glIsEnabled(GL_DEPTH_TEST);
            m_blend = glIsEnabled(GL_BLEND);
            m_cullFace = glIsEnabled(GL_CULL_FACE);
            glGetIntegerv(GL_BLEND_SRC_RGB, &m_srcRGB);
            glGetIntegerv(GL_BLEND_DST_RGB, &m_dstRGB);
        }

        ~GLStateSentinel() {
            glUseProgram(m_program);
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
            
            if (m_depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
            if (m_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
            if (m_cullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
            
            glBlendFunc(m_srcRGB, m_dstRGB);
        }

    private:
        int m_program, m_fbo;
        int m_viewport[4];
        unsigned char m_depthTest, m_blend, m_cullFace;
        int m_srcRGB, m_dstRGB;
    };

}
