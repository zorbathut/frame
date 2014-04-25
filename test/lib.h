
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

// Sets up a working test environment
class TestSDLEnvironment : Frames::detail::Noncopyable {
public:
  TestSDLEnvironment();
  ~TestSDLEnvironment();

  int GetWidth() const;
  int GetHeight() const;

  void Swap();
  void HandleEvents();

private:
  SDL_Window *m_win;
  SDL_GLContext m_glContext;

  int m_width;
  int m_height;
};

class TestLogger;

class TestEnvironment : Frames::detail::Noncopyable {
public:
  TestEnvironment(bool startSDL = true);
  ~TestEnvironment();

  Frames::Environment *operator*() { return m_env; }
  Frames::Environment *operator->() { return m_env; }

  int GetWidth() const { return m_sdl->GetWidth(); }
  int GetHeight() const { return m_sdl->GetHeight(); }

  void Swap() { return m_sdl->Swap(); }
  void HandleEvents() { return m_sdl->HandleEvents(); }

  void AllowErrors();

private:
  TestSDLEnvironment *m_sdl; // mostly taken care of with constructor/destructor
  Frames::Environment *m_env;

  Frames::Ptr<TestLogger> m_logger; // owned by m_env
};

class VerbLog : Frames::detail::Noncopyable {
public:
  VerbLog();
  ~VerbLog();

  void Snapshot();  // auto-snapshot on exit
  template <typename Parameters> void Attach(Frames::Layout *layout, const Frames::Verb<Parameters> &verb) {
    EXPECT_TRUE(m_nameUniqueTest.count(layout->GetName()) == 0 || m_nameUniqueTest[layout->GetName()] == layout);

    typename Frames::Verb<Parameters>::TypeDelegate delegate = typename Frames::Verb<Parameters>::TypeDelegate(this, &VerbLog::RecordEvent);
    layout->EventAttach(verb, delegate);
    m_detachers.push_back(new Detacher<Parameters>(layout, verb, delegate));
  }

private:
  void RecordEvent(Frames::Handle *);

  void RecordResult(const std::string &str);
  std::string m_records;

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

void TestSnapshot(TestEnvironment &env);
void HaltAndRender(TestEnvironment &env); // Intended for debugging with an attached graphics debugger

#endif
