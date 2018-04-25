EXECUTABLE = ../findit
OBJS = main.o findit.o pnmgrf.o pnmio.o pnmerr.o
SRCS = main.c findit.c pnmgrf.c pnmio.c pnmerr.c
RM = rm -f
CFLAGS = -O2 -g -Wall
LDFLAGS = -g
LIBS = -lm

default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

main.o: main.c pnmimg.h

findit.o: findit.c pnmimg.h

pnmgrf.o: pnmgrf.c pnmimg.h

pnmio.o: pnmio.c pnmimg.h

pnmerr.o: pnmerr.c pnmimg.h

clean:
	$(RM) $(EXECUTABLE) 
	$(RM) a.out *.o core *.core gmon.out mon.out result.ppm *~ \#*
