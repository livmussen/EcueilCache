# EcueilCache - CPU Cache Collision Test Project

This project includes two test programs designed to investigate and demonstrate the behavior of CPU caches, particularly focusing on cache collisions and their impact on multi-threaded applications. The programs are written in C and target the Windows operating system.

## Description

The project comprises two main parts, each contained in its own source file:

1. **Multi-threaded Cache Collision Test:**
   - This part of the project explores how multi-threading can affect CPU cache performance. It involves creating multiple threads that either write to or read and write to specific memory addresses. The objective is to observe the impact of concurrent access to shared memory on cache behavior, specifically looking for evidence of cache collisions.

2. **Cache Access Pattern Test:**
   - This section aims to analyze the impact of different memory access patterns on cache performance. It includes tests for both reading and writing operations, executed row-wise and column-wise on a large 2D array. The size of the array exceeds the capacity of the L3 cache, thus providing a clear view of how cache misses and cache hit rates vary with different access patterns.

## Features

- **Multi-threading:** Demonstrates the use of multi-threading in C and how it can be used to simulate concurrent access to shared resources.
- **Cache Behavior Analysis:** Provides insights into how cache collisions occur and their effects on program performance, especially in a multi-threaded environment.
- **Memory Access Patterns:** Examines the impact of row-wise and column-wise access patterns on cache efficiency.
- **Performance Measurement:** Includes timing code to measure the execution time of different operations, offering a practical way to observe the impact of cache behavior on performance.

## Requirements

- **Operating System:** Windows (the code uses Windows-specific headers and functions).
- **Compiler:** A C compiler compatible with Windows and capable of handling Windows-specific constructs (`__declspec`, Windows API functions, etc.).

## Compilation and Execution

To compile and execute the programs, follow these steps:

1. Open the source files in a C-compatible IDE or text editor.
2. Compile the code using a C compiler that supports Windows-specific features.
3. Run the executable from the command line or within the IDE, optionally passing command-line arguments to control aspects like the number of threads, affinity mask, and access patterns.

## Usage

The programs accept several command-line arguments to customize their behavior:

- `--rw`: In the multi-threaded test, this flag determines if the threads perform read-write operations instead of write-only.
- `--mask <mask>`: Sets the CPU affinity mask for threads.
- `--off <offset>`: Specifies the offset for writing in the buffer (multi-threaded test).
- `--byCol`: Determines if the memory access should be column-wise (cache access pattern test).
- `--wr`: Selects between read and write tests in the cache access pattern test.
- `--iter <nbIter>`: Sets the number of iterations for the cache access pattern test.

## Compilation Details

- **Compiler Used:** VisualStudio 2022 (Free version)
- **Compilation Mode:** Release
- **Optimization:** Optimized for speed

## Commands Executed During Presentation

### `simulWriters` Commands

```bash
simulWriters --mask 0x02 --off 1
simulWriters --mask 0x02 --off 1 --mask 0x08 --off 2
simulWriters --mask 0x02 --off 1 --mask 0x08 --off 2 --mask 0x20 --off 3
simulWriters --mask 0x02 --off 1 --mask 0x08 --off 2 --mask 0x20 --off 3 --mask 0x80 --off 4
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32 --mask 0x04 --off 17
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32 --mask 0x04 --off 17 --mask 0x10 --off 25
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32 --mask 0x04 --off 17 --mask 0x10 --off 25 --mask 0x40 --off 33
```

### `matrixWalk` Commands

```bash
matrixWalk --byCol
matrixWalk
matrixWalk --byCol --wr
matrixWalk --wr
```

### Execution Order and Observations for `simulWriters` and `matrixWalk` Commands

#### `simulWriters` Execution Order

1. **Single Thread Execution (Increasing Affinity and Offset)**
   - The first few commands increase the affinity mask and offset sequentially. This allows for observing how different CPU cores (as determined by the affinity mask) handle memory writes to different offsets in a multi-threaded environment.
   - **Observations:** Look for variations in write speed or cache collisions as more threads with different affinities and offsets are added. Depending on the processor's architecture, especially its cache organization and core-to-cache mapping, there may be noticeable differences in performance.

2. **Multi-Thread Execution with Varied Offsets**
   - The later commands introduce more threads with varied offsets, but still within certain ranges (e.g., 8, 16, 24, etc.). This setup is particularly useful for observing the behavior of the CPU cache when handling writes that may or may not fall within the same cache line or cache set.
   - **Observations:** Cache architectures with smaller cache lines or fewer sets might show more pronounced effects of cache thrashing or contention. The performance impact could vary significantly based on the processor's cache design (e.g., size, associativity).

#### `matrixWalk` Execution Order

1. **Column-wise and Row-wise Memory Access**
   - The `matrixWalk` commands alternate between column-wise and row-wise memory access patterns. This difference is crucial in understanding how memory layout and access patterns affect cache usage.
   - **Observations:** Processors with a high cache line size are expected to benefit more from row-wise access due to spatial locality. In contrast, column-wise access might lead to more cache misses, especially in a large matrix that exceeds the cache size.

2. **Read vs. Write Operations**
   - The inclusion of the `--wr` flag switches the operations from read-only to write. This change allows for observing the differences in cache behavior between read and write operations.
   - **Observations:** Write operations might exhibit different caching strategies (write-through vs. write-back), which can significantly impact performance. Additionally, the write buffer's efficiency and the potential for write combining can play a role in overall performance.

### General Observations Based on Processor Architecture

- **Cache Size and Associativity:** Larger caches with higher associativity may handle these operations more efficiently, reducing cache misses.
- **Cache Line Size:** Processors with larger cache lines might perform better in sequential access patterns (like row-wise access) but could suffer more from cache misses in non-sequential patterns (like column-wise access).
- **Core-to-Cache Mapping:** The mapping of cores to caches (e.g., shared L3 cache vs. dedicated L2 caches) will influence how multi-threaded operations impact overall performance.
- **Write Buffering and Strategy:** The efficiency of the write buffer and the employed write strategy (write-through or write-back) will affect the performance of write operations.

In summary, the performance and behavior observed while executing these commands will provide valuable insights into the specific processor architecture's handling of multi-threaded operations, memory access patterns, and cache utilization.

### Conclusion

This project serves as a practical tool for understanding and analyzing CPU cache behavior in multi-threaded applications and under different memory access patterns. It is valuable for developers and researchers interested in system programming, performance optimization, and hardware-software interaction.
