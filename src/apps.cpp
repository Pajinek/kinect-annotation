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

    scale = GTK_SCALE (gtk_builder_get_object (builder, "scale1"));
    adjustment = GTK_ADJUSTMENT (gtk_builder_get_object (builder, "adjustment1"));
    button3 = GTK_BUTTON (gtk_builder_get_object (builder, "button3"));

    gtk_builder_connect_signals (builder, this);

    // signals
    g_signal_connect (drawarea, "expose-event",
                      G_CALLBACK (on_draw_video), this );
    gtk_widget_set_app_paintable (drawarea, true);
    gtk_widget_set_double_buffered (drawarea, true);

    // load file
    if (file != NULL) {
        load_video(file);
    }

    printf("set_timer %u\n", mode);

    // init kinect
    kinect = new freenect ();

    kinect->init (0);
    if ( kinect->get_error() ){
        // FIXME: swo warning message
        /* messagedialog1 = GTK_WIDGET (gtk_builder_get_object (builder, "messagedialog1"));
        gtk_widget_activate  (messagedialog1);
        gtk_widget_show  (messagedialog1); */
    }

    g_object_unref (G_OBJECT (builder));
        
    gtk_widget_show (window);                
    gtk_main ();
}

void App::load_video(char * file){
    printf("INFO: load file %s\n", file);

    capture = cvCaptureFromAVI( file );
    if (capture != NULL) {
        int fps = ( int )cvGetCaptureProperty( capture, CV_CAP_PROP_FPS );
        set_mode ( (gint) MODE_PAUSE );
        printf( "INFO: count of fps %d\n", fps );
    }
}

void App::set_mode(gint _mode){
    printf("set_timer %u\n", _mode);
    this->mode = _mode;
}


void App::play(){

    if ( mode == MODE_PLAY ) {
        set_mode ( MODE_PAUSE );
        gtk_button_set_label (button3, "play" );
        return;
    }
    
    gtk_button_set_label (button3, "pause" );
    set_mode( MODE_PLAY );
    // set timer for recording
    g_timeout_add (1000.0 / TIMER, on_timer, (void *) this );

}

gint App::get_mode(){
    return mode;
}

gboolean App::next_frame(){

    frame_rgb = cvQueryFrame ( capture ); 
    if ( frame_rgb != NULL ) {
        // count frame
        n_frame++;
        gtk_adjustment_set_value (adjustment, n_frame);
        gtk_adjustment_set_upper (adjustment, 1000. );
        return true;
    } else {
        return false;
    }
}

IplImage * App::get_image_rgb(){
    return frame_rgb;
}
