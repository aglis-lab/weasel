# Statement

## Delaration Statement
    let IDENTIFIER (Type)? (= Expression)?

## Compound Statement
    {
        (Expressions)*
    }

## Condition Statement

    if Expression
        CompoundStatement
    else if Expression
        CompoundStatement
    else
        CompoundStatement

## Looping Statement

    for Expression
        CompoundStatement
    for Expression;Expression;Expression
        CompoundStatement

# Expression

## Return Expression
    return Expression

## Break Expression
    break

## Continue Expression
    continue

## Call Expression
    Expression (Params?)

- Params

        Expression ( , Expression )* ,?

## Method Call Expression

    Expression . PathExprSegment (Params? )

## Variable Expression
    
    IDENTIFIER

## Array Expression

    [ArrayElements]

- Array Elements

        Expression ( , Expression )* ,?

## Index Expression

    Expression [ Expression ]

## Struct Expression

- StructExpression

        { (StructExprFields)? }

- StructExprFields :

        StructExprField (, StructExprField)* )

- StructExprField:

        (
            IDENTIFIER
            | (IDENTIFIER | TUPLE_INDEX) : Expression
        )

# Operator Expression

## Borrow Expression

    & Expression

## Dereference Expression

    * Expression

## Binary Expression

    Expression (ArithmeticOperator) Expression

- Arithmetic Operator

        Substract       : -
        Addition        : +
        Multiply        : *
        Divide          : /
        Remainder       : %
        Bitwise AND     : &
        Bitwise OR      : |
        Bitwise XOR     : ^
        Left Shift      : <<
        Right Shift     : >>

## Unary Expression

    (-|--|+|++|~|!) Expression
    Expression (-|--|+|++)

## FieldExpression
    Expression . IDENTIFIER

# Path
