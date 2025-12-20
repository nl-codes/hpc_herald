/*
 * Parallel Matrix Operations with OpenMP
 * --------------------------------------
 * This program reads pairs of matrices from a file, performs various operations
 * (addition, subtraction, element-wise multiplication/division, transpose, and matrix multiplication)
 * using OpenMP for parallelism, and writes the results to "result_<inputfilename>".
 *
 * Features:
 * - Robust error handling for file I/O and matrix parsing.
 * - Skips blank/whitespace lines in input.
 * - Validates matrix headers and row/column counts.
 * - Handles non-numeric tokens gracefully.
 * - Modular, documented code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <ctype.h>
#include <errno.h>

 // Function declarations
double** allocate_matrix(int rows, int cols);
void free_matrix(double** matrix, int rows);
void print_matrix(FILE* file, double** matrix, int rows, int cols);
double** read_matrix_from_file(FILE* file, int* out_rows, int* out_cols);
double** transpose_matrix(double** matrix, int rows, int cols, int num_threads);
double** add_matrices(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads);
double** subtract_matrices(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads);
double** multiply_elementwise(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads);
double** divide_elementwise(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads);
double** multiply_matrices(double** matrix_a, double** matrix_b, int rows_a, int cols_a, int cols_b, int num_threads);
void process_matrix_pair(FILE* file, double** matrix_a, double** matrix_b, int rows_a, int cols_a, int rows_b, int cols_b, int num_threads, int matrix_number);

/**
 * @brief Main entry point. Reads matrix pairs from file, processes them, and writes results.
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, non-zero on error
 */
int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./matrix <file> <threads>\n");
        return 1;
    }

    // Try to open input file
    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Cannot open input file '%s'.\n", argv[1]);
        return 1;
    }

    int num_threads = atoi(argv[2]);
    if (num_threads <= 0) {
        fprintf(stderr, "Error: Invalid number of threads: %s\n", argv[2]);
        fclose(input_file);
        return 1;
    }

    // Dynamically create output file name: result_<inputfilename>
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "result_%s", argv[1]);
    FILE* output_file = fopen(output_filename, "w");
    if (!output_file) {
        fprintf(stderr, "Error: Cannot open output file '%s' for writing.\n", output_filename);
        fclose(input_file);
        return 1;
    }

    int matrix_pair_count = 0;

    // Read and process matrix pairs until end of file
    while (1) {
        int rows_a, cols_a, rows_b, cols_b;
        double** matrix_a = read_matrix_from_file(input_file, &rows_a, &cols_a);
        if (!matrix_a) {
            if (ferror(input_file)) {
                fprintf(stderr, "Error: File read error occurred while reading matrix A (pair %d).\n", matrix_pair_count + 1);
            }
            break;
        }
        double** matrix_b = read_matrix_from_file(input_file, &rows_b, &cols_b);
        if (!matrix_b) {
            if (ferror(input_file)) {
                fprintf(stderr, "Error: File read error occurred while reading matrix B (pair %d).\n", matrix_pair_count + 1);
            }
            free_matrix(matrix_a, rows_a);
            break;
        }

        matrix_pair_count++;
        process_matrix_pair(output_file, matrix_a, matrix_b, rows_a, cols_a, rows_b, cols_b, num_threads, matrix_pair_count);

        free_matrix(matrix_a, rows_a);
        free_matrix(matrix_b, rows_b);
    }

    if (ferror(input_file)) {
        fprintf(stderr, "Error: An error occurred while reading the input file.\n");
    }
    if (ferror(output_file)) {
        fprintf(stderr, "Error: An error occurred while writing to the output file.\n");
    }

    fclose(input_file);
    fclose(output_file);
    return 0;
}

/**
 * @brief Allocates a 2D matrix of doubles.
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to the allocated matrix
 */
double** allocate_matrix(int rows, int cols) {
    double** matrix = malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++)
        matrix[i] = malloc(cols * sizeof(double));
    return matrix;
}

/**
 * @brief Frees a 2D matrix of doubles.
 * @param matrix Pointer to the matrix
 * @param rows Number of rows
 */
void free_matrix(double** matrix, int rows) {
    for (int i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

/**
 * @brief Prints a matrix to a file in CSV format.
 * @param file Output file pointer
 * @param matrix Matrix to print
 * @param rows Number of rows
 * @param cols Number of columns
 */
void print_matrix(FILE* file, double** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%lf", matrix[i][j]);
            if (j < cols - 1) fprintf(file, ", ");
        }
        fprintf(file, "\n");
    }
}

/**
 * @brief Reads a matrix from a file, skipping blank lines and validating input.
 * @param file Input file pointer
 * @param out_rows Pointer to store number of rows
 * @param out_cols Pointer to store number of columns
 * @return Pointer to the allocated matrix, or NULL on error or EOF
 */
