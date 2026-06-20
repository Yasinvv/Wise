VULKAN_SDK=/home/yasinvv/Vulkan/vulkansdk-linux-x86_64-1.4.321.1/1.4.321.1/x86_64
IMGUI_SDK=tools/imgui/imgui.cpp \
      tools/imgui/imgui_draw.cpp \
      tools/imgui/imgui_widgets.cpp \
      tools/imgui/imgui_tables.cpp \
      tools/imgui/imgui_demo.cpp \
      tools/imgui/backends/imgui_impl_sdl3.cpp \
      tools/imgui/backends/imgui_impl_vulkan.cpp
IMGUI_INCLUDES = -Itools/imgui -Itools/imgui/backends
SRC = source/main.cpp
CPPFLAGS = -std=c++23 \
					 -DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS
SDLTest: $(SRC)
	g++ $(CPPFLAGS) -o prog $(SRC) $(IMGUI_SDK) \
	    -I/usr/local/include -I$(VULKAN_SDK)/include $(IMGUI_INCLUDES) \
	    -L/usr/local/lib64 -L$(VULKAN_SDK)/lib \
	    -Wl,-rpath,/usr/local/lib64 -Wl,-rpath,$(VULKAN_SDK)/lib -Wl,--enable-new-dtags \
	    -lSDL3 -lvulkan -lSDL3_image

.PHONY: test clean debug shader

test: SDLTest
	./prog

clean:
	rm -f prog

debug:
	g++ -std=c++20 -g -o prog main.cpp \
	    -I/usr/local/include -I$(VULKAN_SDK)/include \
	    -L/usr/local/lib64 -L$(VULKAN_SDK)/lib \
	    -Wl,-rpath,/usr/local/lib64 -Wl,-rpath,$(VULKAN_SDK)/lib -Wl,--enable-new-dtags \
	    -lSDL3 -lvulkan && lldb ./prog

shader:
	slangc data/shaders/shader.slang -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -entry fragMain -o data/shaders/slang.spv
