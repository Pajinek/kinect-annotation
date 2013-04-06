/****************************************************************************
 *                                                                           *
 *  OpenCVNIWrapper                                                          *
 *  Copyright (C) 2011 PrimeSense Ltd.                                       *
 *                                                                           *                                         *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU Lesser General Public License as published *
 *  by the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  See <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
 *  By Gabriel Sanmartín & Gabriel Moyà                                      *
 *  gabriel.sanmartin@usc.es & gabriel.moya@uib.es                           *
 ****************************************************************************/

#ifndef CVKINECTWRAPPER_H_
#define CVKINECTWRAPPER_H_

#include <XnOS.h>
#include <XnCppWrapper.h>

#include <math.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"

using namespace xn;
using namespace cv;
using namespace std;


#define ENUM_SKELETON_POINT_SIZE 8

#define DISPLAY_MODE_OVERLAY	1
#define DISPLAY_MODE_DEPTH		2
#define DISPLAY_MODE_IMAGE		3
#define DEFAULT_DISPLAY_MODE	DISPLAY_MODE_OVERLAY

#define CHECK_RC(rc, what) \
                if (rc != XN_STATUS_OK)                                     \
                {                                                           \
                    printf("%s failed: %s\n", what, xnGetStatusString(rc)); \
                    return rc;                                              \
                }

class CVKinectWrapper {

public:

	bool init(string CalibFilePath);
	static CVKinectWrapper* getInstance();

    bool reload();

    void getRGB(Mat *rgb);
    void getRawDepth(Mat *rawDepth);
    void getCombo(Mat *combo);
    void getDisplayDepth(Mat *displayDepth);

    u_int get_error(){ return error; };
    IplImage * get_image_rgb();
    IplImage * get_image_depth_rgb();
    float * get_skeleton_float();
    float skeleton[3*10];
	
    ~CVKinectWrapper();

private:
	static CVKinectWrapper *_instance;

	CVKinectWrapper();

	string CalibFilePath;
	bool started;

	Context g_context;
    u_int error;

	ScriptNode g_scriptNode;
	DepthGenerator g_depth;
	ImageGenerator g_image;

	DepthMetaData g_depthMD;
	ImageMetaData g_imageMD;

	Mat *_rgbImage;
	Mat *_depthImage;
	Mat *_comboImage;
	Mat *_rawDepth;

	IplImage *rgbImage;
	IplImage *depthImage;
	IplImage *comboImage;
	IplImage *rawDepth;
};

#endif /* CVKINECTWRAPPER_H_ */
