
#include "config.hpp"


Config::Config(char * filename){

    if (FILE * file = fopen(filename, "r")){
    // file exists

    } else {
    // file doesn't exist

    }
}

Config::~Config(){

}

