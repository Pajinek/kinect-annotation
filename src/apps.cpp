#include "apps.h"

gboolean
  foreach_func (GtkTreeModel *model,
                GtkTreePath  *path,
                GtkTreeIter  *iter,
                gpointer      user_data)
  {
    gchar *first_name, *last_name, *tree_path_str, *year_of_birth;

    /* Note: here we use 'iter' and not '&iter', because we did not allocate
     *  the iter on the stack and are already getting the pointer to a tree iter */

    gtk_tree_model_get (model, iter,
                        0, &first_name,
                        1, &last_name,
                        2, &year_of_birth,
                        -1);

    tree_path_str = gtk_tree_path_to_string(path);

    g_print ("Row %s: %s %s %s\n", tree_path_str,
             first_name, last_name, year_of_birth);

    g_free(tree_path_str);

    g_free(first_name); /* gtk_tree_model_get made copies of       */
    g_free(last_name);  /* the strings for us when retrieving them */

    return FALSE; /* do not stop walking the store, call us with next row */
  }


App::App(char * file){

    // load data from glade xml
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/window.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));

    // init default value
    mode = 0;
    n_frame = 0;
    capture = NULL;
    is_move_pos_video = false;
    // temporary value for create columns
	GtkTreeViewColumn * col;

    // drawarea
    drawarea = GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea1"));
    frame_rgb_small = cvCreateImage(cvSize(348, 250), IPL_DEPTH_8U, 3);
    frame_depth_small = cvCreateImage(cvSize(348, 250), IPL_DEPTH_8U, 3);

    // scale for moving in video
    scale = GTK_SCALE (gtk_builder_get_object (builder, "scale1"));
    adjustment = GTK_ADJUSTMENT (gtk_builder_get_object (builder, "adjustment1"));

    // buttons
    button_play = GTK_BUTTON (gtk_builder_get_object (builder, "button3"));
    button_rec = GTK_BUTTON (gtk_builder_get_object (builder, "button1"));

    // treeview - list for annotations
    list = GTK_TREE_VIEW (gtk_builder_get_object (builder, "treeview1"));


    // signals for drawing
    g_signal_connect (drawarea, "expose-event",
                      G_CALLBACK (on_draw_video), this );

