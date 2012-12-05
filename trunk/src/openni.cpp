/*
 * CVKinectWrapper.cpp
 *
 *  Created on: 02/11/2011
 *      Author: biel
 */

#include "openni.h"

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";
XnBool g_bDrawBackground = TRUE;
XnBool g_bDrawPixels = TRUE;
XnBool g_bDrawSkeleton = TRUE;
XnBool g_bPrintID = TRUE;
XnBool g_bPrintState = TRUE;
UserGenerator g_UserGenerator;

// skeleton
// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d New User %d\n", epochTime, nId);
	// New user found
	if (g_bNeedPose)
	{
		generator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
	}
	else
	{
		generator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Lost user %d\n", epochTime, nId);	
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	printf("%d Calibration started for user %d\n", epochTime, nId);
}
// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		// Calibration succeeded
		printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);		
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("%d Calibration failed for user %d\n", epochTime, nId);
        /*if(eStatus==XN_CALIBRATION_STATUS_MANUAL_ABORT)
        {
            printf("Manual abort occured, stop attempting to calibrate!");
            return;
        }*/
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

#define XN_CALIBRATION_FILE_NAME "UserCalibration.bin"


#include <map>
std::map<XnUInt32, std::pair<XnCalibrationStatus, XnPoseDetectionStatus> > m_Errors;

void XN_CALLBACK_TYPE MyCalibrationInProgress(xn::SkeletonCapability& capability, XnUserID id, XnCalibrationStatus calibrationError, void* pCookie)
{
	m_Errors[id].first = calibrationError;
}
void XN_CALLBACK_TYPE MyPoseInProgress(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie)
{
	m_Errors[id].second = poseError;
}



CVKinectWrapper* CVKinectWrapper::_instance;

CVKinectWrapper::CVKinectWrapper() {

	started = false;
}

CVKinectWrapper* CVKinectWrapper::getInstance(){

	if(!_instance){

		_instance = new CVKinectWrapper();
	}

	return _instance;
}

CVKinectWrapper::~CVKinectWrapper() {

	started = false;
	_instance = 0;

}


bool CVKinectWrapper::init(string CalibFilePath)
{

	this->CalibFilePath = CalibFilePath;

	XnStatus rc;

	EnumerationErrors errors;
	rc = g_context.InitFromXmlFile(CalibFilePath.c_str(), g_scriptNode, &errors);

	if (rc == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		return started;
	}
	else if (rc != XN_STATUS_OK)
	{
		printf("Open failed: %s\n", xnGetStatusString(rc));
		return started;
	}

	rc = g_context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depth);
	if (rc != XN_STATUS_OK)
	{
		printf("No depth node exists! Check your XML.");
		return started;
	}

	rc = g_context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);
	if (rc != XN_STATUS_OK)
	{
		printf("No image node exists! Check your XML.");
		return started;
	}

	g_depth.GetMetaData(g_depthMD);
	g_image.GetMetaData(g_imageMD);

	// Hybrid mode isn't supported in this sample
	if (g_imageMD.FullXRes() != g_depthMD.FullXRes() || g_imageMD.FullYRes() != g_depthMD.FullYRes())
	{
		printf ("The device depth and image resolution must be equal!\n");
		return started;
	}

	// RGB is the only image format supported.
	if (g_imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
	{
		printf("The device image format must be RGB24\n");
		return started;
	}

    // skeleton
	rc = g_context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (rc != XN_STATUS_OK)
	{
		rc = g_UserGenerator.Create(g_context);
		CHECK_RC(rc, "Find user generator");
	}

	XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		printf("Supplied user generator doesn't support skeleton\n");
		return 1;
	}
	rc = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	CHECK_RC(rc, "Register to user callbacks");
	rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
	CHECK_RC(rc, "Register to calibration start");
	rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
	CHECK_RC(rc, "Register to calibration complete");

	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return 1;
		}
		rc = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
		CHECK_RC(rc, "Register to Pose Detected");
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationInProgress(MyCalibrationInProgress, NULL, hCalibrationInProgress);
	CHECK_RC(rc, "Register to calibration in progress");

	rc = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseInProgress(MyPoseInProgress, NULL, hPoseInProgress);
	CHECK_RC(rc, "Register to pose in progress");

	rc = g_context.StartGeneratingAll();
	CHECK_RC(rc, "StartGenerating");


	g_depth.GetAlternativeViewPointCap().SetViewPoint(g_image);

	_rgbImage = new Mat(480, 640, CV_8UC3, Scalar::all(0));
	_comboImage = new Mat(480, 640, CV_8UC3, Scalar::all(0));
	_depthImage = new Mat(480, 640, CV_8UC1, Scalar::all(0));
	_rawDepth = new Mat(480, 640, CV_16UC1, Scalar::all(0));

	rgbImage = cvCreateImage (cvSize (640, 480), IPL_DEPTH_8U, 3);
	depthImage = cvCreateImage (cvSize (640, 480), IPL_DEPTH_8U, 3);
	comboImage =  cvCreateImage (cvSize (640, 480), IPL_DEPTH_8U, 3);
	rawDepth = cvCreateImage (cvSize (640, 480), IPL_DEPTH_8U, 3);

	started = true;
	return started;
}

