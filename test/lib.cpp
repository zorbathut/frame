
#include "lib.h"

#include <gtest/gtest.h>

#include <SDL.h>
#include <frames/os_gl.h>
#include <frames/detail_format.h>
#include <frames/environment.h>
#include <frames/event.h>
#include <frames/stream.h>
#include <frames/layout.h>
#include <frames/loader.h>
#include <frames/texture_config.h>

#include <png.h>

#include <cstdio>
#include <fstream>

struct TestNames {
  std::string testName;
  std::string resultName;
};

TestNames GetTestNames(const std::string &family, const std::string &extension) {
  TestNames rv;

  std::string baseName = Frames::detail::Format("ref/%s_%s", ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name(), ::testing::UnitTest::GetInstance()->current_test_info()->name());

  static std::string s_testNameLast = "";
  static std::map<std::string, int> s_familyIds;

  if (baseName != s_testNameLast) {
    s_testNameLast = baseName;
    s_familyIds.clear();
  }

  std::string testFilePrefix = Frames::detail::Format("%s_%s_%d", baseName, family, s_familyIds[family]++);

  rv.testName = testFilePrefix + extension;
  rv.resultName = testFilePrefix + "_result" + extension;

  // write to the "input" file if that file doesn't exist
  if (!std::ifstream(rv.testName.c_str())) {
    rv.resultName = rv.testName;
  }

  return rv;
}

void TestCompareStrings(const std::string &family, const std::string &data) {
  TestNames testNames = GetTestNames(family, ".txt");

  // Grab our source file (or try to)
  std::string testsrc;
  {
    std::ifstream in(testNames.testName.c_str(), std::ios::binary); // Binary so we don't have to worry about \r\n's showing up in our event results
    if (in) {
      testsrc = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }
  }

  // Write result to disk
  {
    std::ofstream out(testNames.resultName.c_str(), std::ios::binary);
    out << data;
  }

  EXPECT_TRUE(data == testsrc);
}

TestSDLEnvironment::TestSDLEnvironment() : m_win(0), m_glContext(0) {
  EXPECT_EQ(0, SDL_Init(SDL_INIT_VIDEO));

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  m_win = SDL_CreateWindow("Frames test harness", 100, 100, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  EXPECT_TRUE(m_win != NULL);

  m_width = 1280;
  m_height = 720;

  m_glContext = SDL_GL_CreateContext(m_win);
  EXPECT_TRUE(m_glContext != 0);
}

TestSDLEnvironment::~TestSDLEnvironment() {
  if (!m_glContext) {
    SDL_GL_DeleteContext(m_glContext);
  }

  if (m_win) {
    SDL_DestroyWindow(m_win);
  }

  SDL_Quit();
}

int TestSDLEnvironment::GetWidth() const {
  return m_width;
}
int TestSDLEnvironment::GetHeight() const {
  return m_height;
}

void TestSDLEnvironment::Swap() {
  SDL_GL_SwapWindow(m_win);
}

void TestSDLEnvironment::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // We actually just ignore all events
  }
}

class TestLogger : public Frames::Configuration::Logger {
public:
  TestLogger() : m_allowErrors(false) { }
  ~TestLogger() {
    if (m_allowErrors) {
      EXPECT_FALSE(m_loggedErrors.empty());

      TestCompareStrings("error", m_loggedErrors);
    }
  }

  virtual void LogError(const std::string &log) {
    printf("[ERR] %s\n", log.c_str());
    if (!m_allowErrors) {
      GTEST_FAIL();
    }

    m_loggedErrors += log;
    m_loggedErrors += "\n";
  }
  virtual void LogDebug(const std::string &log) {
    printf("[DBG] %s\n", log.c_str());
  }

  void AllowErrors() {
    m_allowErrors = true;
  }

private:
  std::string m_loggedErrors;

  bool m_allowErrors;
};

