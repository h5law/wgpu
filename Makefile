# clang -std=c11 -xc -Wl,-rpath,/usr/local/lib -I/usr/local/include -lwebgpu_dawn -lglfw3 -L/usr/local/lib -o bin/adpater_info -framework Cocoa -framework QuartzCore -framework Metal -framework IOKit src/adapter_info.c src/utils.c

CC=clang
CFLAGS=-Wl,-rpath,/usr/local/lib -Isrc/ -DGLFW_EXPOSE_NATIVE_COCOA
LDFLAGS=-I/usr/local/include -L/usr/local/lib -lwgpu_native -lglfw3
FRAMEWORKS=-x objective-c -framework Cocoa -framework QuartzCore -framework Metal -framework IOKit
FILES=src/demo.c src/utils.c
TARGET=bin/demo

.PHONY: check build run all

check:
	mkdir -p $(CURDIR)/bin

build: check
	$(CC) $(CFLAGS) $(LDFLAGS) $(FRAMEWORKS) -o $(TARGET) $(FILES)

run: check
	./$(TARGET)

all: build run
