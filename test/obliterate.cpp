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

#include <gtest/gtest.h>

#include <frames/sprite.h>

#include "lib.h"

// Obliteration is intrinsically really really complicated, so we're making an entire file dedicated to it.

Frames::Frame *PlaceFrame(const char *name, TestEnvironment *env, Frames::Layout *parent, float x, float y, bool relative = false) {
  Frames::Frame *res = Frames::Frame::Create(relative ? (*env)->RootGet() : parent, name);
  res->PinSet(Frames::CENTER, relative ? parent : (*env)->RootGet(), relative ? 0.5f : x, relative ? 0.5f : y, relative ? x : 0.f, relative ? y : 0.f);
  res->BackgroundSet(Frames::Color(relative ? (x / 120 + 0.5f) : x, relative ? (y / 120 + 0.5f) : y, 0.5f));
  return res;
}

TEST(Obliterate, Basic) {
  TestEnvironment env;

  // Make some things we might obliterate
  Frames::Frame *l = PlaceFrame("l", &env, env->RootGet(), 0.25f, 0.5f);
  Frames::Frame *c = PlaceFrame("c", &env, env->RootGet(), 0.5f, 0.5f);
  Frames::Frame *r = PlaceFrame("r", &env, env->RootGet(), 0.75f, 0.5f);

  TestSnapshot(env);

  c->Obliterate();

  TestSnapshot(env);

  l->Obliterate();
  r->Obliterate();

  TestSnapshot(env);
}

TEST(Obliterate, Parent) {
  TestEnvironment env;

  // Structure - every frame is a child of its prefix

  Frames::Frame *l = PlaceFrame("l", &env, env->RootGet(), 0.25f, 0.5f);
  Frames::Frame *c = PlaceFrame("c", &env, env->RootGet(), 0.5f, 0.5f);
  Frames::Frame *r = PlaceFrame("r", &env, env->RootGet(), 0.75f, 0.5f);
  
  Frames::Frame *la = PlaceFrame("la", &env, l, 0.17f, 0.6f);
  Frames::Frame *lb = PlaceFrame("lb", &env, l, 0.28f, 0.6f);
  Frames::Frame *ca = PlaceFrame("ca", &env, c, 0.42f, 0.6f);
  Frames::Frame *cb = PlaceFrame("cb", &env, c, 0.53f, 0.6f);
  Frames::Frame *ra = PlaceFrame("ra", &env, r, 0.67f, 0.6f);
  Frames::Frame *rb = PlaceFrame("rb", &env, r, 0.78f, 0.6f);

  Frames::Frame *laa = PlaceFrame("laa", &env, la, 0.14f, 0.7f);
  Frames::Frame *lab = PlaceFrame("lab", &env, la, 0.20f, 0.7f);
  Frames::Frame *caa = PlaceFrame("caa", &env, ca, 0.39f, 0.7f);
  Frames::Frame *cab = PlaceFrame("cab", &env, ca, 0.45f, 0.7f);
  Frames::Frame *raa = PlaceFrame("raa", &env, ra, 0.64f, 0.7f);
  Frames::Frame *rab = PlaceFrame("rab", &env, ra, 0.70f, 0.7f);

  TestSnapshot(env);

  laa->Obliterate();
  ca->Obliterate();
  r->Obliterate();

  TestSnapshot(env);

  l->Obliterate();
  cb->Obliterate();

  TestSnapshot(env);

  c->Obliterate();

  TestSnapshot(env);
}

TEST(Obliterate, Linked) {
  TestEnvironment env;
  env.AllowErrors(); // This one creates a bunch of errors

  // Looks a bit weird because they're reverting to default positions; we resize frames to ensure that all the appropriate frames are being displayed.

  Frames::Frame *l = PlaceFrame("l", &env, env->RootGet(), 0.25f, 0.5f);
  Frames::Frame *c = PlaceFrame("c", &env, env->RootGet(), 0.5f, 0.5f);
  Frames::Frame *r = PlaceFrame("r", &env, env->RootGet(), 0.75f, 0.5f);
  
  Frames::Frame *la = PlaceFrame("la", &env, l, -60, 50, true);
  Frames::Frame *lb = PlaceFrame("lb", &env, l, 60, 50, true);
  Frames::Frame *ca = PlaceFrame("ca", &env, c, -60, 50, true);
  Frames::Frame *cb = PlaceFrame("cb", &env, c, 60, 50, true);
  Frames::Frame *ra = PlaceFrame("ra", &env, r, -60, 50, true);
  Frames::Frame *rb = PlaceFrame("rb", &env, r, 60, 50, true);

  Frames::Frame *laa = PlaceFrame("laa", &env, la, -30, 50, true);
  Frames::Frame *lab = PlaceFrame("lab", &env, la, 30, 50, true);
  Frames::Frame *caa = PlaceFrame("caa", &env, ca, -30, 50, true);
  Frames::Frame *cab = PlaceFrame("cab", &env, ca, 30, 50, true);
  Frames::Frame *raa = PlaceFrame("raa", &env, ra, -30, 50, true);
  Frames::Frame *rab = PlaceFrame("rab", &env, ra, 30, 50, true);
  
  caa->WidthSet(50);
  caa->HeightSet(30);
  cab->WidthSet(30);
  cab->HeightSet(50);

  rb->WidthSet(60);
  rb->HeightSet(20);

  TestSnapshot(env);

  laa->Obliterate();
  ca->Obliterate();
  r->Obliterate();

  TestSnapshot(env);

  l->Obliterate();
  cb->Obliterate();

  TestSnapshot(env);

  c->Obliterate();

  TestSnapshot(env);
}
