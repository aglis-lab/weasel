#include <string>
#include <vector>

#define DEFAULT_LINKER "ld64.lld"
#define LLVM_PATH "/opt/homebrew/Cellar/llvm@15/15.0.7"

// Invoke ld.lld (Unix), ld64.lld (macOS), lld-link (Windows), wasm-ld (WebAssembly) instead

// ld -lto_library /opt/homebrew/Cellar/llvm@13/13.0.1_2/lib/libLTO.dylib -no_deduplicate -dynamic -arch arm64 -platform_version macos 14.0.0 14.0.0 -syslibroot /Library/Developer/CommandLineTools/SDKs/MacOSX13.sdk -o temp/a.out temp/main.o -lSystem /opt/homebrew/Cellar/llvm@13/13.0.1_2/lib/clang/13.0.1/lib/darwin/libclang_rt.osx.a
// ld -no_deduplicate -dynamic -arch arm64 -platform_version macos 14.0.0 14.0.0 -syslibroot /Library/Developer/CommandLineTools/SDKs/MacOSX13.sdk -o temp/a.out temp/main.o -lSystem /opt/homebrew/Cellar/llvm@13/13.0.1_2/lib/clang/13.0.1/lib/darwin/libclang_rt.osx.a
// ld -syslibroot /Library/Developer/CommandLineTools/SDKs/MacOSX13.sdk -o temp/a.out temp/main.o -lSystem /opt/homebrew/Cellar/llvm@13/13.0.1_2/lib/clang/13.0.1/lib/darwin/libclang_rt.osx.a

// Link Time Optimization from LLVM
// -lto_library /opt/homebrew/Cellar/llvm@13/13.0.1_2/lib/libLTO.dylib

// Metadata?
// -no_deduplicate -dynamic -arch arm64 -platform_version macos 14.0.0 14.0.0

using namespace std;

namespace weasel
{
    struct BuildArgument
    {
        string key;
        string value;
    };

    class BuildSystem
    {
    private:
        vector<string> _files;
        string _linker;
        vector<BuildArgument> _buildArguments;

    private:
        string buildCommand();

    public:
        BuildSystem(vector<string> objFiles);
        ~BuildSystem();

        int runExecutable();
        void addBuildArgument(BuildArgument arg);
        string exec();
    };

    BuildSystem::BuildSystem(vector<string> objFiles)
    {
        _files = objFiles;
        _linker = string(LLVM_PATH) + "/bin/" + DEFAULT_LINKER;

        // TODO: Need to make it automatic
        // Default Build Arguments
        _buildArguments.push_back({"syslibroot", "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk"});
        _buildArguments.push_back({"lSystem", string(LLVM_PATH) + "/lib/clang/15.0.7/lib/darwin/libclang_rt.osx.a"});
        _buildArguments.push_back({"arch", "arm64"});
        _buildArguments.push_back({"platform_version", "macos 14.0.0 14.0.0"});
        _buildArguments.push_back({"no_deduplicate", ""});
    }

    BuildSystem::~BuildSystem() {}

    int BuildSystem::runExecutable()
    {
        for (auto item : this->_buildArguments)
        {
            if (item.key == "o")
            {
                return system(item.value.c_str());
            }
        }

        return -1;
    }

    void BuildSystem::addBuildArgument(BuildArgument arg)
    {
        this->_buildArguments.push_back(arg);
    }

    string BuildSystem::buildCommand()
    {
        string cmd = this->_linker;

        for (auto item : this->_buildArguments)
        {
            cmd += " -" + item.key + " " + item.value;
        }

        for (auto item : this->_files)
        {
            cmd += " " + item;
        }

        return cmd;
    }

    string BuildSystem::exec()
    {
        auto cmd = this->buildCommand();

        array<char, 128> buffer;
        string result;
        unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe)
        {
            throw runtime_error("popen() failed!");
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }

        return result;
    }
} // namespace weasel
