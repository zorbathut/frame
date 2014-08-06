--[[Copyright 2014 Mandible Games
    
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
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. ]]

local function msbuild(version)
  return function (target, platform, configuration)
    local platlookup = {
      x32 = "Win32",
      x64 = "x64",
    }
    local conflookup = {
      release = "Release",
      debug = "Debug",
    }
    
    -- vs2008 doesn't seem to like multiple environment variables that are identical when compared case-insensitively
    -- because we're lazy, we strip them for all compilers
    return {cli = string.format([[tmp= temp= cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio %s/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=%s /p:platform=%s]],
      version, conflookup[configuration], platlookup[platform]),
      verify = {string.format("lib/%s/%s/%s", target, platform, configuration == "release" and "frames.lib" or "framesd.lib")}
    }
  end
end

local function uebuild(version)
  return function (target, platform, configuration)
    local msb = msbuild("12.0")(target, platform, configuration)
    
    -- not yet included - Development, DebugGame, Shipping
    -- For some reason these don't include the plugin and it is not yet clear why, given that it doesn't run at all. Fix this once it's running. :V
    if platform == "x64" and configuration == "release" then
      msb.cli = msb.cli .. string.format([[ "&&" "C:\Program Files\Unreal Engine\%s\Engine\Build\BatchFiles\Build.bat" plugin_ue4Editor Win64 Development %%CD%%/../../ue4/plugin_ue4.uproject -rocket]], version)
      table.insert(msb.verify, "ue4/Plugins/Frames/Binaries/Win64/UE4Editor-Frames.dll")
    elseif platform == "x64" and configuration == "debug" then
      msb.cli = msb.cli .. string.format([[ "&&" "C:\Program Files\Unreal Engine\%s\Engine\Build\BatchFiles\Build.bat" plugin_ue4Editor Win64 DebugGame %%CD%%/../../ue4/plugin_ue4.uproject -rocket]], version)
      table.insert(msb.verify, "ue4/Plugins/Frames/Binaries/Win64/UE4Editor-Frames-Win64-DebugGame.dll")
    elseif platform == "x32" and configuration == "release" then
      -- shipping will be here
    end
    
    return msb
  end
end

projects = {
  msvc9 = {
    generator = "vs2008",
    build = msbuild("9.0"),
  },
  msvc10 = {
    generator = "vs2010",
    build = msbuild("10.0"),
  },
  msvc11 = {
    generator = "vs2012",
    build = msbuild("11.0"),
  },
  msvc12 = {
    generator = "vs2013",
    build = msbuild("12.0"),
  },
  -- disabled, likely permanently
  --[[ue4_2 = {
    generator = "vs2013",
    parameters = "--ue=4_2",
    build = uebuild("4.2"),
  },]]
  ue4_3 = {
    generator = "vs2013",
    parameters = "--ue=4_3",
    build = uebuild("4.3")
  },
  -- disabled, possibly permanently
  --[[mingw = {
    generator = "gmake",
    --path = "/cygdrive/c/mingw/bin",
    build = "PATH=/cygdrive/c/mingw/bin mingw32-make -j12 config=debug && PATH=/cygdrive/c/mingw/bin mingw32-make -j12 config=release && cp `which pthreadGC2.dll` ../../bin/mingw/test",
  },]]
}