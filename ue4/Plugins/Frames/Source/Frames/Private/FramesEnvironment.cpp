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

#include "FramesPCH.h"

#include "FramesEnvironment.h"

#include "AllowWindowsPlatformTypes.h"
#include <frames/configuration.h>
#include <frames/detail_format.h>
#include <frames/environment.h>
#include <frames/layout.h>
#include <frames/renderer_null.h>
#include "HideWindowsPlatformTypes.h"

DEFINE_LOG_CATEGORY(FramesLog);

class FramesUE4Logger : Frames::Configuration::Logger
{
public:
  FramesUE4Logger() :
    m_onScreenErrors(true),
    m_assertErrors(false)
  {
  }

  virtual void LogError(const std::string &log)
  {
    UE_LOG(FramesLog, Error, TEXT("%s"), *FString(log.c_str()));  // This could probably be a *lot* more efficient

    if (m_onScreenErrors)
    {
      GEngine->AddOnScreenDebugMessage((int32)-1, 2.0f, FLinearColor(0.f,1.f,1.f).ToFColor(true), log.c_str());  // It's possible something like this should be introduced for debug mode; worry about this!
    }
    
    if (m_assertErrors)
    {
      checkf(false && "Frames internal error", TEXT("%s"), *FString(log.c_str()));
    }
  }

  virtual void LogDebug(const std::string &log)
  {
    UE_LOG(FramesLog, Log, TEXT("%s"), *FString(log.c_str()));
  }

private:
  bool m_onScreenErrors;
  bool m_assertErrors;
};

UFramesEnvironment::UFramesEnvironment(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
  Frames::Configuration::Local conf;
  conf.RendererSet(Frames::Configuration::RendererNull());
  conf.LoggerSet(Frames::Configuration::LoggerPtr(new FramesUE4Logger()));
  m_env = Frames::Environment::Create(conf);
}

void UFramesEnvironment::Render(AHUD *hud)
{
  m_env->ResizeRoot(hud->Canvas->SizeX, hud->Canvas->SizeY);
  m_env->Render();
}
