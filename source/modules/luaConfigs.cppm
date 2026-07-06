module;

#include <cstdint>
#include <sol/sol.hpp>

export module luaConfigs;
import context;

namespace WisE {

export void loadLuaConfigs(Configs& configs) {
  sol::state lua;
  lua.open_libraries(sol::lib::base);
  lua.script_file("lua/config.lua");
  configs.MaxFPS = lua["SetMaxFPS"].get<uint8_t>();
  configs.WIDTH = lua["WIDTH"].get<uint16_t>();
  configs.HEIGHT = lua["HEIGHT"].get<uint16_t>();
}
} // namespace WisE
