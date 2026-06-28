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
- No pointers: No aliasing unless explicitly assigned as such.

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

That being said, making Sofia would not be easy. 

For example, imagine a vector of integers. 
Other languages could add parametric types or template. 
Sofia, being a true dynamic language that prizes regularity, cannot do that. 
Instead, it would use sometime like this multi-step approach:
1) Dict to struct using access constant folding. 
2) AOS to SOA. 
3) The type array to constant array. 
4) The type-related things folded away. 
5) Fold away the rest of the overhead. 

Another example could be the control flow, of which there is only one, eval, where recursion, continuation, while loop, for loop, etc, come out of it. Let the tail recursion and so on discover the pattern itself uniformly.

These won't be easy, but it's simply the principled way to do things. 

Addendum:

Now, abstract interpretation might sound like a simple mechanism, but it's powerful, and Sofia is principled. 

Sofia doesn't hardcode sqrt(x).
Sqrt(x) is treated as inverse(a->a*a, a -> a>=0,x)

Sofia doesn't just slap a bunch of heuristics for loop variables. It could model the loop variables as linear algebra update rules then use eigenvalues to analyze how many times you need to unroll. (4 times if eigenvalue = i for example).

Sofia would allow a strong reasoning engine, made of induction fixed point (properties that hold over the first iteration then iteratively weaken the property until a fixed point is reached), using tools like OR-tools to get the best possible bounds for these properties.

Sofia goal is to generate efficient code, not closed-forms solution, so it's not like the symbolic algebra.

However, Sofia's reasoning engine is powerful. Here's a draft test case.

Prove infinitude of primes.

isprime(N) = none(N%i == 0 for i = 2 to N-1)

combine_proof 

abstract_interp_proof( all(
isprime(x) || (factorial(N)+1)%x !=0 for x = N to factorial (N)))

# To be proven by residual class reasoning engine. The basic idea is that if not isprime(x), then the residual class of x wrt some i (from 2 to N-1) is zero. The residual class of factorial(N)+1 wrt to i is always 1. 
Modulo by something with residual class wrt i equal 0 doesn't change the residual class wrt i, which means it must be 1.

 :

abstract_interp_upper_bound(return x if isprime(x) for x=N to factorial(N)+1 else return inf) <= factorial(N)+1

#Again, let the reasoning divide the domain and show either case.

: 

abstract_interp_upper_bound (factorial(N)+1) < inf)

end

Sofia language goals.

-Accessibility: What could be logically described in a few lines of equations should actually take a few lines of code to get running. 

-Performance without contortion: But accessibility alone often forces rewrite in higher-performance languages like C or contorting the code to something like Numpy. This is why Sofia needs to be performant. 

-Regularity: Sofia code should not be plagued by special cases, for it harms the programmer, the compiler, and any functional metaprogram such as autograd.
To be able to compose languages, there should be no need for "json/yaml/etc programming". There should only be Sofia. Whatever DSL you write should be in Sofia. 



- Elegance: The solution should look clean and natural from the chosen reference frame for anyone familiar with it. We cannot guarantee that you feel that using eigenvalue to analyze loop behavior feels easiest, but we can confidently believe that if you're knowledgeable in Linear Algebra, the solution looks obvious. The user should not be forced to manually translate this obvious definition to something to be executed.


Addendum 2: memory management draft.

This is Sofia's memory management model.

Sofia has an automatic memory management model based on 3 primary mechanisms and a lot of heuristics.

Remember that Sofia has no memory guarantee, only that what comes out equals to what goes in.

1) Memory region and escape analysis.
Sofia would create a memory region. Everything that lives within the scope will get used here. 

2) Reference counting.
Reference counting typically is expensive and consumes a lot of memory, so, why is it selected? Because of Sofia's aggressive constant folding. You can prove invariants about the reference count that then allows you to "mark" early for cleanup or reuse. However, Sofia doesn't always immediately clean up, for that fragments memory. Instead, it is simply a hint that the memory slot is vacant. For large objects like arrays, however, this could be significant.

Refcount also has another form that could be triggered by the compiler analysis, the group refcount. In this process, the reference is not measured by each object, but by the entire group. Even if there is a cycle inside, if the entire group has zero reference, it can also be cleaned up. The art of analyzing what group to draw is left to the compiler.

3) Tracing GC. 
This doesn't trigger randomly, only when the compiler reasons that dead objects are sufficient. The chosen algorithm is rather simple. Copy the live objects to a new region and free the entire region. This is a moving GC (because again, Sofia doesn't have a memory location guarantee) which is expensive, but also doubles as a memory defragmentation process.

A lot more compiler optimizations is also possible here. For example, if the compiler can prove the objects form directed acyclic graph, this tracing GC can be eliminated in favor of refcount.

This is normally triggered when a large object goes out of the scope but the refcount can't clean everything up and there are still  some live objects. Can also be called even when not strictly needed to defragment objects.

This is just the basics. There are probably a lot more heuristics at play. 

If there is no escaping object, then the memory region is sufficient. However, refcount allows a more efficient use of the memory region when possible while the tracing one preserves the escaping object.


The first version wouldn't ship with these optimizations, just the basics. The optimizations would be in the libraries.

Addendum 3: concurrency and parallelism.

Sofia is a compute-bound programming language. For such tasks, for each stage, the task is either fully independent, or dependent in the dynamic programming sense, both of which can be modeled through dependency analysis. 

For different tasks, I would rather have people spin up different Sofia process that communicates via IO-like channel IO or events. 

I would therefore propose the most radical design yet...

Sofia thread is optional and can be automatic. Let the compiler allocate thread workers for you up to the maximum amount. For example... if you had... 

spawn(task1)
spawn(task2)
...
in Sofia, but task 1 could make use of many threads, then Sofia might decide not to start task2 right away.

If there are genuinely many tasks, then use processes, not threads.

Interfaces with other processes are done through channels or events (producer-consumer or broadcast). 

Interfaces with shared memory in different processes are done through transaction models. 

Addendum 3: Where does the performance come from? 

Aside from allowing the compiler to not halt and the structure of the compiler that lets the compiler spend effort on code roughly proportional to the runtime cost of running those code, Sofia has a significant edge against languages like C by exploiting the loose semantics

Strengths:

No crazy aliases. Only aliases are direct assignments.
Free memory model. The compiler can do whatever it takes to optimize.
GC also qualifies as above.
No type system. The compiler is allowed to treat types as yet another kind of value.
Again, no layout issues. Structs force certain layouts. Dict-like semantics allow free reordering.
User base is more relaxed on getting a legible compiler. The compiler is allowed to be as opaque as it wants if it generates fast code.

Weaknesses:

No easy type erasure. Types may stay in the runtime if you can't infer types.
No fine-grained control over memory access.
No type-based information about data that may be useful.
Dynamic layout, which could be a huge burden.
GC-like semantics that could complicate analysis or leave residual overhead.

However, abstract interpretation minimizes the weaknesses and maximizes the strength.

Addendum 4: 

I used Futamura projection loosely. More likely, it would simply be optimizing and hoisting out the interpretation overhead and generating code that performs what was determined by the interpreter but in the outer loop or even outside the program execution instead.