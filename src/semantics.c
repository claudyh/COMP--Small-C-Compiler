#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

int semantic_errors = 0;
int error = 0;

int invented_token= 0;
char str_token[2000000];

struct symbol_list *symbol_table, *scope1, *scope2;
enum type return_type = no_type;

struct scope_node {
    struct symbol_list *scope;
    int param;
    struct scope_node *next;
};

struct scope_node *scope_list = NULL;

void add_scope(struct symbol_list *scope, int param) {
    struct scope_node *new_scope_node = (struct scope_node *)malloc(sizeof(struct scope_node));
    new_scope_node->scope = scope;
    new_scope_node->param = param;
    new_scope_node->next = NULL;

    if (scope_list == NULL) {
        scope_list = new_scope_node;
    } else {
        struct scope_node *last = scope_list;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new_scope_node;
    }
}

//--
void check_typespec_fr(struct node *node){
    
    switch(getchild(node, 0)->category){
        case Char:
            getchild(node, 0)->type = integer_type;
            break;
        case Int:
            getchild(node, 0)->type = integer_type;
            break;
        case Void:
            getchild(node, 0)->type = void_type;
            break;
        case Short:
            getchild(node, 0)->type = short_type;
            break;
        case Double:
            getchild(node, 0)->type = double_type;
            break;
        default:
            printf("UGHHHHHHH\n");
            break;
    }
}

void check_typespec(struct node *node){
    
    switch(getchild(node, 0)->category){
        case Char:
            getchild(node, 2)->type = integer_type;
            break;
        case Int:
            getchild(node, 2)->type = integer_type;
            break;
        case Void:
            getchild(node, 2)->type = void_type;
            break;
        case Short:
            getchild(node, 2)->type = short_type;
            break;
        case Double:
            getchild(node, 2)->type = double_type;
            break;
        default:
            printf("UGHHHHHHH\n");
            break;
    }
}

//--
void check_declaration(struct node *declaration, int on_scope) {

    //Typespec --
    struct node *typespec = getchild(declaration, 0);
    if(typespec->category == Void){
        printf("Line %d, column %d: Invalid use of void type in declaration\n", typespec->token_line, typespec->token_column);
        return;
    }

    //DecLst --
    //Identifier
    struct node *id = getchild(declaration, 1);
    enum type type = category_type(getchild(declaration, 0)->category);
    if(on_scope){
        if(search_symbol(scope2, id->token) == NULL) {
            //inserimos na scope table
            insert_symbol(scope2, id->token, type, declaration, 0);
        }
    } else{
        if(search_symbol(symbol_table, id->token) == NULL) {
            //inserimos na symbol table
            insert_symbol(symbol_table, id->token, type, declaration, 0);
        }
    }

    if(getchild(declaration, 2)){
        check_expr(getchild(declaration, 2), no_type);
    }
}

// Function to check the result type of a binary operation
enum type res_type(enum type type1, enum type type2) {
    if (type1 != type2 ) {
        if ((type1 == integer_type && type2 == short_type) || (type1 == short_type && type2 == integer_type))
            return integer_type;
        else if ((type1 == char_type && type2 == integer_type) || (type1 == integer_type && type2 == char_type))
            return integer_type;
        else if ((type1 == double_type && type2 == integer_type) || (type1 == integer_type && type2 == double_type))
            return double_type;
        else if ((type1 == double_type && type2 == short_type) || (type1 == short_type && type2 == double_type))
            return double_type;
        else if ((type1 == double_type && type2 == char_type) || (type1 == char_type && type2 == double_type))
            return double_type;
        else if ((type1 == short_type && type2 == char_type) || (type1 == char_type && type2 == short_type))
            return short_type;
    }
    return type1;
}

