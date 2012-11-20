
#include <gtk/gtk.h>
#include "freenect.h"

#define MODE_PLAY 1
#define MODE_PAUSE 2
#define MODE_REC 3

class App {
    private:
        short mode;

    public:
        gint n_frame;

        GtkBuilder  * builder; 
        GtkWidget   * window;
        GtkWidget   * messagedialog1;
        GtkWidget   * drawarea;
        GtkScale    * scale;

        App(char * file);
        ~App();

        void set_mode(short _mode);
};


