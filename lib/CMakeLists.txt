include_directories(${CMAKE_SOURCE_DIR}/include)

set(
    SOURCES
    AST/AST.cpp
    AST/ASTCodegen.cpp
    AST/ASTPrinter.cpp
    AST/ASTSemantic.cpp
    AST/ASTError.cpp
    AST/ASTEvaluate.cpp
    Analysis/AnalysisSemantic.cpp
    Basic/FileManager.cpp
    Basic/Number.cpp
    Driver/Driver.cpp
    IR/Codegen.cpp
    IR/CodegenLiteral.cpp
    IR/CodegenMeta.cpp
    IR/CodegenOperator.cpp
    IR/CodegenStatement.cpp
    IR/CodegenType.cpp
    Lexer/Lexer.cpp
    Lexer/LexerKeyword.cpp
    Lexer/LexerLiteral.cpp
    Lexer/LexerPunctuation.cpp
    Lexer/LexerType.cpp
    Lexer/Token.cpp
    Metadata/Metadata.cpp
    Parser/Parser.cpp
    Parser/ParserExpression.cpp
    Parser/ParserFunction.cpp
    Parser/ParserHelper.cpp
    Parser/ParserStatement.cpp
    Passes/Passes.cpp
    Printer/Printer.cpp
    Type/Type.cpp
    Type/TypeCodegen.cpp
    Type/TypeError.cpp
    Type/TypeSemantic.cpp
)

add_library(WeaselLib STATIC ${SOURCES})

target_link_libraries(WeaselLib glog::glog)