//--
void check_expr(struct node *expression, enum type function_type) {
    struct node *id1;
    struct node_list *child1;
    struct node_list *child2;
    int exists=0;
    int args_gotted=0;
    int args_expected=0;
    enum type type1, type2;
    char operator[100]="";

    //Expr
    switch(expression->category){
        case Store:

            check_expr(getchild(expression, 0), function_type);
            check_expr(getchild(expression, 1), function_type);

            if(getchild(expression, 0)->category != Identifier){
                printf("Line %d, column %d: Lvalue required\n", getchild(expression, 0)->token_line, getchild(expression, 0)->token_column);
            }
            else{

                // Determine the result type based on operand types
                type1 = getchild(expression, 0)->type;
                type2 = getchild(expression, 1)->type;

                if ( type1 != double_type && type2 == double_type ){
                    printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, type_name(type1), type_name(type2));
                }
                else if ( type1 == undef_type || type2 == undef_type ){
                    printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, type_name(type1), type_name(type2));
                }
                else{
                    expression->type= type1;
                }
            }
            expression->type= type1;
            break;
        case Mod:
            expression->type= integer_type;

            check_expr(getchild(expression, 0), function_type);
            check_expr(getchild(expression, 1), function_type);

            // Determine the result type based on operand types
            type1 = getchild(expression, 0)->type;
            type2 = getchild(expression, 1)->type;

            if ( type1 == double_type || type2 == double_type ){
                printf("Line %d, column %d: Operator %% cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, type_name(type1), type_name(type2));
            }
            break;
        case Add:
        case Sub:
        case Mul:
        case Div:
            check_expr(getchild(expression, 0), function_type);
            check_expr(getchild(expression, 1), function_type);

            // Determine the result type based on operand types
            type1 = getchild(expression, 0)->type;
            type2 = getchild(expression, 1)->type;

            if ( type1 == undef_type || type2 == undef_type || type1 == void_type || type2 == void_type ) {
                printf("Line %d, column %d: Operator %% cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, type_name(type1), type_name(type2));
                expression->type = undef_type;
            }
            else{
                expression->type = res_type(type1, type2);
            }

            break;
        case Or:
        case And:
        case BitWiseAnd:
        case BitWiseOr:
        case BitWiseXor:
            check_expr(getchild(expression, 0), function_type);
            check_expr(getchild(expression, 1), function_type);

            // Determine the result type based on operand types
            type1 = getchild(expression, 0)->type;
            type2 = getchild(expression, 1)->type;

            expression->type = integer_type;

            //operators
            switch (expression->category) {
            case BitWiseOr:     strcpy(operator, "|"); break;
            case BitWiseAnd:    strcpy(operator, "&"); break;
            case BitWiseXor:    strcpy(operator, "^"); break;
            case And:           strcpy(operator, "&&"); break;
            case Or:            strcpy(operator, "||"); break;
            default:            break;
            }
            
            if ( type1 == double_type || type2 == double_type ){
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, operator, type_name(type1), type_name(type2));
            } else if ( type1 == undef_type || type2 == undef_type ){
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, operator, type_name(type1), type_name(type2));
            }
            
            strcpy(operator, "\0");
            break;
        case Eq:
        case Ne:
        case Le:
        case Ge:
        case Lt:
        case Gt:
            check_expr(getchild(expression, 0), function_type);
            check_expr(getchild(expression, 1), function_type);

            // Determine the result type based on operand types
            type1 = getchild(expression, 0)->type;
            type2 = getchild(expression, 1)->type;

            expression->type = integer_type;
            break;
        case Plus:
        case Minus:
            check_expr(getchild(expression, 0), function_type);
            expression->type = getchild(expression, 0)->type;
            break;
        case Not:
            check_expr(getchild(expression, 0), function_type);
            expression->type = integer_type;
            break;
        case Natural:
            //printf("naturalllll\n");
            expression->type = integer_type;
            break;
        case Decimal:
            //printf("decimalllll\n");
            expression->type = double_type;
            break;
        case ChrLit:
            //printf("chrlittt\n");
            expression->type = integer_type;
            break;
        case Identifier:
            
            if(search_symbol(scope2, expression->token) != NULL) {
                expression->type = search_symbol(scope2, expression->token)->type;
            } else if(search_symbol(symbol_table, expression->token) != NULL){
                expression->type = search_symbol(symbol_table, expression->token)->type;
            } else {
                if(strcmp(expression->token, "putchar") == 0){
                    expression->type = putchr_type;
                } else if(strcmp(expression->token, "getchar") == 0){
                    expression->type = gtchr_type;
                } else{
                    //printf("Variable %s (%d:%d) undeclared\n", expression->token, expression->token_line, expression->token_column);
                    expression->type = undef_type;
                    semantic_errors++;
                }
            }

            id1 = expression;
            
            if(function_type != no_type && function_type != expression->type){
                printf("Line %d, column %d: Conflicting types (got %s, expected %s)\n", id1->token_line, id1->token_column, type_name(expression->type), type_name(function_type));
            }

            break;
        case Call:

            //Filhos
            child2 = expression->children;

            while((child2 = child2->next) != NULL){
                //Dar lhes o tipo
                check_expr(child2->node, function_type);
                args_gotted++;
            }

            //tirar o identifier
            args_gotted--;

            struct symbol_list *symbol;
            for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next){
                //encontrar identifier
                if(strcmp(symbol->identifier, getchild(expression, 0)->token) == 0){
                    //TIPO CALL = TIPO IDENTIFIER 
                    expression->type = symbol->type;
                    exists=1;
                }

                //contar parametros
                if(exists==1 && symbol->func == 1){
                    args_expected++;
                }
            }

            //ver se é putchar ou getchar
            if(strcmp(getchild(expression, 0)->token, "putchar") == 0 || strcmp(getchild(expression, 0)->token, "getchar") == 0){
                expression->type = integer_type;
                break;
            }

            //ver se são o mesmo número de parametros e argumentos
            if(args_expected != args_gotted){
                //se nao for
                printf("Line %d, column %d: Wrong number of arguments to function %s (got %d, required %d)\n", expression->token_line, expression->token_column, getchild(expression, 0)->token, args_gotted, args_expected);
            }
            else{
                //se for

                //reset ao child2 (para estar nos parametros)
                child2 = expression->children;
                child2 = child2->next;

                //reset a symbol (para estar nos parametros)
                for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next){
                    //encontrar identifier
                    if(strcmp(symbol->identifier, getchild(expression, 0)->token) == 0){
                        break;
                    }
                }

                //Ver se matcham <3
                while((child2 = child2->next) != NULL){

                    if(symbol->next != NULL && symbol->next->func == 1){
                        //ver se os tipos matcham
                        if(symbol->next->type != child2->node->type){
                            //printf("Line ?, column ?: Conflicting types (got %s, expected %s)\n", type_name(child2->node->type), type_name(symbol->next->type));
                        }
                        symbol = symbol->next;
                    }
                }      
            }
            break;
        case Comma:
            check_expr(getchild(expression, 0), function_type);
            check_expr(getchild(expression, 1), function_type);

            // Determine the result type based on operand types
            type1 = getchild(expression, 0)->type;
            type2 = getchild(expression, 1)->type;

            // Handle integer and double promotions
            expression->type = type2;
            break;
        case StatList:
            child1 = expression->children;
            
            while((child1 = child1->next) != NULL){
                check_expr(child1->node, function_type);
            }
            break;
        default:
            //printf("DEFAULTTTT\n");
            break;
    }
}

