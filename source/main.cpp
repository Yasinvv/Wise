
/*
 * Part of WisE
 * Copyright 2026 Yasinvv
 * Licensed under the Apache License, Version 2.0 (the "License");
 * See /LICENSE for details
 */

#include <assert.h>
#include <cstdlib>
#include <iostream>

import engine;
import luaConfigs;

int main() {
  try {
    APP app;
    WisE::loadLuaConfigs(app.configs);
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
