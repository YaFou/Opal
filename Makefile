SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,target/%.o,$(SRCS))
EXE := target/opal.exe

target:
	mkdir target

$(EXE): target $(OBJS)
	gcc -o $@ $(OBJS) -Wall

target/%.o: src/%.c
	gcc $< -o $@ -c

.PHONY: build
build: $(EXE)

.PHONY: clean
clean:
	rm -rf target
