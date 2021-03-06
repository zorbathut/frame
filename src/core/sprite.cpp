/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#include "frames/sprite.h"

#include "frames/cast.h"
#include "frames/environment.h"
#include "frames/renderer.h"

namespace Frames {
  FRAMES_DEFINE_RTTI(Sprite, Frame);

  Sprite *Sprite::Create(Layout *parent, const std::string &name = "") {
    if (!parent) {
      Configuration::Get().LoggerGet()->LogError("Attempted to create Sprite with null parent");
      return 0;
    }
    return new Sprite(parent, name);
  }

  void Sprite::TextureSet(const std::string &id) {
    // work work work
    m_texture = EnvironmentGet()->TextureChunkFromId(id);
    
    if (m_texture) {
      m_texture_id = id;
      WidthDefaultSet((float)m_texture->WidthGet());
      HeightDefaultSet((float)m_texture->HeightGet());
    } else {
      m_texture_id = "";
      WidthDefaultSet(detail::SizeDefault);
      HeightDefaultSet(detail::SizeDefault);
    }
  }

  void Sprite::RenderElement(detail::Renderer *renderer) const {
    Frame::RenderElement(renderer);

    if (m_texture) {
      Color tint = m_tint * Color(1, 1, 1, renderer->AlphaGet());

      renderer->TextureSet(m_texture->BackingGet());

      const float cx = PointGet(X, 0.5);
      const float cy = PointGet(Y, 0.5);

      const float dx = WidthGet() / 2;
      const float dy = HeightGet() / 2;

      const float s = sin(m_angle);
      const float c = cos(m_angle);

      detail::Renderer::Vertex *v = renderer->Request(1);

      if (v) {
        v[0].p.x = cx - dx * c + dy * s; v[0].p.y = cy - dx * s - dy * c;
        v[1].p.x = cx + dx * c + dy * s; v[1].p.y = cy + dx * s - dy * c;
        v[2].p.x = cx + dx * c - dy * s; v[2].p.y = cy + dx * s + dy * c;
        v[3].p.x = cx - dx * c - dy * s; v[3].p.y = cy - dx * s + dy * c;

        v[0].t = m_texture->BoundsGet().s;
        v[2].t = m_texture->BoundsGet().e;

        v[1].t.x = v[2].t.x;
        v[1].t.y = v[0].t.y;

        v[3].t.x = v[0].t.x;
        v[3].t.y = v[2].t.y;

        v[0].c = tint;
        v[1].c = tint;
        v[2].c = tint;
        v[3].c = tint;

        renderer->Return();
      }
    }
  }

  Sprite::Sprite(Layout *parent, const std::string &name) :
      Frame(parent, name),
      m_tint(1, 1, 1, 1),
      m_angle(0)
  { };
  Sprite::~Sprite() { };
}

