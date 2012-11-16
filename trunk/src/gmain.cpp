#include <gtk/gtk.h>
#include "freenect.h"

#define C_EXTERN    extern "C"
#define TIMER 30

C_EXTERN void 
on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit ();
}

C_EXTERN void 
on_rec_clicked (GtkObject *object, gpointer user_data)
{
    g_print("rec..\n");
}

C_EXTERN void 
on_save_clicked (GtkObject *object, gpointer user_data)
{
    g_print("save..\n");
}

gboolean
on_timer (void *arg)
{
    g_print(".");
    return true;
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
    GtkBuilder      *builder; 
    GtkWidget       *window, * messagedialog1;
    GtkWidget       *drawarea;

    gtk_init (&argc, &argv);

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/window.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
    gtk_builder_connect_signals (builder, window);


    drawarea = GTK_WIDGET (gtk_builder_get_object (builder, "drawarea1"));

    // set timer for recording
    //g_timeout_add (1000.0 / TIMER, on_timer, (void *) drawarea );

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

    return 0;
}
