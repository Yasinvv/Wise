VULKAN_SDK=/home/yasinvv/Vulkan/vulkansdk-linux-x86_64-1.4.321.1/1.4.321.1/x86_64

SDLTest: main.cpp
	g++ -std=c++23 -o prog main.cpp \
	    -I/usr/local/include -I$(VULKAN_SDK)/include \
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
	shader.slang -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -entry fragMain -o slang.spv
