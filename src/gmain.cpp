#include "apps.h"

#define C_EXTERN    extern "C"
#define TIMER 30

C_EXTERN void 
on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit ();
}

C_EXTERN void 
on_rec_clicked (GtkObject *object, gpointer data)
{
    App * app = (App*) &data;
    app->set_mode (MODE_REC);
    g_print ("INFO: switch on record.\n");
}

C_EXTERN void 
on_save_clicked (GtkObject *object, gpointer data)
{
    App * app = (App*) &data;
    g_print ("INFO: save project to xml.\n");
}

gboolean
on_timer (void *arg)
{
    //App * app = (App*) &data;
    g_print (".");
    return true;
}

C_EXTERN gboolean
on_scale_move_slider (GtkScale *object, gdouble value, gpointer data)
{
    g_print ("value: %g %d\n", value, (gint) value);
    App * app = (App*) &data;

    app->n_frame = (gint) value;

    return false;
}


/* FIXME 
static gboolean
on_draw_video (GtkWidget * widget, GdkEvent * eev, gpointer arg)
{

    //gtk_widget_queue_draw(GTK_WIDGET(widget));
    if (!gmain_run || manul_test)
        return false;

    s_gmain *d = (s_gmain *) arg;

    IplImage *cv_image;

    cv_image = d->frn->get_image_rgb ();

    //převedení opencv do gtk
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
    gdk_draw_pixbuf (widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                     pix, 0, 0, 0, 0, 640, 480, GDK_RGB_DITHER_NONE, 0, 0);

    //gtk_widget_queue_draw(widget);
    g_object_unref (pix);


    cv_image = d->frn->get_image_depth_rgb ();

    pix = gdk_pixbuf_new_from_data ((guchar *) cv_image->imageData,
                                    GDK_COLORSPACE_RGB,
                                    FALSE,
                                    cv_image->depth,
                                    cv_image->width,
                                    cv_image->height,
                                    cv_image->widthStep, NULL, NULL);
    //gtkImg = gtk_image_new_from_pixbuf (pix);
    gdk_draw_pixbuf (widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                     pix, 0, 0, 640, 0, 640, 480, GDK_RGB_DITHER_NONE, 0, 0);

    //gtk_widget_queue_draw(widget);
    g_object_unref (pix);

    return true;
} */

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
                printf ("help\n");
        }
    }

    gtk_init (&argc, &argv);

    // init application that contain all data
    App * app = new App (file);

    return 0;
}
