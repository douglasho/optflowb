# Pseudo Code #
This is the high-level operation of the algorithm.
```
calculate(Im1, Im2)
  Texture_Decomposition(Im1, Im2)
  flow = Create_flow()
  PyrIm1 = Gaussian_Pyramid(Im1)
  PyrIm2 = Gaussian_Pyramid(Im2)
  foreach level in PyrIm1 do
    Reshape_Flow(flow)
    Ix = X_Derivative(levelOfIm1)
    Iy = Y_Derivative(levelOfIm1)
    warp = Interpolate_Image(levelOfIm2, flow)
    It = warp - levelOfIm1
    [Operator, B] = Build_Flow_Operator(flow, Du, Dv, Ix, Iy, It)
    X = Solve(Operator, B)
    flow += Parse_Delta_Flow(X)
    flow = Median_Filter(flow)
```