//--
void check_statement(struct node *statement) {

    switch(statement->category){
        case If:

            //Expr Comma
            check_expr(getchild(statement, 0), no_type);

            if ( getchild(statement, 0)->type == double_type || getchild(statement, 0)->type == undef_type || getchild(statement, 0)->type == void_type){
                printf("Line %d, column %d: Conflicting types (got %s, expected int)\n", getchild(statement, 0)->token_line, getchild(statement, 0)->token_column,  type_name(getchild(statement, 0)->type));
            }
            
            //Statement
            check_statement(getchild(statement, 1));

            break;

        case While:
            
            //Expr Comma
            check_expr(getchild(statement, 0), no_type);

            //Statement
            check_statement(getchild(statement, 1));
            
            break;

        case Return:

            //Expr Comma
            check_expr(getchild(statement, 0), return_type);

            break;
        default:

            check_expr(statement, no_type);
            break;
    }
}

//--
void check_function_declaration(struct node *function_declaration) {
    //printf("FUNC DEC???\n");

    int param= 0;
    scope1 = (struct symbol_list *) malloc(sizeof(struct symbol_list));


    //Temp variables
    struct node *paramLstErrors = getchild(function_declaration, 2);
    struct node_list *paramDecErrors = paramLstErrors->children->next;

    //Check for void types
    while(paramDecErrors != NULL){
        //Identifier
        if(getchild(paramDecErrors->node, 1) != NULL){
            enum type type = category_type(getchild(paramDecErrors->node, 0)->category);
            if(type == void_type){
                printf("Line %d, column %d: Invalid use of void type in declaration\n", getchild(paramDecErrors->node, 0)->token_line, getchild(paramDecErrors->node, 0)->token_column);
                return;
            }
        }
        else{
            //Only typespec
            struct node *typespec = getchild(paramDecErrors->node, 0);
            enum type type = category_type(getchild(paramDecErrors->node, 0)->category);

            if( type == void_type){
                printf("Line %d, column %d: Invalid use of void type in declaration\n", typespec->token_line, typespec->token_column);
                return;
            }
        }
        paramDecErrors = paramDecErrors->next;
    }
    
    //Typespec --

    //Function Declarator --
    //Identifier
    struct node *id = getchild(function_declaration, 1);
    enum type type = category_type(getchild(function_declaration, 0)->category);

    if(search_symbol(scope1, id->token) == NULL) {
        //inserimos na symbol table
        insert_symbol(symbol_table, id->token, type, function_declaration, 0);
        //inserimos na scope table
        insert_symbol(scope1, id->token, type, function_declaration, 0);
    } else {
        //printf("Line %d, Column %d: Identifier %s already declared\n", id->token_line, id->token_column, id->token);
        semantic_errors++;
    }

    //Parameter List
    struct node *paramLst = getchild(function_declaration, 2);
    struct node_list *paramDec = paramLst->children->next;
        
    //Parameter Declaration
    while(paramDec != NULL){
        //Typespec
        //check_typespec(paramDec->node);

        //Identifier
        if(getchild(paramDec->node, 1) != NULL){
            struct node *id = getchild(paramDec->node, 1);
            enum type type = category_type(getchild(paramDec->node, 0)->category);

            if(search_symbol(scope1, id->token) == NULL) {
                //inserimos na symbol table
                insert_symbol(symbol_table, id->token, type, paramDec->node, 1);
                //inserimos na scope
                insert_symbol(scope1, id->token, type, paramDec->node, 0);
                param++;
            } else {
                //printf("Line %d, Column %d: Identifier %s already declared\n", id->token_line, id->token_column, id->token);
                semantic_errors++;
            }
        }
        else{
            //Typespec
            sprintf(str_token, "%d", invented_token);
            invented_token++;
            enum type type2 = category_type(getchild(paramDec->node, 0)->category);
            //inserimos na symbol table
            insert_symbol(symbol_table, str_token, type2, paramDec->node, 1);
        }
        paramDec = paramDec->next;
    }

    /* eliminar cenas da scope 1 pff
    if(func_dec_error==0){
        add_scope(scope1, 0);
    }
    */
}

