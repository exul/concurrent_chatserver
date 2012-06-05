all: chat.c
	gcc -Wall -g -lpthread -c server_conn_handling.c -o server_conn_handling.o
	gcc -Wall -g -lpthread -c linked_list.c -o linked_list.o
	gcc -Wall -g -lpthread -c message_handling.c -o message_handling.o
	gcc -Wall -g -lpthread chat.c server_conn_handling.o linked_list.o message_handling.o -o chat
clean:
	rm -f *~ *.bak *.o
