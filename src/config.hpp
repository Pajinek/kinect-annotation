
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gtk/gtk.h>
#include <map>
#include <string.h>

typedef struct {
    u_int b;
    u_int e;
    gchar type[64]; // TODO - it's only temporary limit for string
} list_item;


class AnnList {

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
        u_int get_active();
        list_item * get_active_row();

        std::map<u_int, list_item *> list;
};


class ConfigSkl {
    private:
        unsigned int counter;
        FILE * cfile;
    public:
        ConfigSkl(char * filename);
        void add(float * data, short size);
        void close();
};


class Config {
    char * path_xml;
    char * path_rgb;
    char * path_depth;
    char * path_skeleton;
    bool is_valid;
public:
    Config( char * filename);
    void save(AnnList * anns);
    bool load(AnnList * anns);
    void set_path(char * file_rgb, char * file_depth);
    void get_element_config (xmlNode * a_node, AnnList * anns);
    ~Config();

};
