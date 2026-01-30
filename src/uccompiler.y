
%{

#include "ast.h"
#include <stddef.h>
#include <stdlib.h>

int yylex(void);
void yyerror(char *);

struct node *program;
struct node *typeSpecValue;

%}
%union{
    char *lexeme;
    struct node *node;
}

%locations
%{
#define LOCATE(node, line, column) { node->token_line = line; node->token_column = column; }
%}

%token INT SHORT DOUBLE LBRACE RBRACE SEMI LPAR RPAR COMMA CHAR VOID ASSIGN IF ELSE WHILE RETURN PLUS MINUS MUL DIV MOD OR AND BITWISEAND BITWISEOR BITWISEXOR EQ NE LE GE LT GT NOT RESERVED
%token<lexeme> IDENTIFIER NATURAL CHRLIT DECIMAL
%type<node> FunctionsAndDeclarations FunctionDefinition FunctionBody DeclarationsAndStatements FunctionDeclaration FunctionDeclarator ParameterList
 ParameterDeclaration Declaration TypeSpec 
 Declarator Statement Expr DecLst StmLst ExprComma ExprCall StmError

%left   COMMA
%right  ASSIGN
%left   OR
%left   AND
%left   BITWISEOR
%left   BITWISEXOR
%left   BITWISEAND
%left   EQ NE
%left   LE LT GE GT
%left   PLUS MINUS
%left   MUL DIV MOD
%right  NOT
%left   LPAR RPAR

%nonassoc ELSE

  /*START grammar rules section -- BNF grammar */

%%

FunctionsAndDeclarations: FunctionDefinition		{ $$ = program = newnode(Program, NULL);
													addchild($$, $1);  }
	| FunctionDeclaration					 		{ $$ = program = newnode(Program, NULL);
													addchild($$, $1); }
	| Declaration									{ $$ = program = newnode(Program, NULL);
													adoptChild($$, $1); }
	| FunctionsAndDeclarations FunctionDefinition	{ $$= $1;
													addchild($$, $2); }
	| FunctionsAndDeclarations FunctionDeclaration	{ $$= $1;
													addchild($$, $2); }
	| FunctionsAndDeclarations Declaration			{ $$= $1;
													adoptChild($$, $2); }
	;


FunctionDefinition: TypeSpec FunctionDeclarator FunctionBody { $$ = newnode(FuncDefinition, NULL);
				      			       						  addchild($$, $1);
							       							  adoptChild($$, $2);
							       							  addchild($$, $3); }
	;


FunctionBody: LBRACE DeclarationsAndStatements RBRACE	{ $$ = newnode(FuncBody, NULL);
		       											adoptChild($$, $2); }
	| LBRACE RBRACE				      					{ $$ = newnode(FuncBody, NULL); }
	;


DeclarationsAndStatements: DeclarationsAndStatements Statement 	{ addchild($$, $2); }
	| DeclarationsAndStatements Declaration 		       		{ adoptChild($$, $2); }
	| Statement					       							{ $$ = newnode(Aux, NULL);
	 															addchild($$, $1);}
	| Declaration					       						{ $$ = newnode(Aux, NULL);
	 															adoptChild($$, $1); }
	;


FunctionDeclaration: TypeSpec FunctionDeclarator SEMI	{ $$ = newnode(FuncDeclaration, NULL);
		       											addchild($$, $1);
		       											adoptChild($$, $2); }
	;


FunctionDeclarator: IDENTIFIER LPAR ParameterList RPAR	{ $$ = newnode(Aux, NULL);
		       											addchild($$, newnode(Identifier, $1));
		       											addchild($$, $3);
														LOCATE(getchild($$, 0), @1.first_line, @1.first_column); }
	;


ParameterList: ParameterDeclaration				{ $$ = newnode(ParamList, NULL);
							  					addchild($$, $1); }
	| ParameterList COMMA ParameterDeclaration 	{ $$ = $1;
												addchild($$, $3); }
	;


