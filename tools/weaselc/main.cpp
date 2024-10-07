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
#include <weasel/Driver/Driver.h>
#include <weasel/IR/Codegen.h>
#include <weasel/IR/Module.h>
#include <weasel/Parser/Parser.h>
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

    // TODO: New Convention of File Manager
    // // Deallocate all Source from File Manager
    // defer { FileManager::GetInstance().Close(); };

    // // FileManager
    // auto sourcePath = std::string(argv[1]);
    // FileManager::GetInstance().LoadSource(sourcePath);

    // if (FileManager::GetInstance().GetSourcesCount() == 0)
    // {
    //     cerr << "souces file not found\n";
    //     return -1;
    // }

    // // Multi Threading for parsing
    // vector<thread> threads;
    // auto package = Package();
    // for (auto fileId : FileManager::GetInstance().GetFileIds())
    // {
    //     // Module
    //     auto newModule = make_shared<Module>();
    //     package.addModule(newModule);

    //     // Parser and Lexer
    //     auto parser = Parser(newModule, fileId);

    //     threads.push_back(thread(&Parser::parse, parser));
    // }

    // // Wait for parsing finish
    // for (auto &item : threads)
    // {
    //     item.join();
    // }

    auto filePath = std::string(argv[1]);
    auto filename = splitString(filePath, "/").back();
    auto outputPath = filePath + ".o";
    auto outputExecutable = filePath + ".out";

    auto sourceManager = SourceManager();
    defer { sourceManager.closeSource(); };

    auto souceBuffer = SourceBuffer(filePath);
    assert(souceBuffer.isValid());

    // Prepare Lexer and Parser
    LOG(INFO) << "Parsing...";
    auto weaselModule = Module();
    auto parser = Parser(souceBuffer, &weaselModule);

    parser.parse();

    // // Analysis Semantic
    // LOG(INFO) << "Semantic Analysis...";
    // auto analysis = AnalysisSemantic(&weaselModule);
    // analysis.semanticCheck();

    // // TODO: Change with better Error Print
    // if (!analysis.getTypeErrors().empty())
    // {
    //     for (auto item : analysis.getTypeErrors())
    //     {
    //         LOG(ERROR)
    //             << item->getError()->getMessage()
    //             << " but got '" << item->getError()->getToken().getEscapeValue() << "'"
    //             << " type of " << item->getError()->getToken().getTokenKindToInt()
    //             << " " << item->getError()->getToken().getLocation().toString()
    //             << " from " << item->getToken().getValue();
    //     }

    //     return 0;
    // }

    // if (!analysis.getErrors().empty())
    // {
    //     for (auto item : analysis.getErrors())
    //     {
    //         LOG(ERROR)
    //             << item->getError()->getMessage()
    //             << " but got '" << item->getError()->getToken().getEscapeValue() << "'"
    //             << " type of " << item->getError()->getToken().getTokenKindToInt()
    //             << " " << item->getError()->getToken().getLocation().toString()
    //             << " from " << item->getToken().getValue();
    //     }

    //     return 0;
    // }

    // // Debugging AST
    // LOG(INFO) << "Write Weasel AST " << filename << "...";
    // Printer(filePath + ".ir").print(&weaselModule);

    // if (justEmitIr())
    // {
    //     return 0;
    // }

    // // Initialize LLVM
    // // llvm::InitializeAllTargetInfos();
    // llvm::InitializeNativeTarget();
    // llvm::InitializeNativeTargetAsmParser();
    // llvm::InitializeNativeTargetAsmPrinter();

    // // Prepare for codegen
    // auto llvmContext = llvm::LLVMContext();
    // auto codegen = Codegen(&llvmContext, "CoreModule");
    // auto driver = Driver(&codegen, &weaselModule);

    // LOG(INFO) << "Compiling...";
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