//--
void check_function_definition(struct node *function_definition) {
    
    int param= 0;
    int already_exists=0;
    scope2 = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    
    //Typespec --
    switch(getchild(function_definition, 0)->category){
        case Char:
            return_type = char_type;
            break;
        case Int:
            return_type = integer_type;
            break;
        case Void:
            return_type = void_type;
            break;
        case Short:
            return_type = short_type;
            break;
        case Double:
            return_type = double_type;
            break;
        default:
            break;
    }

    //Function Declarator --
    //Identifier
    struct node *id = getchild(function_definition, 1);
    enum type type = category_type(getchild(function_definition, 0)->category);

    //fzr uma funcao disto - - - - - -
    struct scope_node *current_scope = scope_list;

    while (current_scope != NULL) {
        struct symbol_list *current_symbol = current_scope->scope->next;
        while (current_symbol != NULL) {      
            if(strcmp(current_symbol->identifier, id->token) == 0){
                already_exists=1;
                break;
            }
            current_symbol = current_symbol->next;
        }
        current_scope = current_scope->next;
    }
    //- - - - - - - - - - - - - - - - -   

    //Caso ainda nao tenha sido declarada
    if(search_symbol(symbol_table, id->token) == NULL) {
        //inserimos na symbol table
        insert_symbol(symbol_table, id->token, type, function_definition, 0);
        //inserimos na scope table
        insert_symbol(scope2, id->token, type, function_definition, 0);
        already_exists=2;
    }
    else if(search_symbol(symbol_table, id->token) != NULL && already_exists==0){
        //Caso ja tenha sido declarada (esteja na global mas nao em nenhuma scope)
        //inserimos na scope table
        insert_symbol(scope2, id->token, type, function_definition, 0);
        already_exists=3;
    }
    else {
        //Erro
        printf("Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
        error++;
        semantic_errors++;
    }
    
    if (!error){
        //Parameter List
        struct node *paramLst = getchild(function_definition, 2);
        struct node_list *paramDec = paramLst->children->next;

        //Parameter Declaration
        while(paramDec != NULL){

            //Identifier
            if(getchild(paramDec->node, 1) != NULL){
                struct node *id = getchild(paramDec->node, 1);
                enum type type = category_type(getchild(paramDec->node, 0)->category);

                if(search_symbol(scope2, id->token) == NULL) {

                    //Caso ainda nao tenha sido declarada
                    if(already_exists==2) {
                        //inserimos na symbol table
                        insert_symbol(symbol_table, id->token, type, paramDec->node, 1);
                        //inserimos na scope
                        insert_symbol(scope2, id->token, type, paramDec->node, 0);
                        param++;
                    }
                    else if(already_exists==3){
                        //Caso ja tenha sido declarada (esteja na global mas nao em nenhuma scope)
                        //inserimos na scope
                        insert_symbol(scope2, id->token, type, paramDec->node, 0);
                        param++;
                    }
                } else {
                    //printf("Line %d, Column %d: Identifier %s already declared\n", id->token_line, id->token_column, id->token);
                    semantic_errors++;
                }
            }
            else{
                //Typespec
                sprintf(str_token, "%d", invented_token);
                invented_token++;
                enum type type = category_type(getchild(paramDec->node, 0)->category);
                //inserimos na symbol table
                insert_symbol(symbol_table, str_token, type, paramDec->node, 1);
                
            }
            paramDec = paramDec->next;
        }

        //Function Body --
        if(getchild(function_definition, 3) != NULL){

            //Declarations and Statements
            struct node_list *child = getchild(function_definition, 3)->children;
        
            while((child = child->next) != NULL){
                if (!child->node)
                    continue;
                //Declaration
                if(child->node->category == Declaration){
                    check_declaration(child->node, 1);
                }
                //Statement
                else{
                    check_statement(child->node);
                }
            }
        }

        add_scope(scope2, param);
    }

    error = 0;
    return_type = no_type;
}

//--
int check_program(struct node *program) {
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->next = NULL;    
    struct node_list *child = program->children;
    
    while((child = child->next) != NULL)
        
        //Global symbol table
        if(child->node->category == Declaration){
            check_declaration(child->node, 0);
        }
        //Scope symbol table
        else if(child->node->category == FuncDeclaration){
            check_function_declaration(child->node);
        }
        else if(child->node->category == FuncDefinition){
            check_function_definition(child->node);
        }

    return semantic_errors;
}

struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, struct node *node, int func) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->func = func;
    new->next = NULL;
    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;
            break;
        }
        symbol = symbol->next;
    }
    return new;
}

