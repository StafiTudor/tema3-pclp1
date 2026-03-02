# 314CA Stafi Tudor
# compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

# define targets
TARGETS = image_editor

build: $(TARGETS)

image_editor: image_editor.o
	$(CC) $(CFLAGS) -o image_editor image_editor.o $(LDFLAGS)

image_editor.o: image_editor.c
	$(CC) $(CFLAGS) -c image_editor.c

pack:
	zip -FSr 314CA_StafiTudor-Vasile_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS) *.o

.PHONY: build pack clean