ParameterDeclaration: TypeSpec	{ $$ = newnode(ParamDeclaration, NULL);
								addchild($$, $1); }
	| TypeSpec IDENTIFIER		{ $$ = newnode(ParamDeclaration, NULL);
		       		  			addchild($$, $1);
		       		  			addchild($$, newnode(Identifier, $2));
								LOCATE(getchild($$, 1), @2.first_line, @2.first_column); }
	;


DecLst:	Declarator				{ $$ = newnode(Aux, NULL);
								addchild($$, $1); }
	| DecLst COMMA Declarator	{ $$ = $1;
								addchild($$, $3); }
	;


Declaration: TypeSpec DecLst SEMI	{ $$ = $2; }
	| error SEMI					{ $$ = newnode(Error, NULL); }
	;


TypeSpec: CHAR	{ $$ = typeSpecValue = newnode(Char, NULL); }
	| INT    	{ $$ = typeSpecValue = newnode(Int, NULL); }
	| VOID   	{ $$ = typeSpecValue = newnode(Void, NULL);
				  LOCATE($$, @1.first_line, @1.first_column); }
	| SHORT	 	{ $$ = typeSpecValue = newnode(Short, NULL); }
	| DOUBLE 	{ $$ = typeSpecValue = newnode(Double, NULL); }
	;


Declarator: IDENTIFIER			{ $$ = newnode(Declaration, NULL);
								addchild($$, typeSpecValue); 
								addchild($$, newnode(Identifier, $1));
								LOCATE(getchild($$, 0), @1.first_line, @1.first_column); }
	| IDENTIFIER ASSIGN Expr	{ $$ = newnode(Declaration, NULL);
								addchild($$, typeSpecValue); 
		       		  			addchild($$, newnode(Identifier, $1));
		       		  			addchild($$, $3);
								LOCATE(getchild($$, 0), @1.first_line, @1.first_column);  }
	;

//--
StmLst: StmError		{ $$ = newnode(StatList, NULL);
						if($1) addchild($$, $1); }
	| StmLst StmError	{ if($2) addchild($$, $2); }
	;

//--
StmError: Statement { $$ = $1; }
	| error SEMI	{ $$ = newnode(Error, NULL); }
	;

//--
Statement: SEMI 									{ $$ = NULL; }
	| ExprComma SEMI 								{ $$ = $1; }
	| IF LPAR ExprComma RPAR Statement 				{ $$ = newnode(If, NULL);
		       		    							addchild($$, $3);
		       		    							if($5) addchild($$, $5);
													else addchild($$, newnode(Null, NULL)); 
													addchild($$, newnode(Null, NULL));
													LOCATE($$, @1.first_line, @1.first_column); }
	| IF LPAR ExprComma RPAR Statement ELSE Statement { $$ = newnode(If, NULL);
		       		    		       				addchild($$, $3);
													if($5) addchild($$, $5);
													else addchild($$, newnode(Null, NULL));
		       		    		       				if($7) addchild($$, $7);
													else addchild($$, newnode(Null, NULL));
													LOCATE($$, @1.first_line, @1.first_column); }
	| WHILE LPAR ExprComma RPAR Statement 			{ $$ = newnode(While, NULL);
		       		    	   						addchild($$, $3);
		       		    	   						if($5) addchild($$, $5);
													else addchild($$, newnode(Null, NULL)); }
	| RETURN SEMI 									{ $$ = newnode(Return, NULL); 
													addchild($$, newnode(Null, NULL)); }
	| RETURN ExprComma SEMI 						{ $$ = newnode(Return, NULL);
													addchild($$, $2); }
	| LBRACE StmLst RBRACE 							{ if($2->children->next && $2->children->next->next) $$ = $2;
													  else if($2->children->next) $$ = $2->children->next->node;
													  else $$ = NULL;
													 }
	| LBRACE RBRACE									{ $$ = NULL; }
	| LBRACE error RBRACE 							{ $$ = newnode(Error, NULL); }
	;


