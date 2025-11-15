# About the project
This project was origianlly created for two purposes:
1. Test in practice my '_wild_' idea for supposidley faster sorting algorithm - similar to quicksort, but using smaller, consecutive power's of 2, as another pivots
2. Let me practice some programming in **C** 😎

As it turned out, additionally I was also able to test my computer and patience in practice, as my long break from programming in this intermediate level language (by that I mean, not completely barebone, like in Assembly, or in high level language as in Python) make it hard to eiher operate on the most basic level, or grab concepts generally.

## QuickSort with ... bits?

### Choosing point
In normal Quicksort we do the followng:
1. **Pick value** - usually from the middle of our range, or as a medium value between edge values and middle number
2. **Swap all** values to have 2 groups - smaller numbers, and those which are equal or greater our picked value - aka. pivot
3. **Repeat** - For each of the group start the algorithm - continue until group contains only one element

### My idea for optimalization:
And what if, instead of calculating pivot, we would split values the other way?
-> Let's then, for each new round, simply pick value which is half of the startig value - by default we pick value which is half as big, as the biggest possible number which we can store as an integer which our number of bits

So - if we use 64 bit long number, then we would start from:

`10000000 ... 00000000`

then:

`01000000 ... 00000000`

then

`00100000 ... 00000000`

...

`00000000 ... 00000001`

### Does it work?
Technically - yes. It's still QuickSort - we just alter the way we pick value. However, after I did it, and saw similar
results to the one which are with normal quicksort I've also did my best to try to hack it and work'out something better. More on that below.

### Hybrid approach
Additionally, I've decided to test my original idea against hybrid approach - by that I mean, that each new pivot will be choosen as the next, smaller power of 2, but instead of bit comparison, I will use '>' and '<=' operators, to account for potentially, better hardware implementation.

To sum up, each results with `Alternative` name, will show results for hybrid approach.

## What's inside
To test my idea I've decided to compare it with the proper QuickSort - for the other algorithm I've consulted with my buddy - ChatGPT 😉, - as some of the implementations which I saw on the internet weren't satisfying. I've ask it for an implementation and together we did some work to adjust it to my needs, make a draft for multiprocessing and do the same with my original idea - after first compilation worked, I've took things in my hands (mostly - segfault's were hunting me aaalllll, the wayyy - 🤐). Additionally, during development I've tried to implement other tactics to make it properly - dividie into folder, compile with `make`, etc. 

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
│  ├─ qs_bin.c
│  └─ qs_bin.h
│  ├─ qs_bin_alt.c
│  └─ qs_bin_alt.h
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

It's possible to change size of numbers which are used - `long` by default. For that one would need to alter `functions/__init__.h` file, and modify `elem_t` type. Then, program must be recompiled. This can be easily achieved by running:

```bash
make clean
make
```

**Important!** - using different type than `unsigned` - regardless of size - might yield unpredictable results !

# Results

As of right now it seems that binary sorting can behave like quicksort, but it's not easy to make it more efficient. The behaviour depends on factors such like:
- numbers of elements
- size of elements

Main problem in approach, seems to be `AND` operation, which -> by design is comparing numbers. It seems to be marginally slower, but at scale, it's visible. Additionally, I suspect that for smaller groups, I suspect, that quicksort can faster reduce the size of the group, making it easier to divide elements equally, and thus work a bit faster.

However, for smaller portions of data, it is possible to speed up the process, especially if combined with initianl range optimization, which lowers bit from which we are starting our sorting. This might not be the most useful approach, as in sorting problems, small groups, are not 

## For `long` numbers (default)

