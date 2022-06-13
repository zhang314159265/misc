CHROMIUM_SOURCE_ROOT := /Users/shunting/Documents/system-design/chromium/src
CHROMIUM_BUILD_ROOT := $(CHROMIUM_SOURCE_ROOT)/out/Default
SKIA_SOURCE_ROOT := $(CHROMIUM_SOURCE_ROOT)/third_party/skia

CPP_FLAGS := -I $(SKIA_SOURCE_ROOT)/include/core -I $(SKIA_SOURCE_ROOT)/include/docs -I $(SKIA_SOURCE_ROOT)
CFLAGS := -std=c++17
LD_FLAGS := -L $(CHROMIUM_BUILD_ROOT) -lskia -lgflags -lfmt

all:
	g++ main.cc $(CPP_FLAGS) $(CFLAGS) $(LD_FLAGS)
	DYLD_LIBRARY_PATH=$(CHROMIUM_BUILD_ROOT) ./a.out --draw_func=draw_geo_and_text
