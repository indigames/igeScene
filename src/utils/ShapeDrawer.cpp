#include "utils/ShapeDrawer.h"

#if defined(_WIN32)
#  include <glew.h>
#elif defined(__APPLE__)
#  include "TargetConditionals.h"
#  if TARGET_OS_IPHONE
#    include <OpenGLES/ES3/gl.h>
#    include <OpenGLES/ES3/glext.h>
#  else
#    include <OpenGL/gl3.h>
#  endif
#elif defined(__ANDROID__)
#  include <EGL/egl.h>
#  include <GLES3/gl3.h>
#  include <GLES3/gl3ext.h>
#elif defined(__EMSCRIPTEN__)
#   include <EGL/egl.h>
#   include <GLES3/gl3.h>
#endif

#define MAX_LINES 10 * 16384

namespace ige::scene
{
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__) || TARGET_OS_IPHONE
    constexpr auto GLSL_VERSION = "#version 300 es";
#else
    constexpr auto GLSL_VERSION = "#version 330";
#endif
    auto line_VShader = R"(
precision mediump float;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
uniform mat4 viewProjection;
out vec4 vcolor;
void main()
{
    gl_Position = viewProjection * vec4(position, 1.0);
    vcolor = vec4(color, 1.0);
}
)";

    auto line_FShader = R"(
