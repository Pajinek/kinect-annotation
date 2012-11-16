# Pavel Studeník
# email: pavelstudenik@gmail.com

CXX = g++ -O2 
LD = g++ -O2 #-DDEBUG=y
LDFLAGS =
CFLAGS=-g -Wall `pkg-config --cflags libfreenect`  -Wall -ltheora -ltheoraenc -logg -ltheoradec `xml2-config --cflags` -export-dynamic `pkg-config --cflags gtk+-2.0 gmodule-export-2.0 gthread-2.0 gmodule-2.0` `pkg-config libfreenect --cflags` `pkg-config opencv --cflags`

PROG = kinect-annotation
OBJECTS = freenect.o gmain.o 
#OBJECTS = freenect.o main.o 

# Kinect library
LIBS = `pkg-config libfreenect --libs` `pkg-config opencv --libs`  -lpthread -lm -lusb-1.0 -lfreenect -Wunknown-pragmas
LIBS +=   -lpthread -lm -lusb-1.0 -L/usr/local/lib64/ -I/usr/local/include/libfreenect/ #./src/cvblobs8.3/libblob.a # no fedora -L/usr/local/lib64/
# GTK library
LIBS += `pkg-config --libs gtk+-2.0 gthread-2.0 gmodule-2.0` -export-dynamic
#LIBS += `pkg-config --cflags --libs gtk+-3.0  `
# XML library
LIBS +=  `xml2-config --libs`

all: $(PROG) 

$(PROG): $(OBJECTS)
	$(LD) $(OBJECTS) -o $(PROG) $(LIBS)

%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(LDFLAGS) -c $< 


clean:
	rm -rf *.o $(CVPROG) $(GLPROG)
