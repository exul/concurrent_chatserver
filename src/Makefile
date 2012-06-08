all: chat.c
	gcc -Wall -Werror -g -lpthread -c server_conn_handling.c -o server_conn_handling.o
	gcc -Wall -Werror -g -lpthread -c linked_list.c -o linked_list.o
	gcc -Wall -Werror -g -lpthread -c message_handling.c -o message_handling.o
	gcc -Wall -Werror -g -lpthread -c commands.c -o commands.o
	gcc -Wall -Werror -g -lpthread -c helper.c -o helper.o
	gcc -Wall -Werror -g -lpthread chat.c server_conn_handling.o linked_list.o message_handling.o commands.o helper.o -o chat
clean:
	rm -f *~ *.bak *.o
