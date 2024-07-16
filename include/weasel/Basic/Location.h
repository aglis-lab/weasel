#include <fmt/core.h>

#include <sys/types.h>

using namespace std;

namespace weasel
{
    /*
     * Source Location
     * The text document "ab\ncd" produces:
     *
     * position { line: 0, column: 0 } for `offset` 0. a
     * position { line: 0, column: 1 } for `offset` 1. b
     * position { line: 0, column: 2 } for `offset` 2. /n
     * position { line: 1, column: 0 } for `offset` 3. c
     * position { line: 1, column: 1 } for `offset` 4. d
     * */
    struct SourceLocation
    {
        uint loc = 0;     // Location or index of the character
        uint line = 0;    // Row
        uint column = -1; // Because we increment when we get first character

        // We need to track the last value
        void incrementColumn(uint val)
        {
            column += val;
        }

        void newLine()
        {
            line++;
            column = -1;
        }

        string toString()
        {
            return fmt::format("{{loc: {}, line: {}, column: {}}}", loc, line, column);
        }

        SourceLocation() {}

        SourceLocation(uint loc, uint line, uint column)
        {
            this->loc = loc;
            this->line = line;
            this->column = column;
        }
    };

    // TODO: Range Location with Source Index
    struct RangeLocation
    {
        SourceLocation startLocation;
        SourceLocation endLocation;
    };
} // namespace weasel
