#include "apps.h"


App::App(char * file){

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/window.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));

    // init default value
    mode = 0;
    n_frame = 0;
    capture = NULL;

    drawarea = GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea1"));
    frame_rgb_small = cvCreateImage(cvSize(348, 250), IPL_DEPTH_8U, 3);
    frame_depth_small = cvCreateImage(cvSize(348, 250), IPL_DEPTH_8U, 3);

    scale = GTK_SCALE (gtk_builder_get_object (builder, "scale1"));
    adjustment = GTK_ADJUSTMENT (gtk_builder_get_object (builder, "adjustment1"));
    button3 = GTK_BUTTON (gtk_builder_get_object (builder, "button3"));

    gtk_builder_connect_signals (builder, this);

    // signals
    g_signal_connect (drawarea, "expose-event",
                      G_CALLBACK (on_draw_video), this );
    // ??
    //gtk_widget_set_app_paintable (drawarea, true);
    //gtk_widget_set_double_buffered (drawarea, true);

    config = new Config("config.xml");

    // load file
    if (file != NULL) {
        load_video(file);
    } else {
        /* only temporary */
        capture = cvCaptureFromCAM( -1 );
        set_mode ( (gint) MODE_PAUSE );
        cvGrabFrame ( capture );
        cvSetCaptureProperty( capture, CV_CAP_PROP_CONVERT_RGB, 0.);
        int fps = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS);
        printf( "INFO: count of fps %d\n", fps );
    }

    // init kinect
    kinect = new freenect ();

    kinect->init (0);
    if ( kinect->get_error() ){
        // FIXME: swo warning message
        /* messagedialog1 = GTK_WIDGET (gtk_builder_get_object (builder, "messagedialog1"));
        gtk_widget_activate  (messagedialog1);
        gtk_widget_show  (messagedialog1); */
    }
    set_param_video();

    g_object_unref (G_OBJECT (builder));
        

    gtk_widget_show (window);                
    gtk_main ();
}

App::~App(){
    printf("TODO: free all memory\n");
    // cvReleaseImage(&frame_depth_small);
    // cvReleaseImage(&frame_rgb_small);
}

void App::set_param_video(){
    if (capture != NULL) {
        int fps = ( int ) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT );
        printf( "INFO: count of fps %d\n", fps );
    } else {
        printf("Error: capture is NULL");
    }

    frame_width = 640;
    frame_height = 480;
// 640x480  15 fps 1900 kbps 
// 320x240  30 fps  900 kbps 
// 160x120  30 fps 675 kbps 

    frame_fps = 15;
}

void App::load_video(char * file){
    printf("INFO: load file %s\n", file);

    capture = cvCaptureFromAVI( file );
    if (capture != NULL) {
        int fps = ( int ) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT );
        set_mode ( (gint) MODE_PAUSE );
        cvGrabFrame ( capture );
        cvSetCaptureProperty( capture, CV_CAP_PROP_POS_AVI_RATIO, 0.);
        printf( "INFO: count of fps %d\n", fps );
    }
}

void App::set_mode(gint _mode){
    this->mode = _mode;
}

void App::play(){

    if ( mode == MODE_PLAY ) {
        set_mode ( MODE_PAUSE );
        gtk_button_set_label (button3, "play" );
        g_print ("INFO: pause video.\n");
        return;
    }
    
    gtk_button_set_label (button3, "pause" );
    set_mode( MODE_PLAY );
    // set timer for recording
    g_timeout_add (1000.0 / frame_fps, on_timer, (void *) this );

    g_print ("INFO: play video.\n");

}


void App::record(){
    if ( mode == MODE_REC ) {
        cvReleaseVideoWriter( &writer_rgb );
        cvReleaseVideoWriter( &writer_depth ); 
        return;
    }

    set_mode ( MODE_REC );

    // init new file for record
    int timestamp = (int) time(NULL);
    sprintf(file_rgb,"data/%d.rgb.avi", timestamp);		
    sprintf(file_depth,"data/%d.depth.avi", timestamp);	
    printf("INFO: create files: %s\n" 
           "                    %s\n", file_rgb, file_depth);	
    writer_rgb = cvCreateVideoWriter(file_rgb, CODEC , frame_fps, cvSize(frame_width, frame_height),  1);
    writer_depth = cvCreateVideoWriter(file_depth, CODEC , frame_fps, cvSize(frame_width, frame_width), 1); 

}

gint App::get_mode(){
    return mode;
}

void App::scale_frame(){
   /* gtk_adjustment_set_value (adjustment, n_frame);
    if( gtk_adjustment_get_upper (adjustment) < n_frame )
        gtk_adjustment_set_upper (adjustment, n_frame ); */
}

void App::set_pos_frame(double value){
    printf(">> %d\n",(int) floor(value) );
    //cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0.); //(double) floor(value) );
    printf("!!!!\n");
}

gboolean App::next_frame(){

    frame_rgb = cvQueryFrame ( capture ); 
    if ( frame_rgb != NULL ) {
        // count frames

        cvResize(frame_rgb, frame_rgb_small);

        if ( mode == MODE_REC ) {
            cvWriteFrame( writer_rgb, frame_rgb);
        }

        n_frame ++;
        //scale_frame ();
        return true;
    } else {
        return false;
    }
}

IplImage * App::get_image_rgb(){
    return frame_rgb_small;
}