double** read_matrix_from_file(FILE* file, int* out_rows, int* out_cols) {
    char line[4096];

    // Skip blank/whitespace-only lines before header
    while (fgets(line, sizeof(line), file)) {
        int only_space = 1;
        for (char* p = line; *p; ++p) {
            if (!isspace((unsigned char)*p)) { only_space = 0; break; }
        }
        if (!only_space) break;
    }
    if (feof(file)) return NULL;

    // Parse header: must be "%d,%d"
    int rows = 0, cols = 0;
    if (sscanf(line, " %d , %d", &rows, &cols) != 2 || rows <= 0 || cols <= 0) {
        fprintf(stderr, "Error: Invalid header format or non-positive dimensions: '%s'\n", line);
        return NULL;
    }
    double** matrix = allocate_matrix(rows, cols);
    if (!matrix) {
        fprintf(stderr, "Error: Memory allocation failed for matrix of size %dx%d\n", rows, cols);
        return NULL;
    }

    // Read each row, skipping blank/whitespace-only lines
    for (int i = 0; i < rows; i++) {
        while (fgets(line, sizeof(line), file)) {
            int only_space = 1;
            for (char* p = line; *p; ++p) {
                if (!isspace((unsigned char)*p)) { only_space = 0; break; }
            }
            if (!only_space) break;
        }
        if (feof(file)) {
            fprintf(stderr, "Error: Unexpected end of file. Expected %d rows, got %d.\n", rows, i);
            free_matrix(matrix, rows);
            return NULL;
        }

        // Parse row: must have exactly 'cols' numbers separated by ','
        char* ptr = line;
        for (int j = 0; j < cols; j++) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* endptr;
            errno = 0;
            double val = strtod(ptr, &endptr);
            if (ptr == endptr || errno != 0) {
                fprintf(stderr, "Error: Non-numeric or missing value in row %d, col %d: '%s'\n", i + 1, j + 1, line);
                free_matrix(matrix, rows);
                return NULL;
            }
            matrix[i][j] = val;
            ptr = endptr;
            if (j < cols - 1) {
                // Skip whitespace and comma
                while (*ptr && (isspace((unsigned char)*ptr) || *ptr == ',')) ptr++;
            }
        }
        // After reading expected columns, check for any non-numeric junk
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (*ptr && *ptr != '\n' && *ptr != '\0') {
            fprintf(stderr, "Error: Non-numeric or extra token(s) found after expected columns in row %d: '%s'\n", i + 1, line);
            free_matrix(matrix, rows);
            return NULL;
        }
    }

    *out_rows = rows;
    *out_cols = cols;
    return matrix;
}

/**
 * @brief Performs the transpose of a matrix in parallel.
 * @param matrix Input matrix
 * @param rows Number of rows in matrix
 * @param cols Number of columns in matrix
 * @param num_threads Number of threads to use
 * @return Transposed matrix
 */
double** transpose_matrix(double** matrix, int rows, int cols, int num_threads) {
    double** transposed = allocate_matrix(cols, rows);
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            transposed[j][i] = matrix[i][j];
    return transposed;
}

/**
 * @brief Adds two matrices in parallel.
 * @param matrix_a First matrix
 * @param matrix_b Second matrix
 * @param rows Number of rows
 * @param cols Number of columns
 * @param num_threads Number of threads to use
 * @return Resultant matrix
 */
double** add_matrices(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads) {
    double** result = allocate_matrix(rows, cols);
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i][j] = matrix_a[i][j] + matrix_b[i][j];
    return result;
}

/**
 * @brief Subtracts matrix_b from matrix_a in parallel.
 * @param matrix_a First matrix
 * @param matrix_b Second matrix
 * @param rows Number of rows
 * @param cols Number of columns
 * @param num_threads Number of threads to use
 * @return Resultant matrix
 */
double** subtract_matrices(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads) {
    double** result = allocate_matrix(rows, cols);
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i][j] = matrix_a[i][j] - matrix_b[i][j];
    return result;
}

/**
 * @brief Performs element-wise multiplication of two matrices in parallel.
 * @param matrix_a First matrix
 * @param matrix_b Second matrix
 * @param rows Number of rows
 * @param cols Number of columns
 * @param num_threads Number of threads to use
 * @return Resultant matrix
 */
double** multiply_elementwise(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads) {
    double** result = allocate_matrix(rows, cols);
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i][j] = matrix_a[i][j] * matrix_b[i][j];
    return result;
}

/**
 * @brief Performs element-wise division of two matrices in parallel.
 *        If matrix_b[i][j] == 0, result is set to NaN.
 * @param matrix_a First matrix
 * @param matrix_b Second matrix
 * @param rows Number of rows
 * @param cols Number of columns
 * @param num_threads Number of threads to use
 * @return Resultant matrix
 */
double** divide_elementwise(double** matrix_a, double** matrix_b, int rows, int cols, int num_threads) {
    double** result = allocate_matrix(rows, cols);
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix_b[i][j] == 0)
                result[i][j] = 0.0 / 0.0; // NaN
            else
                result[i][j] = matrix_a[i][j] / matrix_b[i][j];
        }
    }
    return result;
}

