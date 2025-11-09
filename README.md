# About the project
This project was origianlly created for two purposes:
1. Test in practice my '_wild_' idea for supposidley faster sorting algorithm - alternative for quicksort, but using smaller, consecutive power's of 2, as another pivots
2. Let me practice some programming in **C** 😎

As it turned out, additionally I was also able to test my computer and patience in practice, as my long break from programming in this intermediate level (by that I mean, not completely barebone, like in Assembly, or in high level language as in Python) make it hard to eiher operate on the most basic level, or grab concepts generally.

## QuickSort with ... bits?

### Starting point
In normal Quicksort we do the followng:
1. **Pick value** - usually from the middle of our range, or as a medium value between edge values and middle number
2. **Swap all** values to have 2 groups - smaller numbers, and those which are equal or greater our picked value - aka. pivot
3. **Repeat** - For each of the group start the algorithm - continue until group contains only one element

### My idea for optimalizaiton:
And what if, instead of calculating pivot, we would split values the other way?
-> Let's then, for each new round, simply pick value which is half of the startig value - by default we pick value which is half as big, as the biggest possible number which we can store as an integer which our number of bits

So - if we use 64 bit long number, then we would start from:

`10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000`

then:

`01000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000`

then

`00100000 00000000 00000000 00000000 00000000 00000000 00000000 00000000`

...

`00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001`

### Does it work?
Technically - yes. It's still QuickSort - we just alter the way we pick value. However, after I did it, and saw similar
results to the one which are with normal quicksort I've also did my best to try to hack it and work'out something better. More on that below.

## What's inside
To judge my idea I've decided to test it with the proper QuickSort - for the other algorithm I've consulted with my buddy - ChatGPT 😉, - as some of the implementations which I saw on the internet weren't satisfying. I've ask it for an implementation and together we did some work to adjust it to my needs, make a draft for multiprocessing and do the same with my original idea - after first compilation worked, I've took things in my hands (mostly - segfault's were hunting me aaalllll, the wayyy - 🤐). Additionally, during development I've tried to implement other tactics to make it properly - dividie into folder, compile with `make`, etc. 

### Organizaiton
Project have the following elements:
```
quicksort
├─ README.md
├─ functions
│  ├─ __init__.h
│  ├─ functions.h
│  ├─ helpers.c
│  ├─ helpers.h
│  ├─ qs_std.c
│  └─ qs_std.h
├─ main.c
└─ makefile
```
In root folder - `quicksort` we have 3 folders - `functions` - contains headers, and `C` files with functions which are used by `main.c` program in main repo. For compilation, additional folders - `bin` and `obj` - will be created (they exist only after compilation). Lastly, we have the the typical - `.gitignore`, `makefile` and `README.md`.

### Compilation

In the repository folder run the following command:
```bash
make
```
**DONE** - If everthing works, and you have make installed this should do the trick. 

### Running

By default, program would test 6 different sorting algorithms, and approaches, by generating 6 identical tables with numbers and measuring time of sorting for the same input for each apprach. This will be done 100 times, and final results, as well as simple comparison will be shown. 

If user would like to alter data size, it's possible to execute compiled program with first parameter, as number of elements, which to keep in table - default value is `5000000`. Additionally, program accepts a second argument which can change number of retries - `100` by default.

It's possible but not easy to change type of numbers which are used - `long` by default. For that one would need to alter `functions/__init__.h` file, and modify `elem_t` type. Then, program must be recompiled. This can be easily achieved by running:

```bash
make clean
make
```

**Important!** - using different type than `unsigned` - regardless of size - might yield unpredictable results !

# Results

As of right now it seems that binary sorting can behave like quicksort, but it's not easy to make it more efficient. The behaviour depends on factors such like:
- numbers of elements
- size of elements

Main problem in approach, seems to be `AND` operation, which -> by design is comparing
numbers. It seems to be marginally slower, but at scale, it's visible.

## For `long` numbers (default)

### Small pool - 1000 elements
```
./bin/main 1000

============================================================
Benchmark (100 runs, 1000 elements)
------------------------------------------------------------
Binary (1 core):                     0.0065s | avg 0.000065s
Binary (multicore):                  0.0056s | avg 0.000056s
Binary + reduced range (1 core):     0.0052s | avg 0.000052s
Binary + reduced range (multicore):  0.0048s | avg 0.000048s
Clasical (1 core):                   0.0051s | avg 0.000051s
Clasical (multicore):                0.0041s | avg 0.000041s
------------------------------------------------------------
Sequential speedup:                       x0.78
Sequential speedup after reducing range:  x0.98
Parallel speedup:                         x0.72
Parallel speedup after reducing range:    x0.84
============================================================
```

