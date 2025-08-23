CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wvla
LDFLAGS = -shared
VPATH = src src/algorithm src/utils

TARGET_LIB = libmalloc.so

# trouve automatiquement tous les .c dans src et sous-dossiers
SRCS := $(shell find src -name '*.c')
OBJS := $(SRCS:.c=.o)

# detection OS pour rpath
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    RPATH_FLAG = -Wl,-rpath,'$$ORIGIN/../'
endif
ifeq ($(UNAME_S),Darwin)
    RPATH_FLAG = -Wl,-rpath,@executable_path/..
endif

all: library test

# compilation de la librairie
library: $(TARGET_LIB)

$(TARGET_LIB): CFLAGS += -pedantic -fvisibility=hidden -fPIC
$(TARGET_LIB): LDFLAGS += -Wl
$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Compile et lie les tests automatiquement
TEST_SRCS := $(wildcard tests/*.c)
TEST_BINS := $(TEST_SRCS:tests/%.c=tests/%)

test: $(TEST_BINS)

tests/%: tests/%.c $(TARGET_LIB)
	$(CC) $(CFLAGS) -o $@ $< -L. -lmalloc $(RPATH_FLAG)

# Lance tous les tests
check: test
	echo Starting tests...	
	for bin in $(TEST_BINS); do \
		./$$bin; \
	done


debug: CFLAGS += -g
debug: clean $(TARGET_LIB)

clean:
	$(RM) $(TARGET_LIB) $(OBJS)
	$(RM) $(TEST_BINS)

.PHONY: all $(TARGET_LIB) clean test run

