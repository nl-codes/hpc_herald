<table>
  <tr>
    <td align="center">
      <img src="https://heraldcollege.edu.np/images/footer/footer-logo.svg" alt="Herald College Kathmandu" width="180"/>
    </td>
    <td style="width:200px"></td>
    <td align="center">
      <img src="https://nationalrehabilitationcentre.nhs.uk/images/NRC_Academy/wlv_logo_colour_transparent_4.png" alt="University of Wolvehampton" width="180"/>
    </td>
  </tr>
</table>

---

**Module Name:** High Performance Computing

**Module Code:** 6CS005

**Assignment Title:** Portfolio Part-1

**Student Name:** Narayan Lohani

**Student Number:** 2408869

**Submission Date:** 20 Dec 2025

**Tutor:** Bijaya Ghimire

---

# Table of Contents

1. [Introduction](#introduction)
2. [Task 1: Count Frequency of the Word](#task-1-count-frequency-of-the-word)
    - [Problem Statement](#problem-statement)
    - [Solution Overview](#solution-overview)
    - [Key Features](#key-features)
    - [How It Works](#how-it-works-gist)
    - [Detailed Explanation](#detailed-explanation)
    - [Error Handling and Robustness](#error-handling-and-robustness)
    - [Example Usage](#example-usage)
    - [Files](#files)
3. [Task 2: Matrix Operations](#task-2-matrix-operations)
    - [Problem Statement](#problem-statement-1)
    - [Solution Overview](#solution-overview-1)
    - [Key Features](#key-features-1)
    - [How It Works](#how-it-works-gist-1)
    - [Detailed Explanation](#detailed-explanation-1)
    - [Error Handling and Robustness](#error-handling-and-robustness-1)
    - [Example Usage](#example-usage-1)
    - [Files](#files-1)
4. [Dependencies](#dependencies)
5. [File Structure](#file-structure)

---

# Introduction

This project demonstrates the use of parallel and concurrent programming techniques in C to solve two practical problems:

- **[Task 1:](#task-1-count-frequency-of-the-word)** Counting the frequency of each word in a large text file using multithreading (pthreads).
- **[Task 2:](#task-2-matrix-operations)** Performing a variety of matrix operations (addition, subtraction, multiplication, transpose, etc.) on large datasets using parallelism (OpenMP).


---

# Task 1: Count Frequency of the Word

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
- **Simple Usage:** The program is run as `./<object_file> <file_name> <num_threads>`, and results are written to `result.txt`.

### How It Works (Gist)

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

### Detailed Explanation

1. **Initialization and Input Validation**
The program begins with rigorous input validation:
   - **Argument Count Check:** Requires exactly two arguments (filename and thread count) to prevent misuse
   - **Thread Count Validation:** Ensures thread count is positive, preventing logical errors

2. **Memory-Mapped File Processing Strategy**
The `read_file_to_buffer()` function implements an efficient file reading approach:
   - **Single Read Operation:** Entire file content is loaded into memory at once using `fread()`
   - **Memory Optimization:** Buffer size equals exact file size + 1 byte for null termination
   - **Sequential Access:** File pointer operations (`fseek`, `ftell`, `rewind`) efficiently determine file size without reading content twice
   - **Error Handling:** Returns `NULL` on any failure (file open, memory allocation, read error)
   - **File Accessibility:** Attempts to open and read the file early to fail fast if inaccessible

3. **Thread Workload Division**
The correct thread division lies in `fix_thread_boundaries()`:

   **Original Problem:** Naively dividing file by byte count could split words mid-character, causing:

   - Incomplete word extraction
   - Duplicate counting of partial words
   - Incorrect frequency results

   Solution Approach:
   ```c
   long slice = file_size / num_threads;
      for (int i = 0; i < num_threads; i++) {
         long start = i * slice;
         long end = (i == num_threads - 1) ? file_size : (i + 1) * slice;

         // Adjust start (except for first thread) to the next non-alphanumeric character
         if (i != 0) {
               while (start < file_size && isalnum(buffer[start])) {
                  start++;
               }
         }
         // Adjust end (except for last thread) to the next non-alphanumeric character
         if (i != num_threads - 1) {
               while (end < file_size && isalnum(buffer[end])) {
                  end++;
               }
         }
      }
   ```

   - **First Thread:** Starts at exact byte 0 (beginning of file)
   - **Intermediate Threads:** Start boundaries move forward until hitting a non-alphanumeric character (space)
   - **Last Thread:** Ends at exact file end
   - **Load Balancing:** While boundaries are adjusted, the initial equal division ensures relatively balanced workloads

4. **Parallel Processing with Thread Synchronization**

   Each thread executes `thread_routine_find_word_count()`:

   **Word Extraction Algorithm:**

   1. **Character-by-Character Scanning:** Iterates through assigned buffer segment
   2. **Alphanumeric Detection:** Uses `isalnum()` to identify word characters
   3. **Case Normalization:** Converts all characters to lowercase via `tolower()`
   4. **Word Buffering:** Accumulates characters until non-alphanumeric encountered
   5. **Word Completion:** Null-terminates buffer and processes complete word

   **Critical Design Choice:** Each thread uses a local `word[MAX_WORD_LEN]` buffer to avoid contention during character accumulation, only accessing shared data when a complete word is ready.

5. **Thread-Safe Global Data Management**

   The `handle_words()` function implements a synchronized update mechanism:

   **Shared Resources:**
   - `words[]`: Global array of `WordCount` structures
   - `word_count`: Global counter of unique words

   **Synchronization Strategy:**

   ```c
   pthread_mutex_lock(&lock);
   // Critical section: search and update word count
   pthread_mutex_unlock(&lock);
   ```

   **Update Logic:**

   1. **Linear Search:** Checks if word already exists in global array (O(n) per word)
   2. **Two Possible Outcomes:**
      - **Word Exists:** Increment count, release lock, return immediately
      - **New Word:** Copy word to array, set count to 1, increment `word_count`
   3. **Memory Safety:** Uses `strncpy` with bounds checking to prevent buffer overflows

6. **Post-Processing and Output**

   After all threads complete:

   **Sorting Phase:**
   - Uses `qsort()` with `compare_routine()` for alphabetical ordering
   - Comparison function uses `strcmp()` for standard lexicographic ordering

   **Output Generation:**
   - Creates `result.txt` with simple "word count" format
   - One entry per line for easy parsing

7. **Resource Cleanup**

   The program manages resources:

   1. **Mutex Destruction:** Releases synchronization primitive
   2. **Buffer Deallocation:** Frees the file buffer memory
   3. **Implicit Cleanup:** Thread resources released via `pthread_join()`

---

### Error Handling and Robustness

- **Input Validation:** Checks for valid thread count and file accessibility.
- **Memory Management:** All allocated memory is freed after use.
- **Thread Safety:** All shared data is protected by a mutex.
- **Boundary Cases:** Handles empty files, files with non-alphanumeric content, and very large files robustly.

### Example Usage

```sh
gcc -pthread Main.c -o exe
./exe WordOccurrenceDataset.txt 8
```

The output will be written to `result.txt` in the format:

```
word1 count1
word2 count2
...
```

### Files

- `Main.c` — Source code for the multithreaded word counter.
- `WordOccurrenceDataset.txt` — Input file.
- `result.txt` — Output file with sorted word frequencies.

---

# Task 2: Matrix Operations

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

### How It Works (Gist)

1. **Input Reading:**
   The program reads matrices from a file. Each matrix starts with a header line (e.g., `3,4` for a 3x4 matrix), followed by the specified number of data rows. Blank or whitespace-only lines are ignored.

2. **Matrix Pair Processing:**
   Matrices are processed in pairs. For each pair, the following operations are attempted:
   - **Addition, Subtraction, Element-wise Multiplication/Division:** Performed if the matrices have the same dimensions.
   - **Transpose:** Both matrices are transposed and written to the output file.
   - **Matrix Multiplication:** Performed if the number of columns in the first matrix matches the number of rows in the second.

3. **Parallelism:**
   All operations are parallelized using OpenMP, with the number of threads specified by the user at runtime.

4. **Output:**
   Results are written to a file named `result_<inputfilename>`, with clear section headers and CSV-formatted matrices.

### Detailed Explanation

1. **Input Reading**

   Matrices are read using a dedicated parsing function that:

   - **Skips blank or whitespace-only lines**
   - **Validates the matrix header** `rows,cols`
   - **Reads and validates each row** of numerical data
   - **Rejects malformed or incomplete matrices** immediately

   Each matrix is dynamically allocated as a 2D array of doubles.

2. **Matrix Pair Processing**

   Matrices are processed **two at a time**. For each pair:

   #### Element-wise Operations:
    Performed only if both matrices have identical dimensions:

   - **Addition**
   - **Subtraction**
   - **Element-wise multiplication**
   - **Element-wise division** (division by zero yields `NaN`)

   Each operation is parallelized using `#pragma omp parallel for`.

   #### Transpose Operations:
   Both matrices are transposed regardless of size compatibility.
   This operation swaps rows and columns and is fully parallelized.

   #### Matrix Multiplication:
   Executed only when: `columns(A) == rows(B)`

   The resulting matrix has dimensions: `rows(A) × columns(B)`

   If dimensions are incompatible, a descriptive message is written instead of performing the operation.


3. **Parallel Execution**

   - The number of threads is provided by the user at runtime
   - If number of threads exceeds the dimension of matrix, then it is capped
   - OpenMP distributes work across threads at the loop level
   - No shared writable state is used, ensuring thread safety

### Error Handling and Robustness

- **Header Validation:** Ensures each matrix header is in the correct format and specifies positive dimensions.
- **Row/Column Validation:** Checks that each data row contains the expected number of numeric values.
- **Non-numeric Detection:** Reports and aborts on any non-numeric or extra tokens in data rows.
- **Memory Management:** Frees all allocated memory on errors to prevent leaks.
- **Graceful Exit:** Provides clear error messages and exits cleanly on any failure.

### Example Usage

```sh
gcc -fopenmp Main.c -o exe
./exe MatData.txt 8
./exe Matrices.txt 10
```

The output will be written to `result_MatData.txt` and  `result_Matrices.txt` in the same directory.

### Files

- `Main.c` — Source code for the parallel matrix operations.
- `MatData.txt` and `Matrices.txt` — Input files containing matrix data.
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
│   ├── Main.c                     # Source code for Task 1 (Word Frequency Counter)
│   ├── WordOccurrenceDataset.txt  # Input dataset for Task 1
│   └── result.txt                 # Output: word frequencies (Task 1)
├── task2
│   ├── Main.c                     # Source code for Task 2 (Matrix Operations)
│   ├── Matrices.txt               # Input file for Task 2
│   ├── MatData.txt                # Input file for Task 2
│   ├── result_Matrices.txt        # Output for Matrices.txt (Task 2)
│   └── result_MatData.txt         # Output for MatData.txt (Task 2)
```

- Each `Main.c` file is self-contained and well-documented.
- Input and output files are provided for both tasks.
