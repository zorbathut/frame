
#include <gtest/gtest.h>

#include <frames/text.h>

#include "lib.h"

TEST(Text, Basic) {
  TestEnvironment env;

  const int c_tests = 7;

  const int sizes[c_tests] = { 12, 18, 24, 36, 48, 72, 96 };
  const char *text[c_tests] = {
    "Five hexing wizard bots jump quickly.",
    "Pack my box with five dozen liquor jugs.",
    "Waxy and quivering, jocks fumble the pizza.",
    "Both fickle dwarves jinx my pig quiz.",  // stupid dwarves, I liked that pig quiz
    "Cozy sphinx waves quart jug of bad milk.",
    "Jumpy halfling dwarves pick quartz box.", // and now my box! come on guys knock it off
    "The jay, pig, fox, zebra and my wolves quack!", // this is the worst guard wolf ever
  };
  Frames::Text *frames[c_tests] = { 0, };

  Frames::Frame *anchor = Frames::Frame::CreateTagged(env->GetRoot());
  anchor->SetPoint(Frames::BOTTOMLEFT, env->GetRoot(), Frames::TOPLEFT, 20.f, 0.f);
  for (int i = 0; i < c_tests; ++i) {
    Frames::Text *tex = Frames::Text::CreateTagged(env->GetRoot());
    tex->SetSize(sizes[i]);
    tex->SetText(text[i]);
    tex->SetPoint(Frames::TOPLEFT, anchor, Frames::BOTTOMLEFT, 0.f, 5.f);
    anchor = tex;
    frames[i] = tex;
  }

  TestSnapshot(env);

  for (int i = 0; i < c_tests; ++i) {
    if (i % 2 == 0) {
      frames[i]->SetFont("test/geo_1.ttf");
    }
  }

  TestSnapshot(env);

  for (int i = 0; i < c_tests; ++i) {
    frames[i]->SetWordwrap(true);
    frames[i]->SetPoint(Frames::RIGHT, env->GetRoot(), Frames::RIGHT, -20.f, Frames::Nil);
  }

  TestSnapshot(env);

  for (int i = 0; i < c_tests; ++i) {
    frames[i]->ClearPoint(Frames::RIGHT);
    frames[i]->SetWidth(sizes[i] * 7);
  }

  TestSnapshot(env);
}