TestEnvironment::TestEnvironment(bool startSDL) : m_env(0), m_sdl(0) {
  if (startSDL) {
    m_sdl = new TestSDLEnvironment();
  }

  Frames::Configuration config;
  config.fontDefaultId = "LindenHill.otf";
  m_logger = Frames::Ptr<TestLogger>(new TestLogger());
  config.logger = m_logger;
  m_env = new Frames::Environment(config);

  if (startSDL) {
    m_env->ResizeRoot(GetWidth(), GetHeight()); // set this up so we can check coordinates, otherwise we'll currently assume there are no coordinates
  }
}

TestEnvironment::~TestEnvironment() {
  EXPECT_EQ(GL_NO_ERROR, glGetError()); // verify no GL issues
  delete m_env;
  delete m_sdl;
}

void TestEnvironment::AllowErrors() {
  m_logger->AllowErrors();
}

VerbLog::VerbLog() {
}

VerbLog::~VerbLog() {
  if (!m_records.empty()) {
    Snapshot();
  }

  for (int i = 0; i < (int)m_detachers.size(); ++i) {
    delete m_detachers[i];
  }
}

void VerbLog::Snapshot() {
  TestCompareStrings("event", m_records);

  m_records.clear();
}

void VerbLog::RecordEvent(Frames::Handle *handle) {
  RecordResult(Frames::detail::Format("Event %s on %s", handle->GetVerb()->GetName(), handle->GetTarget()->DebugGetName()));
}

void VerbLog::RecordResult(const std::string &str) {
  m_records += str;
  m_records += "\n";
}

void TestSnapshot(TestEnvironment &env) {
  // Do the render
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  env->ResizeRoot(env.GetWidth(), env.GetHeight());
  env->Render();

  TestNames testNames = GetTestNames("screen", ".png");
  
  // We now have our test filename

  // Grab a screenshot
  std::vector<unsigned char> pixels; pixels.resize(4 * env.GetWidth() * env.GetHeight());

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(0, 0, env.GetWidth(), env.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Annoyingly, OpenGL reads coordinates in math quadrant order, not scanline order like the rest of the civilized computer world
  // So . . . go ahead and invert the entire array
  for (int y = 0; y < env.GetHeight() / 2; ++y) {
    for (int x = 0; x < env.GetWidth() * 4; ++x) {
      std::swap(pixels[y * env.GetWidth() * 4 + x], pixels[(env.GetHeight() - 1 - y) * env.GetWidth() * 4 + x]);
    }
  }

  // Grab our source file (or try to)
  std::vector<unsigned char> reference;
  {
    Frames::StreamFile *stream = Frames::StreamFile::Create(testNames.testName);
    if (stream)
    {
      Frames::TextureConfig tex = Frames::Loader::PNG::Load(*env, stream);
      EXPECT_EQ(Frames::TextureConfig::RAW, tex.GetMode());
      EXPECT_EQ(Frames::TextureConfig::MODE_RGBA, tex.Raw_GetType());
      EXPECT_EQ(4, Frames::TextureConfig::GetBPP(Frames::TextureConfig::MODE_RGBA));
      EXPECT_EQ(tex.GetWidth() * 4, tex.Raw_GetStride());
      reference.resize(tex.GetWidth() * tex.GetHeight() * 4);
      memcpy(&reference[0], tex.Raw_GetData(), tex.GetWidth() * tex.GetHeight() * 4);
      delete stream;
    }
  }

  {
    // Write result to disk
    // Don't need this anywhere in Frames so we'll just hack it in here
    FILE *fp = fopen(testNames.resultName.c_str(), "wb");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);

    png_set_IHDR(png_ptr, info_ptr, env.GetWidth(), env.GetHeight(),
      8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    std::vector<unsigned char *> rows;
    for (int i = 0; i < env.GetHeight(); ++i) {
      rows.push_back(&pixels[0] + i * env.GetWidth() * 4);
    }

    png_write_image(png_ptr, &rows[0]);

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
  }

  EXPECT_EQ(reference.size(), pixels.size());
  EXPECT_TRUE(pixels == reference);
}

void HaltAndRender(TestEnvironment &env) {
  while (true) {
    // Do the render
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    env->ResizeRoot(env.GetWidth(), env.GetHeight());
    env->Render();
    env.Swap();
    env.HandleEvents();
  }
}
