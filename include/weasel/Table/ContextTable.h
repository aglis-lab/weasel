#pragma once

#include <llvm/IR/Value.h>

#include <list>
#include <string>

#include <weasel/Table/Table.h>

namespace weasel
{
    class ContextAttribute
    {
    private:
        std::string _identifier = "";
        llvm::Value *_value = nullptr;
        AttributeKind _attrKind = AttributeKind::Empty;

    private:
        ContextAttribute() = default;
        ContextAttribute(const std::string &identifier, llvm::Value *value, AttributeKind attr) : _identifier(identifier), _value(value), _attrKind(attr) {}

    public:
        inline std::string getIdentifier() const { return _identifier; }
        inline llvm::Value *getValue() const { return _value; }
        inline bool isParameter() const { return _attrKind == AttributeKind::Parameter; }
        inline bool isVariable() const { return _attrKind == AttributeKind::Variable; }
        inline bool isEmpty() const { return _attrKind == AttributeKind::Empty; }

    public:
        static ContextAttribute get(const std::string &identifier, llvm::Value *value, AttributeKind attr) { return ContextAttribute(identifier, value, attr); }
        static ContextAttribute getEmpty() { return ContextAttribute(); }
    };

    class ContextTable
    {
    protected:
        std::list<ContextAttribute> _attributes;

    public:
        inline std::list<ContextAttribute> getAttributes() const { return _attributes; }
        inline void addAttribute(const ContextAttribute &attr) { _attributes.push_back(attr); }

        inline void enterScope() { _attributes.push_back(ContextAttribute::getEmpty()); }
        inline void exitScope()
        {
            while (true)
            {
                auto newScope = _attributes.back().isEmpty();
                _attributes.pop_back();

                if (newScope)
                {
                    break;
                }
            }
        }
        inline ContextAttribute findAttribute(const std::string &identifier)
        {
            for (std::list<ContextAttribute>::reverse_iterator item = _attributes.rbegin(); item != _attributes.rend(); ++item)
            {
                if ((*item).getIdentifier() == identifier)
                {
                    return (*item);
                }
            }

            return ContextAttribute::getEmpty();
        }
    };
} // namespace weasel
