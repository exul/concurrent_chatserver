all: chat.c
	gcc -c server_conn_handling.c -o server_conn_handling.o
	gcc chat.c server_conn_handling.o -o chat
clean:
	rm -f *~ *.bak *.o
