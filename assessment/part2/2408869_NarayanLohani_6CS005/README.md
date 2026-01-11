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
2. [Task 3: Password Cracking with Files using CUDA](#task-1-count-frequency-of-the-word)
    - [Problem Statement](#problem-statement)
    - [Solution Overview](#solution-overview)
    - [Key Features](#key-features)
    - [How It Works](#how-it-works-gist)
    - [Detailed Explanation](#detailed-explanation)
    - [Error Handling and Robustness](#error-handling-and-robustness)
    - [Example Usage](#example-usage)
    - [Files](#files)
3. [Task 4: Sobel Edge Detection using Cuda](#task-2-matrix-operations)
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

This project demonstrates the use of Graphics Processing Unit (GPU) to using CUDA to execute following problems:

- **[Task 3:](#task-1-count-frequency-of-the-word)** Cracking encrypted passwords using Brute-Force approach
- **[Task 4:](#task-2-matrix-operations)** Detect edges of a png image using Sobel's algorithm


---

# Task 3: Password Cracking with Files using CUDA

### Problem Statement

Given a file containing SHA512 encrypted passwords, crack its password using brute-force all possible password comparing with the help of GPU power.

### Solution Overview



### Key Features



### How It Works (Gist)

1.
2.
3.
4.
5.

### Detailed Explanation



---

### Error Handling and Robustness



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



### Files

- `CreateEncryptedPasswordText.c` — Source code to generate 10 random encrypted passwords.
- `CrackPassword.cu` — Source code to brute force crack passwords.
- `EncryptedPasswords.txt` — Stores encrypted password (to be cracked).
- `PasswordHint.txt` - Stores the raw and cudaCrypt hash of passwords in **EncryptedPaswords.txt** (can be compared with result.txt)
- `result.txt` - Store the passwords cracked from EncryptedPasswords.txt

---

# Task 4: Sobel Edge Detection using Cuda

### Problem Statement

Given an png image, detect the edges of the images sobel edge detection powered by GPU

### Solution Overview



### Key Features



### How It Works (Gist)

1.
2.
3.
4.
5.

### Detailed Explanation


### Error Handling and Robustness

### Example Usage

```sh
nvcc lodepng.cpp sobel_gpu.cu -o sobel -lm
./sobel_gpu <input.png>                  # auto output: input_sobel_edge_output.png
./sobel_gpu <input.png> custom.png       # explicit output
```



### Files

- `lodepng.cpp` - Library file for lodepng function definitions.
- `lodepng.h` - Header file for lodepng functions
- `sobel_gpu.cu` - Source code for edge detection using Sobel algorithm

---

## Dependencies

- **C Compiler:** GCC or any standard C compiler supporting C99 or later.
- **NVIDIA:** For Task 3 and 4, ..........
- **Standard C Libraries:** ...........

---

## File Structure

```
Convert to tree
narayan@narayan 2408869_NarayanLohani_6CS005 % ls -R
README.md       Task3           Task4

./Task3:
CrackPassword.cu                PasswordHint.txt                result.txt
CreateEncryptedPasswordText.c   crack                           screenshots
EncryptedPasswords.txt          create

./Task3/screenshots:
proof_cracking_passwords.jpeg   proof_creating_passwords.jpeg   proof_generated_files.jpeg

./Task4:
car.png                         lodepng.h                       sobel_gup.cu
car_sobel_edge_output.png       proof_screenshot.jpeg
lodepng.cpp                     sobel
narayan@narayan 2408869_NarayanLohani_6CS005 %
```

- Each `Main.c` file is self-contained and well-documented.
- Input and output files are provided for both tasks.
