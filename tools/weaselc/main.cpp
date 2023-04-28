#include <sstream>

#include <llvm/Support/TargetSelect.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/LegacyPassManager.h>

#include <weasel/AST/AST.h>
#include <weasel/Printer/Printer.h>
#include <weasel/Parser/Parser.h>
#include <weasel/IR/Codegen.h>
#include <weasel/AST/AST.h>
#include <weasel/Symbol/Symbol.h>
#include <weasel/Basic/FileManager.h>
#include <weasel/Driver/Driver.h>
#include <weasel/Analysis/AnalysisSemantic.h>

#include <glog/logging.h>

void debug(const std::vector<weasel::Function *> objects);

int main(int argc, char *argv[])
{
    // Init Loging
    // google::InitGoogleLogging(argv[0]);

    if (argc <= 2)
    {
        std::cerr << "Not Input files\n";
        return 1;
    }

    auto filePath = argv[1];
    auto outputPath = argv[2];
    auto fileManager = weasel::FileManager(filePath);
    if (!fileManager.isValid())
    {
        std::cerr << filePath << " Not exist\n";
        return 0;
    }

    // Prepare Lexer and Parser
    LOG(INFO) << "Initializing Parser...\n";
    auto lexer = weasel::Lexer(&fileManager);
    auto parser = weasel::Parser(&lexer);

    // Parse into AST
    LOG(INFO) << "Parsing...\n";
    parser.parse();

    // Debugging AST
    LOG(INFO) << "Debug AST...\n";
    debug(parser.getFunctions());

    // Initialize LLVM
    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    // Only Codegen that have a relationship with the LLVM System
    /*
    Codegen(LLVMContext);
    Driver(Codegen, Module);
    */

    // Prepare for codegen
    auto llvmContext = llvm::LLVMContext();
    auto context = weasel::WeaselCodegen(&llvmContext, "CoreModule");
    auto codegen = weasel::Driver(&context, &parser);
    // auto analysis = weasel::AnalysisSemantic(&parser);

    LOG(INFO) << "Compiling...\n";
    auto isCompileSuccess = codegen.compile();
    if (!isCompileSuccess)
    {
        if (!codegen.getError().empty())
        {
            std::cerr << "Driver Compile : " << codegen.getError() << "\n";
        }
    }

    // LOG(INFO) << "Semantic Analysis...\n";
    // analysis.semanticCheck();
    // analysis.typeChecking();

    LOG(INFO) << "Create LLVM IR...\n";
    codegen.createIR(outputPath);

    if (!weasel::ErrorTable::getErrors().empty())
    {
        std::cerr << "\n=> Error Information\n";
        weasel::ErrorTable::showErrors();

        return 0;
    }

    LOG(INFO) << "Create Output Objects...\n";
    if (isCompileSuccess)
    {
        codegen.createObject(outputPath);
    }
}

void debug(const std::vector<weasel::Function *> objects)
{
    /*

    out = stdout
    for _, item on module->userTypes {
        item->print(out)
    }

    for _, item on module->functions {
        item->print(out)
    }

    */

    std::cerr << std::endl;

    auto printer = weasel::Printer();
    for (auto obj : objects)
    {
        obj->print(&printer);
    }

    std::cerr << std::endl;
}