/**
 * @brief Multiplies two matrices in parallel.
 * @param matrix_a First matrix (rows_a x cols_a)
 * @param matrix_b Second matrix (cols_a x cols_b)
 * @param rows_a Number of rows in matrix_a
 * @param cols_a Number of columns in matrix_a (and rows in matrix_b)
 * @param cols_b Number of columns in matrix_b
 * @param num_threads Number of threads to use
 * @return Resultant matrix (rows_a x cols_b)
 */
double** multiply_matrices(double** matrix_a, double** matrix_b, int rows_a, int cols_a, int cols_b, int num_threads) {
    double** result = allocate_matrix(rows_a, cols_b);
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < rows_a; i++)
        for (int j = 0; j < cols_b; j++) {
            double sum = 0;
            for (int k = 0; k < cols_a; k++)
                sum += matrix_a[i][k] * matrix_b[k][j];
            result[i][j] = sum;
        }
    return result;
}

/**
 * @brief Processes a pair of matrices: performs all operations and writes results.
 * @param file Output file pointer
 * @param matrix_a First matrix
 * @param matrix_b Second matrix
 * @param rows_a Rows in matrix_a
 * @param cols_a Columns in matrix_a
 * @param rows_b Rows in matrix_b
 * @param cols_b Columns in matrix_b
 * @param num_threads Number of threads to use
 * @param matrix_number Index of the matrix pair
 */
void process_matrix_pair(FILE* file, double** matrix_a, double** matrix_b, int rows_a, int cols_a, int rows_b, int cols_b, int num_threads, int matrix_number) {
    fprintf(file, "================================================================================\n");
    fprintf(file, "----------       First: (%d, %d)   MATRIX PAIR - %d   Second: (%d,%d)      ----------\n", rows_a, cols_a, matrix_number, rows_b, cols_b);
    fprintf(file, "================================================================================\n\n");

    // Element-wise operations (only if dimensions match)
    if (rows_a == rows_b && cols_a == cols_b) {
        fprintf(file, "Addition (%d x %d)\n", rows_a, cols_a);
        double** result = add_matrices(matrix_a, matrix_b, rows_a, cols_a, num_threads);
        print_matrix(file, result, rows_a, cols_a);
        free_matrix(result, rows_a);

        fprintf(file, "\nSubtraction (%d x %d)\n", rows_a, cols_a);
        result = subtract_matrices(matrix_a, matrix_b, rows_a, cols_a, num_threads);
        print_matrix(file, result, rows_a, cols_a);
        free_matrix(result, rows_a);

        fprintf(file, "\nElement-wise Multiply (%d x %d)\n", rows_a, cols_a);
        result = multiply_elementwise(matrix_a, matrix_b, rows_a, cols_a, num_threads);
        print_matrix(file, result, rows_a, cols_a);
        free_matrix(result, rows_a);

        fprintf(file, "\nElement-wise Divide (%d x %d)\n", rows_a, cols_a);
        result = divide_elementwise(matrix_a, matrix_b, rows_a, cols_a, num_threads);
        print_matrix(file, result, rows_a, cols_a);
        free_matrix(result, rows_a);
    } else {
        fprintf(file, "Addition not possible (different sizes [%d,%d] and [%d,%d])\n", rows_a, cols_a, rows_b, cols_b);
        fprintf(file, "Subtraction not possible (different sizes) [%d,%d] and [%d,%d]\n", rows_a, cols_a, rows_b, cols_b);
        fprintf(file, "Element-wise operations not possible (different sizes [%d,%d] and [%d,%d])\n", rows_a, cols_a, rows_b, cols_b);
    }

    // Transpose operations
    fprintf(file, "\nTranspose A (%d x %d)\n", cols_a, rows_a);
    double** transposed = transpose_matrix(matrix_a, rows_a, cols_a, num_threads);
    print_matrix(file, transposed, cols_a, rows_a);
    free_matrix(transposed, cols_a);

    fprintf(file, "\nTranspose B (%d x %d)\n", cols_b, rows_b);
    transposed = transpose_matrix(matrix_b, rows_b, cols_b, num_threads);
    print_matrix(file, transposed, cols_b, rows_b);
    free_matrix(transposed, cols_b);

    // Matrix multiplication (only if matrix_a's columns == matrix_b's rows)
    if (cols_a == rows_b) {
        fprintf(file, "\nMatrix Multiply A x B (%d x %d)\n", rows_a, cols_b);
        double** result = multiply_matrices(matrix_a, matrix_b, rows_a, cols_a, cols_b, num_threads);
        print_matrix(file, result, rows_a, cols_b);
        free_matrix(result, rows_a);
    } else {
        fprintf(file, "\nMatrix Multiply not possible [%d,%d] and [%d,%d] : Acols != Brows\n", rows_a, cols_a, rows_b, cols_b);
    }
    fprintf(file, "\n");
}
