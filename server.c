/* Simple program to demonstrate a basic UDP client.
 * Asisgnment 3 - Server for the Octoput
 * CPSC441
 * Author: Sidney Shane Dizon
 * UCID: 10149277
 *
 *Links for References:
 * http://www.sanfoundry.com/c-program-size-file-handling/
 * https://www.codingunit.com/c-tutorial-file-io-using-text-files
 * https://www.tutorialspoint.com/cprogramming/c_file_io.htm
 * https://stackoverflow.com/questions/4214314/get-a-substring-of-a-char
 */

 #include <time.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <netinet/in.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/uio.h>

 #define MAX_BUF_LEN 512
 #define PORTRANGE_MIN 8000
 #define MAX_NOUNS 5
 #define MAX_ADJS 8
 #define MAX_VERBS 6
 #define MAX_DUMDUMS 3
 #define MAX_LENGTH 10
 #define PORT 8888

int main(){
  //creater variables to be used
  struct sockaddr_in si_server, si_client;
  struct sockaddr *server, *client;
  int sock_id, i, len=sizeof(si_server);
  char client_message[MAX_BUF_LEN];
  char ack_message[MAX_BUF_LEN];
  int readBytes, ack_check;

  //Set up the socket for the Octoput Server
  if ((sock_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    printf("Could not setup the socket for the Octoput Server.\n\n");
    return 1;
  } else {
    printf("Finish Socket set-up.\n");
  }

  //Create Bindings
  memset((char *) &si_server, 0, sizeof(si_server));
  si_server.sin_family = AF_INET;
  si_server.sin_port = htons(PORT);
  si_server.sin_addr.s_addr = htonl(INADDR_ANY);
  server = (struct sockaddr *) &si_server;
  client = (struct sockaddr *) &si_client;
  if (bind(sock_id, server, sizeof(si_server)) == -1){
    printf("Could not bind to port %d!\n\n", PORT);
    return 1;
  } else {
    printf("Bindings created.\n");
  }

  //Variables for doing certain processes
  int quit = 0;
  int send_file = 0;
  //Start looping so that we could get continuous Requests
  while (!quit){
    //Get and read the request of the OCtoput client
    bzero(client_message, MAX_BUF_LEN);
    if ((readBytes = recvfrom(sock_id, client_message, MAX_BUF_LEN, 0, client, &len)) == -1){
      printf("Reading the Octoput clients request failed.\n\n");
      return 1;
    } else {
    client_message[readBytes] = '\0'; // padding with end of string symbol
    printf("Server received command \"%s\" from client %s on port %d\n\n", client_message, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));
    }

    //check if we were requested to quit the Octoput server
    if (strcmp(client_message, "quit") == 0){
      quit = 1;
      printf("quit received.\n");
    }

    //check if we are getting the request for the list of available files that can be sent to the client
    else if (strcmp(client_message, "get") == 0){
      printf("get received.\n");
      //Send the filenames of the list of available files to the client
      char list[] = "The available files are:\n736test\n739test\n8888test\n1ktest\n2ktest\n4ktest\n8ktest\n32ktest\n256ktest\ntest1\n";
      if (sendto(sock_id, list, strlen(list), 0, client, len) == -1){
        printf("Sending valid files to the Octoput client failed.\n\n");
        return 1;
      } else {
        printf("sent to client: %s\n", list);
      }
    }//Check if we have received a request for any of the available files
    else if ((strcmp(client_message, "736test") == 0) || (strcmp(client_message, "739test") == 0) ||
            (strcmp(client_message, "8888test") == 0) || (strcmp(client_message, "1ktest") == 0) ||
            (strcmp(client_message, "2ktest") == 0) || (strcmp(client_message, "4ktest") == 0) ||
            (strcmp(client_message, "8ktest") == 0) || (strcmp(client_message, "32ktest") == 0) ||
            (strcmp(client_message, "256ktest") == 0) || (strcmp(client_message, "test1") == 0)){
      printf("valid request received.\n");
      //Send the requested available file
      sprintf(ack_message, "%s", "Valid request. Server is sending the files.");
      if (sendto(sock_id, ack_message, strlen(ack_message), 0, client, len) == -1){
        printf("Sending valid ack to the Octoput client failed.\n\n");
        return 1;
      } else {
        //This is a flag that a file will be sent to the octoput client
        send_file = 1;
      }
    }//we have received an invalide request
    else{
      char invalid_request[] = "The file you have requested is not on the list. Please try again.";
      if (sendto(sock_id, invalid_request, strlen(invalid_request), 0, client, len) == -1) {
        printf("Sending valid files to the Octoput client failed.\n\n");
        return 1;
      } else {
        printf("invalid request recieved.\n");
      }
    }
    //check if we are stopping the program
    if (quit == 1){
      //send quit ACK to the client
      if (sendto(sock_id, ack_message, strlen(ack_message), 0, client, len) == -1){
        printf("Sending quit ack to the Octoput client failed.\n\n");
        return 1;
      } else {
        sprintf(ack_message, "%s", "OK");
      }
    }

    //reading and sending the file starts here
    if (send_file == 1) {
      //set the file to be opened
      char filename[] = "";
      strcat(filename, client_message);
      strcat(filename, ".txt");
      printf("filename is: %s\n", filename);
      //Opening a file for reading
      FILE *ptr_file, *ptr_size;
      ptr_file = fopen(filename, "r");
      if (!ptr_file){
        printf("Error in reading the file\n");
        return 1;
      } else {
        printf("%s is open for reading.\n", filename);
      }

      //Do another so that we could get the size of the file
      ptr_size = fopen(filename, "r");
      if (!ptr_file){
        printf("Error in ptr_size.\n");
        return 1;
      }
      //Find the size of the file
      fseek(ptr_size, 0 ,2); //file pointer at the end of the file
      int size = ftell(ptr_size); //take a position of the file pointer in size variable
      printf("The size of %s is: %d\n\n", filename, size);
      fclose(ptr_size);

      //reading through the file line by line and storing them to file[]
      char file_content[1200];
      char file[400000]; //400,000 bytes should be ok
      bzero(file, MAX_BUF_LEN); //clear the buffer
      //printf("It goes here\n");
      //store the file to a buffer so that we could send segments by segments, 1200 maximum per line it could read
      while (fgets(file_content, 1200, ptr_file) != NULL){
        //printf("Loop %d : %s", i, file_content);
        strcat(file, file_content);
      }
      //Print for checking
      //printf("\nThe size of the string{file} is: %ld\n", strlen(file));
      //printf("file is:\n%s\n", file);

      //Start dividing the file into segments to fullfil OCTOPUT
      int octo_leg;
      int octo_block = size;
      int start = 0;
      //send as much full octo block possible
      while (octo_block > 8888){
          octo_leg = 1111; //max octo_leg is 1111 bytes
          printf("Segments are by: %d\n", octo_leg);
          char send_octo_leg[1200]; // we can send max of 1111 bytes so 1200 is enough
          //send 8 octo legs for the full octo block
          for (int i = 0; i < 8; i++){
            bzero(send_octo_leg, 1200); // clear the buffer first
            strncpy(send_octo_leg, file+start, octo_leg);
            //put sequence number for each octoleg
            strcat(send_octo_leg, " ");
            char seq[3];
            sprintf(seq, "%d" ,i);
            strcat(send_octo_leg, seq);
            //printf("send_octo_leg: \n%s\n", send_octo_leg);
            if (sendto(sock_id, send_octo_leg, strlen(send_octo_leg), 0, client, len) == -1) {
              printf("Sending full_Octo_block to the Octoput client failed.\n\n");
              return 1;
            } else {
              start = start + octo_leg;
              printf("%ld full_Octo_block bytes sent to client.\n", strlen(send_octo_leg));
              bzero(send_octo_leg, strlen(send_octo_leg));
            }
          }

          //send file sent ACK to the client
          char sent_message[100];
          bzero(sent_message, strlen(sent_message));
          strcat(sent_message, "Has been succesfully sent.");
          if (sendto(sock_id, sent_message, strlen(sent_message), 0, client, len) == -1 ) {
            printf("Sending ACK to the Octoput Server failed.\n\n");
            return 1;
          } else {
            printf("sent to client:\n%s\n", sent_message);
            bzero(sent_message, strlen(sent_message));
            octo_block = octo_block - 8888;//subtract the 8888 bytes we have sent
          }

          //Wait for the ACk message
          bzero(ack_message, strlen(ack_message));
          if ((ack_check = recvfrom(sock_id, ack_message, MAX_BUF_LEN, 0, client, &len)) == -1){
            printf("Reading the Octoput clients ACK failed.\n\n");
            return 1;
          } else {
            ack_message[ack_check] = '\0'; // padding with end of string symbol
            printf("Server received ack: \"%s\" from client %s on port %d\n\n", ack_message, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));
            bzero(ack_message, strlen(ack_message));
          }
      }

      //check for remainder
      printf("The partial_octo_block bytes are: %d\n", octo_block);
      //send a partial octo block if octo_block > 8
      if (octo_block > 8){
        int partial_octo_block = octo_block - (octo_block % 8);
        printf("The partial_octo_block is: %d\n", partial_octo_block);
        octo_leg = partial_octo_block / 8;
        printf("The octo_leg for partial_octo_block is: %d\n", octo_leg);
        char send_partial_octo_leg[1200]; // we can send at most 1111 bytes so 1200 is enough

        //send 8 octo legs for the partial_octo_block
        for (int i = 0; i < 8; i++){
          bzero(send_partial_octo_leg, 1200); // clear the buffer first
          strncpy(send_partial_octo_leg, file+start, octo_leg);
          //put sequence number for each octoleg
          strcat(send_partial_octo_leg, " ");
          char seq[3];
          sprintf(seq, "%d" ,i);
          strcat(send_partial_octo_leg, seq);
          //printf("send_partial_octo_leg: \n%s\n", send_partial_octo_leg);
          printf("%ld partial_octo_block bytes are sent to client.\n", strlen(send_partial_octo_leg));
          if (sendto(sock_id, send_partial_octo_leg, strlen(send_partial_octo_leg), 0, client, len) == -1) {
            printf("Sending ACK to the Octoput Server failed.\n\n");
            return 1;
          } else {
            start = start + octo_leg;
            bzero(send_partial_octo_leg, strlen(send_partial_octo_leg));
          }
        }

        //send file sent ACK to the client
        char sent_message[100];
        bzero(sent_message, strlen(sent_message));
        strcat(sent_message, "Has been succesfully sent.");
        if (sendto(sock_id, sent_message, strlen(sent_message), 0, client, len) == -1 ) {
          printf("Sending ACK to the Octoput Server failed.\n\n");
          return 1;
        } else {
          printf("sent to client:\n%s\n", sent_message);
          bzero(sent_message, strlen(sent_message));
        }
        //Wait for the ACk message
        bzero(ack_message, strlen(ack_message));
        if ((ack_check = recvfrom(sock_id, ack_message, MAX_BUF_LEN, 0, client, &len)) == -1){
          printf("Reading the Octoput clients ACK failed.\n\n");
          return 1;
        } else {
          ack_message[ack_check] = '\0'; // padding with end of string symbol
          printf("Server received ack: \"%s\" from client %s on port %d\n\n", ack_message, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));
        }

        //do things based from the ACK message

        bzero(ack_message, strlen(ack_message));
        octo_block = octo_block - partial_octo_block;

      }
      //check for the tiny octo_block
      printf("The tiny_octo_block bytes are: %d\n", octo_block);
      //send tiny octo block if octo_block > 0 and add padding when necessary
      if (octo_block > 0){
        int non_paddings = octo_block;
        int paddings = 8 - non_paddings;
        //printf("Non_paddings are: %d\nPaddings are: %d\n", non_paddings, paddings);
        char pad_bytes[] = "  ";
        int tiny_octo_block = 8;
        octo_leg = tiny_octo_block / 8;
        printf("The octo_leg for the tiny_octo_block is: %d\n", octo_leg);


        /* send 8 octo legs for the tiny octoblock */
        //send the remainder of the file
        for (int i = 0; i < non_paddings; i++){
          char send_tiny_octo_leg[10]; //we can send at most 1 byte so 10 is enough
          bzero(send_tiny_octo_leg, strlen(send_tiny_octo_leg));
          strncpy(send_tiny_octo_leg, file+start, octo_leg);
          //put sequence number for each octoleg
          strcat(send_tiny_octo_leg, " ");
          char seq[3];
          sprintf(seq, "%d" ,i);
          strcat(send_tiny_octo_leg, seq);
          //printf("send_tiny_octo_leg: \n%s\n", send_tiny_octo_leg);
          printf("%ld tiny_octo_block bytes are sent to client.\n", strlen(send_tiny_octo_leg));
          if (sendto(sock_id, send_tiny_octo_leg, strlen(send_tiny_octo_leg), 0, client, len) == -1) {
            printf("Sending tiny_octo_block to the Octoput Server failed.\n\n");
            return 1;
          } else {
            start = start + octo_leg;
            //printf("Start at this point is: %d\n", start);
            bzero(send_tiny_octo_leg, 10);// clear the entire buffer first
            printf("Sent to client: %s\n", send_tiny_octo_leg);
          }
        }

        //send paddings for remainder of the octo_leg
        for (int i = 0; i < paddings; i++){
          //put sequence number for each octoleg
          char pad_seq[10];
          bzero(pad_seq, 10);
          strcat(pad_seq, pad_bytes);
          char seq[3];
          sprintf(seq, "%d" ,non_paddings);
          strcat(pad_seq, seq);
          printf("pad_seq: %s\n", pad_seq);
          if (sendto(sock_id, pad_seq, strlen(pad_seq), 0, client, len) == -1) {
            printf("Sending tiny_octo_block to the Octoput Server failed.\n\n");
            return 1;
          } else {
            //sendto(sock_id, pad_bytes, strlen(pad_bytes), 0, client, len);
            //printf("The pad \"%s\" was sent to client.\n", pad_bytes);
            bzero(pad_seq, 10);
            non_paddings++;
          }
        }
        //send file sent ACK to the client
        char sent_message[100];
        bzero(sent_message, strlen(sent_message));
        strcat(sent_message, "Has been succesfully sent.");
        if (sendto(sock_id, sent_message, strlen(sent_message), 0, client, len) == -1 ) {
          printf("Sending ACK to the Octoput Server failed.\n\n");
          return 1;
        } else {
          printf("sent to client:\n%s\n", sent_message);
          bzero(sent_message, strlen(sent_message));
        }

        //Wait for the ACk message
        bzero(ack_message, strlen(ack_message));
        if ((ack_check = recvfrom(sock_id, ack_message, MAX_BUF_LEN, 0, client, &len)) == -1){
          printf("Reading the Octoput clients ACK failed.\n\n");
          return 1;
        } else {
          ack_message[ack_check] = '\0'; // padding with end of string symbol
          printf("Server received ack: \"%s\" from client %s on port %d\n\n", ack_message, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));
        }

        //clear ack buffer
        bzero(ack_message, strlen(ack_message));
      }

      //Close the file
      fclose(ptr_file);
      send_file = 0;
      //send file sent ACK to the client
      char all_sent_message[100];
      bzero(all_sent_message, strlen(all_sent_message));
      strcat(all_sent_message, "All are sent.");
      if (sendto(sock_id, all_sent_message, strlen(all_sent_message), 0, client, len) == -1 ) {
        printf("Sending ACK to the Octoput Server failed.\n\n");
        return 1;
      } else {
        printf("sent to client:\n%s\n", all_sent_message);
        bzero(all_sent_message, strlen(all_sent_message));
      }
      start = 0; //restart start to 0 to make sure
      printf("End of the Send file While loop.\n\n");
      send_file = 0;
      bzero(client_message, MAX_BUF_LEN);//clear so that we don't get any problems
    }
  }
  close(sock_id);
  return 0;
}
