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
#include <weasel/Basic/FileManager.h>
#include <weasel/Driver/Driver.h>
#include <weasel/Analysis/AnalysisSemantic.h>
#include <weasel/IR/Module.h>
#include <weasel/Basic/String.h>
#include <weasel-c/BuildSystem.h>

#include <glog/logging.h>

#include <filesystem>

namespace fs = std::__fs::filesystem;
using namespace std;

int main(int argc, char *argv[])
{
    // Init Loging
    google::InitGoogleLogging(argv[0]);

    if (argc <= 1)
    {
        std::cerr << "Not Input files\n";
        return 1;
    }

    // auto outputExecutable = "temp/main";
    auto filePath = std::string(argv[1]);
    auto filename = splitString(filePath, "/").back();
    // auto outputPath = (std::string)fs::temp_directory_path() + filename + ".o";
    auto outputPath = filePath + ".o";
    auto outputExecutable = filePath + ".out";
    auto fileManager = weasel::FileManager(filePath);
    if (!fileManager.isValid())
    {
        std::cerr << filePath << " Not exist\n";
        return 0;
    }

    // Prepare Lexer and Parser
    LOG(INFO) << "Initializing Parser...\n";
    auto weaselModule = weasel::Module();
    auto lexer = weasel::Lexer(fileManager);
    auto parser = weasel::Parser(lexer, &weaselModule);

    // Parse into AST
    LOG(INFO) << "Parsing...\n";
    parser.parse();

    // Analysis Semantic
    LOG(INFO) << "Semantic Analysis...\n";
    auto analysis = weasel::AnalysisSemantic(&weaselModule);
    analysis.semanticCheck();

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

        return 0;
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

        return 0;
    }

    // Debugging AST
    LOG(INFO) << "Write Weasel AST " << filename << "...\n";
    weasel::Printer(filePath + ".ir").print(&weaselModule);

    // Initialize LLVM
    // llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    // Prepare for codegen
    auto llvmContext = llvm::LLVMContext();
    auto codegen = weasel::WeaselCodegen(&llvmContext, "CoreModule");
    auto driver = weasel::Driver(&codegen, &weaselModule);

    LOG(INFO) << "Compiling...\n";
    auto isCompileSuccess = driver.compile();
    if (!isCompileSuccess)
    {
        if (!driver.getError().empty())
        {
            std::cerr << "Driver Compile : " << driver.getError() << "\n";
        }
        return 1;
    }

    LOG(INFO) << "Create LLVM IR...\n";
    driver.createIR(outputExecutable);

    LOG(INFO) << "Create Output Objects...\n";
    if (!isCompileSuccess)
    {
        return 1;
    }

    driver.createObject(outputPath);

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
