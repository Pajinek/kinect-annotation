#include "apps.h"

// smaller thumbnail for gui
float rate_tmp = 0.54375;

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

    // init default value
    mode = 0;
    n_frame = 0;
    capture = NULL;
    is_move_pos_video = false;
    file_video = NULL;
    file_config = file;
    timestamp = time(NULL);
    is_video_load = false;

    config = new Config(file);
    anns = new AnnList();
    if (config->load(anns)){
        config->set_path(file_rgb, file_depth);
        strcpy(file_xml, file);
        printf("video: '%s' '%s' \n", file_rgb, file_depth);
        is_video_load = true;
    } else {
        sprintf(file_rgb,"data/%d.rgb.avi", timestamp);		
        sprintf(file_depth,"data/%d.depth.avi", timestamp);	
        sprintf(file_xml, "data/%d.xml", timestamp); 
        sprintf(file_skeleton, "data/%d.skeleton.txt", timestamp); 
    }
}

short App::Run(){

    // load data from glade xml
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/window.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));

    // temporary value for create columns
	GtkTreeViewColumn * col;

    // drawarea

    drawarea = GTK_WIDGET (gtk_builder_get_object (builder, "drawingarea1"));
    frame_rgb_small = cvCreateImage(cvSize(640 * rate_tmp, 480 * rate_tmp), IPL_DEPTH_8U, 3);
    frame_depth_small = cvCreateImage(cvSize(640 * rate_tmp, 480 * rate_tmp), IPL_DEPTH_8U, 3);

    // scale for moving in video
    scale = GTK_SCALE (gtk_builder_get_object (builder, "scale1"));
    adjustment = GTK_ADJUSTMENT (gtk_builder_get_object (builder, "adjustment1"));

    // buttons
    button_play = GTK_BUTTON (gtk_builder_get_object (builder, "button3"));
    button_rec = GTK_BUTTON (gtk_builder_get_object (builder, "button1"));
    button_list_save = GTK_BUTTON (gtk_builder_get_object (builder, "button4"));
    button_list_e1 = GTK_ENTRY (gtk_builder_get_object (builder, "entry1"));
    button_list_e2 = GTK_ENTRY (gtk_builder_get_object (builder, "entry2"));
    button_list_e3 = GTK_ENTRY (gtk_builder_get_object (builder, "entry3"));

    action_record_rgb = false;
    action_record_depth = false;
    action_record_skeleton = false;

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


    // set default value
    kinect = NULL;
    frame_width = 640;
    frame_height = 480;
    frame_fps = 30;

    if (is_video_load) {
        // laod file
        load_video();

        set_mode( MODE_PAUSE );

        printf("INFO: load video\n");

    } else {
        // run kinect
#ifdef OPENNI 
    	kinect = CVKinectWrapper::getInstance();
	    if(kinect->init("/etc/openni/SamplesConfig.xml"))
            printf("INFO: kinect openni\n");
#else
        // init kinect
        kinect = new freenect ();
        kinect->init (0);
        printf("INFO: kinect libfreenect\n");
#endif
        set_mode( MODE_STOP );

        if ( kinect->get_error() && file_video == NULL ){
            // feedback for kinect

            // TODO: show warning message
            printf ("WARING: kinect not init.\n");
            
            /* messagedialog1 = GTK_WIDGET (gtk_builder_get_object (builder, "messagedialog1"));
            gtk_widget_activate  (messagedialog1);
            gtk_widget_show  (messagedialog1); */
        
            // feedback for video
            capture = cvCaptureFromCAM( -1 );
            set_mode ( (gint) MODE_PAUSE );
            cvGrabFrame ( capture );
            cvSetCaptureProperty( capture, CV_CAP_PROP_CONVERT_RGB, 0.);
            int fps = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS);
            printf( "INFO: count of fps %d\n", fps );

            set_param_video();
        
            printf("INFO: feedback cam\n");
        }
    }

    store = gtk_list_store_new(4, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING);

    // create table column
    GtkCellRenderer     *renderer;
    GtkCellRenderer     *renderer2;
    renderer = gtk_cell_renderer_text_new ();
    renderer2 = gtk_cell_renderer_text_new ();
    g_object_set(renderer, "editable", TRUE, NULL);
    g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), store);
    g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (0));

    col = gtk_tree_view_column_new_with_attributes (
        " Id ", renderer2, "text", 0, NULL);

    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);
    col = gtk_tree_view_column_new_with_attributes (
        "   Begin (FPS)   ", renderer2, "text", 1, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

    col = gtk_tree_view_column_new_with_attributes (
        "   End (FPS)   ", renderer2, "text", 2, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

    col = gtk_tree_view_column_new_with_attributes (
        "   Type", renderer, "text", 3, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

    //gtk_tree_model_foreach(GTK_TREE_MODEL(store), foreach_func, NULL);

    // GtkTreeSelection *selection; 
    // selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
    // g_signal_connect(selection, "changed", G_CALLBACK(on_click_row), label);
    //

    //reload data

    std::map<u_int, list_item *>::iterator it;
    for ( it=anns->list.begin() ; it != anns->list.end(); it++ ){
        printf("%d | %d | %d | %s \n", (*it).first, (*it).second->b, (*it).second->e, (*it).second->type );
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0,  (*it).first, 1,  (*it).second->b, 2, (*it).second->e, 3, (*it).second->type, -1);
        
    }
    
    gtk_builder_connect_signals (builder, (void *) this);
    g_object_unref (G_OBJECT (builder));

    gtk_widget_show (window);                
    gtk_main ();

    return 0;
}

App::~App(){
    printf("TODO: free all memory\n");
    // cvReleaseImage(&frame_depth_small);
    // cvReleaseImage(&frame_rgb_small);
    if (kinect) delete kinect;
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

}

void App::load_video(){
    printf("INFO: load file %s\n", file_rgb);

    capture_rgb = cvCaptureFromAVI( file_rgb );
    if (capture_rgb != NULL) {
        set_mode ( (gint) MODE_PAUSE );

        cvSetCaptureProperty( capture_rgb, CV_CAP_PROP_POS_AVI_RATIO, 0.);
        cvGrabFrame ( capture_rgb );

        // set max size 
        double count = cvGetCaptureProperty( capture_rgb, CV_CAP_PROP_FRAME_COUNT);
        gtk_adjustment_set_upper (adjustment, (gint) count ); 

        is_move_pos_video = true;
        frame_fps = 30;
    } 

    capture_depth = cvCaptureFromAVI( file_depth );
    if (capture_depth != NULL) {

        cvSetCaptureProperty( capture_depth, CV_CAP_PROP_POS_AVI_RATIO, 0.);
        cvGrabFrame ( capture_depth );

    } 
}
void App::load_video(char * file){
    printf("INFO: load file %s\n", file);

    capture = cvCaptureFromAVI( file );
    if (capture != NULL) {
        set_mode ( (gint) MODE_PAUSE );

        cvSetCaptureProperty( capture, CV_CAP_PROP_POS_AVI_RATIO, 0.);
        cvGrabFrame ( capture );

        // set max size 
        double count = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT);
        gtk_adjustment_set_upper (adjustment, (gint) count ); 

        is_move_pos_video = true;
        frame_fps = 30;
    }
}

