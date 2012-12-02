#include "apps.h"

static bool LOCKED_PAINT = false;
static u_int COUNT_PAINT = 0;

C_EXTERN void 
on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit ();
}

C_EXTERN void 
on_rec_clicked (GtkObject *object, gpointer data)
{
    App * app = (App*) data;
    app->record();
    g_print ("INFO: switch on record.\n");
}

C_EXTERN void 
on_save_clicked (GtkObject *object, gpointer data)
{
    App * app = (App*) data;
    g_print ("INFO: save project to xml.\n");
}


C_EXTERN void 
on_play_clicked (GtkObject *object, gpointer data)
{
    App * app = (App*) data;
    app->play();
}

gboolean
on_timer (gpointer data)
{
    App * app = (App *) data;

    if( app->get_mode() == MODE_PAUSE ) return false;

    if(LOCKED_PAINT){
        printf("WARNING: Slow CPU - frame losted (generate locked).\n");
        return true;
    }
    LOCKED_PAINT = true;

    if (! app->next_frame()) {
        printf("WARNING: frame lost - timer\n");
    }

    LOCKED_PAINT = false;

    gtk_widget_queue_draw ( GTK_WIDGET (app->drawarea) );

    return true;
}

C_EXTERN gboolean
on_scale_move_slider (GtkScale * object, gdouble value, gpointer data)
{
    // FIXME apps is bad value
    // App * app = (App*) &data;
    // printf(">> %g %d\n",  value, app->n_frame );
    app->set_pos_frame(value);

    return false;
}

C_EXTERN gboolean
on_draw_video (GtkWidget * object, GdkEvent * eev, gpointer data)
{

    app = (App*) data;
    IplImage * cv_image = NULL;

    if ( app->get_mode() == MODE_REC ) {
        //cv_image = app->kinect->get_image_rgb ();
        cv_image = app->get_image_rgb ();
    }
    else {
        cv_image = app->get_image_rgb ();
    }

    if (cv_image == NULL) {
        printf ("ERROR: Image is NULL\n");
        return true;
    }

    //convert opencv to gtk
    GdkPixbuf *pix = gdk_pixbuf_new_from_data ((guchar *) cv_image->imageData,
                                               GDK_COLORSPACE_RGB,
                                               FALSE,
                                               cv_image->depth,
                                               cv_image->width,
                                               cv_image->height,
                                               cv_image->widthStep,
                                               NULL,
                                               NULL);

    //gtkImg = gtk_image_new_from_pixbuf (pix);
    gdk_draw_pixbuf (object->window,
                     object->style->fg_gc[GTK_WIDGET_STATE (object)],
                     pix, 0, 0, 0, 0, cv_image->width, cv_image->height, GDK_RGB_DITHER_NORMAL, 0, 0); // GDK_RGB_DITHER_NONE

    gdk_draw_pixbuf (object->window,
                     object->style->fg_gc[GTK_WIDGET_STATE (object)],
                     pix, 0, 0, cv_image->width+2, 0, cv_image->width, cv_image->height, GDK_RGB_DITHER_NORMAL, 0, 0);

    //gtk_widget_queue_draw(widget);
    g_object_unref (pix);

    return true;
} 

int help(){
    printf ("TODO: help\n");
    return 0;
}

int
main (int argc, char *argv[])
{

    int opt;
    char * file = NULL;

    while( (opt =  getopt (argc, argv, "hf:") ) != EOF){
        switch(opt){
            case 'f':
                file = optarg;
                break;
            case 'h':
            default:
                return help();
        }
    }
    if (argc > 1 && file == NULL) {
        return help();
    }

    gtk_init (&argc, &argv);

    // init application that contain all data
    App * app = new App (file);

    delete app;
    return 0;
}