precision mediump float;
in vec4 vcolor;
out vec4 frag_color;
void main()
{
    frag_color = vcolor;
}
)";

    std::vector<DebugLine> m_lines;
    std::vector<DebugLine> m_lines2D;
    GLuint m_lineProgramId;
    GLint m_viewProjectionLocation;
    GLuint m_vertexArrayID;
    GLuint m_positionBufferID;
    GLuint m_colorBufferID;
    Mat4 m_viewProjection;
    Mat4 m_viewProjection2D;

    uint32_t ShapeDrawer::loadShaders(const char* vertexShaderCode, const char* fragmentShaderCode)
    {
        GLint result = GL_FALSE;
        int infoLength;

        // Create the shaders
        GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Compile Vertex Shader
        auto code = std::string(GLSL_VERSION) + vertexShaderCode;
        auto vsh = code.c_str();
        glShaderSource(vertexShaderID, 1, &vsh, NULL);
        glCompileShader(vertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLength);
        if (infoLength > 0)
        {
            std::vector<char> message(infoLength);
            glGetShaderInfoLog(vertexShaderID, infoLength, NULL, &message[0]);
            pyxie_printf("Vertex %s\n", &message[0]);
        }

        // Compile Fragment Shader
        code = std::string(GLSL_VERSION) + fragmentShaderCode;
        auto fsh = code.c_str();
        glShaderSource(fragmentShaderID, 1, &fsh, NULL);
        glCompileShader(fragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLength);
        if (infoLength > 0)
        {
            std::vector<char> message(infoLength);
            glGetShaderInfoLog(fragmentShaderID, infoLength, NULL, &message[0]);
            pyxie_printf("Fragment %s\n", &message[0]);
        }

        // Link the program
        GLuint programID = glCreateProgram();
        glAttachShader(programID, vertexShaderID);
        glAttachShader(programID, fragmentShaderID);
        glLinkProgram(programID);

        // Check the program
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLength);
        if (infoLength > 0)
        {
            std::vector<char> message(infoLength);
            glGetProgramInfoLog(programID, infoLength, NULL, &message[0]);
            pyxie_printf("Program %s\n", &message[0]);
        }

        // Clean up
        glDetachShader(programID, vertexShaderID);
        glDetachShader(programID, fragmentShaderID);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        // return
        return programID;
    }

    void ShapeDrawer::initialize()
    {
        m_lineProgramId = loadShaders(line_VShader, line_FShader);
        m_viewProjectionLocation = glGetUniformLocation(m_lineProgramId, "viewProjection");

        glGenVertexArrays(1, &m_vertexArrayID);
        glBindVertexArray(m_vertexArrayID);

        glGenBuffers(1, &m_positionBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glBufferData(GL_ARRAY_BUFFER, MAX_LINES * 3 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

        glGenBuffers(1, &m_colorBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferID);
        glBufferData(GL_ARRAY_BUFFER, MAX_LINES * 3 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    }

    void ShapeDrawer::setViewProjectionMatrix(const Mat4 &viewProjection)
    {
        m_viewProjection = viewProjection;
    }
    
    void ShapeDrawer::setViewProjectionMatrix2D(const Mat4 &viewProjection)
    {
        m_viewProjection2D = viewProjection;
    }

    void ShapeDrawer::drawLine(const Vec3 &start, const Vec3 &end, const Vec3 &color)
    {
        if(m_lines.size() < MAX_LINES)
            m_lines.push_back(DebugLine(start, end, color));
    }

    void ShapeDrawer::drawRect(const Vec3&start, const Vec3&end, const Vec3 &color)
    {
        auto z = (start.Z() + end.Z()) * 0.5f;
        m_lines.push_back(DebugLine({ start.X(), start.Y(), z }, {end.X(), start.Y(), z}, color));
        m_lines.push_back(DebugLine({ end.X(), start.Y(), z }, {end.X(), end.Y(), z}, color));
        m_lines.push_back(DebugLine({ end.X(), end.Y(), z }, { start.X(), end.Y(), z}, color));
        m_lines.push_back(DebugLine({ start.X(), end.Y(), z }, { start.X(), start.Y(), z}, color));
    }
    
    void ShapeDrawer::drawLine2D(const Vec3 &start, const Vec3 &end, const Vec3 &color)
    {
        if(m_lines2D.size() < MAX_LINES)
            m_lines2D.push_back(DebugLine(start, end, color));
    }

    void ShapeDrawer::drawRect2D(const Vec3&start, const Vec3&end, const Vec3 &color)
    {
        auto z = (start.Z() + end.Z()) * 0.5f;
        m_lines2D.push_back(DebugLine({ start.X(), start.Y(), z }, {end.X(), start.Y(), z}, color));
        m_lines2D.push_back(DebugLine({ end.X(), start.Y(), z }, {end.X(), end.Y(), z}, color));
        m_lines2D.push_back(DebugLine({ end.X(), end.Y(), z }, { start.X(), end.Y(), z}, color));
        m_lines2D.push_back(DebugLine({ start.X(), end.Y(), z }, { start.X(), start.Y(), z}, color));
    }

    void ShapeDrawer::flush2D()
    {
        auto numVertices = m_lines2D.size() * 2;
        auto linePositions = new GLfloat[numVertices * 3];
        auto lineColors = new GLfloat[numVertices * 3];

        auto linePos = linePositions;
        auto lineCol = lineColors;

        for (unsigned i = 0; i < m_lines2D.size(); ++i)
        {
            const auto& line = m_lines2D[i];
            linePos[0] = line.start[0];
            linePos[1] = line.start[1];
            linePos[2] = line.start[2];
            linePos[3] = line.end[0];
            linePos[4] = line.end[1];
            linePos[5] = line.end[2];
            linePos += 6;

            lineCol[0] = line.color[0];
            lineCol[1] = line.color[1];
            lineCol[2] = line.color[2];
            lineCol[3] = line.color[0];
            lineCol[4] = line.color[1];
            lineCol[5] = line.color[2];
            lineCol += 6;
        }

        bool depthBackup = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(m_lineProgramId);
        glUniformMatrix4fv(m_viewProjectionLocation, 1, GL_FALSE, m_viewProjection2D.P());

        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * 3 * sizeof(GLfloat), linePositions);

        glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * 3 * sizeof(GLfloat), lineColors);

        glBindVertexArray(m_vertexArrayID);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferID);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glLineWidth(1.f);
        glDrawArrays(GL_LINES, 0, numVertices);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);

        glUseProgram(0);
        if (depthBackup)
            glEnable(GL_DEPTH_TEST);

        delete[] linePositions;
        delete[] lineColors;
        m_lines2D.clear();
    }

    void ShapeDrawer::flush()
    {
        auto numVertices = m_lines.size() * 2;
        auto linePositions = new GLfloat[numVertices * 3];
        auto lineColors = new GLfloat[numVertices * 3];

        auto linePos = linePositions;
        auto lineCol = lineColors;

        for (unsigned i = 0; i < m_lines.size(); ++i)
        {
            const auto& line = m_lines[i];
            linePos[0] = line.start[0];
            linePos[1] = line.start[1];
            linePos[2] = line.start[2];
            linePos[3] = line.end[0];
            linePos[4] = line.end[1];
            linePos[5] = line.end[2];
            linePos += 6;

            lineCol[0] = line.color[0];
            lineCol[1] = line.color[1];
            lineCol[2] = line.color[2];
            lineCol[3] = line.color[0];
            lineCol[4] = line.color[1];
            lineCol[5] = line.color[2];
            lineCol += 6;
        }

        bool depthBackup = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(m_lineProgramId);
        glUniformMatrix4fv(m_viewProjectionLocation, 1, GL_FALSE, m_viewProjection.P());

        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * 3 * sizeof(GLfloat), linePositions);

        glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * 3 * sizeof(GLfloat), lineColors);

        glBindVertexArray(m_vertexArrayID);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferID);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glLineWidth(1.f);
        glDrawArrays(GL_LINES, 0, numVertices);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);

        glUseProgram(0);
        if (depthBackup)
            glEnable(GL_DEPTH_TEST);

        delete[] linePositions;
        delete[] lineColors;
        m_lines.clear();

        ShapeDrawer::flush2D();
    }

} // namespace ige::scene