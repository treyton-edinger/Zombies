libname=geom
target=lib$(libname).a

objs=$(libname).o

CFLAGS += -Wall -g -O0

$(target): $(objs)
	ar -rs $@ $(objs)

%.o : %.c
	$(CC) -o $@ $(CFLAGS) $(includes) -c $<

clean :
	-rm $(target) *.o

