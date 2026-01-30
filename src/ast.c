#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

int function_types=0;
char f_types_string[200000]= "";

// create a node of a given category with a given lexical symbol
struct node *newnode(enum category category, char *token) {
    struct node *new = malloc(sizeof(struct node));
    new->category = category;
    new->token = token;
    new->type = no_type;
    new->children = malloc(sizeof(struct node_list));
    new->children->node = NULL;
    new->children->next = NULL;
    return new;
}

void adoptChild(struct node *parent, struct node *aux){
    struct node_list *aux_children = aux->children;
    struct node_list *parent_children = parent->children;

    while(parent_children->next != NULL){
        parent_children = parent_children->next;
    }
    parent_children->next = aux_children->next;
}

// append a node to the list of children of the parent node
void addchild(struct node *parent, struct node *child) {
    struct node_list *new = malloc(sizeof(struct node_list));
    new->node = child;
    new->next = NULL;
    struct node_list *children = parent->children;
    /*if(children->node == NULL){
        children = new;
        printf("ADDING CHILD TO %d\n", children->node->category);
        return;
    }*/
    while(children->next != NULL)
        children = children->next;
    children->next = new;
}

// get a pointer to a specific child, numbered 0, 1, 2, ...
struct node *getchild(struct node *parent, int position) {
    struct node_list *children = parent->children;
    while((children = children->next) != NULL)
        if(position-- == 0)
            return children->node;
    return NULL;
}

// count the children of a node
int countchildren(struct node *node) {
    int i = 0;
    while(getchild(node, i) != NULL)
        i++;
    return i;
}

const char *category_names[] = {
    "Program",
    "Declaration",
    "FuncDeclaration",
    "FuncDefinition",
    "ParamList",
    "FuncBody",
    "ParamDeclaration",
    "StatList",
    "Or", "And", "Eq", "Ne",
    "Lt", "Gt", "Le", "Ge",
    "Add", "Sub", "Mul", "Div",
    "Mod", "Not", "Minus", "Plus",
    "Store", "Comma", "Call", "BitWiseAnd",
    "BitWiseXor", "BitWiseOr", "Char", "ChrLit",
    "Identifier", "Int", "Short", "Natural",
    "If", "Else", "While", "Return",
    "Double", "Decimal", "Void", "Null", "Aux", "Error"
};


// Function to traverse the AST and print its content
void show(struct node *node, int depth) {

    if (node == NULL) {
        return; // If the node is NULL, return (base case for recursion).
    }

    // Print the current node with indentation based on its depth
    for (int i = 0; i < depth; i++) {
        printf("..");
    }

    // Print the category of the node and its token (if available)
    printf("%s", category_names[node->category]);
    if (node->token != NULL) {
        printf("(%s)", node->token);
    }

    //print type
    if(node->type != no_type){
        //putchar/getchar
        if(node->type == putchr_type){
            printf(" - int(int)");
        } else if(node->type == gtchr_type){
            printf(" - int(void)");
        } else{
            printf(" - %s", type_name(node->type));
        }
    }

    if(function_types==1){
        if(node->type != putchr_type && node->type != gtchr_type){
            printf("%s", f_types_string);
            strcpy(f_types_string, "\0");
        }
        function_types=0;
    }

    printf("\n");

    // Special handling for the "Return" statement
    if (node->category == Return && node->children != NULL) {
        struct node_list *return_children = node->children->next;
        while (return_children != NULL) {
            if (return_children->node->category == Identifier && return_children->node->type != no_type) {
                // If it's a global variable, explicitly print its type with appropriate indentation
                for (int i = 0; i <= depth; i++) {
                    printf("..");
                }
                printf("%s(%s) - %s\n", category_names[return_children->node->category],
                       return_children->node->token, type_name(return_children->node->type));
            } else {
                show(return_children->node, depth + 1);
            }
            return_children = return_children->next;
        }
    } else if (node->category == Call && node->children != NULL) {
        function_types=1;

        struct node_list *call_children = node->children;

        // Check if there is an "Identifier" child (function identifier)
        struct node *identifier_child = getchild(node, 0);
        if (identifier_child != NULL && identifier_child->category == Identifier) {
            //printf(" - %s\n", type_name(identifier_child->type)); // Print the type of the function

            // Check if there are parameters
            struct node_list *parameters = node->children->next;
            parameters = parameters->next;
            if (parameters != NULL && parameters->node != NULL) {
                strcat(f_types_string, "(");

                while (parameters != NULL) {
                    // Print the type of each parameter
                    //printf("%s", type_name(parameters->node->type));
                    strcat(f_types_string, type_name(parameters->node->type));

                    parameters = parameters->next;

                    // If there are more parameters, print a comma
                    if (parameters != NULL) {
                        strcat(f_types_string, ",");
                    }
                }

                strcat(f_types_string, ")\0");
            }
            call_children = call_children->next; // Skip the "Identifier" child
            while (call_children != NULL) {
                show(call_children->node, depth+1);
                call_children = call_children->next;
            }
        } else {
            // If there is no function identifier, simply traverse the children
            while (call_children != NULL) {
                show(call_children->node, depth + 1);
                call_children = call_children->next;
            }
        }
    } else {
        // Traverse the children of the node and call the 'show' function recursively
        struct node_list *children = node->children;
        while (children != NULL) {
            show(children->node, depth + 1);
            children = children->next;
        }
    }
}

// free the AST
void deallocate(struct node *node) {
    if(node != NULL) {
        struct node_list *child = node->children;
        while(child != NULL) {
            deallocate(child->node);
            struct node_list *tmp = child;
            child = child->next;
            free(tmp);
        }
        if(node->token != NULL)
            free(node->token);
        free(node);
    }
}
