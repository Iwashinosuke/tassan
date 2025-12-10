# TASSAN向け変数

# テストケース1つの最大実行時間(ms)．
# 待機時間がunsigned short intで定義されるため，65335msを超す場合別途改変が必要
maxTestTime := 65335


TFLAGS += $(maxTestTime)

CC := clang
CFLAGS := -O2 -g -fPIC 
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
	$(CC) $(TFLAGS) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(LIB) $(OBJS)