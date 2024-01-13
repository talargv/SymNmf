#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "symnmf.h"

double* split(char *line, int len){
    double *out;
    int i;
    char *p;
    char* lineBU;

    lineBU = line;

    out = (double*)malloc(sizeof(double)*len);
    if (out == NULL){
        printf("An Error Has Occurred");
        free(lineBU);
        return NULL;
    }

    i = 0;
    while(i < len) {
        p = NULL;
        out[i] = strtod(line, &p);
        i++;
        line = p+1;
    }

    free(lineBU);
    return out;
}

char* retline(FILE *fp) {
    int cur_size, i;
    char * curline;
    char c;

    cur_size = 10;
    curline = (char*)malloc(sizeof(char)*cur_size);
    if (curline == NULL) {
        free(curline);
        printf("An Error Has Occurred");
        return NULL;
    }
    i = 0;

    while((c = fgetc(fp)) != '\n' && c != EOF) {
        if (i >= cur_size) {
            cur_size*=2;
            curline = (char*)realloc(curline, sizeof(char)*cur_size);
            if (curline == NULL) {
                free(curline);
                printf("An Error Has Occurred");
                return NULL;
            }
        }
        curline[i] = c;
        i++;
    }
    curline = (char*)realloc(curline, sizeof(char)*(i+1));
    if (curline == NULL) {
        free(curline);
        printf("An Error Has Occurred");
        return NULL;
    }
    curline[i]='\0';
    return curline;
}

double** read_file(char* file_name, int* dim_ptr, int* quan_ptr) {
    FILE *fp;
    double **data_points, *cur_point;
    char* line;
    int dim, i, quan, data_points_len;

    /**opening file**/
    fp = fopen(file_name, "r");
    if (fp == NULL){
        printf("%s", "An error has occured");
    }

    /**reading first line**/
    line = retline(fp);
    if (line == NULL) {
        free(line);
        printf("%s", "An error has occured");
        return NULL;
    }

    /**finding dim**/
    dim = 1;
    for (i=0; line[i]!='\0';i++){
        if (*(line+i) == ',') {
            dim++;
        }
    }
    *dim_ptr = dim;

    /**rewinding back**/
    fseek(fp, 0, 0);

    /**generating datapoints**/
    data_points_len = 10;
    data_points = (double**)malloc(sizeof(double*)*data_points_len);
    quan = 0;
    free(line);
    while(strlen((line = retline(fp)))) {
        if(quan == data_points_len) {
            data_points_len *= 2;
            data_points = (double**)realloc(data_points, sizeof(double*)*data_points_len);
            if (data_points == NULL){
                printf("%s", "An error has occured");
                return NULL;
            }
        }
        cur_point = split(line, dim);
        if (cur_point == NULL){
            printf("%s", "An error has occured");
            return NULL;
        }
        data_points[quan] = cur_point;

        quan++;
    }

    free(line);

    /**fininsing realloc**/
    data_points = (double**)realloc(data_points, sizeof(double*)*quan);
    if (data_points == NULL){
        printf("%s", "An error has occured");
        return NULL;
    }

    /**updating quan**/
    *quan_ptr = quan;

    /**close file**/
    fclose(fp);

    return data_points;
}

double ocl_norm(double* point_1, double* point_2, int dim){
    int i;
    double norm;
    norm = 0;
    for(i = 0; i < dim; i++){
        norm += pow((point_1[i]-point_2[i]),2);
    }
    return norm;
}

double** sym(double** data_points, int quan, int dim) {
    int i, j;
    double *entries, **A;

    entries = (double*)malloc(sizeof(double)*quan*quan);
    A = (double**)malloc(sizeof(double*)*quan);
    for (i=0; i < quan; i++){
        A[i] = entries + i*quan;
    }

    for(i = 0; i < quan; i++){
        for(j = 0; j < quan; j++) {
            if (i != j) {
                A[i][j] = exp(-ocl_norm(data_points[i],data_points[j], dim)/2);
            } else {
                A[i][j] = 0;
            }
            
        }
    }

    return A;
}

