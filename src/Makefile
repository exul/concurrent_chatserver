all: chat.c
	gcc -lpthread -c server_conn_handling.c -o server_conn_handling.o
	gcc -lpthread -c linked_list.c -o linked_list.o
	gcc -lpthread chat.c server_conn_handling.o linked_list.o -o chat
clean:
	rm -f *~ *.bak *.o
