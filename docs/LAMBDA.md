# LAMBDA - FUNCTION

### LAMBDA

```

// Simple Callback
fun SimpleCallback(callback fun()) {
}

// Callback with Return Type and Params
fun ParamsCallback(callback fun(int, int) int) {
}

// Callback with generic
fun GenericCallback(callbak fun[T constraint.Primitive](T, T) T) {
}

// Lambda Variable
let a fun()

a = fun() {
}

// Current Return Type is it's FunctionType it self
FunctionType : Type {
  Arguments []Type
  ReturnType Type
}

// Lambda Expression
LamdaExpression {
  Type FunctionTypes  Body CompoundStatement

  Vararg bool
}

FunctionExpression : LambdaExpression {
  ImplStruct StructTypeHandle;

  IsDefine bool
  IsInline bool
  IsExtern bool
  IsStatic bool
}

```
