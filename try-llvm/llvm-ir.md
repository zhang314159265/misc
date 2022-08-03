Global identifiers starts with '@'; local identifiers starts with '%'.

# NOTE
- clang can compile llvm-IR like it can compile C programs.
- the majority of the LLVM LangRef are talking about instructions and llvm intrintics because there are so many of them.

# Hands on Process to Learn LLVM IR
1. write simple C program
2. using clang to compile the C program to llvm IR and learn the generated IR
3. follow the generated IR, manually write the llvm IR doing the same thing
4. compile and run the LLVM IR written manually.

More examples to try:
- prime sieve
- gray sequence

# Get Familiar with LLVM APIs that Manipulate IR
Use the LLVM IR manipulation APIs to 'write' simple programs. Code under irapi/ .
- showmsg.cpp [DONE]
- sum.cpp [DONE]
- ifelse.cpp [DONE]
- `ifelse_phi.cpp` [DONE]
- `ident_mat.cpp` [DONE]
- matmul.cpp -- [DONE]

Advanced feature: it's very interesting to JIT run the generated IR.
