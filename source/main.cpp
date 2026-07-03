#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sol/sol.hpp>
#include <stdexcept>
#include <thread>
#include <vector>

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