struct symbol_list *search_symbol(struct symbol_list *table, char *identifier) {
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next)
        if(strcmp(symbol->identifier, identifier) == 0)
            return symbol;
    return NULL;
}

void show_symbol_table() {
    int has_types=0;
    
    printf("===== Global Symbol Table =====\nputchar\tint(int)\ngetchar\tint(void)");
    struct symbol_list *symbol;
    for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next){
        //printf("\n\nTEST %s\t%s\t%d\n\n", symbol->identifier, type_name(symbol->type), symbol->func);
        //function parameters
        if(symbol->func == 1){
            //Only one
            if(has_types==0 && (symbol->next == NULL || symbol->next->func==0)){
                printf("(%s)", type_name(symbol->type));
                has_types=0;
            }
            else if(has_types==0){
                //First one
                printf("(%s", type_name(symbol->type));
                has_types=1;
            }
            else{
                //Last one
                if(symbol->next == NULL || symbol->next->func==0){
                    printf(",%s)", type_name(symbol->type));
                    has_types=0;
                }
                else{
                    //Middle ones
                    printf(",%s", type_name(symbol->type));
                } 
            }
        }
        else{
            //normal stuff
            printf("\n%s\t%s", symbol->identifier, type_name(symbol->type));
        }
    }

    printf("\n");

    //Temporary scope print --
    struct scope_node *current_scope = scope_list;

    while (current_scope != NULL) {
        struct symbol_list *current_symbol = current_scope->scope->next;
        int param= current_scope->param;

        printf("\n===== Function %s Symbol Table =====\nreturn\t%s\n", current_symbol->identifier, type_name(current_symbol->type));
        current_symbol = current_symbol->next;

        while (current_symbol != NULL) {
            
            if(param != 0){
                printf("%s\t%s\tparam\n", current_symbol->identifier, type_name(current_symbol->type));
                param--;
            }
            else{
                printf("%s\t%s\n", current_symbol->identifier, type_name(current_symbol->type));
            }

            current_symbol = current_symbol->next;
        }

        current_scope = current_scope->next; // Move to the next scope
        //printf("\n");
    }   
}