double** matrix_multiplication(double** matrix1, double** matrix2, int m1, int n ,int m2){
    /** computes matrix1 * matrix2
        assumes matrix1 is m1xn and matrix 2 is nxm2 **/
    int i,j,k;
    double* elems;
    double** A;
    elems = (double *)malloc(sizeof(double)*m1*m2);
    A = (double**)malloc(sizeof(double*)*m1);
    for (i=0; i < m1; i ++){
        A[i] = elems + i*m2;
    }
    for (i=0; i < m1; i++){
        for (j=0; j < m2; j++){
            A[i][j] = 0;
            for (k=0; k < n; k++){
                A[i][j] += matrix1[i][k]*matrix2[k][j];
            }
        }
    }
    return A;
}

double** ddg_sub(double **A, int m, int n) {
    int i,j;
    double *data, **D;
    data = (double*)calloc(m*m,sizeof(double));
    D = (double**)malloc(sizeof(double*)*m);
    for (i=0; i < m; i++){
        D[i] = data + i*m;
    }
    for (i=0; i < m; i++){
        for (j=0; j<n; j++){
            D[i][i] += A[i][j];
        }
    }
    return D;
}

double** ddg(double **data_points, int quan, int dim) {
    double **A, **D;
    A = sym(data_points, quan, dim);
    D = ddg_sub(A, quan, quan);
    free(A[0]);
    free(A);
    return D;
}

void sqrt_diag(double **A, int n) {
    int i;
    for (i = 0; i < n; i++) {
        A[i][i] = 1/sqrt(A[i][i]);
    }
}

double** norm_sub(double **D, double **A, int quan) {
    double **W, **tmp;
    sqrt_diag(D, quan);
    tmp = matrix_multiplication(D, A, quan, quan, quan);
    W = matrix_multiplication(tmp, D, quan, quan, quan);
    free(tmp[0]);
    free(tmp);
    return W;
}

double** norm(double **data_points, int quan, int dim) {
    double **A, **D, **W;
    A = sym(data_points, quan, dim);
    D = ddg_sub(A, quan, quan);
    W = norm_sub(D, A, quan);
    free(A[0]);
    free(A);
    free(D[0]);
    free(D);
    return W;
}

void symnmf(double **H, int n, int k, double **W){
    /** H is nxk. W is nxn.**/
    int i,j,t,l,iter;
    double **next_H, *data, WH, HHTH, err; 
    /** WH and HHTH are products defined in 1.4.2**/
    
    data = (double*)malloc(n*k*sizeof(double));
    next_H = (double**)malloc(n*sizeof(double*));

    for (i=0; i<n; i++){
        next_H[i] = data + k*i;
    }

    err = EPSILON + 1;
    iter = 0;
    while (err >= EPSILON && iter < MAX_ITER){
        err = 0;
        for (i=0; i<n; i++){
            for (j=0; j<k; j++){
                WH = 0;
                HHTH = 0;
                for (l=0; l<n; l++){
                    WH += W[i][l]*H[l][j];
                    for (t=0; t<k; t++){
                        HHTH += H[i][t]*H[l][j]*H[l][t];
                    }
                }
                next_H[i][j] = H[i][j]*(0.5 + (0.5*WH)/HHTH);
                err += pow(next_H[i][j]-H[i][j],2);
            }
        }
        for (i=0; i<n; i++){
            for(j=0; j<k; j++){
                H[i][j] = next_H[i][j];
            }
        }
        iter += 1;

    }
    /** free memory**/
    free(next_H);
    free(data);
}

int main(int argc, char* argv[]) {
    char *goal, *file_name;
    double **data_points, **A;
    int dim, quan, i, j;

    /**checking args**/
    if (argc != 3) {
        printf("%s", "An error has occured");
        return 0;
    }

    /**init args**/
    goal = argv[1];
    file_name = argv[2];

    /**init data-points**/
    data_points = read_file(file_name, &dim, &quan);

    /**make required goal**/
    if (!strcmp(goal, "sym")) {
        A = sym(data_points, quan, dim);
    } else if (!strcmp(goal, "ddg")) {
        A = ddg(data_points, quan, dim);
    } else if (!strcmp(goal, "norm")) {
        A = norm(data_points, quan, dim);
    } else {
        printf("%s", "An error has occured");
        return 0;
    }
    
    for(i=0; i<quan; i++){
        for(j=0; j<quan-1; j++){
            printf("%.4f,",A[i][j]);
        }
        printf("%.4f\n", A[i][j]);
        /** freeing memory for data_points **/
        free(data_points[i]);
    }
    free(data_points);
    free(A[0]);
    free(A);
    return 0;
}