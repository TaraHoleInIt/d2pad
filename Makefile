# Reference source https://bigcode.wordpress.com/2016/12/20/compiling-a-very-basic-mingw-windows-hello-world-executable-in-c-with-a-makefile/

CC=i686-w64-mingw32-gcc
CFLAGS=-g -m32 -D_DEBUG -Wall -Werror -static
LDFLAGS=-mdll -g -static -Bstatic
LIBS=-lxinput
OUTPUT_FILE=version.dll

srcFiles = $(wildcard src/*.c)
objFiles = $(srcFiles:.c=.o)

all: $(OUTPUT_FILE)

$(OUTPUT_FILE): $(objFiles)
	$(CC) $(CFLAGS) -o $(OUTPUT_FILE) $^ $(LDFLAGS) src/d2pad.def $(LIBS)

.PHONY: clean

clean:
	rm -f $(objFiles)
	rm -f $(OUTPUT_FILE)
