target=temple_scene

includes= \
  -I ../srVertex.lib/ \
  -I ../geom.lib/

#objs=$(target).o temple.o pine_tree.o
objs=$(target).o pine_tree.o fence.o guard_tower.o road.o ui.o camera.o edit_line.o status_line.o temple.o house.o person.o zombie.o

libs= \
      -L ../srVertex.lib/ -lsrVertex \
      -L ../geom.lib/ -lgeom \
      -lglut -lGLU -lGL -lm

dep_libs= \
	../geom.lib/libgeom.a  \
	../srVertex.lib/libsrVertex.a

CFLAGS += -Wall -g -O0

$(target): $(objs) $(dep_libs)
	$(CC) -o $@ $(LDFLAGS) $(objs) $(libs)

%.o : %.c
	$(CC) -o $@ $(CFLAGS) $(includes) -c $<

clean :
	-rm $(target) *.o

check : $(target)
	./$(target)