### Small pool - 1000 elements
```
./bin/main 1000

============================================================
Benchmark (100 runs, 1000 elements)
------------------------------------------------------------
Binary (1 core):                                 0.0047s | avg 0.000047s
Binary (multicore):                              0.0029s | avg 0.000029s
Binary + reduced range (1 core):                 0.0021s | avg 0.000021s
Binary + reduced range (multicore):              0.0019s | avg 0.000019s
Alternative binary (1 core):                     0.0016s | avg 0.000016s
Alternative binary (multicore):                  0.0016s | avg 0.000016s
Alternative binary + reduced range (1 core):     0.0016s | avg 0.000016s
Alternative binary + reduced range (multicore):  0.0017s | avg 0.000017s
Clasical (1 core):                               0.0037s | avg 0.000037s
Clasical (multicore):                            0.0023s | avg 0.000023s
------------------------------------------------------------
Sequential speedup:                                     x0.78
Sequential speedup after reducing range:                x1.75
Alternative sequential speedup:                         x2.24
Alternative equential speedup after reducing range:     x2.35
Parallel speedup:                                       x0.78
Parallel speedup after reducing range:                  x1.19
Alternative parallel speedup:                           x1.43
Alternative parallel speedup after reducing range:      x1.37
============================================================
```

### Greater pool - 100000 elements

```
./bin/main 100000

============================================================
Benchmark (100 runs, 100000 elements)
------------------------------------------------------------
Binary (1 core):                                 0.5120s | avg 0.005120s
Binary (multicore):                              0.1374s | avg 0.001374s
Binary + reduced range (1 core):                 0.5082s | avg 0.005082s
Binary + reduced range (multicore):              0.1230s | avg 0.001230s
Alternative binary (1 core):                     0.5065s | avg 0.005065s
Alternative binary (multicore):                  0.1266s | avg 0.001266s
Alternative binary + reduced range (1 core):     0.5043s | avg 0.005043s
Alternative binary + reduced range (multicore):  0.1230s | avg 0.001230s
Clasical (1 core):                               0.4071s | avg 0.004071s
Clasical (multicore):                            0.1073s | avg 0.001073s
------------------------------------------------------------
Sequential speedup:                                     x0.80
Sequential speedup after reducing range:                x0.80
Alternative sequential speedup:                         x0.80
Alternative equential speedup after reducing range:     x0.81
Parallel speedup:                                       x0.78
Parallel speedup after reducing range:                  x0.87
Alternative parallel speedup:                           x0.85
Alternative parallel speedup after reducing range:      x0.87
============================================================
```

### The bigges (default) pool - 50000000 elements
```
./bin/main

============================================================
Benchmark (100 runs, 5000000 elements)
------------------------------------------------------------
Binary (1 core):                                 32.7638s | avg 0.327638s
Binary (multicore):                              5.4328s  | avg 0.054328s
Binary + reduced range (1 core):                 32.7146s | avg 0.327146s
Binary + reduced range (multicore):              5.4575s  | avg 0.054575s
Alternative binary (1 core):                     32.8327s | avg 0.328327s
Alternative binary (multicore):                  5.4496s  | avg 0.054496s
Alternative binary + reduced range (1 core):     32.8447s | avg 0.328447s
Alternative binary + reduced range (multicore):  5.4563s  | avg 0.054563s
Clasical (1 core):                               25.9050s | avg 0.259050s
Clasical (multicore):                            4.0178s  | avg 0.040178s
------------------------------------------------------------
Sequential speedup:                                     x0.79
Sequential speedup after reducing range:                x0.79
Alternative sequential speedup:                         x0.79
Alternative equential speedup after reducing range:     x0.79
Parallel speedup:                                       x0.74
Parallel speedup after reducing range:                  x0.74
Alternative parallel speedup:                           x0.74
Alternative parallel speedup after reducing range:      x0.74
============================================================
```

## For `int` numbers (after recompilaiton)

