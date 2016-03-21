# Usage Example #

The source code seen in this page is included in the svn as `image_tester.cpp`
```
	#define OPTFLOW_TYPE			CV_32F
```
```
	cv::Mat tU;
	cv::Mat tV;
	UtilsFlow::ReadFlowFile("flow10.flo", tU, tV);
	UtilsFlow::DrawFlow(tU, tV, "Ground Truth");
	flowUV GT(tU, tV);
	
	cv::Mat image1 = cv::imread("frame10.png");
	cv::Mat image2 = cv::imread("frame11.png");
	
	cv::Mat image1Gray(image1.rows, image1.cols, CV_MAKETYPE(image1.depth(), 1));
	cv::Mat image2Gray(image2.rows, image2.cols, CV_MAKETYPE(image2.depth(), 1));
	cv::cvtColor(image1, image1Gray, CV_BGR2GRAY);
	cv::cvtColor(image2, image2Gray, CV_BGR2GRAY);
	
	image1.release();
	image2.release();

	cv::Mat image1GrayFloat(image1.rows, image1Gray.cols, OPTFLOW_TYPE);
	cv::Mat image2GrayFloat(image2.rows, image2Gray.cols, OPTFLOW_TYPE);
	image1Gray.convertTo(image1GrayFloat, OPTFLOW_TYPE);
	image2Gray.convertTo(image2GrayFloat, OPTFLOW_TYPE);

	image1Gray.release();
	image2Gray.release();

	OpticalFlowParams params(3, 5, 0.5f, 0.5f, false, 5, 100, 1.9f, false, 0.01f, 3, 1, 0.001f, 
									PenaltyFunctionCompute::Quadratic, 
									PenaltyFunctionCompute::Second, 
									PenaltyFunctionCompute::Quadratic,
									PenaltyFunctionCompute::Second,
									PenaltyFunctionCompute::Quadratic, 
									PenaltyFunctionCompute::Second,
									true, 1.0f/8.0f, 100, 0.95f, false, true);

	cout << "Image tester: calculating flow..." << endl;
		
	flowUV* UV = OpticalFlow::calculate(image1GrayFloat, image2GrayFloat, params);
	cout << "Image tester: ENDED calculating flow in " << endl;

	image1GrayFloat.release();
	image2GrayFloat.release();

	cout << "Image tester: drawing flow..." << endl;
	UtilsFlow::DrawFlow(UV->getU(), UV->getV(), "Flow");
	cout << "Image tester: calculating error..." << endl;
	float* err = FlowError::calcError(*UV, GT, verbose);
	cout << "Image tester: AAE " << err[0] << " STD " << err[1] << " average end point error " << err[2] << endl;

	delete UV;
	cout << "End." << endl;
	cv::waitKey(0);
```

# Explanation #
```
        cv::Mat tU;
        cv::Mat tV;
        UtilsFlow::ReadFlowFile("flow10.flo", tU, tV);
        UtilsFlow::DrawFlow(tU, tV, "Ground Truth");
        flowUV GT(tU, tV);
```
Loading and drawing the Ground Truth file for error calculations (see [Middlebury](http://vision.middlebury.edu/flow/data/)) and load it into a flowUV object.

```
	cv::Mat image1 = cv::imread("frame10.png");
	cv::Mat image2 = cv::imread("frame11.png");
	
	cv::Mat image1Gray(image1.rows, image1.cols, CV_MAKETYPE(image1.depth(), 1));
	cv::Mat image2Gray(image2.rows, image2.cols, CV_MAKETYPE(image2.depth(), 1));
	cv::cvtColor(image1, image1Gray, CV_BGR2GRAY);
	cv::cvtColor(image2, image2Gray, CV_BGR2GRAY);
	
	image1.release();
	image2.release();

	cv::Mat image1GrayFloat(image1.rows, image1Gray.cols, OPTFLOW_TYPE);
	cv::Mat image2GrayFloat(image2.rows, image2Gray.cols, OPTFLOW_TYPE);
	image1Gray.convertTo(image1GrayFloat, OPTFLOW_TYPE);
	image2Gray.convertTo(image2GrayFloat, OPTFLOW_TYPE);

	image1Gray.release();
	image2Gray.release();
```
Loads the 2 images, converts them to grayscale and then convert the cv::Mat to CV\_32F (a matrix of floats).

```
	OpticalFlowParams params(3, 5, 0.5f, 0.5f, false, 5, 100, 1.9f, false, 0.01f, 3, 1, 0.001f, 
									PenaltyFunctionCompute::Quadratic, 
									PenaltyFunctionCompute::Second, 
									PenaltyFunctionCompute::Quadratic,
									PenaltyFunctionCompute::Second,
									PenaltyFunctionCompute::Quadratic, 
									PenaltyFunctionCompute::Second,
									true, 1.0f/8.0f, 100, 0.95f, false, true);
```
Creates the parameters for the optical flow calculation - see Wiki entry for more info about the OpticalFlowParams class.

```
	flowUV* UV = OpticalFlow::calculate(image1GrayFloat, image2GrayFloat, params);
```
Calculate the optical flow.

```
	cout << "Image tester: drawing flow..." << endl;
	UtilsFlow::DrawFlow(UV->getU(), UV->getV(), "Flow");
	cout << "Image tester: calculating error..." << endl;
	float* err = FlowError::calcError(*UV, GT, verbose);
	cout << "Image tester: AAE " << err[0] << " STD " << err[1] << " average end point error " << err[2] << endl;
```
Drawing output flow and calculating error values,
  * AAE - Average Angular Error
  * STD - Standard Deviation