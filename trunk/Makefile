# Pavel Studeník
# email: pavelstudenik@gmail.com

CXX = g++ -O2 
LD = g++ -O2 #-DDEBUG=y

# program use library gtk
PROG_1 = annotation-gtk
OBJECTS_1 = freenect.o openni.o apps.o config.o gmain.o 

# program use library gtkmm
PROG_2 = annotation-gtkmm
OBJECTS_2 = freenect.o config.o ggmain.o 

# Kinect libfreenect library
LIBS = `pkg-config libfreenect --libs` -lm -lusb-1.0 -lfreenect -Wunknown-pragmas
#LIBS +=   -lpthread -lm -lusb-1.0 #-I/usr/local/include/libfreenect/ # no fedora -L/usr/local/lib64/

# kinect openni library
LIBS += -lOpenNI  -Wunknown-pragmas -Wno-unknown-pragmas

# Opencv library
LIBS += `pkg-config opencv --libs`

# XML library
LIBS +=  `xml2-config --libs` -lpthread 

# GTK library
LIBS_1 += $(LIBS) `pkg-config --libs gtk+-2.0 gthread-2.0 gmodule-2.0` -export-dynamic
LIBS_2 += $(LIBS) `pkg-config --libs gtkmm-3.0`

CFLAGS=-g -Wall `pkg-config --cflags libfreenect`  -Wall -ltheora -ltheoraenc -logg -ltheoradec `xml2-config --cflags` `pkg-config opencv --cflags` -I/usr/include/ni/
CFLAGS_1=$(CFLAGS) -export-dynamic `pkg-config --cflags gtk+-2.0 gmodule-export-2.0 gthread-2.0 gmodule-2.0` 
CFLAGS_2=$(CFLAGS) `pkg-config --cflags gtkmm-3.0` 


#all: $(PROG_2) $(PROG_1)
all: $(PROG_1)


$(PROG_1): $(OBJECTS_1)
	$(LD) $(OBJECTS_1) -o $(PROG_1) $(LIBS_1)

$(PROG_2): $(OBJECTS_2)
	$(LD) $(OBJECTS_2) -o $(PROG_2) $(LIBS_2)

ggmain.o: src/ggmain.cpp
	$(CXX) -c $<  $(CFLAGS_2)

openni.o: src/openni.cpp
	$(CXX) -c $< -Wall `pkg-config opencv --cflags --libs` -I/usr/include/ni/  -Wunknown-pragmas -Wno-unknown-pragmas

%.o: src/%.cpp
	$(CXX) $(CFLAGS_1) -c $< 


clean:
	rm -rf *.o $(PROG_2) $(PROG_1)
