#pragma once

/*
Brought from
https://github.com/pocoproject/poco
*/

namespace util
{
    template <class S>
    S &replaceInPlace(S &str, const S &from, const S &to, typename S::size_type start = 0)
    {
        assert(from.size() > 0);

        S result;
        typename S::size_type pos = 0;
        result.append(str, 0, start);
        do
        {
            pos = str.find(from, start);
            if (pos != S::npos)
            {
                result.append(str, start, pos - start);
                result.append(to);
                start = pos + from.length();
            }
            else
                result.append(str, start, str.size() - start);
        } while (pos != S::npos);
        str.swap(result);
        return str;
    }

    std::string normalizeStringLiteral(std::string str)
    {
        return replaceInPlace(str, std::string("\n"), std::string("\\n"));
    }
} // namespace Util
