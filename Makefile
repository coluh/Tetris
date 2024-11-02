
CFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_mixer -lm
CFLAGS += -Werror -Wall
CFLAGS += -g -ggdb -rdynamic

sources := $(shell find src/ -name "*.c" | grep -v "test_")
objects := $(sources:.c=.o)
target := build/tetris.out

.PHONY: all run test clean
all: $(target)

%.o: %.c
	gcc -c $< -o $@ $(CFLAGS)

$(target): $(objects)
	gcc $^ -o $@ $(CFLAGS)

run: $(target)
	./$(target)

tests := $(shell find src/ -name "*.c" | grep "test_")
replace := $(shell echo $(tests) | sed 's/test_//g')
remains := $(filter-out $(replace),$(sources))
test_sources := $(tests) $(remains)
test_objects := $(test_sources:.c=.o)

test: $(test_objects)
	gcc $^ -o build/test.out $(CFLAGS)
	./build/test.out

clean:
	rm -f $(objects) $(test_objects) ./build/*


