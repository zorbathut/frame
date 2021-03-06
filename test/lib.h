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

#ifndef FRAMES_TEST_LIB
#define FRAMES_TEST_LIB

#include <SDL.h>

#include <frames/environment.h>
#include <frames/event.h>
#include <frames/layout.h>

#include <gtest/gtest.h>

namespace Frames {
  class Layout;
  template <typename Parameters> class Verb;
}

class TestWindow : Frames::detail::Noncopyable {
public:
  TestWindow(int width, int height) : m_width(width), m_height(height) { }
  virtual ~TestWindow() { }

  int WidthGet() const { return m_width; }
  int HeightGet() const { return m_height; }

  virtual void Swap() = 0;
  virtual void HandleEvents() = 0;

  virtual Frames::Configuration::RendererPtr RendererGet() = 0;

  virtual void ClearRenderTarget() = 0;
  virtual std::vector<unsigned char> Screenshot() = 0;

private:
  int m_width;
  int m_height;
};

class TestCompare : Frames::detail::Noncopyable {
public:
  TestCompare(const std::string &suffix = "");
  ~TestCompare();

  void Append(const std::string &text);

private:
  std::string m_suffix;

  std::string m_records;
};

class TestLogger : public Frames::Configuration::Logger {
public:
  TestLogger();
  ~TestLogger();

  virtual void LogError(const std::string &log) FRAMES_OVERRIDE;
  virtual void LogDebug(const std::string &log) FRAMES_OVERRIDE;

  void AllowErrors();

private:
  TestCompare m_compare;

  bool m_seenErrors;
  bool m_allowErrors;
};

class TestEnvironment : Frames::detail::Noncopyable {
public:
  TestEnvironment(bool startUI = true, int width = 1280, int height = 720);
  ~TestEnvironment();

  Frames::Environment *operator*() { return m_env.Get(); }
  Frames::Environment *operator->() { return m_env.Get(); }

  int WidthGet() const;
  int HeightGet() const;

  void Swap();
  void HandleEvents();

  void AllowErrors();
  void ClearRenderTarget() { m_tenv->ClearRenderTarget(); }
  std::vector<unsigned char> Screenshot() { return m_tenv->Screenshot(); }

private:
  // mostly taken care of with constructor/destructor
  TestWindow *m_tenv;

  Frames::EnvironmentPtr m_env;

  Frames::Ptr<TestLogger> m_logger; // owned by m_env
};

class VerbLog : Frames::detail::Noncopyable {
public:
  VerbLog(TestCompare *compare, const std::string &descr = "");
  ~VerbLog();

  template <typename Parameters> void Attach(Frames::Layout *layout, const Frames::Verb<Parameters> &verb, float priority = 0.f) {
    EXPECT_TRUE(m_nameUniqueTest.count(layout->NameGet()) == 0 || m_nameUniqueTest[layout->NameGet()] == layout);

    typename Frames::Verb<Parameters>::TypeDelegate delegate = typename Frames::Verb<Parameters>::TypeDelegate(this, &VerbLog::RecordEvent);
    layout->EventAttach(verb, delegate, priority);
    m_detachers.push_back(new Detacher<Parameters>(layout, verb, delegate));
  }

private:
  void RecordEvent(Frames::Handle *handle);
  void RecordEvent(Frames::Handle *handle, int p1);
  void RecordEvent(Frames::Handle *handle, const Frames::Vector &p1);
  void RecordEvent(Frames::Handle *handle, Frames::Input::Key p1);
  void RecordEvent(Frames::Handle *handle, const std::string &p1);

  void RecordResult(Frames::Handle *handle, const std::string &params);
  
  TestCompare *m_compare;

  std::string m_prefix;

  std::map<std::string, Frames::Layout *> m_nameUniqueTest;

  class DetacherBase : Frames::detail::Noncopyable {
  public:
    virtual ~DetacherBase() { }
  };

  template <typename Parameters> class Detacher : public DetacherBase {
  public:
    Detacher(Frames::Layout *layout, const Frames::Verb<Parameters> &verb, typename Frames::Verb<Parameters>::TypeDelegate delegate) : m_layout(layout), m_verb(verb), m_delegate(delegate) {};
    virtual ~Detacher() {
      m_layout->EventDetach(m_verb, m_delegate);
    }

  private:
    Frames::Layout *m_layout;
    const Frames::Verb<Parameters> &m_verb;
    typename Frames::Verb<Parameters>::TypeDelegate m_delegate;
  };

  std::vector<DetacherBase *> m_detachers;
};

class SnapshotConfig {
public:
  SnapshotConfig() : m_delta(2), m_nearest(false) { } // defaulting delta to 2 until I figure out how to deal with that odd off-by-one glitch in, like, all color rendering. Seems to hit textures too?

  SnapshotConfig &File(const std::string &fname) { m_fname = fname; return *this; }
  SnapshotConfig &Delta(int delta) { m_delta = delta; return *this; }
  SnapshotConfig &Nearest(bool nearest) { m_nearest = nearest; return *this; }

  const std::string &FileGet() const { return m_fname; }
  int DeltaGet() const { return m_delta; }
  bool NearestGet() const { return m_nearest; }

private:
  std::string m_fname;
  int m_delta;
  bool m_nearest;
};

void TestSnapshot(TestEnvironment &env, const SnapshotConfig &csf = SnapshotConfig());
void HaltAndRender(TestEnvironment &env);

void RendererIdSet(const std::string &renderer);
std::string &RendererIdGet();

// different platforms do different weird inconsistent things with backbuffer alpha, so we just go ahead and fix it here
void ClampScreenshotAlpha(std::vector<unsigned char> *pixels);

#endif