bool CVKinectWrapper::reload(){

	if (!started) return false;

	XnStatus rc = XN_STATUS_OK;

	const XnDepthPixel* pDepth;

	// Read a new frame
	rc = g_context.WaitAnyUpdateAll();
	if (rc != XN_STATUS_OK)
	{
		printf("Read failed: %s\n", xnGetStatusString(rc));
		return  false;
	}

	g_depth.GetMetaData(g_depthMD);
	g_image.GetMetaData(g_imageMD);

	pDepth = g_depthMD.Data();

	const XnRGB24Pixel* pImageRow = g_imageMD.RGB24Data();
	const XnDepthPixel* pDepthRow = g_depthMD.Data();


	for (XnUInt y = 0; y < g_imageMD.YRes(); ++y){

		const XnRGB24Pixel* pImage = pImageRow;
		const XnDepthPixel* pDepth = pDepthRow;

		for (XnUInt x = 0; x < g_imageMD.XRes(); ++x, ++pImage,++pDepth){

			_rgbImage->at<Vec3b>(y,x) = Vec3b(pImage->nBlue, pImage->nGreen , pImage->nRed);

			if (*pDepth != 0)

				_rawDepth->at<short>(y,x) = *pDepth;
			else

				_rawDepth->at<short>(y,x) = 0 ;
		}

		pDepthRow += g_depthMD.XRes();
		pImageRow += g_imageMD.XRes();
	}

	double min, max;

	Mat aux(480,640,CV_8UC3);

	minMaxLoc(*_rawDepth, &min, &max,NULL,NULL);

	_rawDepth->convertTo(*_depthImage, CV_8UC1, 255.0/max);

	cvtColor(*_depthImage,aux,CV_GRAY2BGR);

	//_rgbImage->copyTo(*_comboImage);
	aux.copyTo(*_comboImage, *_depthImage);


    //skeleton
	char strLabel[50] = "";
	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i){
        XnSkeletonJointPosition Head;
        g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(
           aUsers[i], XN_SKEL_HEAD, Head);
        printf("%d: (%f,%f,%f) [%f]\n", aUsers[i],
               Head.position.X, Head.position.Y, Head.position.Z,
               Head.fConfidence);
    }
    

	return true;
}

void CVKinectWrapper::getRGB(Mat *rgb){

	_rgbImage->copyTo(*rgb);

}

void CVKinectWrapper::getRawDepth(Mat *rawDepth){

	_rawDepth->copyTo(*rawDepth);

}

void CVKinectWrapper::getCombo(Mat *combo){

	_comboImage->copyTo(*combo);
}

void CVKinectWrapper::getDisplayDepth(Mat *displayDepth){

	_depthImage->copyTo(*displayDepth);
}

IplImage * CVKinectWrapper::get_image_rgb(){
	//_rgbImage->copyTo(*rgbImage);
	if (!started) return rgbImage;
    
    memcpy(rgbImage->imageData, (char *) _rgbImage->data, 640*480*3);
    return rgbImage;
}

IplImage * CVKinectWrapper::get_image_depth_rgb(){
	//_comboImage->copyTo(*comboImage);
	
	if (!started) return comboImage;

    memcpy(comboImage->imageData, (char *) _comboImage->data, 640*480*3);
    
    return comboImage;
}



