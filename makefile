digit_display: main.c
	clang -o digit_display -msoft-float main.c

all: digit_display

clean:
	rm -rf digit_display