ExprCall: Expr 				{ $$ = newnode(Aux, NULL);
							addchild($$, $1); }
	| ExprCall COMMA Expr	{ $$ = $1;
							addchild($$, $3);
							LOCATE($$, @2.first_line, @2.first_column ); }
	;


ExprComma: Expr 			{ $$ = $1; }
	| ExprComma COMMA Expr	{ $$ = newnode(Comma, NULL);
							addchild($$, $1);
							addchild($$, $3);
							LOCATE($$, @2.first_line, @2.first_column ); }
	;


Expr: Expr ASSIGN Expr				{ $$ = newnode(Store, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr PLUS Expr				{ $$ = newnode(Add, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr MINUS Expr				{ $$ = newnode(Sub, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr MUL Expr					{ $$ = newnode(Mul, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr DIV Expr					{ $$ = newnode(Div, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr MOD Expr					{ $$ = newnode(Mod, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr OR Expr					{ $$ = newnode(Or, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr AND Expr					{ $$ = newnode(And, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr BITWISEAND Expr			{ $$ = newnode(BitWiseAnd, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr BITWISEOR Expr			{ $$ = newnode(BitWiseOr, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr BITWISEXOR Expr			{ $$ = newnode(BitWiseXor, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr EQ Expr					{ $$ = newnode(Eq, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr NE Expr					{ $$ = newnode(Ne, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr LE Expr					{ $$ = newnode(Le, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr GE Expr					{ $$ = newnode(Ge, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr LT Expr					{ $$ = newnode(Lt, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
	| Expr GT Expr					{ $$ = newnode(Gt, NULL);
									addchild($$, $1);
									addchild($$, $3);
									LOCATE($$, @2.first_line, @2.first_column); }
    | PLUS Expr %prec NOT 			{ $$ = newnode(Plus, NULL);
									addchild($$, $2);
									LOCATE($$, @1.first_line, @1.first_column); }
    | MINUS Expr %prec NOT 			{ $$ = newnode(Minus, NULL);
									addchild($$, $2);
									LOCATE($$, @1.first_line, @1.first_column); }
    | NOT Expr						{ $$= newnode(Not, NULL);
									addchild($$, $2);
									LOCATE($$, @1.first_line, @1.first_column); }
    | IDENTIFIER					{ $$ = newnode(Identifier, $1);
									  LOCATE($$, @1.first_line, @1.first_column); }
    | NATURAL						{ $$ = newnode(Natural, $1);
									 LOCATE($$, @1.first_line, @1.first_column); }
    | CHRLIT						{ $$ = newnode(ChrLit, $1);
									LOCATE($$, @1.first_line, @1.first_column); }
    | DECIMAL						{ $$ = newnode(Decimal, $1);
									LOCATE($$, @1.first_line, @1.first_column); }
    | LPAR ExprComma RPAR			{ $$ = $2; }
	| IDENTIFIER LPAR RPAR 			{ $$ = newnode(Call, NULL);
									addchild($$, newnode(Identifier, $1));
									LOCATE($$, @1.first_line, @1.first_column);
									LOCATE(getchild($$, 0), @1.first_line, @1.first_column); }
	| IDENTIFIER LPAR ExprCall RPAR	{ $$ = newnode(Call, NULL);
									addchild($$, newnode(Identifier, $1));
									adoptChild($$, $3);
									LOCATE($$, @1.first_line, @1.first_column);
									LOCATE(getchild($$, 0), @1.first_line, @1.first_column); }
	| IDENTIFIER LPAR error RPAR	{ $$ = newnode(Error, NULL); }
	| LPAR error RPAR				{ $$ = newnode(Error, NULL); }
    ;

%%
  /*START subroutines section  

  all needed functions are collected in the .l and ast.* files*/
