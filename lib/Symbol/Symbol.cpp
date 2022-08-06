#include <iostream>
#include "weasel/Symbol/Symbol.h"

/// Symbol Init ///
std::vector<weasel::Error> weasel::ErrorTable::_errors;

std::nullptr_t weasel::ErrorTable::addError(Token token, std::string msg)
{
    _errors.push_back(Error(token, msg));
    return nullptr;
}

void weasel::ErrorTable::showErrors()
{
    if (_errors.empty())
    {
        std::cerr << "No Error found\n";
    }
    else
    {
        for (const auto &item : _errors)
        {
            auto token = item.getToken();
            auto loc = token.getLocation();

            if (token.isUndefined())
            {
                std::cerr << "Error : " << item.getMessage() << std::endl;
                continue;
            }

            std::cerr << "Error : " << item.getMessage() << " but found " << token.getValue() << " kind of " << token.getTokenKindToInt();
            std::cerr << " At (" << loc.row << ":" << loc.col << ")\n";
        }
    }
}
