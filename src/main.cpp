
#include "main.h"

extern int freenect_angle;

void keyPressed(unsigned char key)
{
	if (key == 27) {
		//die = 1;
		//pthread_join(freenect_thread, NULL);
		//free(depth_mid);
		//free(depth_front);
		//free(rgb_back);
		//free(rgb_mid);
		//free(rgb_front);
		// Not pthread_exit because OSX leaves a thread lying around and doesn't exit
		exit(0);
	}
	if (key == 119 /* w*/) {
		freenect_angle++;
		if (freenect_angle > 30) {
			freenect_angle = 30;
		}
	}
	if (key == 's') {
		freenect_angle = 0;
	}/*
	if (key == 'f') {
		if (requested_format == FREENECT_VIDEO_IR_8BIT)
			requested_format = FREENECT_VIDEO_RGB;
		else if (requested_format == FREENECT_VIDEO_RGB)
			requested_format = FREENECT_VIDEO_YUV_RGB;
		else
			requested_format = FREENECT_VIDEO_IR_8BIT;
	}*/
	if (key == 120 /*'x'*/) {
		freenect_angle--;
		if (freenect_angle < -30) {
			freenect_angle = -30;
		}
	}

    /* Control of kinect
	if (key == '1') {
		freenect_set_led(f_dev,LED_GREEN);
	}
	if (key == '2') {
		freenect_set_led(f_dev,LED_RED);
	}
	if (key == '3') {
		freenect_set_led(f_dev,LED_YELLOW);
	}
	// 5 is the same as 4
	if (key == '4' || key == '5') {
		freenect_set_led(f_dev,LED_BLINK_GREEN);
	}
	if (key == '6') {
		freenect_set_led(f_dev,LED_BLINK_RED_YELLOW);
	}
	if (key == '0') {
		freenect_set_led(f_dev,LED_OFF);
	}
	if (key == 'r') {
        if(!record_video){
        	printf("record video: ON\n");
        	char buffer_h[32];
        	char buffer_v[32];
	        sprintf(buffer_v,"data/video_%d.avi",(int) time(NULL));		
	        sprintf(buffer_h,"data/depth_%d.avi",(int) time(NULL));		
            writer_v = cvCreateVideoWriter(buffer_v, CV_FOURCC('M', 'J', 'P', 'G') , 20, cvSize(640,480),  1);
            writer_h = cvCreateVideoWriter(buffer_h, CV_FOURCC('M', 'J', 'P', 'G') , 20 ,cvSize(640,480), 1); 
            record_video = true;
        } else {
           printf("record video: OFF\n");
           cvReleaseVideoWriter( &writer_v );
           cvReleaseVideoWriter( &writer_h ); 
           record_video = false;
        }
    }
	freenect_set_tilt_degs(f_dev,freenect_angle);
   */
}

void on_mouse(int event, int x, int y, int flags, void* param){
   if (event ==CV_EVENT_LBUTTONDOWN )
      printf("%d %d\n", x, y);
}

int main(int argc, char **argv)
{

    // init kinect
    freenect * fr = new freenect();
    fr->init(0);

    // set filter for kinect
    fr->set_filter(612, 950);

    cvNamedWindow("win rgb", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("win depth", CV_WINDOW_AUTOSIZE);

    IplImage * depth; //= cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
    IplImage * rgb; // = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);

    int key;

    fr->reload();
    depth = fr->get_image_depth_rgb();

    cvSetMouseCallback("win2", on_mouse, 0);

    for(;;){
        key=cvWaitKey(20);

        // get data from kinect
        fr->reload();
        // get depth image
        depth = fr->get_image_depth_rgb();
        // get rgb image from camera
        rgb = fr->get_image_rgb();

        cvShowImage("win rgb", rgb);
        cvShowImage("win depth", depth);

        keyPressed(key);
        if(key == 1048689 || key == 27 ) {
            break;
        }
    }

    cvDestroyWindow("win rgb");
    cvDestroyWindow("win depth");
	return 0;
}
