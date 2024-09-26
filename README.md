# Compiler

## Grammar

$$
\begin{align}
[\text{Program}] & \to  [\text{Scope}] \\
[\text{Scope}] & \to [\text{Statement}]^*  \\
[\text{Statement}] &\to  \begin{cases}
	exit\text{ [Expression];}  \\
	let\text{ identifier  = [Expression];}  \\
	if\text{ [Expression] [Statement] }  \\
	\{[\text{Scope}]\}
\end{cases}  \\
[\text{Expression}] &\to  \begin{cases}
	\text{integer }  \\
	\text{([Expression])} \\
	\text{[Term]} \\
	\text{identifier}
\end{cases}  \\
[\text{Term}] &\to  \begin{cases}
	\text{[Expression] * [Expression]}  & \text{prec = 2} \\
	\text{[Expression] / [Expression]}  & \text{prec = 2} \\
	\text{[Expression] + [Expression]} & \text{prec = 1}  \\
	\text{[Expression] -- [Expression]} & \text{prec = 1}
\end{cases}
\end{align}
$$
