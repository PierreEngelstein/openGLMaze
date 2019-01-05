CXX = g++    # The c++ compiler
LD = g++     # The c++ linker

CFLAGS = -Wall   #Enables all the warning errors
LIBDIR = -L/usr/include/glm -L/usr/lib  #The library include directories
#The files needed by the linker
LIB = 	/usr/lib/x86_64-linux-gnu/libGL.so   \
	/usr/lib/x86_64-linux-gnu/libGLU.so  \
	/usr/lib/x86_64-linux-gnu/libglut.so
LDFLAGS = -lGL -lX11   #Linker flags

all: debug release
clean: clean_debug clean_release

#-------------------------------------#
#---- The Debug compilation setup ----#
#-------------------------------------#
INC_DEBUG = -Iinclude
CFLAGS_DEBUG = $(CFLAGS) -g
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = ../obj/Debug
DEP_DEBUG = 
OUT_DEBUG = ../Debug/openGL

OBJ_DEBUG = $(OBJDIR_DEBUG)/main.o\
	    $(OBJDIR_DEBUG)/maze.o\
	    $(OBJDIR_DEBUG)/lodePNG.o

debug: before_debug out_debug after_debug

before_debug: 
	test -d ../Debug || mkdir -p ../Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

after_debug:
	$(OUT_DEBUG)

$(OBJDIR_DEBUG)/%.o: %.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $< -o $@

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf ../Debug
	rm -rf $(OBJDIR_DEBUG)

#---------------------------------------#
#---- The Release compilation setup ----#
#---------------------------------------#
INC_RELEASE = -Iinclude
CFLAGS_RELEASE = $(CFLAGS) -O2
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../obj/Release
DEP_RELEASE = 
OUT_RELEASE = ../Release/maze

OBJ_RELEASE = $(OBJDIR_RELEASE)/main.o\
	      $(OBJDIR_RELEASE)/maze.o\
	      $(OBJDIR_RELEASE)/lodePNG.o
release: before_release out_release after_release

before_release: 
	test -d ../Release || mkdir -p ../Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

after_release:
	$(OUT_RELEASE)

$(OBJDIR_RELEASE)/%.o: %.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $< -o $@

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf ../Release
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release