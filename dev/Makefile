CC = gcc
CFLAGS = -Wall -Wextra -O0

# 直で同梱コンパイル(デバッグ用)
TARGET = tester_direct
SOURCES = tester.c ../Implements/implement.c
OBJECTS = $(SOURCES:.c=.o)
LIBS = -I../Implements

# # 静的ライブラリのリンクとして生成(本番時のみ適用)
# TARGET = tester
# SOURCES = tester.c
# OBJECTS = $(SOURCES:.c=.o)
# LIBS = -L. -ltassan-imp

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

debug:
	SOURCES += ../Implements/implement.c
	OBJECTS = $(SOURCES:.c=.o)
	LIBS = -I../Implements
	$(CC) $(CFLAGS) -g -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean