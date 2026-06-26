## HOW TO COMPILE

build :
```zsh
git clone https://github.com/Yasinvv/Wise.git
cd wise
cmake -B build && cmake --build build
```

## DEMO

<p align="center">
  FPS CAP DISABLED
  <br>
  <img width="800" alt="DEMO_Pic2" src="https://github.com/user-attachments/assets/736303b9-0c68-4a7e-bbc0-8c3cb625be2e" />
  <br>
  CUSTOM FPS CAP ENABLED
  <br>
  <img width="800" alt="DEMO_Pic" src="https://github.com/user-attachments/assets/b8401df6-a555-4f32-8036-e17e82540cf6">
  <br>
  GIF
  <br>
  <img width="640" height="360" alt="DEMO" src="https://github.com/user-attachments/assets/dc194d41-8d6b-4f61-b3ff-a0ad58060023">
</p>

## License & Credits

This project is a derivative work based on the concepts and source code and tutorials from [Vulkan Tutorial](https://docs.vulkan.org/tutorial/latest/00_Introduction.html) by Alexander Overvoorde.

### Key Enhancements:
* **Framework Migration:** Completely ported the windowing, event loop, and OS abstraction layer from the original GLFW implementation to **SDL3/SDL**.
* **Architecture:** Refactored the linear tutorial structure into a more modular and reusable codebase.

As a derivative work, The code in this repository is licensed under the **Creative Commons Attribution-ShareAlike 4.0 International License** (CC BY-SA 4.0). 

Copyright (c) 2026 Yasinvv