/*  Events for drawing
    gtk_signal_connect (GTK_OBJECT(drawarea),"configure_event",
                      (GtkSignalFunc) on_draw_video, NULL);
    gtk_signal_connect (GTK_OBJECT (drawarea), "motion_notify_event",
                      (GtkSignalFunc) on_draw_video, NULL);
    gtk_signal_connect (GTK_OBJECT (drawarea), "button_press_event",
                      (GtkSignalFunc) on_draw_video, NULL);*/

    gtk_widget_set_events (drawarea, GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
                         | GDK_BUTTON_RELEASE_MASK 
                         | GDK_POINTER_MOTION_MASK
                         | GDK_POINTER_MOTION_HINT_MASK);
    // is it need ?
    gtk_widget_set_app_paintable (drawarea, true);
    gtk_widget_set_double_buffered (drawarea, true);

    config = new Config("config.xml");
    anns = new AnnList();

    // load video file
    if (file != NULL) {
        load_video(file);
    } else {
        /* only temporary */
        capture = cvCaptureFromCAM( -1 );
        set_mode ( (gint) MODE_PAUSE );
        cvGrabFrame ( capture );
        cvSetCaptureProperty( capture, CV_CAP_PROP_CONVERT_RGB, 1.);
        int fps = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS);
        printf( "INFO: count of fps %d\n", fps );
    }

    // init kinect
    kinect = new freenect ();

    kinect->init (0);
    if ( kinect->get_error() ){
        // TODO: show warning message
        
        /* messagedialog1 = GTK_WIDGET (gtk_builder_get_object (builder, "messagedialog1"));
        gtk_widget_activate  (messagedialog1);
        gtk_widget_show  (messagedialog1); */
    }
    set_param_video();


    // create table column
    GtkCellRenderer     *renderer;
    renderer = gtk_cell_renderer_text_new ();

    col = gtk_tree_view_column_new_with_attributes (
        " Id ", renderer, "text", 0, NULL);

    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);
    col = gtk_tree_view_column_new_with_attributes (
        "   Begin (FPS)   ", renderer, "text", 1, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

    col = gtk_tree_view_column_new_with_attributes (
        "   End (FPS)   ", renderer, "text", 2, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

    col = gtk_tree_view_column_new_with_attributes (
        "   Type", renderer, "text", 3, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

    store = gtk_list_store_new(4, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

    //gtk_tree_model_foreach(GTK_TREE_MODEL(store), foreach_func, NULL);

    // GtkTreeSelection *selection; 
    // selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
    // g_signal_connect(selection, "changed", G_CALLBACK(on_click_row), label);
    //
    gtk_builder_connect_signals (builder, (void *) this);
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

        // set max value for video play
        gtk_adjustment_set_upper (adjustment, fps ); 

        int no_of_frames = (int) cvGetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES);
        printf( "INFO: CV_CAP_PROP_POS_FRAMES %d\n", no_of_frames );

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
        set_mode ( (gint) MODE_PAUSE );
        cvGrabFrame ( capture );
        cvSetCaptureProperty( capture, CV_CAP_PROP_POS_AVI_RATIO, 0.);

        is_move_pos_video = true;
    }
}

void App::set_mode(gint _mode){
    this->mode = _mode;
}

void App::play(){

    if ( mode == MODE_PLAY ) {
        set_mode ( MODE_PAUSE );
        gtk_button_set_label (button_play, "play" );
        g_print ("INFO: pause video.\n");
        return;
    }
    
    gtk_button_set_label (button_play, "pause" );
    set_mode( MODE_PLAY );
    // set timer for recording
    g_timeout_add (1000.0 / frame_fps, on_timer, (void *) this );

    g_print ("INFO: play video.\n");

}

void App::record(){
    if ( mode == MODE_REC ) {
        cvReleaseVideoWriter( &writer_rgb );
        cvReleaseVideoWriter( &writer_depth ); 
        set_mode ( MODE_PAUSE );
        return;
    }

    set_mode ( MODE_REC );

    // init new file for record
    int timestamp = (int) time(NULL);
    sprintf(file_rgb,"data/%d.rgb.avi", timestamp);		
    sprintf(file_depth,"data/%d.depth.avi", timestamp);	
    printf("INFO: create files: %s\n" 
           "                    %s\n", file_rgb, file_depth);	
    writer_rgb = cvCreateVideoWriter(file_rgb, CODEC , frame_fps, cvSize(frame_width, frame_height),  1 /* is color */ );
    writer_depth = cvCreateVideoWriter(file_depth, CODEC , frame_fps, cvSize(frame_width, frame_width), 1 /* is color */ ); 

    gtk_button_set_label (button_rec, "stop rec" );
}

gint App::get_mode(){
    return mode;
}

void App::scale_frame(){
    gtk_adjustment_set_value (adjustment, n_frame);
    if( mode == MODE_REC && gtk_adjustment_get_upper (adjustment) < n_frame ){
        gtk_adjustment_set_upper (adjustment, n_frame ); 
    }
}

void App::set_pos_frame(double value){
    int val = (int) floor(value);

    if (!is_move_pos_video) return;

    // avi move only about one frame
    if( n_frame  <= val + 1 || n_frame >= val - 1 ) {
        cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, val);

        frame_rgb = cvQueryFrame ( capture ); 
        cvResize(frame_rgb, frame_rgb_small);
        n_frame = val;

        // re-draw new frame
        gtk_widget_queue_draw ( GTK_WIDGET (drawarea) );
    }
}

gboolean App::next_frame(){

    frame_rgb = cvQueryFrame ( capture ); 
    if ( frame_rgb != NULL ) {

        cvResize(frame_rgb, frame_rgb_small);

        if ( mode == MODE_REC ) {
            cvWriteFrame( writer_rgb, frame_rgb);
        }

        // count frames
        n_frame ++;
        scale_frame ();
        return true;
    } else {
        return false;
    }
}

IplImage * App::get_image_rgb(){
    return frame_rgb_small;
}

u_int App::list_add_new(u_int start, u_int end, gchar * type){
    GtkTreeIter iter;
    u_int index = anns->add( start, end, type );

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, index, 1, start, 2, end, 3, type, -1);

    anns->debug();
}



// list

AnnList::AnnList(){
    last_index = 0;
}

AnnList::~AnnList(){

}

u_int AnnList::add(u_int begin, u_int end, gchar * type){
    list_item item;
    item.b = begin;
    item.e = end;
    strcpy(item.type, type);

    list[last_index] =  item; 
    return ++last_index;
}

void AnnList::update(u_int index, u_int begin, u_int end, gchar * type){

}

void AnnList::remove(u_int index){

}


void AnnList::debug(){
  
    std::map<u_int, list_item>::iterator it;

    // show content:
    printf ("===========================\n");
    for ( it=list.begin() ; it != list.end(); it++ ){
        printf("%d | %d | %d | %s \n", (*it).first, (*it).second.b, (*it).second.e, (*it).second.type );
    }
    printf ("===========================\n");
    

}
