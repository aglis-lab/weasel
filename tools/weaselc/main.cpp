#include <sstream>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/LegacyPassManager.h>

#include <weasel/Parser/Parser.h>
#include <weasel/IR/Context.h>
#include <weasel/AST/AST.h>
#include <weasel/Symbol/Symbol.h>
#include <weasel/Basic/FileManager.h>
#include <weasel/Codegen/Codegen.h>
#include <weasel/Analysis/AnalysisSemantic.h>

#include <weasel-c/debug.h>

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

    // Initialize LLVM TO BULK
    LOG(INFO) << "Initializing...\n";
    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    // Prepare Lexer and Parser
    auto lexer = weasel::Lexer(&fileManager);
    auto parser = weasel::Parser(&lexer);

    // Parse into AST
    LOG(INFO) << "Parsing...\n";
    parser.parse();

    // Debugging AST
    LOG(INFO) << "Debug AST...\n";
    debug(parser.getFunctions());

    // Prepare for codegen
    auto llvmContext = llvm::LLVMContext();
    auto context = weasel::Context(&llvmContext, "codeModule");
    auto codegen = weasel::Codegen(&context, &parser);
    // auto analysis = weasel::AnalysisSemantic(&parser);

    LOG(INFO) << "Compiling...\n";
    auto isCompileSuccess = codegen.compile();
    if (!isCompileSuccess)
    {
        if (!codegen.getError().empty())
        {
            std::cerr << "Codegen Compile : " << codegen.getError() << "\n";
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
