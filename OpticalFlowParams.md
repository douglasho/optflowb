# OpticalFlowParams #

OpticalFlowParams is used to transfer parameters into `OpticalFlow::calculate` function in the OpticalFlow class.

# Implementation Details #
```
	OpticalFlowParams(	const float alpha = 3.0f, 
				const int pyramidLevels = 5, 
				const float pyramidSpacing = 0.5f,
				const float weightedDeriveFactor = 0.5f,
				const bool displayDerivativs = false,
				const int medianFilterRadius = 5,
				const int sorIters = 100,
				const float overRelaxation = 1.9f,
				const bool checkResidualTolerance = true,
				const float residualTolerance = 0.01f,
				const int iters = 3, 
				const int linearIters = 1,
				const double penaltySigma = 0.01f,
				const PenaltyFunctionCompute::RobustFunctionType pfSpatialX = PenaltyFunctionCompute::Quadratic,
				const PenaltyFunctionCompute::Derivative pfSpatialXDeriv = PenaltyFunctionCompute::Second,
				const PenaltyFunctionCompute::RobustFunctionType pfSpatialY = PenaltyFunctionCompute::Quadratic,
				const PenaltyFunctionCompute::Derivative pfSpatialYDeriv = PenaltyFunctionCompute::Second,
				const PenaltyFunctionCompute::RobustFunctionType pdData = PenaltyFunctionCompute::Quadratic,
				const PenaltyFunctionCompute::Derivative pdDataDeriv = PenaltyFunctionCompute::Second,
				const bool useTextureDecomposition = true,
				const float TDtheta = 1.0f/8.0f,
				const int TDnIters = 100,
				const float TDalp = 0.95f,
				const bool displayTextureDecompositionOutput = false,
				const bool display = true)
```

  * alpha - smoothness weight in the flow operator.
  * pyramidLevels - the number of levels in the gaussian pyramid.
  * pyramidSpacing - the spacing factor between each level in the gaussian pyramis.
  * weightedDeriveFactor - weighting factor between the versions of the derivative (numerical from images and from interpolated image according to the current calculated flow).
  * displayDerivativs - display the derivatives.
  * medianFilterRadius - the radius of the median filter.
  * sorIters - number of iterations for the SOR run.
  * overRelaxation - over relaxation factor for the SOR run.
  * checkResidualTolerance - set whether to check if Ax is withing tolerance value from B in the SOR run.
  * residualTolerance - tolerance value for the SOR run.
  * iters - number of iterations per pyramid level.
  * linearIters - number of linearization iterations.
  * penaltySigma - sigma.
  * pfSpatialX - penalty for the X direction.
  * pfSpatialXDeriv - derivative for the X direction.
  * pfSpatialY - penalty for the Y direction.
  * pfSpatialYDeriv - derivative for the Y direction.
  * pdData - penalty for the data term.
  * pdDataDeriv - derivative for the data term.
  * useTextureDecomposition - use texture decomposition.
  * TDtheta - theta for the texture decomposition.
  * TDnIters - number of iterations for the texture decomposition.
  * TDalp - alpha for the texture decomposition.
  * displayTextureDecompositionOutput - display the return value of the texture decomposition run.
  * display - display the flow for each iteration.