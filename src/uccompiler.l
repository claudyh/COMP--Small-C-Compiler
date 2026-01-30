/* COMP - META FINAL
   Cláudia Torres & Diogo Silva */

digit   [0-9]
letter [a-zA-Z]
exp e|E
whitespace "\t"|" "
linebreak "\n"|"\r"|"\n\r"|"\r\n"
reserved "["|"]"|"++"|"--"|register|typedef|case|extern|union|float|unsigned|const|auto|long|switch|break|enum|for|signed|continue|goto|sizeof|volatile|default|static|do|struct|_Packed|inline|restrict|_Bool|_Complex|_Imaginary|"long long"|"unsigned long long"
CHAR          char
INT           int
SHORT         short
DOUBLE        double
IF            if
ELSE          else
WHILE         while
RETURN        return
VOID          void
NOT           "!"
BITWISEAND    "&"
BITWISEOR     "|"
BITWISEXOR    "^"
AND           "&&"
OR            "||"
EQ            "=="
NE            "!="
GE            ">="
LE            "<="
GT            ">"
LT            "<"
LBRACE        "{"
RBRACE        "}"
LPAR          "("
RPAR          ")"
ASSIGN        "="
MUL           "*"
COMMA         ","
DIV           "/"
MINUS         "-"
MOD           "%"
PLUS          "+"
SEMI          ";"

%{

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "y.tab.h"
#include "ast.h"
#include "semantics.h"

#define YY_USER_ACTION { syn_line = line; syn_column = column; column+=yyleng; yylloc.first_line = syn_line; yylloc.first_column = syn_column; }
#define PASS_TOKEN() yylval.lexeme = strdup(yytext);

int line = 1;
int column = 1;
int print_tokens = 0;
int comment_line = 0;
int comment_column = 0;

int syn_line = 1, syn_column = 1;
int errors = 0;

int yyparse(void);
void yyerror(char *);

extern struct node *program;

%}

%option noyywrap

%x COMMENT
%x COMMENT_BLOCK

%%

"//"						{ BEGIN(COMMENT); column += 2; }
<COMMENT>.					{ /*column+=yyleng*/; }
<COMMENT>{linebreak}		{ line++; column = 1; syn_line=line; syn_column=column; BEGIN(INITIAL); }

"/*"						{ BEGIN(COMMENT_BLOCK); comment_line = line; comment_column = column; /*column+=yyleng*/; }
<COMMENT_BLOCK>.			{ /*column+=yyleng*/; }
<COMMENT_BLOCK>{linebreak}	{ line++; column = 1; }
<COMMENT_BLOCK><<EOF>>		{ printf("Line %d, column %d: unterminated comment\n", comment_line, comment_column); BEGIN(INITIAL); }	
<COMMENT_BLOCK>"*/"			{ /*column+=yyleng*/; BEGIN(INITIAL); }

{whitespace}				{ syn_column=column;}

{linebreak}					{ line ++;
							column = 1;
							syn_column=column;
							syn_line=line; }

{digit}+                	{ if(print_tokens) printf("NATURAL(%s)\n", yytext);
						  	 
						  	if(!print_tokens){
								PASS_TOKEN();
								return NATURAL;
							}
							}

{digit}+"."(({digit}+)?({exp}[+-]?{digit}+)?)?	{ if(print_tokens) printf("DECIMAL(%s)\n", yytext);
						  						 
						  						if(!print_tokens){
													PASS_TOKEN();
													return DECIMAL;
												}
												}

"."{digit}+({exp}[+-]?{digit}+)?		{ if(print_tokens) printf("DECIMAL(%s)\n", yytext);
						  				 
						  				if(!print_tokens){
											PASS_TOKEN();
											return DECIMAL;
										}
										}

{digit}+{exp}[+-]?{digit}+				{ if(print_tokens) printf("DECIMAL(%s)\n", yytext);
						  				 
						  				if(!print_tokens){
											PASS_TOKEN();
											return DECIMAL;
										}
										}

{reserved}					{ if (print_tokens)
							printf("RESERVED(%s)\n", yytext);
							 
							if(!print_tokens){ return RESERVED; }
							}

