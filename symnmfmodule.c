#include <stdio.h>
#include <math.h>
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"

double** data_points_from_python(PyObject *self, PyObject *args, int *n_ptr, int *m_ptr) {
    double **data_points, *dp; //dp is short for 'data_point'
    int n, m, i, j;
    PyObject *p_data_points, *p_dp, *p_double; //p_dp is short for 'p_data_point'

    // parsing arguments
    if(!PyArg_ParseTuple(args, "Oii", &p_data_points, &n, &m)) {
        PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
        return NULL;
    }

    // init data_points
    data_points = (double**)malloc(sizeof(double*)*n);
    if (data_points == NULL) {
        PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
        return NULL;
    }

    // getting data_points from the pyObject representing the entries
    for (i = 0; i < n; i++) {
        // getting the current data_point and init the C data_point
        p_dp = PyList_GetItem(p_data_points, i);
        dp = (double*)malloc(sizeof(double)*m);
        if(dp == NULL){
            PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
            return NULL;
        }
        for(j = 0; j < m; j++) {
            p_double = PyList_GetItem(p_dp, j);
            dp[j] = PyFloat_AsDouble(p_double);
        }
        data_points[i] = dp;
    }

    *n_ptr = n;
    *m_ptr = m;
    return data_points;
}

double** two_matrices_from_python(PyObject *self, PyObject *args, int *n1_ptr, int *m1_ptr, int *n2_ptr, int *m2_ptr, double ***mat_ptr) {
    // assumes matrix1 is nxm and matrix 2 is nxn
    double **matrix1, **matrix2, *dp; //dp is short for 'data_point'
    int n, m, i, j;
    PyObject *p_matrix1, *p_dp, *p_double, *p_matrix2; //p_dp is short for 'p_data_point'

    // parsing arguments
    if(!PyArg_ParseTuple(args, "OiiO", &p_matrix1, &n, &m, &p_matrix2)) {
        PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
        return NULL;
    }

    // init matrix1
    matrix1 = (double**)malloc(sizeof(double*)*n);
    if (matrix1 == NULL) {
        PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
        return NULL;
    }

    matrix2 = (double**)malloc(sizeof(double*)*n);
    if (matrix2 == NULL) {
        PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
        return NULL;
    }

    // getting matrix1 from the pyObject representing the entries
    for (i = 0; i < n; i++) {
        // getting the current data_point and init the C data_point
        p_dp = PyList_GetItem(p_matrix1, i);
        dp = (double*)malloc(sizeof(double)*m);
        if(dp == NULL){
            PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
            return NULL;
        }
        for(j = 0; j < m; j++) {
            p_double = PyList_GetItem(p_dp, j);
            dp[j] = PyFloat_AsDouble(p_double);
        }
        matrix1[i] = dp;
    }

    for (i = 0; i < n; i++) {
        // getting the current data_point and init the C data_point
        p_dp = PyList_GetItem(p_matrix2, i);
        dp = (double*)malloc(sizeof(double)*n);
        if(dp == NULL){
            PyErr_SetString(PyExc_MemoryError, "An Error Has Occurred");
            return NULL;
        }
        for(j = 0; j < n; j++) {
            p_double = PyList_GetItem(p_dp, j);
            dp[j] = PyFloat_AsDouble(p_double);
        }
        matrix2[i] = dp;
    }

    *n1_ptr = n;
    *m1_ptr = m;
    *n2_ptr = n;
    *m2_ptr = n;
    *mat_ptr = matrix2;
    return matrix1;
}

PyObject* data_points_to_python(double** result_matrix, int n, int m) {
    PyObject *p_result_matrix, *result_vector, *p_double;
    int i, j;
    p_result_matrix = PyList_New(n);
    
    for(i = 0; i < n; i++) {
        result_vector = PyList_New(m);
        for(j = 0; j < m; j++) {
            p_double = Py_BuildValue("d", result_matrix[i][j]);
            PyList_SetItem(result_vector, j, p_double);
        }
        PyList_SetItem(p_result_matrix, i, result_vector);
    }
    return p_result_matrix;
}

static PyObject* symnmf_py(PyObject *self, PyObject *args) {
    int H_n, H_m, W_n, W_m, i;
    double **H, **W;
    PyObject *res;

    H = two_matrices_from_python(self, args, &H_n, &H_m, &W_n, &W_m, &W);
    symnmf(H, H_n, H_m, W);
    res = data_points_to_python(H, H_n, H_m);
    for (i=0; i<H_n; i++){
        free(H[i]);
    }
    for (i=0; i<W_n; i++){
        free(W[i]);
    }
    free(H);
    free(W);
    return res;
}

static PyObject* sym_py(PyObject *self, PyObject *args) {
    int quan, dim, i;
    double **data_points, **result_matrix;
    PyObject *res;

    data_points = data_points_from_python(self, args, &quan, &dim);
    result_matrix = sym(data_points, quan, dim);
    res = data_points_to_python(result_matrix, quan, quan);

    for (i=0; i<quan; i++){
        free(data_points[i]);
    }

    free(data_points);
    free(result_matrix[0]);
    free(result_matrix);

    return res;
}

static PyObject* ddg_py(PyObject *self, PyObject *args) {
    int quan, dim, i;
    double **data_points, **result_matrix;
    PyObject *res;

    data_points = data_points_from_python(self, args, &quan, &dim);
    result_matrix = ddg(data_points, quan, dim);
    res = data_points_to_python(result_matrix, quan, quan);

    for (i=0; i<quan; i++){
        free(data_points[i]);
    }

    free(data_points);
    free(result_matrix[0]);
    free(result_matrix);

    return res;
}

static PyObject* norm_py(PyObject *self, PyObject *args) {
    int quan, dim, i;
    double **data_points, **result_matrix;
    PyObject *res;

    data_points = data_points_from_python(self, args, &quan, &dim);
    result_matrix = norm(data_points, quan, dim);
    res = data_points_to_python(result_matrix, quan, quan);

    for (i=0; i<quan; i++){
        free(data_points[i]);
    }

    free(data_points);
    free(result_matrix[0]);
    free(result_matrix);
    
    return res;
}

static PyMethodDef symnmf_methods[] = {
    {"symnmf",                   /* the Python method name that will be used */
      (PyCFunction) symnmf_py, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters accepted for this function */
      PyDoc_STR("Given a set of data-points in a matrix and the matrix's dimensions,\
                 performs the full symNMF proccess and returns the result") /*  The docstring for the function */
    },
    {"sym",                   /* the Python method name that will be used */
      (PyCFunction) sym_py, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters accepted for this function */
      PyDoc_STR("Given a set of data-points in a matrix and the matrix's dimensions,\
                 calculates and returns the similarity matrix") /*  The docstring for the function */
    },
    {"ddg",                   /* the Python method name that will be used */
      (PyCFunction) ddg_py, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters accepted for this function */
      PyDoc_STR("Given a set of data-points in a matrix and the matrix's dimensions,\
                 calculates and returns the diagonal degree matrix") /*  The docstring for the function */
    },
    {"norm",                   /* the Python method name that will be used */
      (PyCFunction) norm_py, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters accepted for this function */
      PyDoc_STR("Given a set of data-points in a matrix and the matrix's dimensions,\
                 calculates and returns the normalized similarity matrix") /*  The docstring for the function */
    },
    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */
};

static struct PyModuleDef symnmf_mod = {
    PyModuleDef_HEAD_INIT,
    "symnmf_module", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    symnmf_methods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_symnmf_module(void) {
    PyObject *m;
    m = PyModule_Create(&symnmf_mod);
    if(!m) {
        return NULL;
    }
    return m;
}