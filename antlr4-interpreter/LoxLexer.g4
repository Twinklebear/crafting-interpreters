lexer grammar LoxLexer;

SEMICOLON: ';';
COMMA: ',';
PERIOD: '.';

TRUE: 'true';
FALSE: 'false';

NIL: 'nil';

FUN: 'fun';

CLASS: 'class';

VAR: 'var';

IF: 'if';
ELSE: 'else';

WHILE: 'while';

FOR: 'for';

PRINT: 'print';

RETURN: 'return';

LEFT_BRACE: '{';
RIGHT_BRACE: '}';

LEFT_PAREN: '(';
RIGHT_PAREN: ')';

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
EQUAL: '=';
STAR: '*';
SLASH: '/';
AND: 'and';
OR: 'or';
LESS_EQUAL: '<=' ;
GREATER: '>' ;
GREATER_EQUAL: '>=' ;
NOT_EQUAL: '!=' ;
EQUAL_EQUAL: '==' ;

WS : [ \t\n\r]+ -> skip;
COMMENT: '//' .*? '\r'? '\n' -> skip;
