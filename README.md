# Weasel Language

Weasel Language named after name of animal https://en.wikipedia.org/wiki/Weasel

### Run debug with GLOG Logs

GLOG_logtostderr=1 build/tools/weaselc/weaselc temp/main.we

### Debug

llc temp/main.ir -filetype=obj && clang++ temp/main.ir.o -o temp/main.out && ./temp/main.out

# BACKGROUND

Weasel language is a project i created to prove of concept that we can support heterogenous internally inside a language. We can use GPGPU Computing directly supported by the language. Which mean you just directly code to the language and just works.

And we can create computationally task easyly solved. Like Machine Learning and even AI, because you can switch which program or code need to run on GPU or CPU or any compute hardware without hassle.

We also can binding the library created by weasel language into another languages like python to create better environment for end user to create fast program.

# Flow Code

- FileManager -> Lexer -> Parser <- Module
- Module -> Codegen(LLVM Module) -> Driver
- Module -> Printer

### Module

Module is a parser manager to make parsing data can be parallel

###

# Third-Party

- fmt (install on system)
- glog (install on system)

# SETUP (see LLVM Documentation)

- Add LLVM Project
- Before build change linker to llvm lld then Build LLVM Project
- Set LLVM DIR
- Build Project

## Installing Weasel Language

### Linux Version

- Untuk pengguna linux anda dapat menggunakannya pre-release version di github https://github.com/zy0709/underrated_project/releases. Ikuti instruksinya dan langsung gunakan atau ikuti instruksi untuk menginstall dari source code.

### Windows Version

- No binary release for windows, follow installing from source code.

### Mac OS X

- no binary release for mac os x, follow installing from source code.

## Install from Source Code

### 1. Install Clang

#### Untuk linux

- anda tinggal run
- `sudo apt update`
- `sudo apt install clang-12`

#### Untuk Windows

- Download clang di https://releases.llvm.org/download.html pilih versi 11.0.0
- Extract dan tambahkan ke variable environment

#### Untuk Mac OS X

- Clang merupakan default compiler di Mac OS X, jadi tidak perlu menginstall lagi

### 2. Clone Project

- Tinggal clone project git clone https://github.com/zy0709/underrated_project ke folder project anda

### 3. Set LLVM DIR

#### Untuk Linux

- Tidak perlu diganti

#### Untuk Windows

- Buka file CMakeLists.txt
- Ganti pathnya LLVM_DIR ke {folder clang}/lib/cmake/llvm

#### Untuk Mac OS X

-Set ke folder cmake llvm (not tested, limited hardware resource)

### 4. Set Library LLVM SPIRV

#### Untuk Linux

- Membuat folder third-party
- Download llvm-spirv-11 di https://github.com/zy0709/underrated_project/releases
- Extract lib dan include di file llvm-spirv-11 ke third-party

#### Untuk Windows

- Build dari source code, ikuti petunjuk dari github aslinya di https://github.com/KhronosGroup/SPIRV-LLVM-Translator
- Lalu pindah libLLVMSPIRVLib.a ke folder third-party/lib
- Dan copy semua file dari include ke third-party/include

#### Untuk Mac OS X

- Build dari source code, ikuti petunjuk dari github aslinya di https://github.com/KhronosGroup/SPIRV-LLVM-Translator
- Lalu pindah libLLVMSPIRVLib.a ke folder third-party/lib
- Dan copy semua file dari include ke third-party/include

### 5. Build Source

#### Untuk Linux

- mkdir build
- cd build
- cmake ..
- cmake --build .
- tunggu process selesai dan anda bisa menggunakan file executablenya

#### Untuk Windows

- mkdir build
- cd build
- cmake ..
- cmake --build .
- tunggu process selesai dan anda bisa menggunakan file executablenya

#### Untuk Mac OS X

- mkdir build
- cd build
- cmake ..
- cmake --build .
- tunggu process selesai dan anda bisa menggunakan file executablenya

---

---

- ![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) NOTE : On progress to automate the installation.
- ![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) NOTE : Mail to github project for more information.

---

# FOLDER STRUCTURE

- cmake (Cmake File to avoid boilerplate of cmake file)
- include (folder header)
- lib (Library of the header)
- libparallel (Library that used for creating parallel execution. This is an abstraction of OpenCL FrameWork)
- tools (tools program)
- runtime-rt (Source program that used for testing)
- unittests (program for to be compiled)
