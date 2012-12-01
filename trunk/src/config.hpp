
#include <libxml/parser.h>
#include <libxml/tree.h>


class Config {
    char * path_xml;
    char * path_rgb;
    char * path_depth;

public:
    Config( char * filename);
    ~Config();

};
