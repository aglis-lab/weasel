#include <sstream>

#include <llvm/Support/TargetSelect.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/LegacyPassManager.h>

#include <weasel/AST/AST.h>
#include <weasel/Printer/Printer.h>
#include <weasel/Parser/Parser.h>
#include <weasel/IR/Codegen.h>
#include <weasel/Symbol/Symbol.h>
#include <weasel/Basic/FileManager.h>
#include <weasel/Driver/Driver.h>
#include <weasel/Analysis/AnalysisSemantic.h>
#include <weasel/IR/Module.h>
#include <weasel/Basic/String.h>
#include <weasel-c/BuildSystem.h>

#include <glog/logging.h>

#include <filesystem>

namespace fs = std::__fs::filesystem;

int main(int argc, char *argv[])
{
    // Init Loging
    google::InitGoogleLogging(argv[0]);

    if (argc <= 1)
    {
        std::cerr << "Not Input files\n";
        return 1;
    }

    auto outputExecutable = "temp/main";
    auto filePath = argv[1];
    auto filename = splitString(filePath, "/").back();
    auto outputPath = (std::string)fs::temp_directory_path() + filename + ".o";
    auto fileManager = weasel::FileManager(filePath);
    if (!fileManager.isValid())
    {
        std::cerr << filePath << " Not exist\n";
        return 0;
    }

    // Prepare Lexer and Parser
    LOG(INFO) << "Initializing Parser...\n";
    auto weaselModule = weasel::Module();
    auto lexer = weasel::Lexer(&fileManager);
    auto parser = weasel::Parser(&lexer, &weaselModule);

    // Parse into AST
    LOG(INFO) << "Parsing...\n";
    parser.parse();

    // Debugging AST
    // LOG(INFO) << "Debug AST...\n\n";
    // weasel::Printer().print(&weaselModule);

    // Initialize LLVM
    // llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    // Prepare for codegen
    auto llvmContext = new llvm::LLVMContext();
    auto codegen = weasel::WeaselCodegen(llvmContext, "CoreModule");
    auto driver = weasel::Driver(&codegen, &parser);
    // auto analysis = weasel::AnalysisSemantic(&parser);

    LOG(INFO) << "Compiling...\n";
    auto isCompileSuccess = driver.compile();
    if (!isCompileSuccess)
    {
        if (!driver.getError().empty())
        {
            std::cerr << "Driver Compile : " << driver.getError() << "\n";
        }
    }

    // LOG(INFO) << "Semantic Analysis...\n";
    // analysis.semanticCheck();
    // analysis.typeChecking();

    LOG(INFO) << "Create LLVM IR...\n";
    driver.createIR(outputExecutable);
    if (!weasel::ErrorTable::getErrors().empty())
    {
        std::cerr << "\n=> Error Information\n";
        weasel::ErrorTable::showErrors();

        return 0;
    }

    LOG(INFO) << "Create Output Objects...\n";
    if (isCompileSuccess)
    {
        driver.createObject(outputPath);
    }

    LOG(INFO) << "Create Executable File...\n";
    weasel::BuildSystem buildSystem({outputPath});
    buildSystem.addBuildArgument({"o", outputExecutable});
    auto result = buildSystem.exec();
    if (!result.empty())
    {
        std::cerr << result << std::endl;
    }
    else
    {
        buildSystem.runExecutable();
    }
}
