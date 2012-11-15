# Pavel Studeník
# email: pavelstudenik@gmail.com

CXX = g++ -O2 
LD = g++ -O2 #-DDEBUG=y
LDFLAGS =
CFLAGS=-g -Wall `pkg-config --cflags libfreenect`  -Wall -ltheora -ltheoraenc -logg -ltheoradec `xml2-config --cflags`

GLPROG = kinect-annotation
OBJECTS = main.o

OBJECTS2 = freenect.o main.o 

# Kinect library
LIBS = `pkg-config libfreenect --libs --cflags` `pkg-config opencv --libs --cflags`  -lpthread -lm -lusb-1.0 -lfreenect -Wunknown-pragmas
LIBS +=   -lpthread -lm -lusb-1.0 -L/usr/local/lib64/ -I/usr/local/include/libfreenect/ #./src/cvblobs8.3/libblob.a # no fedora -L/usr/local/lib64/
# GTK library
LIBS += `pkg-config --cflags --libs gtk+-2.0 gmodule-export-2.0 --libs gthread-2.0`
# XML library
LIBS +=  `xml2-config --libs`

all: $(PROG)

$(PROG): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(GLPROG) $(GLLIBS)

%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(LDFLAGS) -c $<  $(LIBS) 


clean:
	rm -rf *.o $(CVPROG) $(GLPROG)
