# clang -std=c11 -xc -Wl,-rpath,/usr/local/lib -I/usr/local/include -lwebgpu_dawn -lglfw3 -L/usr/local/lib -o bin/adpater_info -framework Cocoa -framework QuartzCore -framework Metal -framework IOKit src/adapter_info.c src/utils.c

CC=clang
CFLAGS=-std=c11 -Wl,-rpath,/usr/local/lib -Isrc/ -g
LDFLAGS=-I/usr/local/include -L/usr/local/lib -lwebgpu_dawn -lglfw3
OBJCFLAGS=-x objective-c -framework Cocoa -framework CoreVideo -framework IOKit -framework QuartzCore -framework Metal
FILES=src/demo.c src/utils.c src/glfw3webgpu.c
TARGET=bin/demo

.PHONY: check build run all

check:
	mkdir -p $(CURDIR)/bin

build: check
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJCFLAGS) -o $(TARGET) $(FILES)

run: check
	./$(TARGET)

all: build run
