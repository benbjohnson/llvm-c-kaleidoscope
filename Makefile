################################################################################
# Variables
################################################################################

CFLAGS=-g -Wall -Wextra -Wno-self-assign -std=c99
CXXFLAGS=-g -Wall -Wextra -Wno-self-assign

LEX_SOURCES=$(wildcard src/*.l) 
LEX_OBJECTS=$(patsubst %.l,%.c,${LEX_SOURCES}) $(patsubst %.l,%.h,${LEX_SOURCES})

YACC_SOURCES=$(wildcard src/*.y) 
YACC_OBJECTS=$(patsubst %.y,%.c,${YACC_SOURCES}) $(patsubst %.y,%.h,${YACC_SOURCES})

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,${SOURCES}) $(patsubst %.l,%.o,${LEX_SOURCES}) $(patsubst %.y,%.o,${YACC_SOURCES})
LIB_SOURCES=$(filter-out kaleidoscope.c,${SOURCES})
LIB_OBJECTS=$(filter-out kaleidoscope.o,${OBJECTS})
TEST_SOURCES=$(wildcard tests/*_tests.c)
TEST_OBJECTS=$(filter-out tests/codegen_tests,$(patsubst %.c,%,${TEST_SOURCES}))

LEX=flex
YACC=bison
YFLAGS=-dv

LLVM_CC_FLAGS=`llvm-config --cflags`
LLVM_LINK_FLAGS=`llvm-config --libs --cflags --ldflags core analysis executionengine jit interpreter native`

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

src/kaleidoscope.c: ${LEX_OBJECTS}

src/kaleidoscope.o: src/kaleidoscope.c
	${CC} ${LLVM_CC_FLAGS} ${CFLAGS} -c -o $@ $^

build/kaleidoscope: ${OBJECTS}
	$(CXX) $(LLVM_LINK_FLAGS) $(CXXFLAGS) -rdynamic -Isrc -o $@ src/kaleidoscope.o build/libkaleidoscope.a
	chmod 700 $@

build:
	mkdir -p build


################################################################################
# Bison / Flex
################################################################################

src/lexer.c: src/parser.c
	${LEX} --header-file=src/lexer.h -o $@ src/lexer.l

src/parser.c: src/parser.y
	mkdir -p build/bison
	${YACC} ${YFLAGS} --report-file=build/bison/report.txt -o $@ $^


################################################################################
# LLVM
################################################################################

src/codegen.o: src/codegen.c
	${CC} ${LLVM_CC_FLAGS} ${CFLAGS} -c -o $@ $^


################################################################################
# Tests
################################################################################

.PHONY: test
test: $(TEST_OBJECTS) build/tests/codegen_tests
	@sh ./tests/runtests.sh

build/tests:
	mkdir -p build/tests

$(TEST_OBJECTS): %: %.c build/tests build/libkaleidoscope.a
	$(CC) $(CFLAGS) -Isrc -o build/$@ $< build/libkaleidoscope.a

build/tests/codegen_tests.o: tests/codegen_tests.c build/libkaleidoscope.a
	$(CC) $(LLVM_CC_FLAGS) $(CFLAGS) -Isrc -c -o $@ tests/codegen_tests.c build/libkaleidoscope.a

build/tests/codegen_tests: build/tests/codegen_tests.o build/libkaleidoscope.a
	$(CXX) $(LLVM_LINK_FLAGS) $(CXXFLAGS) -Isrc -o $@ build/tests/codegen_tests.o build/libkaleidoscope.a


################################################################################
# Clean up
################################################################################

clean: 
	rm -rf kaleidoscope ${OBJECTS} ${LEX_OBJECTS} ${YACC_OBJECTS}
