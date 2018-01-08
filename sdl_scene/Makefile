target=sdl_scene

includes= \
  -I ../geom.lib/ \
  -I /usr/include/freetype2

#objs=$(target).o temple.o pine_tree.o
objs= \
     $(target).o \
     ui.o \
     camera.o \
     scene.o \
     pine_tree.o \
     temple.o \
     tex_terrain.o \
     cnv_model.o \
     image_util.o \
     stb_image.o \
     status_line.o

libs= \
      -L ../geom.lib/ -lgeom \
      -lSDL2 -lftgl -lGLU -lGL -lm

dep_libs= \
	../geom.lib/libgeom.a  

CFLAGS += -Wall -g -O0

$(target): $(objs) $(dep_libs)
	$(CC) -o $@ $(LDFLAGS) $(objs) $(libs)

%.o : %.c
	$(CC) -o $@ $(CFLAGS) $(includes) -c $<

clean :
	-rm $(target) *.o

check : $(target)
	./$(target)
	