### Small pool - 1000 elements
```
./bin/main 1000

============================================================
Benchmark (100 runs, 1000 elements)
------------------------------------------------------------
Binary (1 core):                                 0.0052s | avg 0.000052s
Binary (multicore):                              0.0036s | avg 0.000036s
Binary + reduced range (1 core):                 0.0026s | avg 0.000026s
Binary + reduced range (multicore):              0.0022s | avg 0.000022s
Alternative binary (1 core):                     0.0019s | avg 0.000019s
Alternative binary (multicore):                  0.0017s | avg 0.000017s
Alternative binary + reduced range (1 core):     0.0018s | avg 0.000018s
Alternative binary + reduced range (multicore):  0.0018s | avg 0.000018s
Clasical (1 core):                               0.0041s | avg 0.000041s
Clasical (multicore):                            0.0023s | avg 0.000023s
------------------------------------------------------------
Sequential speedup:                                     x0.79
Sequential speedup after reducing range:                x1.59
Alternative sequential speedup:                         x2.16
Alternative equential speedup after reducing range:     x2.23
Parallel speedup:                                       x0.66
Parallel speedup after reducing range:                  x1.07
Alternative parallel speedup:                           x1.33
Alternative parallel speedup after reducing range:      x1.30
============================================================
```

### Greater pool - 100000 elements
```
./bin/main 100000

============================================================
Benchmark (100 runs, 100000 elements)
------------------------------------------------------------
Binary (1 core):                                 0.5219s | avg 0.005219s
Binary (multicore):                              0.1389s | avg 0.001389s
Binary + reduced range (1 core):                 0.5194s | avg 0.005194s
Binary + reduced range (multicore):              0.1252s | avg 0.001252s
Alternative binary (1 core):                     0.5184s | avg 0.005184s
Alternative binary (multicore):                  0.1284s | avg 0.001284s
Alternative binary + reduced range (1 core):     0.5160s | avg 0.005160s
Alternative binary + reduced range (multicore):  0.1263s | avg 0.001263s
Clasical (1 core):                               0.4013s | avg 0.004013s
Clasical (multicore):                            0.1058s | avg 0.001058s
------------------------------------------------------------
Sequential speedup:                                     x0.77
Sequential speedup after reducing range:                x0.77
Alternative sequential speedup:                         x0.77
Alternative equential speedup after reducing range:     x0.78
Parallel speedup:                                       x0.76
Parallel speedup after reducing range:                  x0.85
Alternative parallel speedup:                           x0.82
Alternative parallel speedup after reducing range:      x0.84
============================================================
```

### The bigges (default) pool - 50000000 elements
```
./bin/main

============================================================
Benchmark (100 runs, 5000000 elements)
------------------------------------------------------------
Binary (1 core):                                 32.5849s | avg 0.325849s
Binary (multicore):                              5.4058s  | avg 0.054058s
Binary + reduced range (1 core):                 32.6244s | avg 0.326244s
Binary + reduced range (multicore):              5.4100s  | avg 0.054100s
Alternative binary (1 core):                     32.5916s | avg 0.325916s
Alternative binary (multicore):                  5.4106s  | avg 0.054106s
Alternative binary + reduced range (1 core):     32.6236s | avg 0.326236s
Alternative binary + reduced range (multicore):  5.4096s  | avg 0.054096s
Clasical (1 core):                               25.2952s | avg 0.252952s
Clasical (multicore):                            4.0093s  | avg 0.040093s
------------------------------------------------------------
Sequential speedup:                                     x0.78
Sequential speedup after reducing range:                x0.78
Alternative sequential speedup:                         x0.78
Alternative equential speedup after reducing range:     x0.78
Parallel speedup:                                       x0.74
Parallel speedup after reducing range:                  x0.74
Alternative parallel speedup:                           x0.74
Alternative parallel speedup after reducing range:      x0.74
============================================================
```

# References

Although I'd thought, I've discoverd something new, _**unfortunately**_ it seems not to be the case. I've tried to look for similar implementations, and was able  to find some earlier work, preceeding my _**strike of genious**_. So, as for anyone willing to continue work on this topic (including me in the future), I'm leaving references which I was able to find.

- https://github.com/michael105/bitsort
- https://github.com/yuempek/bitSort4
- https://github.com/GhOsttMan/bit-sort-algorithm
- https://rdrr.io/cran/bit/man/bitsort.html
- https://web.stanford.edu/class/archive/cs/cs166/cs166.1206/lectures/17/Small17.pdf
- https://www.researchgate.net/publication/259044206_The_Bitwise_Operations_Related_to_a_Fast_Sorting_Algorithm
