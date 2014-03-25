CC = gcc
OBJECTS = adduser gchat_server
ALL:$(OBJECTS)
%:%.c
	$(CC) -o $@ $< -Wall -pthread
.PHENY:clean
clean:
	rm -rf $(OBJECTS) *.o *.db
