
#include <gtk/gtk.h>
#include "config.hpp"
#include <cv.h>
#include <highgui.h> // contain function for loading video

#define MODE_PLAY   1
#define MODE_PAUSE  2
// if value > 10
#define MODE_SHOW  11
#define MODE_REC   12
#define MODE_REC_PAUSE 13
#define MODE_STOP  14

#define C_EXTERN    extern "C"

#define CODEC       CV_FOURCC('T', 'H', 'E', 'O')
//#define CODEC       CV_FOURCC('M', 'J', 'P', 'G')
//#define CODEC       CV_FOURCC('D','I','V','X')
//#define CODEC       0

#define OPENNI True

#ifdef OPENNI 
    #include "openni.h"
#else
    #include "freenect.h"
#endif


gboolean 
on_timer (gpointer arg);

C_EXTERN gboolean
on_draw_video (GtkWidget * object, GdkEvent * eev, gpointer data);

C_EXTERN gboolean
on_scale_move_slider (GtkScale * object, gdouble value, gpointer data);

C_EXTERN void cell_edited (GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data);


class App {
    private:
        gint mode;
        // limits of frames
        gint frames_max;
        gint frames_min;

        int timestamp;
 
        // file path of save video and xml
        char file_rgb[32];
        char file_depth[32];
        char file_xml[32];
        char file_skeleton[32];
        char  * file_config;
        char  * file_video;

        CvCapture * capture;
        CvCapture * capture_rgb;
        CvCapture * capture_depth;
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
        GtkButton   * button_list_save;
        GtkEntry    * button_list_e1;
        GtkEntry    * button_list_e2;
        GtkEntry    * button_list_e3;

        GtkTreeView * list;
        GtkListStore * store;

        bool is_move_pos_video;
        bool is_video_load;

        // (x,y,z) * 5 (head, two shoulder and 2 hands)
        std::vector<float * > v_skeleton;
        ConfigSkl * config_skl;
    public:
        gint n_frame;
        AnnList * anns;
        GtkTreeIter iter;
        bool action_record_rgb, action_record_depth, action_record_skeleton;

        GtkBuilder  * builder; 
        GtkWidget   * window;
        GtkWidget   * messagedialog1;
        GtkWidget   * drawarea;
        GtkScale    * scale;
        GtkAdjustment * adjustment;

#ifdef OPENNI 
        CVKinectWrapper * kinect;
#else
        freenect    * kinect;
#endif

        Config      * config;

        App(char * file);
        short Run();
        ~App();

        void set_mode(gint _mode);
        gint get_mode();
        void load_video(char * file);
        void load_video();
        void set_param_video();
        gboolean next_frame();
        void set_pos_frame(double value);
        void set_text_row();
        void update_active_row();
        void play();
        void record();
        void save_skeleton(float * data, short size);
        void load_skeleton();
        void save_xml();

        // list
        u_int list_add_new(u_int start, u_int end, gchar * type);

        IplImage * get_image_rgb();
        IplImage * get_image_depth();
};

// fix problem with reference on pointer this
static App * app;