### Greater pool - 100000 elements

```
./bin/main 100000

============================================================
Benchmark (100 runs, 100000 elements)
------------------------------------------------------------
Binary (1 core):                     0.7180s | avg 0.007180s
Binary (multicore):                  0.1940s | avg 0.001940s
Binary + reduced range (1 core):     0.7156s | avg 0.007156s
Binary + reduced range (multicore):  0.1906s | avg 0.001906s
Clasical (1 core):                   0.5473s | avg 0.005473s
Clasical (multicore):                0.1577s | avg 0.001577s
------------------------------------------------------------
Sequential speedup:                       x0.76
Sequential speedup after reducing range:  x0.76
Parallel speedup:                         x0.81
Parallel speedup after reducing range:    x0.83
============================================================
```

### The bigges (default) pool - 50000000 elements
```
./bin/main

============================================================
Benchmark (100 runs, 5000000 elements)
------------------------------------------------------------
Binary (1 core):                     45.5812s | avg 0.455812s
Binary (multicore):                  10.6172s | avg 0.106172s
Binary + reduced range (1 core):     45.5137s | avg 0.455137s
Binary + reduced range (multicore):  10.6705s | avg 0.106705s
Clasical (1 core):                   35.6271s | avg 0.356271s
Clasical (multicore):                8.0665s | avg 0.080665s
------------------------------------------------------------
Sequential speedup:                       x0.78
Sequential speedup after reducing range:  x0.78
Parallel speedup:                         x0.76
Parallel speedup after reducing range:    x0.76
============================================================
```

## For `int` numbers (after recompilaiton)

### Small pool - 1000 elements
```
============================================================
Benchmark (100 runs, 1000 elements)
------------------------------------------------------------
Binary (1 core):                     0.0064s | avg 0.000064s
Binary (multicore):                  0.0056s | avg 0.000056s
Binary + reduced range (1 core):     0.0047s | avg 0.000047s
Binary + reduced range (multicore):  0.0042s | avg 0.000042s
Clasical (1 core):                   0.0049s | avg 0.000049s
Clasical (multicore):                0.0043s | avg 0.000043s
------------------------------------------------------------
Sequential speedup:                       x0.76
Sequential speedup after reducing range:  x1.03
Parallel speedup:                         x0.77
Parallel speedup after reducing range:    x1.03
============================================================
```

### Greater pool - 100000 elements
```
./bin/main 100000

============================================================
Benchmark (100 runs, 100000 elements)
------------------------------------------------------------
Binary (1 core):                     0.7033s | avg 0.007033s
Binary (multicore):                  0.1857s | avg 0.001857s
Binary + reduced range (1 core):     0.6999s | avg 0.006999s
Binary + reduced range (multicore):  0.1842s | avg 0.001842s
Clasical (1 core):                   0.5444s | avg 0.005444s
Clasical (multicore):                0.1519s | avg 0.001519s
------------------------------------------------------------
Sequential speedup:                       x0.77
Sequential speedup after reducing range:  x0.78
Parallel speedup:                         x0.82
Parallel speedup after reducing range:    x0.82
============================================================
```

### The bigges (default) pool - 50000000 elements
```
./bin/main

============================================================
Benchmark (100 runs, 5000000 elements)
------------------------------------------------------------
Binary (1 core):                     46.0704s | avg 0.460704s
Binary (multicore):                  10.3811s | avg 0.103811s
Binary + reduced range (1 core):     46.1657s | avg 0.461657s
Binary + reduced range (multicore):  10.3716s | avg 0.103716s
Clasical (1 core):                   36.1776s | avg 0.361776s
Clasical (multicore):                7.6535s | avg 0.076535s
------------------------------------------------------------
Sequential speedup:                       x0.79
Sequential speedup after reducing range:  x0.78
Parallel speedup:                         x0.74
Parallel speedup after reducing range:    x0.74
============================================================
```

# References

Although I'd thought, I've discoverd something new, _**unfortunately**_ it seems not to be the case. I've tried to look for similar implementations, and was able  to find some earlier work, preceeding my _**strike of genious**_. So, as for anyone willing to continue work on this topic (including me in the future), I'm leaving references which I was able to find.

https://github.com/michael105/bitsort
https://github.com/yuempek/bitSort4
https://github.com/GhOsttMan/bit-sort-algorithm
https://rdrr.io/cran/bit/man/bitsort.html
https://web.stanford.edu/class/archive/cs/cs166/cs166.1206/lectures/17/Small17.pdf
https://www.researchgate.net/publication/259044206_The_Bitwise_Operations_Related_to_a_Fast_Sorting_Algorithm
