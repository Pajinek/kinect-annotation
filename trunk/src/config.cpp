
#include "config.hpp"


ConfigSkl::ConfigSkl(char * filename){
    counter = 0;
    printf("TXT:open file %s\n", filename);
    if (filename != NULL && (cfile = fopen(filename, "w"))){

    } else {
        printf("ERROR: can't open\n");        
    }
}

void ConfigSkl::add(float * data, short size){
    counter++;

    fprintf(cfile, "%d: ", counter);
    for (short i = 0; i < size; i++) {
        fprintf(cfile, "%f %f %f; ", data[i*3], data[i*3+1], data[i*3+2]);
    }
/*
    short i = 1;
    fprintf(cfile, "%f %f %f; ", data[i*3], data[i*3+1], data[i*3+2]);
    printf("%f %f %f - ", data[i*3], data[i*3+1], data[i*3+2]);
    i = 5;
    fprintf(cfile, "%f %f %f; ", data[i], data[i*3+1], data[i*3+2]);
    printf("%f %f %f - ", data[i*3], data[i*3+1], data[i*3+2]);
    i = 7;
    fprintf(cfile, "%f %f %f; ", data[i], data[i*3+1], data[i*3+2]);
    printf("%f %f %f\n", data[i], data[i*3+1], data[i*3+2]);*/
    fprintf(cfile, "\n");
}

void ConfigSkl::close(){
    fclose(cfile);
}

Config::Config(char * filename){
    

    FILE * cfile;

    path_rgb = (char *) malloc(64);
    path_depth = (char *) malloc(64);
    path_xml = (char *) malloc(64);
    path_skeleton = (char *) malloc(64);
    is_valid = false;

    if ( filename == NULL) return;

    printf("XML:open file %s\n", filename);
    if (filename != NULL && (cfile = fopen(filename, "r"))){
    // file exists
        printf("XML:opening file %s\n", filename);
        strcpy(path_xml, filename);
        is_valid = true;
        fclose(cfile);
    } else {
    // file doesn't exist
        sprintf(path_rgb, "%s.rgb.avi", filename); 
        sprintf(path_depth, "%s.depth.avi", filename); 
        sprintf(path_skeleton, "%s.skeleton.xml", filename); 
        strcpy(path_xml, filename); 
    }
}

void 
get_element_item (xmlNode * a_node, int  * begin, int * end, char * type )
{

    xmlNode *cur_node = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next)
      {
          if (cur_node->type == XML_ELEMENT_NODE)
            {
                // filter
                if (strcmp ((char *) cur_node->name, "type") == 0)
                    strcpy(type, ((char *) xmlNodeGetContent (cur_node)));
                if (strcmp ((char *) cur_node->name, "begin") == 0)
                    *begin =  atoi ((char *) xmlNodeGetContent (cur_node));
                if (strcmp ((char *) cur_node->name, "end") == 0)
                    *end = atoi ((char *) xmlNodeGetContent (cur_node));
            }
          get_element_item (cur_node->children, begin, end, type);
      }
}
void 
Config::get_element_config (xmlNode * a_node, AnnList * anns)
{
    xmlNode *cur_node = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next)
      {
          if (cur_node->type == XML_ELEMENT_NODE)
            {
                if (strcmp ((char *) cur_node->name, "video") == 0)
                    strcpy(path_rgb, (char *) xmlNodeGetContent (cur_node)) ;
                if (strcmp ((char *) cur_node->name, "depth") == 0)
                    strcpy(path_depth, (char *) xmlNodeGetContent (cur_node)) ;
                if (strcmp ((char *) cur_node->name, "skeleton") == 0)
                    strcpy(path_skeleton, (char *) xmlNodeGetContent (cur_node)) ;

                // filter
                if (strcmp ((char *) cur_node->name, "item") == 0){
                    int begin, end;
                    char type[126];
                    get_element_item(cur_node->children,  &begin, &end, type);
                    anns->add(begin, end, (gchar *) type);
                }
            }

          get_element_config (cur_node->children, anns);
      }
}


bool Config::load(AnnList * anns){
        if (! is_valid) return false;

        xmlDocPtr doc;              /* the resulting document tree */
        printf("ok.. %s\n", path_xml);
        doc = xmlReadFile (path_xml, NULL, 0);
        printf("ok..\n");
        xmlNode *node = xmlDocGetRootElement (doc);
        get_element_config (node->children, anns);
        xmlFreeDoc (doc);
        xmlCleanupParser ();
        anns->debug();
        return true;
}

void Config::save(AnnList * anns){

    std::map<u_int, list_item *>::iterator it;
    printf("save to %s\n", path_xml);
    FILE * file = fopen(path_xml, "w");

    // show content:
    fprintf (file, "<annotation>\n");
    fprintf (file, "<video>%s</video>\n", path_rgb);
    fprintf (file, "<depth>%s</depth>\n", path_depth);
    fprintf (file, "<skeleton>%s</skeleton>\n", path_skeleton);
    for ( it=anns->list.begin() ; it != anns->list.end(); it++ ){
        fprintf (file, "<item>\n");
        fprintf (file, "<begin>%d</begin><end>%d</end><type>%s</type>\n", (*it).second->b, (*it).second->e, (*it).second->type );
        fprintf (file, "</item>\n");
    }
    fprintf (file, "</annotation>\n");
    fclose(file);

}

void Config::set_path(char * file_rgb, char * file_depth){
    strcpy(file_rgb, path_rgb);
    strcpy(file_depth, path_depth);
}

Config::~Config(){

    free (path_rgb);
    free (path_depth);
    free (path_skeleton);
    free (path_xml);
}

// list

AnnList::AnnList(){
    last_index = 0;
    activate = 0;
}

AnnList::~AnnList(){

}

u_int AnnList::add(u_int begin, u_int end, gchar * type){
    list_item  * item = new list_item() ;
    item->b = begin;
    item->e = end;
    strcpy(item->type, type);

    list[last_index] =  item; 
    return ++last_index;
}

void AnnList::update(u_int index, u_int begin, u_int end, gchar * type){

    strcpy(list[index]->type, type);
    debug();
}

void AnnList::remove(u_int index){

}

u_int AnnList::get_active(){
    return activate;
}


list_item * AnnList::get_active_row(){
    return list[activate];
}


void AnnList::set_active(u_int index){
    activate = index;

    debug();
}

void AnnList::debug(){
  
    std::map<u_int, list_item *>::iterator it;

    // show content:
    printf ("===========================\n");
    printf ("active: %d\n", activate);
    printf ("---------------------------\n");
    for ( it=list.begin() ; it != list.end(); it++ ){
        printf("%d | %d | %d | %s \n", (*it).first, (*it).second->b, (*it).second->e, (*it).second->type );
    }
    printf ("===========================\n");
    

}

