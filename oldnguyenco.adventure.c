#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
struct room{
	char* name;
	char* room_type;
	int num_outgoing_connections;
	char* outgoing_connections[6];
};

struct room* getStartRoom(struct room* used_rooms){
	int i = 0;
	for(i = 0; i < 7; i++){
	   /*this loop only needs to iterate over the 7 rooms being used because 1 and only 1 of them will be the starting room*/
		if(strcmp(used_rooms[i].room_type, "START_ROOM") == 0){
		   /*this returns the starting room*/
			return &used_rooms[i];
		}
	}
}

void printPaths(struct room* cur){
	int i;
	int end = cur->num_outgoing_connections;
	printf("CURRENT LOCATION: %s\n", cur->name);
	printf("POSSIBLE CONNECTIONS:");
	for(i = 0; i < end; i++){
	   	if(i != (end-1)){
			printf(" %s,", cur->outgoing_connections[i]);
			/*prints out all the connections with a comma*/
		}
		else{
			printf(" %s.\n", cur->outgoing_connections[i]);
			/*prints out all the last connection with a period and a new line*/
		}
	}
	printf("WHERE TO? >");

}

int checkValidPath(struct room* cur, char* next_room){
	int i;
	int end = cur->num_outgoing_connections;
	for(i = 0; i < end; i++){
	   /*
	   printf("%s", next_room);
	   printf("%s\n", cur->outgoing_connections[i]);
	   printf("%d\n", strcmp(cur->outgoing_connections[i], next_room));
	   */
	   if(strcmp(cur->outgoing_connections[i], next_room) == -10){
	      /*returns true if next room in the current rooms list of outgoing connections*/
	      return 1;
	   }
	}
	return 0;
}

struct room* changeRoom(char* next_room, struct room* used_rooms, int* count_moves, char** moves){
   int i;
   for(i = 0; i < 7; i++){
      if(strcmp(used_rooms[i].name, next_room) == -10){
	 /*this returns the room that has the specified name that is entered by the user*/
	 moves[*count_moves] = malloc(10*sizeof(char));
	 strcpy(moves[*count_moves], next_room);
	 *count_moves += 1;
	 return &used_rooms[i];
      }
   }
}

void endgame(char** moves, int count_moves){
   int i;
   printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
   printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", count_moves);
   for(i = 0; i < count_moves; i++){
      printf("%s", moves[i]);
   }
}
char* getDirectoryName(){
   int newestDirTime = -1;
   char targetDirPrefix[32] = "nguyenco.rooms.";
   char newestDirName[256];
   memset(newestDirName, '\0', sizeof(newestDirName));

   DIR* dirToCheck;
   struct dirent *fileInDir;
   struct stat dirAttributes;

   dirToCheck = opendir(".");
   if(dirToCheck > 0){
      while((fileInDir = readdir(dirToCheck)) != NULL){
	 if(strstr(fileInDir->d_name, targetDirPrefix) != NULL){
	    /*	
		printf("Found the prefex: %s\n", fileInDir->d_name);
		*/
	    stat(fileInDir->d_name, &dirAttributes);
	    if((int)dirAttributes.st_mtime > newestDirTime){
	       newestDirTime = (int)dirAttributes.st_mtime;
	       memset(newestDirName, '\0', sizeof(newestDirName));
	       strcpy(newestDirName, fileInDir->d_name);
	       /*printf("Newer Subdir: %s, new time: %d\n", fileInDir->d_name, newestDirTime);*/

	    }
	 }
      }
   }
   closedir(dirToCheck);
   /*
      printf("Newest entry found is: %s\n", newestDirName);	
      */
   char* dir_name = malloc(32*sizeof(char));
   strcpy(dir_name, newestDirName);
   return dir_name;
}

