grammar Lox;

file: declaration* ;

declaration: functionDecl
           | classDecl
           | varDecl
           | statement
           ;

functionDecl: 'fun' function ;

classDecl: 'class' IDENTIFIER '{' function* '}' ;

varDecl: 'var' IDENTIFIER ('=' expr)? ';' ;

function: IDENTIFIER '(' parameters? ')' block ;

parameters: IDENTIFIER (',' IDENTIFIER)? ;

block: '{' declaration* '}' ;

statement: ifStmt
         | whileStmt
         | forStmt
         | printStmt
         | returnStmt
         | block
         | exprStmt
         ;

ifStmt: 'if' '(' expr ')' statement ('else' statement)? ;

whileStmt: 'while' '(' expr ')' statement ;

forStmt: 'for' '(' (varDeclStmt | expr)? ';' expr? ';' expr? ')' statement ;

varDeclStmt: 'var' IDENTIFIER ('=' expr)? ;

printStmt: 'print' expr ';' ;

returnStmt: 'return' expr ';' ;

exprStmt: expr ';' ;

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

expr: ('-' | '!') expr               # Unary
    | callExpr                       # Call
    | expr '*' expr                  # Mult
    | expr '/' expr                  # Div
    | expr ('+' | '-') expr          # AddSub
    | expr ('<' | '<=' | '>' | '>=') expr # Comparison
    | expr ('!=' | '==') expr        # Equality
    | expr 'and' expr                # LogicAnd
    | expr 'or'  expr                # LogicOr
    | ( callExpr '.' )? IDENTIFIER '=' expr     # Assign
    | '(' expr ')'                  # Parens
    | (IDENTIFIER | NUMBER | STRING |  'true' | 'false' | 'nil') # Primary
    ;

callExpr: IDENTIFIER ( '(' arguments? ')' | '.' IDENTIFIER )* ;

arguments: expr (',' expr)* ; 

NUMBER: ('0' | [1-9]) [0-9]* ('.' [0-9]+)?;
STRING: '"' .*? '"' ;
IDENTIFIER: [a-zA-Z_] [a-zA-Z0-9_]* ;

WS : [ \t\n\r]+ -> skip;
COMMENT: '//' .*? '\r'? '\n' -> skip;
