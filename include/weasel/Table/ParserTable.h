#pragma once

#include <list>
#include <string>

#include <weasel/Table/Table.h>

namespace weasel
{
    class Type;

    class ParserAttribute
    {
    private:
        std::string _identifier = "";
        Type *_type = nullptr;
        AttributeKind _attrKind = AttributeKind::Empty;

    private:
        ParserAttribute() = default;
        ParserAttribute(const std::string &identifier, Type *type, AttributeKind attr) : _identifier(identifier), _type(type), _attrKind(attr) {}

    public:
        inline std::string getIdentifier() const { return _identifier; }
        inline Type *getValue() const { return _type; }
        inline bool isParameter() const { return _attrKind == AttributeKind::Parameter; }
        inline bool isVariable() const { return _attrKind == AttributeKind::Variable; }
        inline bool isEmpty() const { return _attrKind == AttributeKind::Empty; }

    public:
        static ParserAttribute get(const std::string &identifier, Type *type, AttributeKind attr) { return ParserAttribute(identifier, type, attr); }
        static ParserAttribute getEmpty() { return ParserAttribute(); }
    };

    class ParserTable
    {
    protected:
        std::list<ParserAttribute> _attributes;

    public:
        inline std::list<ParserAttribute> getAttributes() const { return _attributes; }
        inline void addAttribute(const ParserAttribute &attr) { _attributes.push_back(attr); }

        inline void enterScope() { _attributes.push_back(ParserAttribute::getEmpty()); }
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
        inline ParserAttribute findAttribute(const std::string &identifier)
        {
            for (std::list<ParserAttribute>::reverse_iterator item = _attributes.rbegin(); item != _attributes.rend(); ++item)
            {
                if ((*item).getIdentifier() == identifier)
                {
                    return (*item);
                }
            }

            return ParserAttribute::getEmpty();
        }
    };
} // namespace weasel
