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
    app->save_xml();
}

C_EXTERN void 
on_active_row_save (GtkObject *object, gpointer data)
{
    App * app = (App*) data;
    g_print ("INFO: update active row.\n");

    app->update_active_row();
}

C_EXTERN void
on_play_clicked (GtkObject *object, gpointer data)
{
    App * app = (App*) data;
    app->play();
}

static int mouse_event_start;
C_EXTERN void
on_mouse_down (GtkObject *object, gpointer data)
{
    // FIXME
    // App * app = (App*) data;
    printf("INFO: mouse down\n");
    mouse_event_start = app->n_frame;
}

C_EXTERN void
on_mouse_up (GtkObject *object, gpointer data)
{
    // FIXME
    // App * app = (App*) data;
    printf("INFO: mouse up %u %u\n", mouse_event_start, app->n_frame);

    // add value to list
    app->list_add_new(mouse_event_start, app->n_frame, (gchar *) "none");
}

C_EXTERN void 
cell_edited (GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data) {

	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
	GtkTreeIter iter;

	gint column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));

	gtk_tree_model_get_iter (model, &iter, path);
	
	gint i;

	i = gtk_tree_path_get_indices (path)[0];

	gtk_list_store_set (GTK_LIST_STORE (model), &iter, 3,
                            new_text, -1);

    gchar * text = g_strdup_printf("%s", new_text);
    app->anns->update(i, 0, 0, text);

	gtk_tree_path_free (path);
}

C_EXTERN void
on_click_row (GtkObject *object, gpointer data){

    // FIXME
    // App * app = (App*) data;

  GtkTreeModel * model;
  guint value0;
  guint value1;
  guint value2;
  gchar * value3;

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(object), &model, &(app->iter) )) {

    gtk_tree_model_get(model, &(app->iter), 0, &value0,  -1);
    gtk_tree_model_get(model, &(app->iter), 1, &value1,  -1);
    gtk_tree_model_get(model, &(app->iter), 2, &value2,  -1);
    gtk_tree_model_get(model, &(app->iter), 3, &value3,  -1);

    printf("row %d: %d %d %s\n", value0, value1, value2, value3) ;

    app->anns->set_active(value0);
    app->set_text_row();
  }
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
        LOCKED_PAINT = false;
        return false;
    }

    gtk_widget_queue_draw ( GTK_WIDGET (app->drawarea) );

    LOCKED_PAINT = false;
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
    IplImage * cv_image2 = NULL;

    cv_image = app->get_image_rgb ();
    cv_image2 = app->get_image_depth ();

    if (cv_image == NULL || cv_image2 == NULL ) {
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

    g_object_unref (pix);

    pix = gdk_pixbuf_new_from_data ((guchar *) cv_image2->imageData,
                                               GDK_COLORSPACE_RGB,
                                               FALSE,
                                               cv_image2->depth,
                                               cv_image2->width,
                                               cv_image2->height,
                                               cv_image2->widthStep,
                                               NULL,
                                               NULL);
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
