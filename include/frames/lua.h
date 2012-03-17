// Lua binding utilities

#ifndef FRAMES_LUA
#define FRAMES_LUA

#include <lua.hpp>
#include <lauxlib.h>

namespace Frames {
  template <typename T> T *l_checkframe_fromregistry(lua_State *L, int index, int registry) {
    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushvalue(L, index);
    lua_rawget(L, registry);

    if (lua_isnil(L, -1)) {
      luaL_error(L, "Failed to find frame of type %s for parameter %d", T::GetStaticType(), index);
    }

    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    T *result = (T*)lua_touserdata(L, -1);

    lua_pop(L, 1);

    return result;
  }

  template <typename T> T *l_checkframe(lua_State *L, int index) {
    if (index < 0) index += lua_gettop(L) + 1;

    // TODO: remove this doublecheck in optimized mode
    luaL_checktype(L, lua_upvalueindex(1), LUA_TTABLE);
    lua_getfield(L, lua_upvalueindex(1), "id");
    const char *res = luaL_checklstring(L, -1, NULL);
    if (strcmp(res, T::GetStaticType()) != 0) {
      luaL_error(L, "Internal error, mismatch in expected registry table %s against actual table %s", T::GetStaticType(), res);
    }
    lua_pop(L, 1);

    return l_checkframe_fromregistry<T>(L, index, lua_upvalueindex(1));
  }

  template <typename T> T *l_checkframe_external(lua_State *L, int index) {
    if (index < 0) index += lua_gettop(L) + 1;

    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rg");
    lua_getfield(L, -1, T::GetStaticType());

    T *result = l_checkframe_fromregistry<T>(L, index, lua_gettop(L));

    lua_pop(L, 2); // get rid of the lookup table and friend

    return result;
  }

  inline void l_checkparams(lua_State *L, int mn, int mx = -1) {
    if (mx == -1) mx = mn;
    int lgt = lua_gettop(L);
    if (lgt < mn || lgt > mx) {
      luaL_error(L, "Incorrect number of parameters %d (expecting between %d and %d)", lgt, mn, mx);
    }
  }
}

#endif