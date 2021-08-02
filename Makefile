SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,target/%.o,$(SRCS))
EXE := target/opal

.SILENT:

target:
	mkdir target

$(EXE): target $(OBJS)
	echo "Compiling executable..."
	gcc -o $@ $(OBJS) -Wall

target/%.o: src/%.c
	echo "Compiling $@ from $<..."
	gcc $< -o $@ -c

.PHONY: build
build: $(EXE)

.PHONY: clean
clean:
	rm -rf target

test: $(EXE)
	(./tests/run)
