# AFLplusplusのパスは適宜変更してください
CC := clang
CFLAGS := -O3 -g -fPIC -I /AFLplusplus/include
LDFLAGS := -shared
SRCS := *.c
OBJS := $(SRCS:.c=.o)
LIB := libtassan-mutator.so

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	@rm -f $(OBJS)
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(LIB) $(OBJS)