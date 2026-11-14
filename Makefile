
CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Isrc
LDLIBS = -pthread -lrt

SOURCES = src/main.c \
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

OBJECTS = $(SOURCES:src/%.c=build/%.o)
TARGET = build/campuscompute

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

build:
	mkdir -p build

build/%.o: src/%.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
