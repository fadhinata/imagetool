libbgi.a: drawing.o misc.o mouse.o palette.o text.o winbgi.o winthread.o file.o dibutil.o winbgi.h winbgim.h graphics.h
	ar -rv libbgi.a drawing.o misc.o mouse.o palette.o text.o winthread.o winbgi.o file.o dibutil.o

clean:
	rm -f *.o 
	rm -f *.exe 
	rm -f libbgi.a

drawing.o: drawing.cxx winbgi.h winbgitypes.h
	g++ -c drawing.cxx

misc.o: misc.cxx winbgi.h winbgitypes.h
	g++ -c misc.cxx

mouse.o: mouse.cxx winbgi.h winbgitypes.h
	g++ -c mouse.cxx

palette.o: palette.cxx winbgi.h winbgitypes.h
	g++ -c palette.cxx

text.o: text.cxx winbgi.h winbgitypes.h
	g++ -c text.cxx

winbgi.o: winbgi.cxx winbgi.h winbgitypes.h
	g++ -c winbgi.cxx

winthread.o: winthread.cxx winbgi.h winbgitypes.h
	g++ -c winthread.cxx

dibutil.o: dibutil.c dibutil.h dibapi.h
	g++ -c dibutil.c

file.o: file.c dibutil.h dibapi.h winbgitypes.h
	g++ -c file.c

graphics.h: winbgi.h
	rm -f graphics.h
	cp winbgi.h graphics.h

winbgim.h: winbgi.h
	rm -f winbgim.h
	cp winbgi.h winbgim.h

bgi.exe: bgi.cxx
	g++ bgi.cxx -o bgi

