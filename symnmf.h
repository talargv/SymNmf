# ifndef SYMNMF_H_
# define SYMNMF_H_

void symnmf(double **H, int n, int k, double **W);
double** sym(double** data_points, int quan, int dim);
double** ddg(double **data_points, int quan, int dim);
double** norm(double **data_points, int quan, int dim);

# endif

# ifndef EPSILON
#define EPSILON 0.0001
# endif

# ifndef MAX_ITER
#define MAX_ITER 300
# endif

