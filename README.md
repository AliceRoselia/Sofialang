Update: My first attempt officially failed. I am still not good enough at handling complex system. I will return.

Sofia language is based on this trifecta.

Sofia supports a kind of metaprogramming called interpreter metaprogramming where you interpret any code using a custom interpreter. Now, unlike macros, which only know the syntax of the expressions, interpreters know the functions called, how they were implemented, the runtime type and value, and so on. The idea is that you write the interpreter that interprets Sofia in a nonstandard way.

Now, that alone might be slow, but let's figure out what interpreter metaprogramming enables. It enables abstract interpretation by overriding. For example, instead of interpreting numbers, you can use interval or residual arithmetic instead. That basically is abstract interpretation on steroid. Instead of complex symbolic analysis, just interpret the code in different ways.

But what does that enable? Abstract interpretation is constant propagation on steroid. Just that instead of absolute constants, you can take a range and so on.

But what does constant propagation enable? Futamura projection. This turns interpreters into a compiler and, in this case, with interpreter layers, optimizing compilers.

These close the loop and create an expressive, performant language.

Here is the general philosophy behind.

The bitter lessons in compilers:

In a lot of programming languages, people want abstractions and features. If that's all they want, they can be implemented at runtime in any language, but they also want those abstractions to be optimized away so the programs could still be fast. So, naturally, they add and proliferate a lot of concepts into the compilers. Initially, this speeds up some use cases, but it complicates the language, and now every new compiler patch must support this new feature, until features accumulate and compiler development grinds to a halt.  Moreover, adding these concepts eat away precious compile time budget that the compiler can use to optimize code. This has to change. Just because an abstraction is implemented with the same API as runtime code doesn't mean the compiler will fail to optimize it away. Just because types aren't treated as special things, but as values, doesn't mean that the compiler would fail to infer types. Giving low-level control to the programmers, for example, are tempting, because they allow benchmarks to be optimized, and they will be faster, but then, the entire compiler needs to obey by these guarantees, creating a web of constraints it must navigate. General optimizations work, and if other optimizations can be implemented in term of it, you should. One useful concept in particular is adding abstract interpretation to the semantic. This allows features like garbage collection to not only be implemented, but allow them to tell the compilers how they can be optimized away. This will not initially be faster than tailored solutions because these are optimized for the benchmarks at hands, but long term, this will work much better for general language use cases. A programming language is a general-purpose software, and thus demands general-purpose solutions. This is not the first time we've had this lesson. This type of lesson was learned the hard way in Risc-V, chess, and so on. It's time compilers learn too.

Drawing from these principles, these would be the features. The key uniqueness is that while many languages are fast DESPITE it, Sofia aims to be fast BECAUSE OF it.

- Dynamic typing: faster because types can be folded away without needing special compile-time handling.
- Eval/interpreter: Using Futamura projection and higher-order specialization, one can meta-optimize the optimization passes or specialization passes to effectively make a more performant compiler. (Basically, this language loves compiler optimization so much that the compiler optimization has its own compiler optimization.)
- Attribute dict semantic: Optimized into efficient data storage, whether SOA, AOS, or anything else. Potentially more efficient than rigid struct system.
- Garbage collection semantic: Gives the control over memory management to the compiler, allowing it to more efficiently manage memory, whether by moving memories around for cache efficiency, re-using memory, and so on.

And it aims to be able to do so with its extreme capability enabled by the trifecta.

As for the difficulty of doing Futamura projection correctly, I would understand that a language with an interpreter-handled type system, a bunch of complex, hard-coded (in another complex language) interpreter logic, and so on would make Futamura projection hard. However, Sofia's base layer is very simple. There is no type at all. Everything would be treated as one of a few kinds of raw data, raw numbers, dictionaries, and perhaps (but not necessarily) functions, and maybe a few more, only as necessary. This means that there is no type at the language level. So, where did the type come from? Simple, it comes from the emergence property of dictionary itself. You could have an object of type "int", for example, as a dictionary {type:int, value:5} for example. This would be the bootstrapped type system that would get compiled away to the level lower than that of even languages like C. It would be optimized straight to typeless data.

Now, another issue is with the slow partial evaluation/abstract interpretation that may not halt. How do you deal with it? First of all, the abstract interpretation are not just optimization pass, but also real semantics your code can reason about (for example, your code can even check if it was being interpreted abstractly or concretely and so on). Now, what does this mean? It means that if the original code doesn't halt, the abstract interpreter does not need to halt either. It is part of the semantic/runtime in another interpretation pass. This means that an abstract interpreter can be extremely aggressive. Instead of doing slow analysis on a constant expression, for example, it could just compile the expression and replace it with the result.

Practical implementation notes:

As for the practical implementation, I think I would go for a minimal C interpreter->self-hosted interpreter->multiple interpreters for different abstract interpretation passes->full optimizing compiiler.
 
As for the memory management, Sofia allows memory management policy to be built as a library and optimized away when possible (for example, by inferring the constants in the refcount, escape analysis allowing you to reclaim entire chunks of memory, compiler size hint, or other strategies written by memory allocator experts and so on. There would probably be optimized tracing/refcount/etc and a bunch of memory management libraries available as interpreter metaprograms.

The interpreter is a simple core because the language has no type.

The partial evaluator is simple because it doesn't have to worry about halting or such and could just run the program with a constant value concretely or abstractly to replace those values with the evaluated ones.

The abstract interpretation is simple because it doesn't use complicated analysis, just simple techniques that make use of emergent data type, like using interval arithmetic types for doing interval analysis or using residual class arithmetic for doing residual analysis. Again, these libraries are relatively easy to implement in high-level languages, which could be an issue for a low-level compiler, but since our bootstrap starts from a minimal yet complete interpreter, this shouldn't be an issue here.

Another objection: code size explosion from all the inlining, specialization, etc. Solution. Rethink the inlining mechanism entirely. Currently, hardware supports function calls at the instruction level, but the uses for such functions are to reduce the binary size and enable recursion. When it comes to programming languages, however, functions have been used for interface boundaries, separation of concerns, overriding, etc. You will notice that from the perspective of the hardware, human programmer's function might look very arbitrary. So, instead of inlining certain code and leaving certain functions as they are, the compiler could adopt code similar to LZ78 and so on, adapted with cost evaluation and so on, that inlines everything possible into a flat binary first before re-compressing them into functions for good i-cache usage at low indirection cost.