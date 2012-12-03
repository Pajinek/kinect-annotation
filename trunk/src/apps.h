
#include <gtk/gtk.h>
#include "freenect.h"
#include "config.hpp"
#include <cv.h>
#include <highgui.h> // contain function for loading video

#define MODE_PLAY 1
#define MODE_PAUSE 2
#define MODE_REC 3

#define C_EXTERN    extern "C"

//#define CODEC       CV_FOURCC('T', 'H', 'E', 'O')
//#define CODEC       CV_FOURCC('M', 'J', 'P', 'G')
#define CODEC       CV_FOURCC('D','I','V','X')

gboolean 
on_timer (gpointer arg);

C_EXTERN gboolean
on_draw_video (GtkWidget * object, GdkEvent * eev, gpointer data);

C_EXTERN gboolean
on_scale_move_slider (GtkScale * object, gdouble value, gpointer data);

typedef struct {
    u_int b;
    u_int e;
    gchar type[64]; // TODO - it's only temporary limit for string
} list_item;


class AnnList {

        std::map<u_int, list_item> list;
        u_int last_index;
        u_int activate;

    public:
        AnnList();
        ~AnnList();
        u_int add(u_int begin, u_int end, gchar * type);
        void update(u_int index, u_int begin, u_int end, gchar * type);
        void remove(u_int index);
        void debug();
        void set_active(u_int index);
};


class App {
    private:
        gint mode;
        // limits of frames
        gint frames_max;
        gint frames_min;
        
        // file path of save video and xml
        char file_rgb[32];
        char file_depth[32];
        char file_xml[32];

        CvCapture * capture;
        IplImage  * frame_rgb;
        IplImage  * frame_depth;
        IplImage  * frame_rgb_small;
        IplImage  * frame_depth_small;
        u_int frame_fps;
        u_int frame_width;
        u_int frame_height;
        CvVideoWriter * writer_rgb;
        CvVideoWriter * writer_depth; 

        void scale_frame();

        GtkButton   * button_rec;
        GtkButton   * button_play;

        GtkTreeView * list;
        GtkListStore * store;

        bool is_move_pos_video;

    public:
        gint n_frame;
        AnnList * anns;

        GtkBuilder  * builder; 
        GtkWidget   * window;
        GtkWidget   * messagedialog1;
        GtkWidget   * drawarea;
        GtkScale    * scale;
        GtkAdjustment * adjustment;
        freenect    * kinect;
        Config      * config;

        App(char * file);
        ~App();

        void set_mode(gint _mode);
        gint get_mode();
        void load_video(char * file);
        void set_param_video();
        gboolean next_frame();
        void set_pos_frame(double value);
        void play();
        void record();

        // list
        u_int list_add_new(u_int start, u_int end, gchar * type);

        IplImage * get_image_rgb();
};

// fix problem with reference on pointer this
static App * app;


