#include "weasel/IR/Codegen.h"

weasel::WeaselCodegen::WeaselCodegen(llvm::LLVMContext *context, const std::string &moduleName)
{
    _context = context;
    _module = new llvm::Module(moduleName, *_context);
    _mdBuilder = new llvm::MDBuilder(*_context);
    _builder = new llvm::IRBuilder<>(*_context);
    _metaData = Metadata(context, *_module);
}

// void weasel::WeaselCodegen::traverseAllocaExpression(Expression *expr)
// {
//     LOG(INFO) << "Traversal Alloca Expression";

//     auto declExpr = dynamic_cast<DeclarationStatement *>(expr);
//     if (declExpr)
//     {
//         auto declType = declExpr->getType();
//         auto valueExpr = declExpr->getValue();
//         if (declType == nullptr && valueExpr != nullptr && valueExpr->getType() != nullptr)
//         {
//             declType = valueExpr->getType();
//             declExpr->setType(declType);
//         }

//         auto varName = declExpr->getIdentifier();
//         auto declTypeV = declType->codegen(this);
//         assert(declTypeV != nullptr);

//         auto alloc = this->getBuilder()->CreateAlloca(declTypeV, nullptr);

//         this->setAllocaMap(declExpr, alloc);
//         return;
//     }

//     auto compoundExpr = dynamic_cast<CompoundStatement *>(expr);
//     if (compoundExpr)
//     {
//         for (auto item : compoundExpr->getBody())
//         {
//             traverseAllocaExpression(item);
//         }

//         return;
//     }

//     auto condExpr = dynamic_cast<ConditionStatement *>(expr);
//     if (condExpr)
//     {
//         for (auto item : condExpr->getStatements())
//         {
//             traverseAllocaExpression(item);
//         }

//         return;
//     }

//     auto loopExpr = dynamic_cast<LoopingStatement *>(expr);
//     if (loopExpr)
//     {
//         auto isInfinity = loopExpr->isInfinityCondition();
//         auto isSingleCondition = loopExpr->isSingleCondition();

//         // Initial //
//         if (!isInfinity && !isSingleCondition)
//         {
//             traverseAllocaExpression(loopExpr->getConditions()[0]);
//         }

//         traverseAllocaExpression(loopExpr->getBody());
//         return;
//     }
// }
