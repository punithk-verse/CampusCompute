CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -Isrc
LDLIBS ?= -pthread -lrt

SOURCES = \
	src/main.c \
	src/input.c \
	src/job.c \
	src/fcfs.c \
	src/rr.c \
	src/priority.c \
	src/banker.c \
	src/logger.c \
	src/ipc.c \
	src/worker.c \
	src/resource_limit.c
OBJECTS = $(patsubst src/%.c,build/%.o,$(SOURCES))
TARGET = build/campuscompute

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDLIBS) -o $@

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
