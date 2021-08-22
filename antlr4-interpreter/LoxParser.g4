parser grammar LoxParser;

options {
    tokenVocab = LoxLexer;
}

file: declaration* ;

declaration: functionDecl
           | classDecl
           | varDeclStmt
           | statement
           ;

functionDecl: FUN function ;

classDecl: CLASS IDENTIFIER LEFT_BRACE function* RIGHT_BRACE ;

varDeclStmt: varDecl SEMICOLON ;

varDecl: VAR IDENTIFIER (EQUAL expr)? ;

function: IDENTIFIER LEFT_PAREN parameters? RIGHT_PAREN block ;

parameters: IDENTIFIER (COMMA IDENTIFIER)? ;

block: LEFT_BRACE declaration* RIGHT_BRACE ;

statement: ifStmt
         | whileStmt
         | forStmt
         | printStmt
         | returnStmt
         | block
         | exprStmt
         ;

ifStmt: IF LEFT_PAREN expr RIGHT_PAREN statement (ELSE statement)? ;

whileStmt: WHILE LEFT_PAREN expr RIGHT_PAREN statement ;

// Separate names for the possible expressions in the for loop so it's easier to distinguish them
// Note: statement can also be optional in most languages (C/C++/etc)
forStmt: FOR LEFT_PAREN (varDecl | forInit)? SEMICOLON forCond? SEMICOLON forAdvance? RIGHT_PAREN statement ;

forInit: expr ;

forCond: expr ;

forAdvance: expr ;

printStmt: PRINT expr SEMICOLON ;

returnStmt: RETURN expr SEMICOLON ;

exprStmt: expr SEMICOLON ;

/*
// Grammar from the book, more nested
expr: assignment ;

assignment: ( call '.' )? IDENTIFIER '=' assignment
          | logicOr
          ;

logicOr: logicAnd ( 'or' logicAnd )* ;
logicAnd: equality ( 'and' equality )* ;
equality: comparison ( ('!=' | '==') comparison )* ;
comparison: term ( ('<' | '<=' | '>' | '>=') term )* ;
term: factor ( ('-' | '+') factor )* ;
factor: unary ( ('/' | '*') unary )* ;

unary: ('!' | '-') unary | call ;
call: primary ( '(' arguments? ')' | '.' IDENTIFIER )* ;
primary: 'true'
       | 'false'
       | 'nil'
       | 'this'
       | NUMBER
       | STRING
       | IDENTIFIER
       | '(' expr ')'
       ;

*/

expr: (MINUS | BANG) expr                                     # Unary
    | callExpr                                                # Call
    | expr STAR expr                                          # Mult
    | expr SLASH expr                                         # Div
    | expr (PLUS | MINUS) expr                                # AddSub
    | expr (LESS | LESS_EQUAL | GREATER | GREATER_EQUAL) expr # Comparison
    | expr (NOT_EQUAL | EQUAL_EQUAL) expr                     # Equality
    | expr AND expr                                           # LogicAnd
    | expr OR expr                                            # LogicOr
    | ( callExpr PERIOD )? IDENTIFIER EQUAL expr              # Assign
    | LEFT_PAREN expr RIGHT_PAREN                             # Parens
    | (IDENTIFIER | NUMBER | STRING | TRUE | FALSE | NIL)     # Primary
    ;

callExpr: IDENTIFIER ( arguments | PERIOD memberIdentifier )* ;

arguments: LEFT_PAREN expr? (COMMA expr)* RIGHT_PAREN ; 

memberIdentifier: IDENTIFIER ;
