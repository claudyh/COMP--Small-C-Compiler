#ifndef _AST_H
#define _AST_H

enum category { Program,
              Declaration,
              FuncDeclaration,
              FuncDefinition,
              ParamList,
              FuncBody,
              ParamDeclaration,
              StatList,
              Or, And, Eq, Ne,
              Lt, Gt, Le, Ge,
              Add, Sub, Mul, Div,
              Mod, Not, Minus, Plus,
              Store, Comma, Call, BitWiseAnd,
              BitWiseXor, BitWiseOr, Char, ChrLit,
              Identifier, Int, Short, Natural,
              If, Else, While, Return,
              Double, Decimal, Void, Null, Aux, Error, Putchar, Getchar, Undef };

enum type {integer_type, double_type, char_type, short_type, void_type, no_type, putchr_type, gtchr_type, undef_type};
#define type_name(type) (type == integer_type ? "int" : (type == double_type ? "double" : (type == short_type ? "short" : (type == char_type ? "char" : (type == void_type ? "void" : (type == putchr_type ? "putchar" : (type == gtchr_type ? "getchar" : (type == undef_type ? "undef" : "none"))))))))
#define category_type(category) (category == Int ? integer_type : (category == Double ? double_type : (category == Short ? short_type : (category == Char ? char_type : (category == Void ? void_type : (category == Putchar ? putchr_type : (category == Getchar ? gtchr_type : (category == Undef ? undef_type : no_type))))))))

struct node {
    enum category category;
    char *token;
    int token_line, token_column;
    enum type type;
    struct node_list *children;
};

struct node_list {
    struct node *node;
    struct node_list *next;
};

struct node *newnode(enum category category, char *token);
void addchild(struct node *parent, struct node *child);
void adoptChild(struct node *parent, struct node *aux);
struct node *getchild(struct node *parent, int position);
int countchildren(struct node *node);
void show(struct node *root, int depth);
void deallocate(struct node *root);

#endif
