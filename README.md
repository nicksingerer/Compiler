# Compiler

## Description

Simple Compiler in C++. Just look trough it starting from main.cpp.  

## Grammar

$$
\begin{align}
[\text{Program}] & \to  [\text{Scope}] \\
[\text{Scope}] & \to [\text{Statement}]^*  \\
[\text{Statement}] & \to \begin{cases}
	exit\text{ [Expression];}  \\
	let\text{ identifier  = [Expression];}  \\
	if\text{ [Expression] [Statement] } (\text{else [Statement]})^{(0-1)} \\
	\{[\text{Scope}]\}
\end{cases}  \\
[\text{Expression}] & \to  \begin{cases}
	\text{integer}  \\
	\text{([Expression])} \\
	\text{[Term]} \\
	\text{identifier}
\end{cases}  \\
[\text{Term}] & \to  \begin{cases}
	\text{[Expression] * [Expression]}  & \text{prec = 2} \\
	\text{[Expression] / [Expression]}  & \text{prec = 2} \\
	\text{[Expression] + [Expression]} & \text{prec = 1}  \\
	\text{[Expression] -- [Expression]} & \text{prec = 1}
\end{cases}
\end{align}
$$

## Example

``` Javascript
// Variables
let x = 3;

// + - * /
let y = 2 * (x-2) + 4 / 2;

// Round Brackets are optional
if (0) {
    let exitCode = 123;
    exit(exitCode);
}
if 0 {
    let exitCode = 124;
    exit exitCode;
}

{
    let veryUsefulScopes = 1;
}

// If One Liners:
if 0 exit 0;
if 1 let z = x + y;

// If One if One if One Liners
if 1 if 1 if 0 exit (3);
// :)
```
