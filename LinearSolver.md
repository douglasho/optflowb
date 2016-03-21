# LinearSolver #
Linear Solver for equations of type Ax = B using SOR method for CRS sparse matrices.

# Implementation Details #


The LinearSolver static class implements 2 version of the SOR method
```
LinearSolver::sparseMatSorNoResidual(CRSSparseMat& A, FArray& X0, FArray& X, FArray& B, const float w, const int iters)
```
This implementation provides the simplest method to solve using the SOR method, provide a the left side CRS sparse matrix, an initial solution X0, a destenation X, the right side B, the over relaxation factor w and the total number of iterations.
As implied by the function name, this implementation of the SOR method does not use any residual calculation to stop its operation when the answer Ax is withing a tolerance value from B.
```
LinearSolver::sparseMatSor(CRSSparseMat& A, FArray& X0, FArray& X, FArray& B, const float w, const int iters, const float tolerance)
```
In addition to what specified for the previous implementation of the SOR method, the second implementation provides a method to stop the calculation when Ax is within a tolerance value from B, this tolerance value is specified by the last float parameter of the function.
_**Please notice, the tolerance is checked every 5 iterations of the SOR, this can be bad for performance if the number of iteration is set to a high value.** (if you're using a high value, it might be better to use the first, residual-less implementation)_