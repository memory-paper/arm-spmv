/**
 * @file    : sparse_matrix.h
 * @author  : theSparky Team
 * @version :
 *
 * Functions for Sparse Matrix Init.
 */
#include <string.h>

#include "matrix.h"

// ====================================================
// ############### COO_Matrix Functions ###############
// ====================================================

COO_Matrix::COO_Matrix() : nrow(0), ncol(0), nnz(0), row_ind(0), col_ind(0), values(0) {}

COO_Matrix::COO_Matrix(int _n, int _m, int _nnz, int* _row_ind, int* _col_ind, double* _values)
    : nrow(_n), ncol(_m), nnz(_nnz), row_ind(_row_ind), col_ind(_col_ind), values(_values)
{
}

COO_Matrix::COO_Matrix(const COO_Matrix& A)
    : nrow(A.nrow), ncol(A.ncol), nnz(A.nnz), row_ind(new int[A.nnz]), col_ind(new int[A.nnz]), values(new double[A.nnz])
{
    int*    Ai = A.row_ind;
    int*    Aj = A.col_ind;
    double* Av = A.values;

    for (int k = 0; k < nnz; ++k)
    {
        row_ind[k] = Ai[k];
        col_ind[k] = Aj[k];
        values[k]  = Av[k];
    }
}

COO_Matrix::~COO_Matrix()
{
    if (row_ind)
        delete[] row_ind;
    if (col_ind)
        delete[] col_ind;
    if (values)
        delete[] values;
}

COO_Matrix& COO_Matrix::operator=(const COO_Matrix& A)
{
    Free();

    int*    Ai = A.row_ind;
    int*    Aj = A.col_ind;
    double* Av = A.values;

    nrow = A.nrow;
    ncol = A.ncol;
    nnz  = A.nnz;

    row_ind = new int[nnz];
    col_ind = new int[nnz];
    values  = new double[nnz];

    for (int k = 0; k < nnz; ++k)
    {
        row_ind[k] = Ai[k];
        col_ind[k] = Aj[k];
        values[k]  = Av[k];
    }
    return *this;
}

void COO_Matrix::Free()
{
    if (row_ind)
        delete[] row_ind;
    if (col_ind)
        delete[] col_ind;
    if (values)
        delete[] values;

    nrow    = 0;
    ncol    = 0;
    nnz     = 0;
    row_ind = 0;
    col_ind = 0;
    values  = 0;
}

// ====================================================
// ############### CSR_Matrix Functions ###############
// ====================================================
CSR_Matrix::CSR_Matrix() : nrow(0), ncol(0), row_ptr(0), col_ind(0), values(0), diagonal(0) {}

CSR_Matrix::CSR_Matrix(int _n, int _m, int* _row_ptr, int* _col_ind, double* _values, double* _diagonal)
    : nrow(_n), ncol(_m), row_ptr(_row_ptr), col_ind(_col_ind), values(_values), diagonal(_diagonal)
{
}

CSR_Matrix::CSR_Matrix(const CSR_Matrix& A)
    : nrow(A.nrow), ncol(A.ncol), row_ptr(new int[A.nrow + 1]), col_ind(new int[A.row_ptr[A.nrow]]), values(new double[A.row_ptr[A.nrow]]),
      diagonal(new double[A.nrow])
{
    int*    Ap = A.row_ptr;
    int*    Aj = A.col_ind;
    double* Av = A.values;
    double* Ad = A.diagonal;

    for (int i = 0; i <= nrow; ++i)
        row_ptr[i] = Ap[i];

    for (int j = 0; j < Ap[nrow]; ++j)
    {
        col_ind[j] = Aj[j];
        values[j]  = Av[j];
    }

    for (int i = 0; i < nrow; ++i)
        diagonal[i] = Ad[i];
}

CSR_Matrix::CSR_Matrix(const COO_Matrix& A) : row_ptr(new int[A.nrow + 1]), col_ind(new int[A.nnz]), values(new double[A.nnz]), diagonal(new double[A.nrow])
{
    int     nnz = A.nnz;
    int*    Ai  = A.row_ind;
    int*    Aj  = A.col_ind;
    double* Av  = A.values;

    nrow = A.nrow;
    ncol = A.ncol;

    for (int i = 0; i <= nrow; ++i)
    {
        row_ptr[i] = 0;
    }

    for (int k = 0; k < nnz; ++k)
    {
        ++row_ptr[Ai[k]];
    }

    for (int i = 0; i < nrow; ++i)
    {
        row_ptr[i + 1] += row_ptr[i];
    }

    for (int k = nnz - 1; k >= 0; --k)
    {
        col_ind[--row_ptr[Ai[k]]] = Aj[k];
        values[row_ptr[Ai[k]]]    = Av[k];
    }

    int count = 0;
    for (int k = 0; k < nnz; ++k)
    {
        if (Ai[k] == Aj[k])
        {
            diagonal[count++] = Av[k];
        }
    }
}

