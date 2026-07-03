module;

#include <cstdint>
#include <sol/sol.hpp>

export module luaConfigs;
namespace WisE {

export struct luaConfigs {
  uint8_t MaxFPS{30U};
};

export void loadLuaConfigs(luaConfigs& luaConfigs) {
  sol::state lua;
  lua.open_libraries(sol::lib::base);
  lua.script_file("lua/config.lua");
  luaConfigs.MaxFPS = lua["SetMaxFPS"].get<uint8_t>();
}
} // namespace WisE
