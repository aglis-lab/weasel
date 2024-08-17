##### Build and Run #####
rm.build:
	rm -rf build

preset:
	cmake --preset=default

preset.release: rm.build
	cmake --preset=default -DCMAKE_BUILD_TYPE=Release

build.cmake:
	cmake --build build -j 8

run:
	GLOG_logtostderr=1 ./build/tools/weaselc/weaselc temp/test.we

run.leaks:
	leaks --atExit -- ./build/tools/weaselc/weaselc temp/test.we

dev.leaks: build.cmake run.leaks

dev: build.cmake run

dev.ir: build.cmake
	emit_ir=true GLOG_logtostderr=1 ./build/tools/weaselc/weaselc temp/test.we

##### SPIRV #####
spirv:
	./config/spirv.sh

spirv.cross:
	./config/spirv-cross.sh

spirv.clspv:
	./config/spirv-clspv.sh

##### TEST #####
test.cross-test:
	./build/test/cross-test/cross-test

test.metal-test:
	./build/test/metal-test/metal-test

test.vulkan-test:
	./build/test/vulkan-test/vulkan-test

test.parallel-test:
	./build/test/parallel-test/parallel-test

test.spirv-test:
	./build/test/spirv-test/spirv-test

test.glsl-test:
	./build/test/glsl-test/glsl-test temp/test.we

##### File Count #####
count:
	python3 file_count.py

count.github:
	curl -L https://api.codetabs.com/v1/loc?github=aglis-lab/weasel