CSR_Matrix::~CSR_Matrix()
{
    if (row_ptr)
        delete[] row_ptr;
    if (col_ind)
        delete[] col_ind;
    if (values)
        delete[] values;
    if (diagonal)
        delete[] diagonal;
}

CSR_Matrix& CSR_Matrix::operator=(const CSR_Matrix& A)
{
    Free();

    int*    Ap = A.row_ptr;
    int*    Aj = A.col_ind;
    double* Av = A.values;
    double* Ad = A.diagonal;

    nrow = A.nrow;
    ncol = A.ncol;

    row_ptr  = new int[nrow + 1];
    col_ind  = new int[Ap[nrow]];
    values   = new double[Ap[nrow]];
    diagonal = new double[nrow];

    for (int i = 0; i <= nrow; ++i)
        row_ptr[i] = Ap[i];

    for (int k = 0; k < Ap[nrow]; ++k)
    {
        col_ind[k] = Aj[k];
        values[k]  = Av[k];
    }

    for (int i = 0; i < nrow; ++i)
        diagonal[i] = Ad[i];

    return *this;
}

CSR_Matrix& CSR_Matrix::operator=(const COO_Matrix& A)
{
    Free();

    int     nnz = A.nnz;
    int*    Ai  = A.row_ind;
    int*    Aj  = A.col_ind;
    double* Av  = A.values;

    nrow = A.nrow;
    ncol = A.ncol;

    row_ptr  = new int[nrow + 1];
    col_ind  = new int[nnz];
    values   = new double[nnz];
    diagonal = new double[nrow];

    for (int i = 0; i < nrow; ++i)
    {
        row_ptr[i] = 0;
    }

    for (int k = 0; k < nnz; ++k)
    {
        ++row_ptr[Ai[k]];
    }

    for (int i = 0; i < nrow; ++i)
    {
        row_ptr[i + 1] += row_ptr[i];
    }

    for (int k = nnz - 1; k >= 0; --k)
    {
        col_ind[--row_ptr[Ai[k]]] = Aj[k];
        values[row_ptr[Ai[k]]]    = Av[k];
    }

    int count = 0;
    for (int k = 0; k < nnz; ++k)
    {
        if (Ai[k] == Aj[k])
        {
            diagonal[count++] = Av[k];
        }
    }

    return *this;
}

void CSR_Matrix::Free()
{
    if (row_ptr)
        delete[] row_ptr;
    if (col_ind)
        delete[] col_ind;
    if (values)
        delete[] values;
    if (diagonal)
        delete[] diagonal;

    nrow     = 0;
    ncol     = 0;
    row_ptr  = 0;
    col_ind  = 0;
    values   = 0;
    diagonal = 0;
}

// ====================================================
// ############### CSC_Matrix Functions ###############
// ====================================================
CSC_Matrix::CSC_Matrix() : nrow(0), ncol(0), col_ptr(0), row_ind(0), values(0) {}

CSC_Matrix::CSC_Matrix(int _n, int _m, int* _col_ptr, int* _row_ind, double* _values)
    : nrow(_n), ncol(_m), col_ptr(_col_ptr), row_ind(_row_ind), values(_values)
{
}

CSC_Matrix::CSC_Matrix(const CSC_Matrix& A)
    : nrow(A.nrow), ncol(A.ncol), col_ptr(new int[A.ncol + 1]), row_ind(new int[A.col_ptr[A.ncol]]), values(new double[A.col_ptr[A.ncol]])
{
    int*    Ap = A.col_ptr;
    int*    Ai = A.row_ind;
    double* Av = A.values;

    for (int j = 0; j <= ncol; ++j)
        col_ptr[j] = Ap[j];

    for (int i = 0; i < Ap[ncol]; ++i)
    {
        row_ind[i] = Ai[i];
        values[i]  = Av[i];
    }
}

