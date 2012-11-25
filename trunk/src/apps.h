
#include <gtk/gtk.h>
#include "freenect.h"
#include <cv.h>
#include <highgui.h> // contain function for loading video

#define MODE_PLAY 1
#define MODE_PAUSE 2
#define MODE_REC 3

#define TIMER 20
#define C_EXTERN    extern "C"

//#define CODEC       CV_FOURCC('T', 'H', 'E', 'O')
//#define CODEC       CV_FOURCC('M', 'J', 'P', 'G')
#define CODEC       CV_FOURCC('D','I','V','X')

gboolean 
on_timer (gpointer arg);

C_EXTERN gboolean
on_draw_video (GtkWidget * object, GdkEvent * eev, gpointer data);

class App {
    private:
        gint mode;
        // limits of frames
        gint frames_max;
        gint frames_min;
        
        char file_rgb[32];
        char file_depth[32];
        char file_xml[32];

        CvCapture * capture;
        IplImage  * frame_rgb;
        IplImage  * frame_depth;
        IplImage  * frame_rgb_small;
        IplImage  * frame_depth_small;
        CvVideoWriter * writer_rgb;
        CvVideoWriter * writer_depth; 

        void scale_frame();

    public:
        gint n_frame;

        GtkBuilder  * builder; 
        GtkWidget   * window;
        GtkWidget   * messagedialog1;
        GtkWidget   * drawarea;
        GtkScale    * scale;
        GtkButton   * button3;
        GtkAdjustment * adjustment;
        freenect    * kinect;

        App(char * file);
        ~App();

        void set_mode(gint _mode);
        gint get_mode();
        void load_video(char * file);
        gboolean next_frame();
        void set_pos_frame(double value);
        void play();
        void record();

        IplImage * get_image_rgb();
};


