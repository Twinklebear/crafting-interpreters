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

expr: (MINUS | BANG) expr               # Unary
    | callExpr                       # Call
    | expr '*' expr                  # Mult
    | expr '/' expr                  # Div
    | expr (PLUS | MINUS) expr          # AddSub
    | expr (LESS | LESS_EQUAL | GREATER | GREATER_EQUAL) expr # Comparison
    | expr (NOT_EQUAL | EQUAL_EQUAL) expr        # Equality
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

// Not sure what the easiest way is to get these symbols for rules
// that match multiple ones, like unary is - or !, but in the visitor
// how to check which is which? Or for something more complicated like
// comparison or addsub where the +/-/etc token is between the expr,
// not clear how to easily get that
MINUS: '-' ;
BANG: '!' ;
PLUS: '+' ;
LESS: '<' ;
LESS_EQUAL: '<=' ;
GREATER: '>' ;
GREATER_EQUAL: '>=' ;
NOT_EQUAL: '!=' ;
EQUAL_EQUAL: '==' ;

WS : [ \t\n\r]+ -> skip;
COMMENT: '//' .*? '\r'? '\n' -> skip;
