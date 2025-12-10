CC = gcc
CFLAGS = -Wall -Wextra
TARGET = tester
SOURCES = tester.c
OBJECTS = $(SOURCES:.c=.o)
LIBS = -L. -ltassan-imp

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean