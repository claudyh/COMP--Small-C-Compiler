## UC Language Compiler
*in C programming language (Lex & Yacc)*

&nbsp;

### 📋 Project Description
---

This project consists of the development of a compiler for UC, a subset of the C language (based on the C99 standard). The compiler processes UC source code and performs lexical, syntactic, and semantic analysis, building an Abstract Syntax Tree (AST) and symbol tables to validate program correctness. The project was developed using Lex for lexical analysis and Yacc for syntax analysis, following a multi-stage compiler design approach.

&nbsp;

*Language Overview*

The UC language supports:
- Primitive data types (char, short, int, double, void)
- Arithmetic, logical, relational, and bitwise expressions
- Variable declarations and assignments
- Control flow structures (if, if-else, while)
- Function declarations and definitions
- Input/output using predefined functions getchar() and putchar()
- Single-line (//) and block (/* ... */) comments


&nbsp;

*Compiler Architecture*

The compiler is structured into independent but sequential phases, where each phase builds upon the results of the previous one.
1. Lexical Analysis: Converts source code into tokens
2. Syntax Analysis: Validates program structure and builds the AST
3. Semantic Analysis: Validates types, scopes, and symbol usage
4. Code Generation: Not implemented

&nbsp;

*Phase 1 - Lexical Analysis*

The lexical analyzer is implemented using Lex and is responsible for converting the input source code into a stream of tokens:
- Tokens are defined using regular expressions
- Supported tokens include identifiers, numeric literals, character literals, reserved keywords, operators, and delimiters
- Whitespace and line breaks are treated as separators
- Lexical errors are detected and reported with precise line and column information

This phase ensures that only valid lexical elements are passed to later stages of the compiler.

&nbsp;

*Phase 2 - Syntax Analysis*

The syntax analyzer is implemented using Yacc and validates the grammatical structure of UC programs:
- The original EBNF grammar was rewritten to support bottom-up parsing
- Operator precedence and associativity follow C language semantics
- Error recovery rules allow the compiler to continue analysis after syntax errors

During this phase, an Abstract Syntax Tree (AST) is built to represent the hierarchical structure of the program.

&nbsp;

**Abstract Syntax Tree (AST)** - During syntax analysis, the compiler builds an Abstract Syntax Tree (AST) that represents the hierarchical structure of the UC program in a simplified and semantic-oriented form. Each node corresponds to a relevant language construct, such as declarations, statements, or expressions, and stores its category, associated token (when applicable), inferred type, and a list of child nodes. Auxiliary nodes created during parsing are eliminated through child adoption, ensuring the AST remains compact and free of redundant structures. The tree can be recursively traversed and printed with indentation that reflects node depth, allowing a clear visualization of program structure and serving as the foundation for semantic analysis.

&nbsp;

*Phase 3 - Semantic Analysis*

The semantic analyzer validates the meaning of syntactically correct programs. This phase includes:

**Symbol Tables**
- A global symbol table containing functions and global declarations
- Per-function symbol tables for parameters and local variables
- Symbols store identifier names, types, and associated AST nodes

**Semantic Validation**
- Detection of undeclared or redeclared symbols
- Type compatibility checks in expressions and assignments
- Validation of function declarations and definitions
- Verification of function calls and argument counts
- Annotation of expression nodes in the AST with inferred types

&nbsp;

*Compiler Options*

The compiler executable (uccompiler) supports the following options:

**-l** - Executes lexical analysis only, printing tokens and lexical errors

**-t** - Executes syntax analysis, printing the AST if no syntax errors exist

**-s** - Executes semantic analysis, printing symbol tables and the annotated AST

&nbsp;

---

**NOTE:** This project implements a compiler for the UC language with fully functional lexical and syntax analysis, including AST construction and error recovery. The semantic analysis phase is largely implemented, covering symbol tables, type checking, and error detection, but some validations remain incomplete. The code generation phase (LLVM IR) was not implemented due to time constraints. This project was developed within the scope of a Computer Science course by Cláudia Torres and Diogo Silva.
