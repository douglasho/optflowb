# COOrdSparseMat #

COOrdSparseMat is an implemetation of the [COO Sparse Matrix](http://en.wikipedia.org/wiki/Sparse_matrix#Coordinate_list_.28COO.29).
The advantages of the COO (Coordinate list) is fast insertion time, but the access time is O(non zero) which can be huge, thus we use the COOrdSparseMat to build a CRSSparseMat using the `CRSSparseMat(COOrdSparseMat&)` Constructor.


# Implementation Details #
## Constructor ##
```
COOrdSparseMat(int M, int N, int nz, float* val, int* row, int* col)
```
  * M - number of rows.
  * N - number of columns.
  * nz - number of estimated non zeros (better be higher then lower than the actual value).
  * val - an array of floats, will hold the values of the cells
  * row - an array of ints, the i-th cell will hold the row index of the i-th value.
  * col - an array of ints, the i-th cell will hold the column index of the i-th value.

## Dimension functions ##
```
int rows();
```
Number of rows.
```
int cols();
```
Number of columns.
```
int nonZeros();
```
Number of non zeros in the sparse matrix _(Please notice: the COOrdSparseMat & CRSSparseMat classes dont guarantee that the internal nonZeros() value is correct)_.


## Access Values Functions ##
```
float operator()(int i, int j);
```
Gets the value in the i-th row and the j-th column in the matrix. (works at O(non zeros), not recommended)

## Set Values Functions ##
```
void set(int i, int j, float val);
```
Sets the value of an already existing non-zero value in the sparse matrix, **it will not add a new non-zero value to the matrix**.