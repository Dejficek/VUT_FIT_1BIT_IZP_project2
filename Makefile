CFLAGS = -std=c99 -Wall -Wextra -Werror proj2.c -o proj2 -lm

comp:
	gcc $(CFLAGS)
clean:
	rm proj2