void App::set_mode(gint _mode){
    this->mode = _mode;
}

void App::save_skeleton(float * data, short size){

    // (x,y,z) * 5 (head, two shoulder and 2 hands)
    float * n_data = (float *) malloc(sizeof(float) * size * 3);
    for(short i = 0; i < size*3; i++){
       n_data[i] = data[i]; 
    }
    //v_skeleton.push_back( n_data );
    config_skl->add(n_data, size);

    //config_skl->add(data, size);
}

void App::load_skeleton(){
    
}

void App::play(){
    printf ("mode %d\n", mode);
    if ( mode == MODE_PLAY ) {
        set_mode ( MODE_PAUSE );
        gtk_button_set_label (button_play, "play" );
        g_print ("INFO: pause video.\n");

        return;
    }
    
    gtk_button_set_label (button_play, "pause" );

    if ( mode < MODE_SHOW ) { 
        set_mode( MODE_PLAY );
    } else {
        set_mode( MODE_SHOW );
    }

    printf("FPS: %d\n", frame_fps);
    // set timer for recording
    g_timeout_add (1000.0 / frame_fps, on_timer, (void *) this );

    g_print ("INFO: play video.\n");

}

void App::record(){


    if ( mode < MODE_SHOW ) {
        printf("can't record %d\n", mode);
        return;
    }

    // disable record
    if ( mode == MODE_REC ) {
        if ( action_record_rgb ) cvReleaseVideoWriter( &writer_rgb );
        if ( action_record_depth ) cvReleaseVideoWriter( &writer_depth ); 
        if ( action_record_skeleton ) config_skl->close(); 
        set_mode ( MODE_STOP );
        gtk_button_set_label (button_rec, "rec" );
        return;
    }

    // enable record
    set_mode ( MODE_REC );

    // init new file for record
    printf("INFO: create files: %s\n" 
           "                    %s\n"	
           "                    %s\n", file_rgb, file_depth, file_skeleton);	
    if ( action_record_rgb ) {
        writer_rgb = cvCreateVideoWriter(
                            file_rgb, CODEC , frame_fps, 
                            cvSize(frame_width, frame_height),  1 /* is color */ );
    }
    if ( action_record_depth ) {
        writer_depth = cvCreateVideoWriter(
                            file_depth, CODEC , frame_fps,
                            cvSize(frame_width, frame_height), 1 /* is color */ ); 
    }

    if ( action_record_skeleton ) {
        config_skl = new ConfigSkl(file_skeleton);
    }

    gtk_button_set_label (button_rec, "stop rec" );
}

