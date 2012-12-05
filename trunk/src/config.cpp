
#include "config.hpp"


Config::Config(char * filename){

    FILE * file;

    path_rgb = (char *) malloc(32);
    path_depth = (char *) malloc(32);
    path_xml = (char *) malloc(32);
    sprintf(path_rgb, "%s.rgb.avi", filename); 
    sprintf(path_depth, "%s.depth.avi", filename); 
    sprintf(path_xml, "%s.xml", filename); 

    if (file = fopen(filename, "r")){
    // file exists

       // fclose(file);
    } else {
    // file doesn't exist

    }
}


void Config::save(AnnList * anns){

    std::map<u_int, list_item *>::iterator it;
    
    FILE * file = fopen(path_xml, "w");

    // show content:
    fprintf (file, "<annotation>\n");
    fprintf (file, "<video>%s</video>\n", path_rgb);
    fprintf (file, "<depth>%s</depth>\n", path_depth);
    for ( it=anns->list.begin() ; it != anns->list.end(); it++ ){
        fprintf (file, "<item>\n");
        fprintf (file, "<begin>%d</begin><end>%d</end><type>%s</type>\n", (*it).second->b, (*it).second->e, (*it).second->type );
        fprintf (file, "</item>\n");
    }
    fprintf (file, "</annotation>\n");
    fclose(file);

}

Config::~Config(){

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