CSC_Matrix::CSC_Matrix(const COO_Matrix& A) : col_ptr(new int[A.ncol + 1]), row_ind(new int[A.nnz]), values(new double[A.nnz])
{
    int     nnz = A.nnz;
    int*    Ai  = A.row_ind;
    int*    Aj  = A.col_ind;
    double* Av  = A.values;

    nrow = A.nrow;
    ncol = A.ncol;

    for (int j = 0; j <= ncol; ++j)
    {
        col_ptr[j] = 0;
    }

    for (int k = 0; k < nnz; ++k)
    {
        ++col_ptr[Aj[k]];
    }

    for (int j = 0; j < ncol; ++j)
    {
        col_ptr[j + 1] += col_ptr[j];
    }

    for (int k = 0; k < nnz; ++k)
    {
        row_ind[k] = Ai[k];
        values[k]  = Av[k];
    }
}

CSC_Matrix::~CSC_Matrix()
{
    if (col_ptr)
        delete[] col_ptr;
    if (row_ind)
        delete[] row_ind;
    if (values)
        delete[] values;
}

CSC_Matrix& CSC_Matrix::operator=(const CSC_Matrix& A)
{
    Free();

    int*    Ap = A.col_ptr;
    int*    Ai = A.row_ind;
    double* Av = A.values;

    nrow = A.nrow;
    ncol = A.ncol;

    col_ptr = new int[ncol + 1];
    row_ind = new int[Ap[ncol]];
    values  = new double[Ap[ncol]];

    for (int j = 0; j <= ncol; ++j)
        col_ptr[j] = Ap[j];

    for (int k = 0; k < Ap[ncol]; ++k)
    {
        row_ind[k] = Ai[k];
        values[k]  = Av[k];
    }

    return *this;
}

CSC_Matrix& CSC_Matrix::operator=(const COO_Matrix& A)
{
    Free();

    int     nnz = A.nnz;
    int*    Ai  = A.row_ind;
    int*    Aj  = A.col_ind;
    double* Av  = A.values;

    nrow = A.nrow;
    ncol = A.ncol;

    col_ptr = new int[ncol + 1];
    row_ind = new int[nnz];
    values  = new double[nnz];

    for (int j = 0; j < ncol; ++j)
    {
        col_ptr[j] = 0;
    }

    for (int k = 0; k < nnz; ++k)
    {
        ++col_ptr[Aj[k]];
    }

    for (int j = 0; j < nrow; ++j)
    {
        col_ptr[j + 1] += col_ptr[j];
    }

    for (int k = 0; k < nnz; ++k)
    {
        row_ind[k] = Ai[k];
        values[k]  = Av[k];
    }

    return *this;
}

void CSC_Matrix::Free()
{
    if (col_ptr)
        delete[] col_ptr;
    if (row_ind)
        delete[] row_ind;
    if (values)
        delete[] values;

    nrow    = 0;
    ncol    = 0;
    col_ptr = 0;
    row_ind = 0;
    values  = 0;
}

// ====================================================
// ############### ELL_Matrix Functions ###############
// ====================================================
ELL_Matrix::ELL_Matrix() : nrow(0), ncol(0), nnz(0), nonzeros_in_row(0), col_ind(0), values(0), diagonal(0) {}

ELL_Matrix::ELL_Matrix(int _n, int _m, int _nnz, int _nonzeros_in_row, int* _col_ind, double* _values, double* _diagonal)
    : nrow(_n), ncol(_m), nnz(_nnz), nonzeros_in_row(_nonzeros_in_row), col_ind(_col_ind), values(_values), diagonal(_diagonal)
{
}

ELL_Matrix::ELL_Matrix(const ELL_Matrix& A)
    : nrow(A.nrow), ncol(A.ncol), nnz(A.nnz), nonzeros_in_row(A.nonzeros_in_row), col_ind(new int[A.nrow * A.nonzeros_in_row]),
      values(new double[A.nrow * A.nonzeros_in_row]), diagonal(new double[A.nrow])
{
    int*    Aj = A.col_ind;
    double* Av = A.values;
    double* Ad = A.diagonal;

    int total_ell = A.nrow * A.nonzeros_in_row;

    for (int k = 0; k < total_ell; ++k)
    {
        col_ind[k] = Aj[k];
        values[k]  = Av[k];
    }

    for (int i = 0; i < nrow; i++)
        diagonal[i] = Ad[i];
}

