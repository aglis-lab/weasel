namespace weasel
{
    // Source Location
    struct SourceLocation
    {
        unsigned line = 1;
        unsigned column = 0;
        unsigned inlineAt = 0;

        void toNextLine(char val)
        {
            if (val == '\n')
            {
                line++;
                column = 1;
            }
            else
            {
                column++;
            }

            inlineAt++;
        }
    };
} // namespace weasel