{CHAR}                                      { if(print_tokens)printf("CHAR\n");   if(!print_tokens)return CHAR;}
{INT}                                       { if(print_tokens)printf("INT\n");   if(!print_tokens)return INT;}
{SHORT}                                     { if(print_tokens)printf("SHORT\n");   if(!print_tokens)return SHORT;}
{DOUBLE}                                    { if(print_tokens)printf("DOUBLE\n");   if(!print_tokens)return DOUBLE;}
{IF}                                        { if(print_tokens)printf("IF\n");   if(!print_tokens)return IF;}
{ELSE}                                      { if(print_tokens)printf("ELSE\n");   if(!print_tokens)return ELSE;}
{WHILE}                                     { if(print_tokens)printf("WHILE\n");   if(!print_tokens)return WHILE;}
{RETURN}                                    { if(print_tokens)printf("RETURN\n");   if(!print_tokens)return RETURN;}
{VOID}                                      { if(print_tokens)printf("VOID\n");   if(!print_tokens)return VOID;}
{NOT}                                       { if(print_tokens)printf("NOT\n");   if(!print_tokens)return NOT;}
{BITWISEAND}                                { if(print_tokens)printf("BITWISEAND\n");   if(!print_tokens)return BITWISEAND;}
{BITWISEOR}                                 { if(print_tokens)printf("BITWISEOR\n");   if(!print_tokens)return BITWISEOR;}
{BITWISEXOR}                                { if(print_tokens)printf("BITWISEXOR\n");   if(!print_tokens)return BITWISEXOR;}
{AND}                                       { if(print_tokens)printf("AND\n");   if(!print_tokens)return AND;}
{OR}                                        { if(print_tokens)printf("OR\n");   if(!print_tokens)return OR;}
{EQ}                                        { if(print_tokens)printf("EQ\n");   if(!print_tokens)return EQ;}
{NE}                                        { if(print_tokens)printf("NE\n");   if(!print_tokens)return NE;}
{GE}                                        { if(print_tokens)printf("GE\n");   if(!print_tokens)return GE;}
{LE}                                        { if(print_tokens)printf("LE\n");   if(!print_tokens)return LE;}
{GT}                                        { if(print_tokens)printf("GT\n");   if(!print_tokens)return GT;}
{LT}                                        { if(print_tokens)printf("LT\n");   if(!print_tokens)return LT;}
{LBRACE}                                    { if(print_tokens)printf("LBRACE\n");   if(!print_tokens)return LBRACE;}
{RBRACE}                                    { if(print_tokens)printf("RBRACE\n");   if(!print_tokens)return RBRACE;}
{LPAR}                                      { if(print_tokens)printf("LPAR\n");   if(!print_tokens)return LPAR;}
{RPAR}                                      { if(print_tokens)printf("RPAR\n");   if(!print_tokens)return RPAR;}
{ASSIGN}                                    { if(print_tokens)printf("ASSIGN\n");   if(!print_tokens)return ASSIGN;}
{MUL}                                       { if(print_tokens)printf("MUL\n");   if(!print_tokens)return MUL;}
{COMMA}                                     { if(print_tokens)printf("COMMA\n");   if(!print_tokens)return COMMA;}
{DIV}                                       { if(print_tokens)printf("DIV\n");   if(!print_tokens)return DIV;}
{MINUS}                                     { if(print_tokens)printf("MINUS\n");   if(!print_tokens)return MINUS;}
{MOD}                                       { if(print_tokens)printf("MOD\n");   if(!print_tokens)return MOD;}
{PLUS}                                      { if(print_tokens)printf("PLUS\n");   if(!print_tokens)return PLUS;}
{SEMI}                                      { if(print_tokens)printf("SEMI\n");   if(!print_tokens)return SEMI;}

({letter}|"_")({letter}|{digit}|"_")*		{ if (print_tokens) printf("IDENTIFIER(%s)\n", yytext);
						  					 
						  					if(!print_tokens){
												PASS_TOKEN();
												return IDENTIFIER;
											}
											}

'(([^\n\'\r\\])|(\\[nt\'\"\\])|(\\[0-7]{1,3}))'	{ if (print_tokens) printf("CHRLIT(%s)\n", yytext);
						  						 
						  						if(!print_tokens){
													PASS_TOKEN(); 
													return CHRLIT;
												}
												}

'([^\n\'\\]|\\.)*(\\)?{linebreak}|'		{ printf("Line %d, column %d: unterminated char constant\n", line, column-yyleng);
						  				column = 1;
						  				line++; 
										syn_column = column;
										syn_line = line;}

'([^\n\'\\]|\\.)*'						{ printf("Line %d, column %d: invalid char constant (%s)\n", line, column-yyleng, yytext);
						  				  }

.										{ printf("Line %d, column %d: unrecognized character (%s)\n", line, column-yyleng, yytext);
						  				  }

<<EOF>>                 				{ YY_USER_ACTION; return 0; }

%%

extern int yylex();

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            print_tokens = 1;
			yylex();
            break;
        }
    }

    if(print_tokens == 0){
		yyparse();
	}
    if(argc >= 2 && strcmp(argv[1], "-t") == 0 && errors == 0) {
		show(program, 0);
    }
	if(argc >= 2 && strcmp(argv[1], "-s") == 0 && errors == 0) { 
        errors += check_program(program);
        show_symbol_table();
		printf("\n");
        show(program, 0);
    }

    //deallocate(program);
    //yylex_destroy();
    return 0;
}

void yyerror(char *error) {
    errors++;
    printf("Line %d, column %d: %s: %s\n", syn_line, syn_column, error, yytext);
}
