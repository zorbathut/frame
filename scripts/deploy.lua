#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/util"

local version = io.open("version", "rb"):read("*line")

os.execute("rm -rf Den* scripts version TODO")

os.execute(("mkdir frames-%s && mv * frames-%s"):format(version, version))

-- I never used the "binary deploy" option, so I'm just pulling it
--[[
-- compress the whole shebang with binaries
os.execute(("zip -r -9 -q frames-%s-bin.zip frames-%s"):format(version, version))]]

-- remove binary output - we'll make this a little more careful if we later have deployable executables
os.execute(("rm -rf frames-%s/bin"):format(version))

-- create the final deployable
os.execute(("zip -r -9 -q frames-%s.zip frames-%s"):format(version, version))
