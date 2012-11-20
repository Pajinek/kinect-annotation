class App {
    private:

    public:
        gint n_frame;

        GtkBuilder  * builder; 
        GtkWidget   * window;
        GtkWidget   * messagedialog1;
        GtkWidget   * drawarea;
        GtkScale    * scale;

        App(char * file);
        ~App();
};


App::App(char * file){

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/window.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));

    n_frame = 1;

    gtk_builder_connect_signals (builder, (void *) this);

    drawarea = GTK_WIDGET (gtk_builder_get_object (builder, "drawarea1"));

    scale = GTK_SCALE (gtk_builder_get_object (builder, "scale1"));

    // set timer for recording
    //g_timeout_add (1000.0 / TIMER, on_timer, (void *) drawarea );
    //

    // load file
    if (file != NULL) {
        printf ("INFO: load video: %s \n", file);
    }

    // init kinect
    freenect * kinect = new freenect ();
    kinect->init (0);
    if ( kinect->get_error() ){
        messagedialog1 = GTK_WIDGET (gtk_builder_get_object (builder, "messagedialog1"));
        gtk_widget_activate  (messagedialog1);
        gtk_widget_show  (messagedialog1);
    }

    g_object_unref (G_OBJECT (builder));
        
    gtk_widget_show (window);                
    gtk_main ();
}
