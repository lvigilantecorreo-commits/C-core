# C-Core

![image alt](https://github.com/lvigilantecorreo-commits/C-core/blob/50e86e5896e16c760da9fcaef25365b0ae08ca15/C-Core_logo.png)

A minimal compiler I'm writing from scratch in C++. It takes a small C-like language I made up (C-Core) and outputs real x86-64 assembly for Windows, which then gets assembled with NASM and linked into an actual .exe. No LLVM, no libraries doing the heavy lifting, no bytecode VM. Text goes in, machine code comes out.

I started this because I wanted to actually understand how compilers work instead of just using them. Reversing binaries in Ghidra got me curious about the other direction: how does source code become the assembly I keep reading? So I started building the answer.

## What works right now

- Integer variables (`int x = 5;`) with proper stack allocation
- Arithmetic: `+ - * /` with correct operator precedence
- Comparisons: `< > == != <= >=`
- `if` / `else`
- `while` loops
- `for` loops (parsed, generator pending)
- Functions with parameters and return values (`int suma(int a, int b) { return a + b; }`)
- Function calls, including nested ones as arguments (`suma(cuadrado(3), z)`)
- Strings (parsed, no runtime support yet since there's no print)
- Basic error recovery in the parser (panic mode: skip to next `;` on parse errors)

Everything above is verified end-to-end: source → tokens → AST → NASM → linked .exe → correct exit code.

## Example

int cube(int n) {
return n * n;
}

int plus(int a, int b) {
return a + b;
}

int x = 10;
int y = 3;
int z = x + y * 2;

if (z > 15) {
z = z - 5;
}

int r = plus(cube(3), z);
r;


Compiles, runs, exits with code 20. The whole pipeline works.

![image alt](https://github.com/lvigilantecorreo-commits/C-core/blob/10fc05a52776954e86f42d795d8de77f18a0c94a/asmoutput1.png)

![image alt](https://github.com/lvigilantecorreo-commits/C-core/blob/e11119c63a5ecbb910e1d298112a7d00184cc7ff/asmoutput2.png)

## How it's built

Three phases, one file each, kept intentionally simple:

- **Lexer** (`lexer.cpp`) turns raw text into tokens (keywords, identifiers, numbers, symbols, string literals). Handles multi-char operators (`==`, `!=`, etc), line comments, unterminated string errors.
- **Parser** (`parser.cpp`) builds an AST out of the tokens using recursive descent. Precedence is handled by layering the functions: `ParseComparison` calls `ParseSum` calls `ParseMultiplication` calls `ParseNum`. Symbol table is a stack of maps for nested scopes.
- **Code generator** (`codegen.cpp`) walks the AST and emits NASM. Result of every expression ends up in `rax`. Sub-expressions use `push`/`pop` to avoid stepping on each other. Functions follow Windows x64 calling convention (first args in `rcx`, `rdx`).

Main writes the assembly to `output.asm` in two parts: `main`'s body first, then all the function definitions after the `ExitProcess` call, so execution doesn't fall through into a function body by accident.

## Building and running

You need NASM and the Microsoft linker (`link.exe`) on your PATH. Easiest way on Windows is to open the "x64 Native Tools Command Prompt for VS" so `link.exe` is available.

![image alt](https://github.com/lvigilantecorreo-commits/C-core/blob/73601e6ba3a520e75f24dcf62ba6ce9206356777/testccore.png)

## Roadmap

Stuff I know is missing or broken and want to get to:

- `coutf` (print) — main priority, so programs can actually say something instead of just returning an exit code
- Input from keyboard
- Proper `%` (modulo) operator
- `++` for the `for` loop step
- Better error messages with line numbers
- Multi-file source support
- Eventually: a small visual inspector for the generated ASM (this is the long-term vision, kind of like a mini Ghidra listing showing the code stepping through)

This is my first real C++ project of any size (before this I could write a program that checked a hardcoded password, that was about it). It's not perfect and I know parts of it are messy — I'll keep improving it as I learn more. The point was to build the whole thing myself and understand every line, and that part I got.

## License

MIT.

— Ader
