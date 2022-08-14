# Notation


# Library

    (Funcion)+

# Function

    fun IDENTIFIER (Params?)
        CompoundStatement

# Params

    IDENTIFIER Type (, IDENTIFIER Type)*

# Statement

## Compound Statement
    {
        (Expression)+
    }

## Condition Statement

- If Statement

        if Expression
            CompoundStatement

- Else If Statement

        else if Expression
            CompoundStatement

- Else Statement

        else
            CompoundStatement

## Looping Statement

- For Condition Statement

        for Expression
            CompoundStatement

- For Condition and Increment Statement

        for Expression;Expression;Expression
            CompoundStatement

## Delaration Statement

    let IDENTIFIER (Type)? (= Expression)? (CompoundStatement)?

## Expression Statement
    
    Expression

# Expression

## Return Expression

    return Expression

## Break Expression

    break (Expression)?

## Continue Expression

    continue (Expression)?

## Call Expression

    Expression (Params?)

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

        StructType? { (StructExprFields)? }

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
