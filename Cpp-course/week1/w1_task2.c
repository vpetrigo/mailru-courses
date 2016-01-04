#include <stdio.h>
#include <stdlib.h>

typedef struct stack {
    char ch;
    struct stack *next;
} stack_elem_t;

typedef stack_elem_t *stack_elem_ptr;

stack_elem_ptr read_character(void);
void reverse_string_from_input(void);
void push_in_stack(stack_elem_ptr *head, stack_elem_ptr elem);
char pop_from_stack(stack_elem_ptr *head);

int main() {
    reverse_string_from_input();
    
    return 0;
}

void push_in_stack(stack_elem_ptr *head, stack_elem_ptr elem) {
    // if this is the first elemen
    if (!*head) {
        *head = elem;
        (*head)->next = NULL;
    }
    else {
        elem->next = *head;
        *head = elem;
    }
}

char pop_from_stack(stack_elem_ptr *head) {
    stack_elem_ptr elem = NULL;
    char ch = '\0';
    // if stack is not empty
    if (*head) {
        elem = *head;
        *head = (*head)->next;
        ch = elem->ch;
        free(elem);
    }
    
    return ch;
}

stack_elem_ptr read_character(void) {
    char ch = '\0';
    stack_elem_ptr elem = NULL;
    
    // if there is a character left in the input
    if ((ch = getchar()) != EOF) {
        elem = malloc(sizeof(stack_elem_t));
        elem->ch = ch;
        elem->next = NULL;
    }
    
    return elem;
}

void reverse_string_from_input(void) {
    stack_elem_ptr head = NULL;
    stack_elem_ptr new_elem = NULL;
    char rev_ch = '\0';
    
    while ((new_elem = read_character()) != NULL) {
        push_in_stack(&head, new_elem);
    }
    
    while ((rev_ch = pop_from_stack(&head))) {
        printf("%c", rev_ch);
        // after we printed a stack element we do not need it anymore
    }
    printf("\n");
}