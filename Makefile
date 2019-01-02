it:
	mkdir -p out
	gcc main.c -o out/tsh

debug:
	mkdir -p out
	gcc main.c -o out/tsh -g -static

clean:
	rm -r out
