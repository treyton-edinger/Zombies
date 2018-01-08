libname=srVertex
target=lib$(libname).a

includes=-I../geom.lib
objs=$(libname).o

CFLAGS += -Wall -g -O0 $(includes)

$(target): $(objs)
	ar -rs $@ $(objs)

%.o : %.c
	$(CC) -o $@ $(CFLAGS) $(includes) -c $<

clean :
	-rm $(target) *.o

