# Builds all files for the project

# Common options to all C files
CFLAGS = -Wall -g -MMD # All warnings, with debug, generate .d files

# The list of targets to make
all : client

# The Recipe to make each target
client : readPNG.o display.o client.c
	gcc $^ -lpthread -o $@ -lpng -lSDL2

sujet.pdf : sujet.dvi
	dvipdf $^

sujet.dvi : sujet.tex logoInfo.eps
	latex sujet.tex
	latex sujet.tex

#---------------------------------------------------------------------------------

%.o : %.c
	gcc -c ${CFLAGS} $<

Phony : clean

clean :
	rm -f *~ *.d *.o *.aux *.dvi *.log *.out *.ps pdfa.xmpi

-include *.d
