#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_WORDS 5000
#define MAX_WORD_LEN 50

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordCount;

typedef struct {
    char* text;
    long start;
    long end;
} ThreadData;


WordCount words[MAX_WORDS];
int word_count = 0;
pthread_mutex_t lock;

long get_file_size(FILE* fptr);
char* read_file_to_buffer(const char* filename, long* buffer_size);
void fix_thread_boundaries(char* buffer, long file_size, int num_threads, ThreadData* thread_data);
void* thread_routine_find_word_count(void* arg);
void handle_words(const char* w);
int compare_routine(const void* a, const void* b);
void write_results(const char* filename);


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
    char* buffer = read_file_to_buffer(argv[2], &file_size);
    if (!buffer) {
        printf("Failed to read file.\n");
        return 1;
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    pthread_mutex_init(&lock, NULL);

    fix_thread_boundaries(buffer, file_size, num_threads, thread_data);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, thread_routine_find_word_count, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    qsort(words, word_count, sizeof(WordCount), compare_routine);
    write_results("result.txt");

    pthread_mutex_destroy(&lock);
    free(buffer);

    return 0;
}

long get_file_size(FILE* fptr) {
    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    rewind(fptr);
    return file_size;
}

char* read_file_to_buffer(const char* filename, long* buffer_size) {
    FILE* fptr = fopen(filename, "r");
    if (!fptr) return NULL;
    long size = get_file_size(fptr);

    char* buffer_memroy = malloc(size + 1);
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

void fix_thread_boundaries(char* buffer, long file_size, int num_threads, ThreadData* thread_data) {
    long slice = file_size / num_threads;
    for (int i = 0; i < num_threads; i++) {
        long start = i * slice;
        long end = (i == num_threads - 1) ? file_size : (i + 1) * slice;

        // Adjust start (except for first thread)
        if (i != 0) {
            while (start < file_size && isalnum(buffer[start])) {
                start++;
            }
        }
        // Adjust end (except for last thread)
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

void* thread_routine_find_word_count(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char word[MAX_WORD_LEN];
    int char_index = 0;

    for (long i = data->start; i < data->end; i++) {
        char c = data->text[i];
        if (isalnum(c)) {
            if (char_index < MAX_WORD_LEN - 1) {
                word[char_index++] = tolower(c);
            }
        } else {
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

void handle_words(const char* w) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < word_count; i++) {
        // If word already exists, increase count
        if (strcmp(words[i].word, w) == 0) {
            words[i].count++;
            pthread_mutex_unlock(&lock);
            return;
        }
    }
    // If word doesn't exits, add word
    strncpy(words[word_count].word, w, MAX_WORD_LEN - 1);
    words[word_count].word[MAX_WORD_LEN - 1] = '\0';
    words[word_count].count = 1;
    word_count++;
    pthread_mutex_unlock(&lock);
}

int compare_routine(const void* a, const void* b) {
    return strcmp(((WordCount*)a)->word, ((WordCount*)b)->word);
}

void write_results(const char* filename) {
    FILE* out = fopen(filename, "w");
    if (!out) return;
    for (int i = 0; i < word_count; i++)
        fprintf(out, "%s %d\n", words[i].word, words[i].count);
    fclose(out);
}
