#include <sstream>
#include <filesystem>

#include <llvm/Support/TargetSelect.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/LegacyPassManager.h>

#include <weasel/Analysis/AnalysisSemantic.h>
#include <weasel/AST/AST.h>
#include <weasel/Basic/Defer.h>
#include <weasel/Basic/String.h>
#include <weasel/IR/Driver.h>
#include <weasel/IR/Codegen.h>
#include <weasel/Package/Package.h>
#include <weasel/Package/PackageManager.h>
#include <weasel/Parser/Parser.h>
#include <weasel/Parser/ParserPool.h>
#include <weasel/Printer/Printer.h>
#include <weasel/Source/SourceManager.h>
#include <weasel-c/BuildSystem.h>

#include <glog/logging.h>

namespace fs = std::__fs::filesystem;

using namespace std;

bool justEmitIr()
{
    auto val = getenv("emit_ir");
    if (val == NULL)
    {
        return false;
    }

    return string_view(val) == "true";
}

int main(int argc, char *argv[])
{
    // Init Loging
    google::InitGoogleLogging(argv[0]);

    if (argc <= 1)
    {
        std::cerr << "Not Input files";
        return 1;
    }

    auto sourceManager = SourceManager();
    defer { sourceManager.closeSource(); };

    auto sourcePath = string(argv[1]);
    auto package = PackageManager::getInstance().createPackageApp();
    auto parserPool = ParserPool();

    sourceManager.loadSource(sourcePath);
    for (auto item : sourceManager.getSources())
    {
        // Prepare Lexer and Parser
        LOG(INFO) << "Parsing";

        parserPool.createPool(item, package->createModule());
    }

    // Wait Thread Finish
    parserPool.wait();

    // Load Package Header
    package->loadPackageHeader();

    // Analysis Semantic
    LOG(INFO) << "Semantic Analysis...";
    auto analysis = AnalysisSemantic(package);
    analysis.semanticCheck();

    // TODO: Change with better Error Print
    if (!analysis.getTypeErrors().empty())
    {
        for (auto item : analysis.getTypeErrors())
        {
            LOG(ERROR)
                << item->getError()->getMessage()
                << " but got '" << item->getError()->getToken().getEscapeValue() << "'"
                << " type of " << item->getError()->getToken().getTokenKindToInt()
                << " " << item->getError()->getToken().getLocation().toString()
                << " from " << item->getToken().getValue();
        }
        return 1;
    }

    if (!analysis.getErrors().empty())
    {
        for (auto item : analysis.getErrors())
        {
            LOG(ERROR)
                << item->getError()->getMessage()
                << " but got '" << item->getError()->getToken().getEscapeValue() << "'"
                << " type of " << item->getError()->getToken().getTokenKindToInt()
                << " " << item->getError()->getToken().getLocation().toString()
                << " from " << item->getToken().getValue();
        }
        return 1;
    }

    // Debugging AST
    LOG(INFO) << "Write Weasel AST";
    auto sourceBuild = fmt::format("{}/_build", sourcePath);
    if (!fs::exists(fs::path(sourceBuild)))
    {
        if (!fs::create_directory(fs::path(sourceBuild)))
        {
            cerr << "error create directory\n";
            exit(-1);
        }
    }

    auto printer = Printer(fmt::format("{}/{}.ir", sourceBuild, "_source"));
    printer.print(package);

    // Check only emit ir
    if (justEmitIr())
    {
        return 0;
    }

    const auto processorCount = thread::hardware_concurrency();

    // Every File Will Individual FileAST, Codegen, and Module
    fmt::println("~~ Header Package ~~");
    fmt::println("Global Variables {}", package->getGlobalVariables().size());
    fmt::println("User Types {}", package->getUserTypes().size());
    fmt::println("Functions {}", package->getFunctions().size());
    fmt::println("");
    fmt::println("FileAST {}", sizeof(FileAST));
    fmt::println("CODEGEN {}", sizeof(Codegen));
    fmt::println("Driver {}", sizeof(Driver));
    fmt::println("PackageHandle {}", sizeof(PackageHandle));
    fmt::println("PackageList {}", sizeof(PackageList));
    fmt::println("PackageManager {}", sizeof(PackageManager));
    fmt::println("LLVMContext {}", sizeof(llvm::LLVMContext));
    fmt::println("Module {}", sizeof(llvm::Module));
    fmt::println("MDBuilder {}", sizeof(llvm::MDBuilder));
    fmt::println("IRBuilder {}", sizeof(llvm::IRBuilder<>));
    fmt::println("Processor Count {}", processorCount);

    // // Initialize LLVM
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    // // Prepare for codegen
    // auto llvmContext = llvm::LLVMContext();
    // auto codegen = Codegen(&llvmContext, "CoreModule");
    // auto driver = Driver(&codegen, &package);

    // LOG(INFO) << "Compiling";
    // auto isCompileSuccess = driver.compile();
    // if (!isCompileSuccess)
    // {
    //     if (!driver.getError().empty())
    //     {
    //         std::cerr << "\nDriver Compile : " << driver.getError() << "\n";
    //     }
    //     return 1;
    // }

    // LOG(INFO) << "Create LLVM IR...";
    // driver.createIR(outputExecutable);

    // LOG(INFO) << "Create Output Objects...";
    // if (!isCompileSuccess)
    // {
    //     return 1;
    // }

    // driver.createObject(outputPath);

    // LOG(INFO) << "Create Executable File...";
    // BuildSystem buildSystem({outputPath});
    // buildSystem.addBuildArgument({"o", outputExecutable});
    // auto result = buildSystem.exec();
    // if (!result.empty())
    // {
    //     std::cerr << result << std::endl;
    // }
    // else
    // {
    //     buildSystem.runExecutable();
    // }
}
