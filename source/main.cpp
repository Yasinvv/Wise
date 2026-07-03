#include <assert.h>
#include <cstdlib>
#include <iostream>

import renderer;
import engine;

int main() {
  loadLuaConfigs();
  try {
    APP app;
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
