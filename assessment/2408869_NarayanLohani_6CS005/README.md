# Table of Contents

1. [Introduction](#introduction)
2. [Task 1: Count Frequency of the Word](#task-1-count-frequency-of-the-word)
    - [Problem Statement](#problem-statement)
    - [Solution Overview](#solution-overview)
    - [Key Features](#key-features)
    - [How It Works](#how-it-works)
    - [Error Handling and Robustness](#error-handling-and-robustness)
    - [Example Usage](#example-usage)
    - [Files](#files)
3. [Task 2: Matrix Operations](#task-2-matrix-operations)
    - [Problem Statement](#problem-statement-1)
    - [Solution Overview](#solution-overview-1)
    - [Key Features](#key-features-1)
    - [How It Works](#how-it-works-1)
    - [Error Handling and Robustness](#error-handling-and-robustness-1)
    - [Example Usage](#example-usage-1)
    - [Files](#files-1)
4. [Dependencies](#dependencies)
5. [File Structure](#file-structure)

---

## Introduction

This project demonstrates the use of parallel and concurrent programming techniques in C to solve two practical problems:

- **[Task 1:](#task-1-count-frequency-of-the-word)** Counting the frequency of each word in a large text file using multithreading (pthreads).
- **[Task 2:](#task-1-count-frequency-of-the-word)** Performing a variety of matrix operations (addition, subtraction, multiplication, transpose, etc.) on large datasets using parallelism (OpenMP).

The solutions are robust, efficient, and designed to handle real-world data with comprehensive error handling and clear output formatting.

---

## Task 1: Count Frequency of the Word

### Problem Statement

Given a large text file, efficiently count the frequency of each unique word (case-insensitive, alphanumeric only) using multithreading. The results should be written to a file in sorted order.

### Solution Overview

The solution uses POSIX threads (pthreads) to parallelize the word counting process. The input file is read into memory, and the workload is divided among multiple threads. Each thread processes a unique chunk of the file, with boundaries adjusted to ensure words are not split between threads. A global word count array, protected by a mutex, is used to aggregate results from all threads.

### Key Features

- **Multithreading:** Utilizes pthreads to process the file in parallel, improving performance on large datasets.
- **Word Boundary Handling:** Thread boundaries are carefully adjusted so that no word is split between two threads, ensuring accurate counting.
- **Case-Insensitive & Alphanumeric:** Words are normalized to lowercase and only alphanumeric characters are considered.
- **Thread-Safe Aggregation:** A mutex ensures that updates to the shared word count array are safe from race conditions.
- **Sorted Output:** The final word counts are sorted alphabetically before being written to the output file.
- **Simple Usage:** The program is run as `./output <num_threads> <file_name>`, and results are written to `result.txt`.

### How It Works

1. **File Reading:**
   The entire input file is read into a buffer in memory.

2. **Thread Partitioning:**
   The buffer is divided into nearly equal-sized chunks, one per thread. Each chunk's start and end are adjusted to word boundaries to avoid splitting words.

3. **Parallel Processing:**
   Each thread scans its assigned chunk, extracts words, and updates the global word count array. All words are converted to lowercase for case-insensitive counting.

4. **Synchronization:**
   A mutex is used to synchronize access to the global word count array, preventing race conditions when threads add or update word counts.

5. **Sorting and Output:**
   After all threads finish, the word counts are sorted alphabetically and written to `result.txt`, with each line containing a word and its frequency.

### Error Handling and Robustness

- **Input Validation:** Checks for valid thread count and file accessibility.
- **Memory Management:** All allocated memory is freed after use.
- **Thread Safety:** All shared data is protected by a mutex.
- **Boundary Cases:** Handles empty files, files with non-alphanumeric content, and very large files robustly.

### Example Usage

```sh
gcc -pthread -o output Main.c
./output 8 WordOccurrenceDataset.txt
```

The output will be written to `result.txt` in the format:

```
word1 count1
word2 count2
...
```

### Files

- `Main.c` — Source code for the multithreaded word counter.
- `WordOccurrenceDataset.txt` — Example input file.
- `result.txt` — Output file with sorted word frequencies.

---

## Task 2: Matrix Operations

### Problem Statement

Given a file containing multiple matrices, perform a series of mathematical operations on each pair of matrices using parallel processing. The operations include addition, subtraction, element-wise multiplication and division, transpose, and matrix multiplication. The results should be written to a dynamically named output file.

### Solution Overview

This solution leverages OpenMP to parallelize matrix operations for improved performance on large datasets. The program reads matrices from a file, processes them in pairs, and writes the results to an output file named `result_<inputfilename>`. The code is modular, robust, and includes comprehensive error handling for real-world data.

### Key Features

- **Parallel Processing:** Uses OpenMP to accelerate all matrix operations by distributing work across multiple threads.
- **Robust Input Parsing:** Skips blank lines, validates matrix headers, checks for non-numeric tokens, and ensures correct row/column counts.
- **Dynamic Output Naming:** Output files are named based on the input file for easy identification.
- **Comprehensive Operations:** Supports addition, subtraction, element-wise multiplication/division, transpose, and matrix multiplication.
- **Clear Error Reporting:** Provides informative error messages for all input and runtime errors.
- **Memory Safety:** Ensures all allocated memory is properly freed, even in error cases.

### How It Works

1. **Input Reading:**
   The program reads matrices from a file. Each matrix starts with a header line (e.g., `3,4` for a 3x4 matrix), followed by the specified number of data rows. Blank or whitespace-only lines are ignored.

2. **Matrix Pair Processing:**
   Matrices are processed in pairs. For each pair, the following operations are attempted:
   - **Addition, Subtraction, Element-wise Multiplication/Division:** Performed if the matrices have the same dimensions.
   - **Transpose:** Both matrices are transposed and printed.
   - **Matrix Multiplication:** Performed if the number of columns in the first matrix matches the number of rows in the second.

3. **Parallelism:**
   All operations are parallelized using OpenMP, with the number of threads specified by the user at runtime.

4. **Output:**
   Results are written to a file named `result_<inputfilename>`, with clear section headers and CSV-formatted matrices.

### Error Handling and Robustness

- **Header Validation:** Ensures each matrix header is in the correct format and specifies positive dimensions.
- **Row/Column Validation:** Checks that each data row contains the expected number of numeric values.
- **Non-numeric Detection:** Reports and aborts on any non-numeric or extra tokens in data rows.
- **Memory Management:** Frees all allocated memory on errors to prevent leaks.
- **Graceful Exit:** Provides clear error messages and exits cleanly on any failure.

### Example Usage

```sh
gcc -fopenmp -o matrix Main.c
./matrix MatData.txt 8
```

The output will be written to `result_MatData.txt` in the same directory.

### Files

- `Main.c` — Source code for the parallel matrix operations.
- `MatData.txt` or `Matrices.txt` — Example input files containing matrix data.
- `result_<input_file>` — Output file generated by the program.

---


## Dependencies

- **C Compiler:** GCC or any standard C compiler supporting C99 or later.
- **OpenMP:** For Task 2, ensure your compiler supports OpenMP (e.g., GCC with `-fopenmp`).
- **POSIX Threads:** For Task 1, pthreads are used (standard on Unix-like systems).
- **Standard C Libraries:** stdio, stdlib, string, ctype, errno, pthread (Task 1), omp.h (Task 2).

---

## File Structure

```
.
├── README.md
├── task1
│   ├── Main.c                   # Source code for Task 1 (Word Frequency Counter)
│   ├── WordOccurrenceDataset.txt# Input dataset for Task 1
│   └── result.txt               # Output: word frequencies (Task 1)
├── task2
│   ├── Main.c                   # Source code for Task 2 (Matrix Operations)
│   ├── Matrices.txt             # Example input file for Task 2
│   ├── MatData.txt              # Another example input file for Task 2
│   ├── result_Matrices.txt      # Output for Matrices.txt (Task 2)
│   └── result_MatData.txt       # Output for MatData.txt (Task 2)
```

- Each `Main.c` file is self-contained and well-documented.
- Input and output files are provided for both tasks for easy testing and demonstration.
