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
        int line = 0;
        int column = -1; // Because we increment when we get first character

        // We need to track the last value
        void incrementColumn(int val)
        {
            column += val;
        }

        void newLine()
        {
            line++;
            column = 0;
        }
    };

    // TODO: Range Location with Source Index
    struct RangeLocation
    {
        SourceLocation startLocation;
        SourceLocation endLocation;
    };
} // namespace weasel
