CC = gcc
OBJECTS = client server

ALL:$(OBJECT)
%:%.c
	$(CC) -o $@ $< -Wall
.PHONY:clean
clean:
	rm -rf $(OBJECT) *.o
