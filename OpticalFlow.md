# OpticalFlow #
The main class in the project, calculates the optical flow via the static function `calculate`.
To learn more about the internal operation of the algorithm, see TheoryPseudoCode page in the Wiki.

# Implementation Details #

```
static flowUV* OpticalFlow::calculate (const cv::Mat& Im1, const cv::Mat& Im2, const OpticalFlowParams& params, flowUV* oldFlow = NULL);
```
Caluclates the optical flow between 2 images with respect of the OpticalFlowParams instance.
  * Im1 - The first image (of type CV\_32F and in grayscale color space).
  * Im2 - The second image (of type CV\_32F and in grayscale color space).
  * params - an instance of OpticalFlowParams (see Wiki entry).
  * oldFlow - an initial guess (or old flow) for the flow, meant for usage when calculating flow of an image sequence or video under the assumption that the flow wont change drastically between consecutive frames in a high frame rate video.

  * return - a flowUV object with the result flow, needs to be deleted manually.