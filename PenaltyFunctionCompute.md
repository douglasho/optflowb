# PenaltyFunctionCompute #
PenaltyFunctionCompute is a bundle of 6 different functions that restrict the current flow values to achieve improved results.

# Implementation Details #
```
static void compute(cv::Mat & mat, PenaltyFunctionCompute::RobustFunctionType type, double sigma = 1, PenaltyFunctionCompute::Derivative derivative = Value, const double aSigma = 0.45);
```
  * mat - the cv::Mat matrix to alter.
  * type - which penalty (AKA Robust) function should be used.
  * sigma - used for the inner calculation of the penalty function, default value = 1.
  * derivative - which derivative of the chosen penalty function should be used when calculating, default value = Value.
  * aSigma - used for the inner calculation of the GeneralizedCharbonnier penalty function, default value = 0.45.

# enum RobustFunctionType #
```
	enum RobustFunctionType
	{
		Charbonnier,
		GeneralizedCharbonnier,
		Gaussian,
		Geman_mcclure,
		Lorentzian,
		Quadratic
	};
```
  * Charbonnier
  * Generalized Charbonnier
  * Gaussian
  * Geman Mcclure
  * Lorentzian
  * Quadratic

# enum Derivative #
```
	enum Derivative
	{
		Value,
		First,
		Second
	};
```
  * Value - Function value, no derivative.
  * First - First derivative.
  * Second - Second derivative.