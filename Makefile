################################################################################
# Variables
################################################################################

CFLAGS=-g -Wall -Wextra -Wno-self-assign -std=c99

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,${SOURCES})
LIB_SOURCES=$(filter-out kaleidoscope.c,${SOURCES})
LIB_OBJECTS=$(filter-out kaleidoscope.o,${OBJECTS})
TEST_SOURCES=$(wildcard tests/*_tests.c)
TEST_OBJECTS=$(patsubst %.c,%,${TEST_SOURCES})


################################################################################
# Default Target
################################################################################

all: build/libkaleidoscope.a build/kaleidoscope ${OBJECTS} test


################################################################################
# Binaries
################################################################################

build/libkaleidoscope.a: build ${LIB_OBJECTS}
	rm -f build/libkaleidoscope.a
	ar rcs $@ ${LIB_OBJECTS}
	ranlib $@

build/kaleidoscope: ${OBJECTS}
	$(CC) $(CFLAGS) src/kaleidoscope.o -o $@ build/libkaleidoscope.a
	chmod 700 $@

build:
	mkdir -p build

################################################################################
# Tests
################################################################################

.PHONY: test
test: $(TEST_OBJECTS)
	@sh ./tests/runtests.sh

build/tests:
	mkdir -p build/tests

$(TEST_OBJECTS): %: %.c build/tests build/libkaleidoscope.a
	$(CC) $(CFLAGS) -Isrc -o build/$@ $< build/libkaleidoscope.a

################################################################################
# Clean up
################################################################################

clean: 
	rm -rf kaleidoscope ${OBJECTS}
