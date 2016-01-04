#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PARAM_Q 2

typedef int **matrix;

matrix read_matrix(const int row, const int col);
matrix transposition(const matrix m, const int row, const int col);
matrix alloc_matrix(const int row, const int col);
void print_matrix(const matrix m, const int row, const int col);

int main() {
    int row = 0;
    int col = 0;
    
    scanf("%d%d", &row, &col);
    assert(row != 0 && col != 0);
    
    matrix ma = read_matrix(row, col);
    matrix tr_ma = transposition(ma, row, col);
    
//    print_matrix(ma, row, col);
    print_matrix(tr_ma, col, row);   
    
    return 0;
}

matrix read_matrix(const int row, const int col) {
    matrix m = NULL;
    int len = row * col;
    int number;
    
    m = alloc_matrix(row, col);
    
    for (int i = 0; i < len; ++i) {
        if (scanf("%d", &number) == 0) {
            perror("You have to provide only integers value\n");
            return NULL;
        }
        m[0][i] = number;
    }
    
    return m;
}

matrix transposition(const matrix m, const int row, const int col) {
    matrix tr_m = alloc_matrix(col, row);
    
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            tr_m[j][i] = m[i][j];
        }
    }
    
    return tr_m;
}

matrix alloc_matrix(const int row, const int col) {
    matrix m = NULL;
    int len = row * col;
    
    m = malloc(row * sizeof(int *));
    *m = malloc(len * sizeof(int));
    
    for (int i = 1; i < row; ++i) {
        m[i] = m[i - 1] + col;
    }
    
    return m;
}

void print_matrix(const matrix m, const int row, const int col) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            printf("%d ", m[i][j]);
        }
        printf("\n");
    }
}