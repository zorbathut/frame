
#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/text.h>
#include <frames/detail_format.h>

#include "lib.h"
#include "doc/colors.h"

TEST(Framebasics, Example) {
  TestEnvironment env(true, 640, 360);

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(120);
    frame->HeightSet(70);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 50, 30);
    frame->BackgroundSet(tdc::red);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(50);
    frame->HeightSet(250);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 270, 50);
    frame->BackgroundSet(tdc::blue);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(110);
    frame->HeightSet(110);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 440, 60);
    frame->BackgroundSet(tdc::green);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(220);
    frame->HeightSet(180);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 30, 130);
    frame->BackgroundSet(tdc::cyan);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(120);
    frame->HeightSet(90);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 350, 200);
    frame->BackgroundSet(tdc::orange);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(90);
    frame->HeightSet(120);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 490, 230);
    frame->BackgroundSet(tdc::purple);
  }

  TestSnapshot(env, "ref/doc/framebasics_example");
};

void LayerMake(Frames::Layout *root, float width, float height, float x, float y, float layer, Frames::Color color, float text_y) {
  Frames::Frame *frame = Frames::Frame::Create(root, "layer");
  frame->WidthSet(width);
  frame->HeightSet(height);
  frame->PinSet(Frames::TOPLEFT, root, Frames::TOPLEFT, x, y);
  frame->BackgroundSet(color);
  frame->LayerSet(layer);

  Frames::Text *text = Frames::Text::Create(frame, "text");
  text->PinSet(0, text_y, frame, 0, text_y, 3, 3 * (1 - text_y * 2));
  text->SizeSet(12);
  text->FontSet("geo_1.ttf");
  text->TextSet(Frames::detail::Format("Layer %g", layer));
}

TEST(Framebasics, Layering) {
  TestEnvironment env(true, 640, 360);

  LayerMake(env->RootGet(), 80, 80, 40, 40, 0, tdc::blue, 0);
  LayerMake(env->RootGet(), 80, 80, 60, 60, 1, tdc::orange, 0);
  LayerMake(env->RootGet(), 80, 80, 80, 80, 2, tdc::red, 0);

  LayerMake(env->RootGet(), 80, 80, 220, 40, -1, tdc::purple, 0);
  LayerMake(env->RootGet(), 80, 80, 280, 60, 0, tdc::red, 0);
  LayerMake(env->RootGet(), 80, 80, 240, 80, 1, tdc::cyan, 0);

  LayerMake(env->RootGet(), 80, 80, 460, 80, 0, tdc::red, 1);
  LayerMake(env->RootGet(), 80, 80, 420, 60, 0.5, tdc::orange, 1);
  LayerMake(env->RootGet(), 80, 80, 440, 40, 20, tdc::green, 1);

  LayerMake(env->RootGet(), 80, 80, 40, 260, -1000, tdc::red, 1);
  LayerMake(env->RootGet(), 80, 80, 60, 240, -0.2f, tdc::orange, 1);
  LayerMake(env->RootGet(), 80, 80, 80, 220, 0.01f, tdc::green, 1);
  LayerMake(env->RootGet(), 80, 80, 100, 200, 0.015f, tdc::cyan, 1);

  LayerMake(env->RootGet(), 80, 80, 240, 200, 9000, tdc::cyan, 0);
  LayerMake(env->RootGet(), 80, 80, 220, 220, 9001, tdc::green, 0);
  LayerMake(env->RootGet(), 80, 80, 280, 240, 9002, tdc::purple, 0);
  LayerMake(env->RootGet(), 80, 80, 260, 260, 9003, tdc::blue, 0);

  LayerMake(env->RootGet(), 80, 80, 440, 260, 42, tdc::orange, 1);
  LayerMake(env->RootGet(), 80, 80, 460, 240, 113, tdc::red, 1);
  LayerMake(env->RootGet(), 80, 80, 420, 220, 1024, tdc::cyan, 1);
  LayerMake(env->RootGet(), 80, 80, 480, 200, 2600, tdc::purple, 1);

  TestSnapshot(env, "ref/doc/framebasics_layering");
};