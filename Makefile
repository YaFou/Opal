SRCS := src/debug.c \
	src/error.c \
	src/main.c \
	src/memory.c \
	src/module.c \
	src/scan.c \
	src/util.c

OBJS := $(patsubst src/%.c,target/%.o,$(SRCS))
EXE := target/opal

.SILENT:

target:
	mkdir target

tmp:
	mkdir tmp

$(EXE): target tmp $(OBJS)
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