ELL_Matrix::ELL_Matrix(const COO_Matrix& A)
{
    nrow = A.nrow;
    ncol = A.ncol;
    nnz  = A.nnz;

    int max_nnz_in_row = 0;
    int nnz_per_row[nrow];
    memset(nnz_per_row, 0, sizeof(int) * nrow);

    for (int k = 0; k < nnz; ++k)
    {
        nnz_per_row[A.row_ind[k]]++;
    }

    for (int i = 0; i < nrow; ++i)
    {
        max_nnz_in_row = (nnz_per_row[i] > max_nnz_in_row) ? nnz_per_row[i] : max_nnz_in_row;
    }

    nonzeros_in_row = max_nnz_in_row;

    int total_ell = nrow * max_nnz_in_row;
    col_ind       = new int[total_ell]();
    values        = new double[total_ell]();

    int*    Ai = A.row_ind;
    int*    Aj = A.col_ind;
    double* Av = A.values;

    for (int k = nnz - 1; k >= 0; --k)
    {
        int    krow = Ai[k];
        int    kcol = Aj[k];
        double kval = Av[k];
        // 列主序存储
        int r                    = --nnz_per_row[krow];
        col_ind[krow + r * nrow] = kcol;
        values[krow + r * nrow]  = kval;
    }

    diagonal  = new double[nrow];
    int count = 0;
    for (int k = 0; k < nnz; ++k)
    {
        if (Ai[k] == Aj[k])
        {
            diagonal[count++] = Av[k];
        }
    }
}

ELL_Matrix::~ELL_Matrix()
{
    if (col_ind)
        delete[] col_ind;
    if (values)
        delete[] values;
    if (diagonal)
        delete[] diagonal;
}

ELL_Matrix& ELL_Matrix::operator=(const ELL_Matrix& A)
{
    Free();
    int*    Aj = A.col_ind;
    double* Av = A.values;
    double* Ad = A.diagonal;

    nrow            = A.nrow;
    ncol            = A.ncol;
    nnz             = A.nnz;
    nonzeros_in_row = A.nonzeros_in_row;

    int total_ell = A.nrow * A.nonzeros_in_row;
    col_ind       = new int[total_ell];
    values        = new double[total_ell];

    for (int k = 0; k < total_ell; ++k)
    {
        col_ind[k] = Aj[k];
        values[k]  = Av[k];
    }

    diagonal = new double[nrow];
    for (int i = 0; i < nrow; i++)
        diagonal[i] = Ad[i];

    return *this;
}

ELL_Matrix& ELL_Matrix::operator=(const COO_Matrix& A)
{
    Free();

    nrow = A.nrow;
    ncol = A.ncol;
    nnz  = A.nnz;

    int max_nnz_in_row = 0;
    int nnz_per_row[nrow];
    memset(&nnz_per_row, 0, sizeof(int) * A.nrow);

    for (int k = 0; k < nnz; ++k)
    {
        nnz_per_row[A.row_ind[k]]++;
    }

    for (int i = 0; i < nrow; ++i)
    {
        max_nnz_in_row = (nnz_per_row[i] > max_nnz_in_row) ? nnz_per_row[i] : max_nnz_in_row;
    }

    nonzeros_in_row = max_nnz_in_row;

    int total_ell = nrow * max_nnz_in_row;
    col_ind       = new int[total_ell]();
    values        = new double[total_ell]();

    int*    Ai = A.row_ind;
    int*    Aj = A.col_ind;
    double* Av = A.values;

    for (int k = nnz - 1; k >= 0; --k)
    {
        int    krow = Ai[k];
        int    kcol = Aj[k];
        double kval = Av[k];

        int r                    = --nnz_per_row[krow];
        col_ind[krow + r * nrow] = kcol;
        values[krow + r * nrow]  = kval;
    }

    diagonal  = new double[nrow];
    int count = 0;
    for (int k = 0; k < nnz; ++k)
    {
        if (Ai[k] == Aj[k])
        {
            diagonal[count++] = Av[k];
        }
    }

    return *this;
}

void ELL_Matrix::Free()
{
    if (col_ind)
        delete[] col_ind;
    if (values)
        delete[] values;
    if (diagonal)
        delete[] diagonal;

    nrow            = 0;
    ncol            = 0;
    nnz             = 0;
    nonzeros_in_row = 0;
    col_ind         = 0;
    values          = 0;
    diagonal        = 0;
}