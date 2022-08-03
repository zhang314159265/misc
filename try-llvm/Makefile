# export PATH := /Users/shunting/Documents/lib-git/llvm-project/build/bin:$(PATH)
# Opt build is so much faster... (37 s -> 2s)
export PATH := /Users/shunting/Documents/lib-git/llvm-project/build-opt/bin:$(PATH)

CFLAGS := -I/Applications/Xcode_13.4.0_fb.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/ -I/Applications/Xcode_13.4.0_fb.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1

LDFLAGS := -Wl,-syslibroot /Applications/Xcode_13.4.0_fb.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -L/usr/local/lib

LLVM_API_FLAGS := `llvm-config --cxxflags --ldflags --system-libs --libs core`

copied_toy:
	clang++ -g -O3 copied_toy.cc $(LLVM_API_FLAGS) -o copied_toy $(CFLAGS) $(LDFLAGS)

array:
	clang++ -S -emit-llvm array.cc -o out/array.ll

# gen ir and run
genir_%: 
	mkdir -p out
	clang -S -emit-llvm $*.c -o out/$*.ll
	clang out/$*.ll -o out/a.out
	out/a.out

# run the clang generated IR
run_%:
	clang out/$*.ll -o out/a.out
	out/a.out

manual_%: manual-ir/%.ll
	clang $^ -o out/a.out
	out/a.out

irapi_%: irapi/%.cpp
	clang++ $^ -o out/a.out $(CFLAGS) $(LDFLAGS) $(LLVM_API_FLAGS)
	rm -f /tmp/gen.ll
	out/a.out
	cat /tmp/gen.ll
	clang /tmp/gen.ll -o out/generated
	out/generated