void readFromFiles(char* dir_name, char room_names[10][10], struct room *used_rooms){
   int i;
   int count = 0;
   char roomtypes[3][12] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
   FILE* file_descriptor;
   char* file_name = malloc(32*sizeof(char));
   for(i = 0; i < 10; i++){
      strcpy(file_name, dir_name);
      strcat(file_name, "/");
      strcat(file_name, room_names[i]);
      strcat(file_name, "_room");
      file_descriptor = fopen(file_name, "r");
      if(file_descriptor == NULL){
	 /*that room is not one used in this program*/
	 /* do nothing*/
	 /* this prints out the supposed file names of those that are not actually in that directory
	    printf("%s\n", file_name);
	    */
      }
      else{
	 char line[256];
	 int j;
	 int x = 0;
	 /* this line of code prints out the file name if it is in that directory
	    printf("%s\n", room_names[i]);
	    */


	 /* read data from that file */
	 /* read line*/
	 while(fgets(line, 255,file_descriptor)){
	    /*if strstr(line, room_name) */
	    if(strstr(line, "ROOM NAME")!= NULL){
	       /*strstr(line, every room) in order to see what room name that is*/
	       for(j = 0; j < 10; j++){
		  if(strstr(line, room_names[j]) != NULL){
		     /*set used_rooms[i].name = room_names[j]*/
		     used_rooms[count].name = malloc(10*sizeof(char));
		     strcpy(used_rooms[count].name, room_names[j]);
		  }
	       }	   
	    }
	    /*else if strstr(line, connection) */
	    else if(strstr(line, "CONNECTION") != NULL){
	       /*strstr(line, every room) in order to see what room connected for that one is*/
	       for(j = 0; j < 10; j++){

		  if(strstr(line, room_names[j]) != NULL){
		     /*set used_rooms[i].outgoing_connections[num_outgoing_connections] = room_names[j]*/
		     used_rooms[count].outgoing_connections[x] = malloc(16*sizeof(char));
		     strcpy(used_rooms[count].outgoing_connections[x], room_names[j]);
		     x += 1;
		  }
	       }	   
	    }


	    /*else if strstr(line, room_type) */
	    else if(strstr(line, "ROOM TYPE") != NULL){


	       /*strstr(line, the 3 types of rooms) to get their room type*/
	       for(j = 0; j < 3; j++){
		  if(strstr(line, roomtypes[j]) != NULL){
		     /*set used_rooms[i].room_type = roomtypes[j]*/
		     used_rooms[count].room_type = malloc(16*sizeof(char));
		     strcpy(used_rooms[count].room_type, roomtypes[j]);
		  }
	       }
	    }
	 }
	 used_rooms[count].num_outgoing_connections = x;
	 count++;
	 fclose(file_descriptor);
      }
   }
}


int main(){
   char* moves[100]; /*this holds the name of all the moves that the player has chosen*/
   int count_moves = 0; /* holds the number of steps that the user has taken*/
   char room_names[10][10] = {"OREGON\0", "UTAH\0", "KANSAS\0", "ALASKA\0", "HAWAII\0", "IDAHO\0", "NEVADA\0", "VERMONT\0", "TEXAS\0", "MAINE\0"};
   struct room used_rooms[7]; /*this holds the 7 rooms that are used in the game */
   char* next_room = malloc(32*sizeof(char));
   char* dir_name = getDirectoryName();
   size_t characters;
   size_t size = 32; 
   int i;
   int j;

   readFromFiles(dir_name, room_names, used_rooms);
   /* the following below just prints out all the information stored in the used_rooms variable
      for(i = 0; i < 7; i++){
      printf("%s\n", used_rooms[i].name);
      printf("%d outgoing connections\n", used_rooms[i].num_outgoing_connections); 
      for(j = 0; j < used_rooms[i].num_outgoing_connections; j++){
      printf("CONNECTION %d: %s\n", j, used_rooms[i].outgoing_connections[j]);
      }
      printf("ROOM TYPE: %s\n", used_rooms[i].room_type);
      }
      */
   /*
      printf("directory : %s\n", dir_name);
      */
   /* STILL NEED TO CREATE A FUNCTION THAT READS IN ALL THE DATA FROM THE FILES */

   struct room* cur;
   cur = getStartRoom(used_rooms);


   while(strcmp(cur->room_type, "END_ROOM") != 0){

      /*this while loop checks the condition to see if the user is at the end*/
      printPaths(cur);
      
      /*getline(next_room) gets user input and stores it in a variable*/
      characters = getline(&next_room, &size, stdin);
      strcat(next_room, "\0");
      
      while(checkValidPath(cur, next_room) == 0){
	 /*this while loop executes when the user entered a choice that is not available */
	 printf("\n");
	 printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
	 printPaths(cur);

	 /*getline(next_room) gets user input and stores it in that variable*/
	 memset(next_room, '\0', sizeof(next_room));
	 characters = getline(&next_room, &size, stdin);
      	 strcat(next_room, "\0");
      }
      cur = changeRoom(next_room, used_rooms, &count_moves, moves);
      printf("\n");


   }
   endgame(moves, count_moves);
   return 0;
}
