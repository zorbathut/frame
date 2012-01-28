
#include "frames/renderer.h"

#include "boost/static_assert.hpp"

#include <GL/glew.h>

#include <vector>
#include <algorithm>

using namespace std;

BOOST_STATIC_ASSERT(sizeof(Frames::GLfloat) == sizeof(GLfloat));
BOOST_STATIC_ASSERT(sizeof(Frames::GLushort) == sizeof(GLushort));
BOOST_STATIC_ASSERT(sizeof(Frames::GLuint) == sizeof(GLuint));

namespace Frames {
  const int bufferElements = 1 << 16; // fit in a ushort
  const int bufferSize = bufferElements * sizeof(Renderer::Vertex);

  Renderer::Renderer() : m_buffer_pos(bufferElements) {  // set to bufferElements so we create a real buffer when we need it
    glGenBuffers(1, &m_buffer);
    glGenBuffers(1, &m_elements);

    // init the elements buffer - later we should think more about how to make this general-purpose, but for now, we won't really have repeated vertices anyway
    {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elements);
      vector<GLushort> elements(1<<16);
      for (int i = 0; i < elements.size(); ++i) {
        elements[i] = i;
      }
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
  };

  Renderer::~Renderer() {
    glDeleteBuffers(1, &m_buffer);
    glDeleteBuffers(1, &m_elements);
  }

  void Renderer::Begin(int width, int height) {
    glPushAttrib(~0);
    glPushClientAttrib(~0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glTranslatef(-1.f, 1.f, 0.f);
    glScalef(2.f / width, -2.f / height, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elements);

    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, t));
    glColorPointer(4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, r));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    // toggle texture on and off as necessary
  }

  void Renderer::End() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glPopClientAttrib();
    glPopAttrib();
  }

  Renderer::Vertex *Renderer::Request(int vertices) {
    int size = vertices * sizeof(Vertex);
    if (m_buffer_pos + vertices > bufferElements) {
      // we'll have to clear it out
      glBufferData(GL_ARRAY_BUFFER, max(size, bufferSize), 0, GL_STREAM_DRAW);
      m_buffer_pos = 0;
    }

    // now we have acceptable data
    Vertex *rv = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, m_buffer_pos * sizeof(Vertex), size, GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);

    m_last_pos = m_buffer_pos;
    m_last_vertices = vertices;
    m_buffer_pos += vertices;

    return rv;
  }

  void Renderer::Return(int mode) {
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glDrawElements(mode, m_last_vertices, GL_UNSIGNED_SHORT, (void*)m_last_pos);
  }
}