gint App::get_mode(){
    return mode;
}

void App::scale_frame(){
    gtk_adjustment_set_value (adjustment, n_frame);
    if( mode == MODE_REC && gtk_adjustment_get_upper (adjustment) <= n_frame ){
        gtk_adjustment_set_upper (adjustment, n_frame ); 
    }
}

void App::set_pos_frame(double value){
    int val = (int) floor(value);

    if (!is_move_pos_video) return;

    if (n_frame == val) return;
    
    n_frame = val;

    // avi move only about one frame
    if( val > 0) {
        // FIXME !!
        cvSetCaptureProperty(capture_rgb, CV_CAP_PROP_POS_FRAMES, val);
        frame_rgb = cvQueryFrame ( capture_rgb );

        cvSetCaptureProperty(capture_depth, CV_CAP_PROP_POS_FRAMES, val);
        frame_depth = cvQueryFrame ( capture_depth );

        cvResize(frame_rgb, frame_rgb_small);
        cvResize(frame_depth, frame_depth_small);
        cvCvtColor(frame_depth_small, frame_depth_small, CV_BGR2RGB);
        cvCvtColor(frame_rgb_small, frame_rgb_small, CV_BGR2RGB);

        // re-draw new frame
        gtk_widget_queue_draw ( GTK_WIDGET (drawarea) );
   }
}

void App::set_text_row(){
    list_item * it = anns->get_active_row();
    gchar begin[32];
    gchar end[32];
    sprintf(end, "%d", it->e);
    sprintf(begin, "%d", it->b);
    gtk_entry_set_text(button_list_e2, begin);
    gtk_entry_set_text(button_list_e3, end);
    gtk_entry_set_text(button_list_e1, it->type);
}


void App::update_active_row(){
    //  gtk_list_store_remove(GTK_LIST_STORE(model), &iter); 
    //
/*  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(object), &model, &iter )) {

        gtk_tree_model_set(model, &iter, 0, "new",  -1);
    }*/
}

gboolean App::next_frame (){
    float * skl = NULL;
    if ( is_move_pos_video ){   
        // camera for feedback
        frame_rgb = cvQueryFrame ( capture_rgb ); 
        // cvCopy(frame_rgb, frame_depth);
        frame_depth = cvQueryFrame ( capture_depth ); 
;
    } else {
        if(kinect->reload ()){
            frame_rgb = kinect->get_image_rgb ();
            frame_depth = kinect->get_image_depth_rgb ();
            if (kinect->is_user_tracker()){
                skl = kinect->get_skeleton_float_p();
            }
        } else {
            printf("INFO ANNOTATOR: cpu slow\n");
        }
    }

    if ( frame_rgb != NULL or frame_depth != NULL ) {

        cvResize(frame_rgb, frame_rgb_small);
        cvResize(frame_depth, frame_depth_small);
        cvCvtColor(frame_depth_small, frame_depth_small, CV_BGR2RGB);
        cvCvtColor(frame_rgb_small, frame_rgb_small, CV_BGR2RGB);

        if (kinect->is_user_tracker()){
            //printf("%f %f %f\n", skl[0]*rate_tmp, skl[1]*rate_tmp, skl[2]);
            //printf("%f %f %f\n", skl[0], skl[1], skl[2]);
            for( short i = 0; i < 8; i++){
                cvCircle(frame_depth_small, cvPoint( skl[i*3]*rate_tmp, skl[i*3+1]*rate_tmp), 3, cvScalar(255,0,0), -1);
            }
        }

        if ( mode == MODE_REC ) {
            if ( action_record_rgb ) {
                cvWriteFrame( writer_rgb, frame_rgb);
            }
            if ( action_record_depth ) {
                cvWriteFrame( writer_depth, frame_depth);
            }
            if ( action_record_skeleton ) {
                skl = kinect->get_skeleton_float();
                save_skeleton(skl, 8);
                //printf("%f %f %f\n", skl[0], skl[1], skl[2]);
            }
            n_frame ++;
        } else if ( mode == MODE_PLAY ) {
            n_frame ++;
        }

        // count frames
        scale_frame ();
        return true;
    } else {
        return false;
    }
}

IplImage * App::get_image_rgb(){
    return frame_rgb_small;
}

IplImage * App::get_image_depth(){
    return frame_depth_small;
}

u_int App::list_add_new(u_int start, u_int end, gchar * type){
    GtkTreeIter iter;
    u_int index = anns->add( start, end, type );

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, (index - 1), 1, start, 2, end, 3, type, -1);

    anns->debug();
}

void App::save_xml(){
    Config * config = new Config(file_xml);
    printf("save to %s\n", file_xml);
    config->save(anns);
    delete config;
}
