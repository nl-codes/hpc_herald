/*
 * Multithreaded Word Counter
 * --------------------------
 * This program reads a text file, splits the work among multiple threads,
 * and counts the occurrences of each word (case-insensitive, alphanumeric only).
 * Results are written to "result.txt" in sorted order.
 *
 * Thread boundaries are adjusted to word boundaries to avoid splitting words.
 * Each thread processes a unique chunk of the file, and a mutex is used to
 * synchronize access to the shared word count array.
 *
 * Usage:
 *   ./output <num_threads> <file_name>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_WORDS 500      // Maximum number of unique words
#define MAX_WORD_LEN 50     // Maximum length of a word

 // Structure to store a word and its count
typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordCount;

// Structure to pass data to each thread
typedef struct {
    char* text;     // Pointer to the text buffer
    long start;     // Start index (inclusive)
    long end;       // End index (exclusive)
} ThreadData;

// Global word count array and related variables
WordCount words[MAX_WORDS];
int word_count = 0;
pthread_mutex_t lock;   // Mutex for synchronizing access to words array

// Function declarations
long get_file_size(FILE* fptr);
char* read_file_to_buffer(const char* filename, long* buffer_size);
void fix_thread_boundaries(char* buffer, long file_size, int num_threads, ThreadData* thread_data);
void* thread_routine_find_word_count(void* arg);
void handle_words(const char* w);
int compare_routine(const void* a, const void* b);
void write_results(const char* filename);

/**
 * @brief Main entry point. Parses arguments, launches threads, and writes results.
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, non-zero on error
 */
int main(int argc, char* argv[]) {
    // Check the usage
    if (argc != 3) {
        printf("Usage invalid.\nFormat : ./output thread_nums file_name");
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        printf("Number of threads must be greater than 0.\n");
        return 1;
    }

    long file_size;
    // Read the entire file into a buffer
    char* buffer = read_file_to_buffer(argv[2], &file_size);
    if (!buffer) {
        printf("Failed to read file.\n");
        return 1;
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    pthread_mutex_init(&lock, NULL);

    // Adjust thread boundaries to word boundaries
    fix_thread_boundaries(buffer, file_size, num_threads, thread_data);

    // Launch threads to process their respective slices
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, thread_routine_find_word_count, &thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    // Sort the words alphabetically
    qsort(words, word_count, sizeof(WordCount), compare_routine);

    // Write the results to a file
    write_results("result.txt");

    pthread_mutex_destroy(&lock);
    free(buffer);

    return 0;
}

/**
 * @brief Get the size of a file in bytes.
 * @param fptr Pointer to the opened file
 * @return File size in bytes
 */
long get_file_size(FILE* fptr) {
    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    rewind(fptr);
    return file_size;
}

/**
 * @brief Reads the entire file into a dynamically allocated buffer.
 * @param filename Name of the file to read
 * @param buffer_size Pointer to store the size of the buffer
 * @return Pointer to the buffer (must be freed by caller), or NULL on failure
 */
char* read_file_to_buffer(const char* filename, long* buffer_size) {
    FILE* fptr = fopen(filename, "r");
    if (!fptr) return NULL;
    long size = get_file_size(fptr);

    char* buffer_memroy = malloc(size + 1);  // +1 for null terminator
    if (!buffer_memroy) {
        fclose(fptr);
        return NULL;
    }
    fread(buffer_memroy, 1, size, fptr);
    buffer_memroy[size] = '\0';
    fclose(fptr);
    *buffer_size = size;
    return buffer_memroy;
}

/**
 * @brief Adjusts thread boundaries so that no word is split between threads.
 *        Each thread will start and end at a word boundary.
 * @param buffer The text buffer
 * @param file_size Size of the buffer
 * @param num_threads Number of threads
 * @param thread_data Array to fill with thread data
 */
void fix_thread_boundaries(char* buffer, long file_size, int num_threads, ThreadData* thread_data) {
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

        thread_data[i].text = buffer;
        thread_data[i].start = start;
        thread_data[i].end = end;
    }
}

/**
 * @brief Thread routine: processes a slice of text, extracts words, and updates counts.
 * @param arg Pointer to ThreadData struct
 * @return NULL
 */
void* thread_routine_find_word_count(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char word[MAX_WORD_LEN];
    int char_index = 0;

    // Iterate through assigned slice
    for (long i = data->start; i < data->end; i++) {
        char c = data->text[i];
        if (isalnum(c)) {
            // Build the word, converting to lowercase
            if (char_index < MAX_WORD_LEN - 1) {
                word[char_index++] = tolower(c);
            }
        } else {
            // End of a word
            if (char_index > 0) {
                word[char_index] = '\0';
                handle_words(word);
                char_index = 0;
            }
        }
    }
    // Add last word if present
    if (char_index > 0) {
        word[char_index] = '\0';
        handle_words(word);
    }
    return NULL;
}

/**
 * @brief Adds a word to the global word list or increments its count if it exists.
 *        Uses a mutex to synchronize access.
 * @param w The word to add or increment
 */
void handle_words(const char* found_word) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < word_count; i++) {
        // If word already exists, increase count
        if (strcmp(words[i].word, found_word) == 0) {
            words[i].count++;
            pthread_mutex_unlock(&lock);
            return;
        }
    }
    // If word doesn't exits, add word
    strncpy(words[word_count].word, found_word, MAX_WORD_LEN - 1);
    words[word_count].word[MAX_WORD_LEN - 1] = '\0';
    words[word_count].count = 1;
    word_count++;
    pthread_mutex_unlock(&lock);
}

/**
 * @brief Comparison function for qsort: sorts words alphabetically.
 * @param a Pointer to first WordCount
 * @param b Pointer to second WordCount
 * @return <0 if a < b, 0 if equal, >0 if a > b
 */
int compare_routine(const void* a, const void* b) {
    return strcmp(((WordCount*)a)->word, ((WordCount*)b)->word);
}

/**
 * @brief Writes the sorted word counts to a file.
 * @param filename Output file name
 */
void write_results(const char* filename) {
    FILE* out = fopen(filename, "w");
    if (!out) return;
    for (int i = 0; i < word_count; i++)
        fprintf(out, "%s %d\n", words[i].word, words[i].count);
    fclose(out);
}
