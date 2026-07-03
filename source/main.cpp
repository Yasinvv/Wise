#include <assert.h>
#include <cstdlib>
#include <iostream>

import engine;
import luaConfigs;

int main() {
  try {
    APP app;
    WisE::loadLuaConfigs(app.luaConfigs);
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
