struct chat_client {
   int socket; 
   struct linked_list *ll;
   char nickname[50];
   int index;
};

int new_connection (int sfd);
void tcp_read (struct chat_client *chat_client_p);
