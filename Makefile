PEBBLE := pebble
PEBBLE_FLAGS :=
PEBBLE_DEBUG_FLAGS := --debug
PEBBLE_RELEASE_FLAGS :=
TEST_CFLAGS := -g3 -O0 $(shell pkg-config --cflags cmocka)
TEST_LDFLAGS := -g3 -O0
TEST_LIBS := $(shell pkg-config --libs cmocka)
VALGRIND := valgrind

.PHONY: all clean build-clean test test-clean

all: build/c25k-pebble.pbw

release:
	$(PEBBLE) build $(PEBBLE_FLAGS) $(PEBBLE_RELEASE_FLAGS)

build/c25k-pebble.pbw: $(wildcard src/c/*.[ch]) package.json wscript
	$(PEBBLE) build $(PEBBLE_FLAGS) $(PEBBLE_DEBUG_FLAGS)

TEST_TARGETS := tests/programme

test: $(patsubst %,%.out,$(TEST_TARGETS))

tests/%.out: tests/%
	$(VALGRIND) --leak-check=full $<

tests/%: tests/%.o $(wildcard src/c/*.[ch])
	$(CC) -o $@ $(TEST_LDFLAGS) $< $(TEST_LIBS)

tests/%.o: tests/%.c $(wildcard tests/*.h)
	$(CC) -Itests $(TEST_CFLAGS) -o $@ -c $<

clean: build-clean test-clean

build-clean:
	rm -rf build

test-clean:
	rm -f $(TEST_TARGETS) tests/*.o
	rm -rf tests/lib
