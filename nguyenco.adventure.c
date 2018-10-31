/*COULBY NGUYEN 932-439-525
 * before i give this program a proper header, i accidentally deleted the buildrooms file because i compiled gcc nguyenco.buildrooms.c -o nguyenco.buildroom.c 
 * and had to start over....... feelsgoodman
 * ok so this program takes input from files and then uses that data to play a game given by the user input*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <assert.h>

/*this struct is the struct of the room, it holds the room name, what type of room, and how many outgoing connections and the names for all the connections*/
struct room{
	char* name;
	char* room_type;
	int num_outgoing_connections;
	char* outgoing_connections[6];
};

/*this function searches from the used_rooms data that was just read in, to see which room is the starting place by comparing the room_type to "start_room"*/
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

/*this function prints all the paths that are reachable by the current location
 * it does this by going throught the outgoing connections as many times as the number of outgoing connections*/
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
/*this function checks to see if the input by the user is a valid option
 * it does this by going through the current rooms outgoing connections and
 * using strcmp to see if they are equal, but since the file i read doesn't have an \n at the end
 * and when the user hits enter it adds that when reading in user input so instead of == 0 
 * i set it to == -10*/
int checkValidPath(struct room* cur, char* next_room){
	int i;
	int end = cur->num_outgoing_connections;
	if(strcmp(next_room, "time\n") == 0){
		return 1;
	}
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

/*this function returns the address of the next room, increments the moves count, and adds the next room 
 * into the array of rooms*/
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
/*this function prints out the end credits
 * aka the number of moves, and all the decisions the user 
 * entered to get there*/
void endgame(char** moves, int count_moves){
   int i;
   printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
   printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", count_moves);
   for(i = 0; i < count_moves; i++){
      printf("%s", moves[i]);
   }
}
/*this function was taken directly out of the notes
 * it opens the current directory we are in and finds the newest directory
 * and then returns that directory name*/
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
/*this function reads all the files data into the correct spots*/
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
/*this function is the function for the time threading*/
void* perform_work(void* argument){
	struct tm* timeinfo;
	char buffer[256];
	time_t rawtime;
	int file_descriptor;

   	FILE* file_descriptor2;
	int nwritten;
	char line[256];
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	/*creates the time as a string and stores that string into buffer*/
	strftime(buffer, 255, "%I:%M%p, %A, %B %d, %Y\n", timeinfo);
	
	/*output the time to file*/
	file_descriptor = open("./currentTime.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	nwritten = write(file_descriptor, buffer, strlen(buffer)*sizeof(char));
	close(file_descriptor);

	/*open that file and read that input*/
	file_descriptor2 = fopen("./currentTime.txt", "r");
	fgets(line, 255, file_descriptor2);
	printf("    \n%s", line);
	return NULL;
	 
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
   pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
   int result_code;


   readFromFiles(dir_name, room_names, used_rooms);

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
      if(strcmp(next_room, "time\n") == 0){
	 pthread_t timethread;
	 result_code = pthread_create(&timethread, NULL, perform_work, NULL);
	 assert(0 == result_code);

	 pthread_mutex_lock(&myMutex);
	 result_code = pthread_join(timethread,NULL);
	 assert(0 == result_code);
	 pthread_mutex_unlock(&myMutex);

      }
      else{
	 /*if the user choice wasnt time then it was an valid move to change to and the following calls changes the cur pointer to the next room*/
	 cur = changeRoom(next_room, used_rooms, &count_moves, moves);
      }
      printf("\n");


   }
   endgame(moves, count_moves);
   return 0;
}
