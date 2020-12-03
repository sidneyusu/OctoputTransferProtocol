/* Simple program to demonstrate a basic UDP client.
 * Asisgnment 3 - Client for the Octoput
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

/* Standard Libraries */
 #include <stdio.h>
 #include <unistd.h>
 #include <stdlib.h>

 /* Library for parsing Strings */
 #include <string.h>

 /* Libraries for socket programming */
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>

 #define MAX_BUF_LEN 512

 /* Hardcode the IP address of the server (local or remote) */
 #define SERVER_IP "127.0.0.1"   /* loopback interface */
 /* #define SERVER_IP "136.159.5.22"  /* rsx1.cpsc.ucalgary.ca */

 /* Edit as needed to match port of server */
 #define SERVER_PORT 8888

int main(void){
  //Socket Creation
  struct sockaddr_in server;
  struct sockaddr *server_addr;
  int sock_id, i, len = sizeof(server);
  char user_cmd[MAX_BUF_LEN], server_response[MAX_BUF_LEN];
  int response_check;
  int quit, save_file;

  //Set-up the socket to use for the Octoput CLient
  if ((sock_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    printf("Could not setup a socket for the Octoput client.\n\n");
    return 1;
  } else {
    printf("Succesful in set-up the socket\n");
  }
  //Socket Binding
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  server_addr = (struct sockaddr *) &server;
  if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) == 0){
    printf("inet_pton() failed\n\n");
    return 1;
  } else {
    printf("Succesful in Binding.\n\n");
  }
  //Console messages
  printf("This is the client side of the octoput Assignment.\nThe commands are \"get\", \"quit\" and filenames.\n");
  printf("You can request from here the text/ASCII that you want the server to send to you from the list below.\n");
  //Make a connection handshake with the Octoput Server
  char list_message [] = "get"; //Request the List of available files
  if (sendto(sock_id, list_message, strlen(list_message), 0, server_addr, sizeof(server)) == -1){
    printf("Sending connection What to the Octoput server.\n\n");
    return 1;
  }
  //Receive the list of files available
  if ((response_check = recvfrom(sock_id, server_response, MAX_BUF_LEN, 0, server_addr, &len)) == -1){
    printf("Error in reading the list of available files.\n\n");
    quit = 1;
  }
  server_response[response_check] = '\0'; //padidng with end of string symbol
  //print the list of available files from the server
  printf("%s\n\n", server_response);


  quit = 0;
  save_file = 0;
  //Start of the looping for continuous requests and receive data
  while(!quit){
    printf("Please type the command or filename here: ");
    scanf("%s", user_cmd);
    char file_title[MAX_BUF_LEN];
    //save the title of the file for later use
    strcpy(file_title, user_cmd);

    //check if we are quiting the client side of the octoput
    if (strcmp(user_cmd, "quit") == 0){
      quit = 1;
      printf("Closing connection with the Octoput Server.\n");
    }
    //Else send the user request to the server side
    if (sendto(sock_id, user_cmd, strlen(user_cmd), 0, server_addr, sizeof(server)) == -1){
      printf("Sending request to the Octoput Server failed.\n\n");
      return 1;
    } else {
      printf("Command sent to the server.\n");
    }
    //Read the response of the Octoput Server
    bzero(server_response, MAX_BUF_LEN); // clear the buffer
    if((response_check = recvfrom(sock_id, server_response, MAX_BUF_LEN, 0, server_addr, &len)) == -1){
      printf("Error in reading the response of the Octoput Server,\n\n");
      quit = 1;
    } else {
      server_response[response_check] = '\0'; //padding with end of string symbol
      printf("\nResponse received from the server \"%s\" \non port \"%s\" is %d\n\n",server_response, inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    }
    //check if we have recieved a file to be saved
    if (strcmp(server_response, "Valid request. Server is sending the files.") == 0){
      //Flag that we need to save the file being sent
      save_file = 1;
    }

    //Octo files are being sent, make a loop?
    while (save_file){
      //this is the whole content of the file requested
      char file[400000];
      bzero(file, strlen(file));
      int size = 0;
      //Flags for segments of file being received
      int f0 = 0;
      int f1 = 0;
      int f2 = 0;
      int f3 = 0;
      int f4 = 0;
      int f5 = 0;
      int f6 = 0;
      int f7 = 0;
      //When received the "sent succesfully" make the flag of stop waiting for recieving to false
      int stop_receiving = 0;
      while (!stop_receiving){

        //COntinuous receiving
        char received[1200]; //1200 bytes should be enough for receiving, max buffer receive is 1111
        if((response_check = recvfrom(sock_id, received, sizeof(received), 0, server_addr, &len)) == -1){
          printf("Error in receiving the file content.\n\n");
          return 1;
        } else {
          //printf("The file_content received is:\n%s\n", received);
          printf("The size received is: %d\n", response_check);
        }
        //check if we have recieved all the content of the file requested
        if (strstr(received, "All are sent.") != NULL){
          stop_receiving = 1;
          printf("END OF FILE RECEIVED.\n%s\n", received);
          bzero(received, 1200); //clear buffer so that we don't get problems
        }//check if we are getting the end of the octo block
        else if (strstr(received, "Has been succesfully sent.") != NULL){
          //packet loss generator
          /* Pick a random number between 0 to 9 and make one of the flags 0 */
          int random_number;
          srand(time(NULL));
          random_number = rand() % 10;
          printf("The random number is %d\n", random_number);
          if (random_number == 0){
              f0 = 0;
          } else if (random_number == 1){
            f1 = 0;
          }



          //send for ACK to client if all flags are 1
          if (f0 && f1 && f2 && f3 && f4 && f5 && f6 && f7){
            //sending ACK to server
            char ack_sent[] = "ACK";
            if (sendto(sock_id, ack_sent, strlen(ack_sent), 0, server_addr, sizeof(server)) == -1){
              printf("Sending ACK to the Octoput Server failed.\n\n");
              return 1;
            } else {
              printf("\'%s\' sent to the server.\n", ack_sent);
              //clear buffer
              bzero(received, strlen(received));
            }
          } else {

            printf("There are lost segments, request again the octoblock.\n");
            //sending NAK to server
            char nack_sent[] = "NAK";
            if (sendto(sock_id, nack_sent, strlen(nack_sent), 0, server_addr, sizeof(server)) == -1){
              printf("Sending NAK to the Octoput Server failed.\n\n");
              return 1;
            } else {
              printf("\'%s\' sent to the server.\n", nack_sent);
              //clear buffer
              bzero(received, strlen(received));
              bzero(file, strlen(file));
            }

          }

          //reset flags after check for the flags
          f0 = 0;
          f1 = 0;
          f2 = 0;
          f3 = 0;
          f4 = 0;
          f5 = 0;
          f6 = 0;
          f7 = 0;
          //printf("The flags are:\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", f0, f1, f2, f3, f4, f5, f6, f7);
        }//we keep receiving segments of files from server
        else {
          //check for total file received
          size = size + response_check;

          //printf("The file_content received is:\n%s\n", received);
          //bzero(received, strlen(received)); //clear the buffer so that there are no problems

          /* parse for the sequence number so that we could concatenate in the
          *  proper order; Isolate for the content as well
          */
          char *space = strstr(received, " "); //search for the empty line from the web_reply
          //printf("check for received: \n%s\n", received);
          char sequence_number[3];
          char file1[1200];
          //int seq_num;

          //Isolate sequence number
          if (space){
            sscanf(space, " %s", sequence_number);
            //seq_num = atoi(sequence_number);
            //printf("Sequence_number is: %s\n", sequence_number);
          }
          //Isolate the content of the file
          char received_copy[1200];
          strcpy(received_copy, received);
          char *content = strtok(received_copy, " ");
          //printf("content for strtok:\n%s\n", content);

          //check for paddings received
          if (strcmp(content, sequence_number) == 0){
            printf("Padding received.\n");
          } else {
            strcat(file, content);
          }

          /* check for flags for checking of we get all 8 octo legs */
          if (strcmp(sequence_number, "0") == 0){
            f0 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else if (strcmp(sequence_number, "1") == 0) {
            f1 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else if (strcmp(sequence_number, "2") == 0) {
            f2 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else if (strcmp(sequence_number, "3") == 0) {
            f3 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else if (strcmp(sequence_number, "4") == 0) {
            f4 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else if (strcmp(sequence_number, "5") == 0) {
            f5 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else if (strcmp(sequence_number, "6") == 0) {
            f6 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else if (strcmp(sequence_number, "7") == 0) {
            f7 = 1;
            printf("sequence %s received.\n", sequence_number);
          } else {
            printf("There is an error in checking for flags and sequence numbers.\n");
          }

          //Clear buffers so that we don't get problems
          bzero(received_copy, strlen(received_copy));
          bzero(file1, strlen(file1));
          bzero(sequence_number, strlen(sequence_number));
          bzero(received, strlen(received)); //clear buffer so that we don't get problems
        }
      }

      //printf("File is:\n%s\n", file);
      //printf("The size of the file received is: %ld, = %d\n", strlen(file), size);
      //Save the file to a txt file
      FILE *ptr_file;
      char file_location[] = "newFile";
      printf("Title is: %s\n", file_location); //there is a problem here
      //strcat(file_location, file_title);
      strcat(file_location, ".txt");
      //Create the file
      ptr_file = fopen(file_location, "w");
      if (!ptr_file){
        printf("There was an error in creating %s\n", file_location);
        return 1;
      } else {
        printf("%s file is created.\n", file_location);
      }
      //Start writing to file
      fputs(file, ptr_file);
      fclose(ptr_file);
      printf("File is saved.\n");
      save_file = 0;
      bzero(file, strlen(file));
    }
  }
  close(sock_id);
  return 0;
}
