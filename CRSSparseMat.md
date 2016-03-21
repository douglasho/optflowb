# CRSSparseMat #

CRSSparseMat is an implemetation of the [CRS Sparse Matrix](http://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_.28CSR_or_CRS.29).
The advantages of the CRS (Compressed sparse row) are fast cell access (with the correct implementation can be reduced to O(1)) but on the other hand, building such sparse matrix is rather slow, the solution is explained in the `build` function and the `CRSSparseMat(COOrdSparseMat&)` Constructor.


# Implementation Details #
## Constructor ##
```
CRSSparseMat()
```
Default constructor.

## Initialization functions ##
```
void build(const COOrdSparseMat&, int nz);
void build(const COOrdSparseMat&);
```
Both functions provide a method for building a CRS sparse matrix out of a COO sparse matrix (The COO form is the simplest form of sparse matrix, and is as fast as it gets regarding to insertion of new values).
  * nz - the number of non zero values in the COOrdSparseMat.
  * the function `void build(const COOrdSparseMat&);` calls `void build(const COOrdSparseMat&, int nz);` with the COOrdSparseMat's 'nonZeros()' value _(Please notice: the COOrdSparseMat & CRSSparseMat classes dont guarantee that the internal nonZeros() value is correct)_.

## Dimension functions ##
```
int dimR();
```
Number of rows.
```
int dimC();
```
Number of columns.
```
int nonZeros();
```
Number of non zeros in the sparse matrix _(Please notice: the COOrdSparseMat & CRSSparseMat classes dont guarantee that the internal nonZeros() value is correct)_.

## Arithmetic Operations ##
```
void MulVector(const float* x, float* b);
```
Multiply the Matrix by an array x and put the answer in b.
```
void MulScalar(const float x);
```
Multiply the Matrix by a float scalar x.

## Access Values Functions ##
```
float operator()(int i, int j);
```
Gets the value in the i-th row and the j-th column in the matrix.
This is the simplest method to get a value out of the matrix, but it is not very efficient for sequential read of the matrix. _(For sequential read of the matrix use the next 3 functions)_
```
float& val(int i);
```
Gets the i-th value out of the values array.
```
int& rowPtr(int i);
```
Gets the index to the values and columns arrays in which the i-th row starts at.
```
int& colIdx(int i);
```
Gets the i-th value out of the column array.