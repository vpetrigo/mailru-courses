#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define WORD_SIZE 100

double count_word_occurence(const char *word);
int read_word_in_sentence(char *word_snt);

int main() {
    // input word
    char word[WORD_SIZE];
    // check if it is possible to read a word
    int readed_cnt = scanf("%s", word);
    
    if (!readed_cnt) {
        perror("You didn't enter the word");
        
        return 1;
    }
    
    printf("%f\n", count_word_occurence(word));
    
    return 0;
}

double count_word_occurence(const char *word) {
    int found_the_word = 0;
    int amount_of_sentences = 0;
    int read_res = 0;
    char word_sent[WORD_SIZE];
    
    while ((read_res = read_word_in_sentence(word_sent)) != 0) {
        if (read_res == 1) {
            ++amount_of_sentences;
        }
        else {
            if (strcmp(word, word_sent) == 0) {
                ++found_the_word;
            }
        }
    }
    
    return (double) found_the_word / amount_of_sentences;
}

// function for reading words from input sentences
// in case of EOF, returns 0
// in case of punctuation, returns 1
// in case of good output, fill the word and returns 2
int read_word_in_sentence(char *word_snt) {
    char ch = '\0';
    
    // pass spaces
    while ((ch = getchar()) != EOF && isspace(ch)) {
    }
    
    if (ch == EOF) {
        return 0;
    }
    else if (ispunct(ch)) {
        return 1;
    }
    else {
        ungetc(ch, stdin);
    }
    
    while ((ch = getchar()) != EOF && !isspace(ch) && !ispunct(ch)) {
        *word_snt++ = ch;
    }
    
    *word_snt = '\0';
    
    ungetc(ch, stdin);
    
    return 2;
}
