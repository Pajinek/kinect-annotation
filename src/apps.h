
#include <gtk/gtk.h>
#include "freenect.h"

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


