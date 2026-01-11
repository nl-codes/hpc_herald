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

**Assignment Title:** Portfolio Part-2

**Student Name:** Narayan Lohani

**Student Number:** 2408869

**Submission Date:** 11 Jan 2026

**Tutor:** Bijaya Ghimire

---

# Table of Contents

1. [Introduction](#introduction)
2. [Task 3: Password Cracking with Files using CUDA](#task-3-password-cracking-with-files-using-cuda)
    - [Problem Statement](#problem-statement)
    - [Solution Overview](#solution-overview)
    - [Key Features](#key-features)
    - [How It Works](#how-it-works-gist)
    - [Detailed Explanation](#detailed-explanation)
    - [Error Handling and Robustness](#error-handling-and-robustness)
    - [Example Usage](#example-usage)
    - [Files](#files)
3. [Task 4: Sobel Edge Detection using Cuda](#task-4-sobel-edge-detection-using-cuda)
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

This project demonstrates the use of Graphics Processing Unit (GPU) using CUDA to execute the following problems:

- **[Task 3:](#task-3-password-cracking-with-files-using-cuda)** Cracking encrypted passwords using Brute-Force approach
- **[Task 4:](#task-4-sobel-edge-detection-using-cuda)** Detect edges of a PNG image using Sobel's algorithm


---

# Task 3: Password Cracking with Files using CUDA

### Problem Statement

Given a file containing SHA512 encrypted passwords, crack its password using brute-force approach by comparing all possible password combinations with target hashes in the encrypted file.

### Solution Overview

The solution uses CUDA to parallelize the generation of all possible 4-character password combinations (format: `[a-z][a-z][0-9][0-9]`). Each thread generates a candidate password and transforms it using a custom `cudaCrypt` algorithm. The transformed passwords are then encrypted using SHA512 in CPU and compared against target SHA512 hashes  to find matches.

### Key Features

- **Parallel Password Generation:** Uses CUDA kernel to generate all 67,600 possible password combinations in parallel
- **Custom Transformation Algorithm:** Implements `cudaCrypt` function that converts 4-character passwords to 10-character raw passwords
- **SHA512 Hashing:** Uses the `crypt()` function with `$6$AS$` salt for secure password comparison
- **2D Thread Block Structure:** Employs 16×16 thread blocks for efficient GPU utilization
- **Progress Tracking:** Displays progress every 12,000 combinations tested
- **Ordered Output:** Results are written in the same order as the input hashes

### How It Works (Gist)

1. Load target SHA512 hashes from the input file (up to 10 hashes)
2. Allocate GPU memory and launch CUDA kernel to generate all possible raw passwords
3. Copy generated raw passwords back to host memory
4. For each raw password, compute SHA512 hash and compare against target hashes
5. Write cracked passwords to `result.txt` in the original hash order

### Detailed Explanation

**Password Space:** The password format `[a-z][a-z][0-9][0-9]` yields 26 × 26 × 10 × 10 = 67,600 unique combinations.

**CUDA Kernel (`generatePasswordsKernel`):** Each thread calculates its unique index and decomposes it into four components (two letters, two digits). The candidate password is then transformed using `cudaCryptDevice`, which applies arithmetic transformations and bounds checking to produce a 10-character raw password.

**Index Decomposition:** The formula breaks down a linear index into password components:
- `letter1_idx = idx / (26 × 10 × 10)`
- `letter2_idx = (idx % (26 × 10 × 10)) / (10 × 10)`
- `digit1_idx = (idx % 100) / 10`
- `digit2_idx = idx % 10`

**Hash Comparison:** After kernel execution, raw passwords are copied to host memory. Each is hashed using SHA512 (`crypt()` with salt `$6$AS$`) and compared against target hashes until all passwords are found.

---

### Error Handling and Robustness

- **File I/O Validation:** Checks if input file can be opened before proceeding
- **CUDA Error Checking:** Verifies memory allocation and kernel launch success
- **Argument Validation:** Ensures correct number of command-line arguments
- **Newline Handling:** Strips newline characters from loaded hashes
- **Progress Indicators:** Reports progress during the cracking process

### Example Usage

Create Encrypted passwords using:

```sh
gcc CreateEncryptedPasswordText.c -o create -lcrypt

./create
```

Crack the passwords using:

```sh
nvcc CrackPassword.cu -o crack -lcrypt

./crack EncryptedPasswords.txt
```

The output of cracked (raw) passwords will be written to `result.txt` in the format:

```
bo07
xp18
dz43
...
```

### Files

- `CreateEncryptedPasswordText.c` - Source code to generate 10 random encrypted passwords.
- `CrackPassword.cu` - CUDA source code to brute force crack passwords.
- `EncryptedPasswords.txt` - Stores encrypted passwords (to be cracked).
- `PasswordHint.txt` - Stores the raw and cudaCrypt hash of passwords stored in **EncryptedPasswords.txt** (can be compared with result.txt)
- `result.txt` - Stores the cracked passwords from EncryptedPasswords.txt

---

# Task 4: Sobel Edge Detection using Cuda

### Problem Statement

Given a PNG image, detect the edges of the image using Sobel edge detection algorithm powered by GPU acceleration.

### Solution Overview

The solution uses CUDA to perform Sobel edge detection on PNG images. The program loads an RGBA image using LodePNG, transfers it to GPU memory, and applies both grayscale conversion and Sobel convolution in a single optimized kernel. The resulting edge map is saved as a grayscale PNG.

### Key Features

- **Single Optimized Kernel:** Performs both grayscale conversion and Sobel filtering in one kernel pass
- **Constant Memory Usage:** Sobel kernels (Gx and Gy) stored in CUDA constant memory for fast access
- **Zero-Padding Boundary Handling:** Handles image borders gracefully using zero-padding
- **Automatic Output Naming:** Auto-generates output filename if not specified
- **2D Thread Configuration:** Uses 16×16 thread blocks for efficient 2D image processing
- **Luminance-based Grayscale:** Uses standard luminance formula (0.299R + 0.587G + 0.114B)

### How It Works (Gist)

1. Load the input PNG image as 32-bit RGBA using LodePNG
2. Allocate GPU memory and transfer the RGBA image to device
3. Launch 2D CUDA kernel that converts to grayscale and applies Sobel operators
4. Compute gradient magnitude √(Gx² + Gy²) for each pixel
5. Copy the edge map back to host and save as grayscale PNG

### Detailed Explanation

**Sobel Operators:** The algorithm uses two 3×3 convolution kernels:
- **Gx (Horizontal):** `[-1, 0, 1; -2, 0, 2; -1, 0, 1]` - detects vertical edges

```
-1  0  1
-2  0  2
-1  0  1
```

- **Gy (Vertical):** `[-1, -2, -1; 0, 0, 0; 1, 2, 1]` - detects horizontal edges

```
-1  -2  -1
 0   0   0
 1   2   1
```

**CUDA Kernel (`sobel_kernel`):** Each thread processes one pixel:
1. Maps thread indices to pixel coordinates (x, y)
2. Iterates over the 3×3 neighborhood around the pixel
3. For each neighbor, converts RGBA to grayscale using luminance formula
4. Applies Sobel convolution to compute Gx and Gy gradients
5. Computes edge magnitude as √(Gx² + Gy²) and clamps to [0, 255]

**Memory Management:**
- Input RGBA image stored in global memory (4 bytes per pixel)
- Output edge map uses 1 byte per pixel
- Sobel kernels stored in constant memory for cached access across all threads

**Grid Configuration:** Thread blocks of 16×16 threads are mapped to image regions. Grid dimensions are calculated as `ceil(width/16) × ceil(height/16)`.

### Error Handling and Robustness

- **CUDA Error Macro:** `CUDA_CHECK` macro validates all CUDA API calls and reports errors with file/line information
- **File Validation:** Checks if image can be decoded and reports LodePNG error codes
- **Memory Allocation Checks:** Validates both host and device memory allocations
- **Boundary Handling:** Uses zero-padding for pixels at image boundaries
- **Argument Flexibility:** Supports both automatic and user-specified output filenames

### Example Usage

```sh
nvcc lodepng.cpp sobel_gup.cu -o sobel -lm
./sobel <input.png>                  # auto output: input_sobel_edge_output.png
./sobel <input.png> <custom.png>       # explicit output
```

### Files

- `lodepng.cpp` - Library file for LodePNG function definitions.
- `lodepng.h` - Header file for LodePNG functions
- `sobel_gup.cu` - CUDA source code for edge detection using Sobel algorithm

---

## Dependencies

- **C Compiler:** GCC or any standard C compiler supporting C99 or later.
- **NVIDIA CUDA Toolkit:** For Task 3 and Task 4, requires NVCC compiler and CUDA runtime. Compatible with CUDA 10.0 or later.
- **libcrypt:** Required for SHA512 password hashing in Task 3 (`-lcrypt` flag).
- **Standard C Libraries:** `stdio.h`, `stdlib.h`, `string.h`, `math.h` for basic I/O and operations.
- **LodePNG:** PNG encoding/decoding library (included in Task 4 folder).

---

## File Structure

```
2408869_NarayanLohani_6CS005/
├── README.md
├── Task3/
│   ├── CrackPassword.cu                  # CUDA password cracking source
│   ├── CreateEncryptedPasswordText.c     # Password generator source
│   ├── EncryptedPasswords.txt            # Encrypted passwords input
│   ├── PasswordHint.txt                  # Reference hints for verification
│   ├── result.txt                        # Cracked passwords output
│   ├── crack                             # Compiled cracker executable
│   ├── create                            # Compiled generator executable
│   └── screenshots/
│       ├── proof_cracking_passwords.jpeg
│       ├── proof_creating_passwords.jpeg
│       └── proof_generated_files.jpeg
└── Task4/
    ├── sobel_gup.cu                      # CUDA Sobel edge detection source
    ├── lodepng.cpp                       # LodePNG library source
    ├── lodepng.h                         # LodePNG header file
    ├── sobel                             # Compiled Sobel executable
    ├── car.png                           # Sample input image
    ├── car_sobel_edge_output.png         # Edge detection output
    └── proof_screenshot.jpeg             # Execution proof
```

- Each source file is self-contained and well-documented.
- Input and output files are provided for both tasks.
