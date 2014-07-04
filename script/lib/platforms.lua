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

local function msbuild(core)
  return function (platform, configuration)
    local platlookup = {
      x32 = "Win32",
      x64 = "x64",
    }
    local conflookup = {
      release = "Release",
      debug = "Debug",
    }
    return core:gsub("<configuration>", conflookup[configuration]):gsub("<platform>", platlookup[platform])
  end
end

projects = {
  msvc9 = {
    generator = "vs2008",
    build = msbuild([[tmp= temp= cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=<configuration> /p:platform=<platform>]]),  -- vs2008 doesn't seem to like multiple environment variables that are identical when compared case-insensitively
  },
  msvc10 = {
    generator = "vs2010",
    build = msbuild([[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=<configuration> /p:platform=<platform>]]),
  },
  msvc11 = {
    generator = "vs2012",
    build = msbuild([[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 11.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=<configuration> /p:platform=<platform>]]),
  },
  msvc12 = {
    generator = "vs2013",
    build = msbuild([[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 12.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=<configuration> /p:platform=<platform>]]),
  },
  ue4_2 = {
    generator = "vs2013",
    parameters = "--ue=4_2",
    build = msbuild([[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 12.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=<configuration> /p:platform=<platform>]]),
  },
  -- disabled, possibly permanently
  --[[mingw = {
    generator = "gmake",
    --path = "/cygdrive/c/mingw/bin",
    build = "PATH=/cygdrive/c/mingw/bin mingw32-make -j12 config=debug && PATH=/cygdrive/c/mingw/bin mingw32-make -j12 config=release && cp `which pthreadGC2.dll` ../../bin/mingw/test",
  },]]
}