SRCS := src/error.c \
	src/main.c \
	src/memory.c \
	src/module.c \
	src/parse.c \
	src/scan.c \
	src/types.c \
	src/util.c

EXE := target/opal
DEBUG := 0
CFLAGS :=

ifeq ($(DEBUG),1)
	CFLAGS := $(CFLAGS) -D _DEBUG
	SRCS := $(SRCS) src/debug.c
endif

OBJS := $(patsubst src/%.c,target/%.o,$(SRCS))

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
	gcc -c $(CFLAGS) $< -o $@

.PHONY: build
build: $(EXE)

.PHONY: clean
clean:
	rm -rf target

test: $(EXE)
	(./tests/run)
