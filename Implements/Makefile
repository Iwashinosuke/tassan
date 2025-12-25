# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = .
OBJ_DIR = obj
LIB_DIR = .

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
ifeq ($(findstring -DDIRECT_READ_MODE,$(CFLAGS)),-DDIRECT_READ_MODE)
	LIBRARY = $(LIB_DIR)/libtassan-imp-direct.a
else
	LIBRARY = $(LIB_DIR)/libtassan-imp.a
endif

# Targets
all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(LIBRARY)

.PHONY: all clean