# Exercise configuration

OBJS = build/main.o \
       build/meta_list.o \
       build/meta_node.o \
       build/dir_list.o \
       build/dir_node.o \
       build/file.o

BIN = mydiz

# Flags passed to preprocessor, compiler, and linker

CPPFLAGS += -D_XOPEN_SOURCE=700 -iquoteinclude
CFLAGS += -std=c99 -pedantic-errors
ifeq ($(CC), clang)
	CFLAGS += -Weverything -g
else
	CFLAGS += -Wall -Wextra -g3
endif

# Build targets

.PHONY = all clean
all : $(BIN)
clean :
	$(RM) -r build/* mydiz

# Exercise builds

build/main.o : src/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/meta_list.o : src/meta_list.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/meta_node.o : src/meta_node.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/dir_list.o : src/dir_list.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/dir_node.o : src/dir_node.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/file.o : src/file.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN) : $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)
