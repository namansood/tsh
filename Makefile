it:
	mkdir -p out
	gcc main.c \
		modules/commands.c \
		modules/common.c \
		modules/input.c \
		-o out/tsh

debug:
	mkdir -p out
	gcc main.c \
	modules/commands.c \
	modules/common.c \
	modules/input.c \
	-o out/tsh \
	-g -static -Wall

clean:
	rm -r out
