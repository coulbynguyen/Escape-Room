#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
/*this is the struct of the room, it holds the name, the room type, the number of outgoing connections, and all of the names that it is connected to*/
struct room{
	char* name;
	char* room_type;
	int num_outgoing_connections;
	char* outgoing_connections[6];
};

/*this function uses a hashtable to select 7 out of the 10 rooms that will be used for the adventure game and populates the used_room with that data*/
void selectRandomRooms(struct room* used_rooms, char all_rooms[10][10]){
	/*has table to see if that number ahd already been selected and used*/
	int hash[10];
	srand(time(NULL));
	int i;
	int count = 0;
	int index;
	for(i = 0; i < 10; i++){
		hash[i] = 0;
	}
	while(count < 7){
		index = rand()%10;
		/*if that hash hasnt been used yet then it selects that name from the room names*/
		if(hash[index] == 0){
		   	used_rooms[count].name = malloc(16*sizeof(char));
			strcpy(used_rooms[count].name, all_rooms[index]);
			used_rooms[count].num_outgoing_connections = 0;
			hash[index] = 1;
			count++;
		}
	}

}
/*this function returns a random room froiim a possibility of 7*/
struct room* getRandomRoom(struct room* used_rooms){
	int index = rand()%7;
	return &used_rooms[index];	
}
/*this function sets the room type for the rooms that are being used,
 * one room is selected as the start room, another room is selected,
 * as the end room and it checks to make sure they are different, all the
 * other rooms that aren't selected get defaulted to mid_rooms */
void settingRoomType(struct room* used_rooms){
	int i;
   	int start = rand()%7;
	used_rooms[start].room_type = malloc(16*sizeof(char));
	strcpy(used_rooms[start].room_type, "START_ROOM");
	int end = rand()%7;
	while(start == end){
		end = rand()%7;
	}
	used_rooms[end].room_type = malloc(16*sizeof(char));
	strcpy(used_rooms[end].room_type, "END_ROOM");
	for(i = 0; i < 7; i++){
		if((i != start) && (i != end)){
			used_rooms[i].room_type = malloc(16*sizeof(char));
			strcpy(used_rooms[i].room_type, "MID_ROOM");
		}
	}
}
/*this function checks to see if the graph is full by checking whether or not there are at 
 * least 3 connections for every room*/
int isGraphFull(struct room* used_rooms){
   	/*return 0 if false return 1 if true*/
	int i;
	for(i = 0; i < 7; i++){
		if(used_rooms[i].num_outgoing_connections < 3){
			return 0;
		}
	}
	return 1;
}
/*this function  checks to see if a room is the same as another
 * it does this by using strcmp to see if the names are the same
 * and returns 1 if true and 0 if false*/
int isSameRoom(struct room* a, struct room* b){
	if(strcmp(a->name, b->name) == 0){
		return 1;
	}
	return 0;
}
/*this function checks to see if the number of outgoing connections is less than 6
 * because if it is then a connection can be made*/
int canAddConnection(struct room* a){
	if(a->num_outgoing_connections <  6){
		return 1;
	}
	return 0;
}

/*this function goes thru a structs list of names and using strcmp
 * to see if the second structs name is already contained inside the connections*/
int connectionAlreadyExists(struct room* a, struct room* b){
	int i = 0; 
	int end = a->num_outgoing_connections;
	for(i = 0; i < end; i++){
	   	
		if(strcmp(a->outgoing_connections[i], b->name) == 0){
			return 1;
		}
	}
	return 0;
}
/*this function connects structs b to a, by adding the name of struct b to 
 * struct a's list and incrementing the count of connections*/
void connect(struct room* a, struct room* b){
   	a->outgoing_connections[a->num_outgoing_connections] = malloc(16*sizeof(char));
	strcpy(a->outgoing_connections[a->num_outgoing_connections], b->name);
	a->num_outgoing_connections += 1;
}


/*this function adds a random connection by selecting a random room for 'a' and 
 * another random room for 'b' and connecting them if they dont have a connection
 * or if they arent the same room, and they are able to add a connection*/
void addRandomConnection(struct room* used_rooms){
	struct room* a;
	struct room* b;
	int condition = 1;
	while(condition){
	   a = getRandomRoom(used_rooms);
	   if(canAddConnection(a) == 1){
	      condition = 0;
	      /*			break;*/
	   }

	}
	do{
	   b = getRandomRoom(used_rooms);
	}while((canAddConnection(b)==0) || (isSameRoom(a,b) == 1) || (connectionAlreadyExists(a,b) == 1));
	connect(a,b);
	connect(b,a);

}
/*this function creates the directory by appending the string with the pid 
 * and using the mkdir function call*/
char * createDirectory(){
	char *dir_name = malloc(32*sizeof(char));
   	int num;
	int x;
	int y;
	char pidstring[10];
	
	num = getpid();
	strcpy(dir_name, "nguyenco.rooms.");
	
	x = sprintf(pidstring, "%d", num);
	
	strcat(dir_name, pidstring);
/*	printf("%s\n", dir_name); */
	y = mkdir(dir_name, 0755);
	return dir_name;


}
/*this function actually writes to the files, it writes the room name
 * then enters a for loop to write out all the connections, and lastly
 * it writes to the file the room name*/
void writeToFiles(struct room* a, int file_descriptor){
	int nwritten;
	int i;
	int z;
	char istring[10];
	int end = a->num_outgoing_connections;
	char *room_name = "ROOM NAME: ";
	char *connection = "CONNECTION ";
	char *room_type = "ROOM TYPE: ";
	nwritten = write(file_descriptor, room_name, strlen(room_name)*sizeof(char));
	nwritten = write(file_descriptor, a->name, strlen(a->name)*sizeof(char));
	nwritten = write(file_descriptor, "\n", strlen("\n")*sizeof(char));

	for(i = 0; i < end; i++){
		nwritten = write(file_descriptor, connection, strlen(connection)*sizeof(char));
		z = sprintf(istring, "%d", i+1);
		nwritten = write(file_descriptor, istring, strlen(istring)*sizeof(char));
	      	nwritten = write(file_descriptor, ": ", strlen(": ")*sizeof(char));
		nwritten = write(file_descriptor, a->outgoing_connections[i], strlen(a->outgoing_connections[i])*sizeof(char));
		nwritten = write(file_descriptor, "\n", strlen("\n")*sizeof(char));	
	}

	nwritten = write(file_descriptor, room_type, strlen(room_type)*sizeof(char));
	nwritten = write(file_descriptor, a->room_type, strlen(a->room_type)*sizeof(char));
	nwritten = write(file_descriptor, "\n", strlen("\n")*sizeof(char));
	

}

/*this function creates the file names to write to by appending the directory name
 * with the name of the room as well as appending '_room' */
void createFiles(char* dir_name, struct room* used_rooms){
	int i;
	int file_descriptor;
	for(i = 0; i < 7; i++){
		char* file_name = malloc(64*sizeof(char));
		strcpy(file_name, dir_name);
		strcat(file_name, "/");
		strcat(file_name, used_rooms[i].name);
		strcat(file_name, "_room");
		/*printf("%s\n", file_name);*/
		file_descriptor = open(file_name, O_WRONLY | O_CREAT, 0600);
		writeToFiles(&used_rooms[i], file_descriptor);
		
	}
	

}

/*this function is the menu that calls all the directory and file
 * functions*/
void printToFile(struct room* used_rooms){
	/*create the directory*/
   	/*create the files and write text to those files*/
	char *dir_name; 
	dir_name = createDirectory();
	createFiles(dir_name, used_rooms);

}


int main(){
   srand(time(NULL));
   struct room used_rooms[7];
   /*hardcoded list of names*/
   char all_rooms[10][10] = {"OREGON\0", "UTAH\0", "KANSAS\0", "ALASKA\0", "HAWAII\0", "IDAHO\0", "NEVADA\0", "VERMONT\0", "TEXAS\0", "MAINE\0"};
   int i = 0;
   int j;
   /*this is the menu for the whole build rooms*/
   /*initalizes the used_rooms array*/
   selectRandomRooms(used_rooms, all_rooms);
   /*sets the room types for the rooms in used_rooms array*/
   settingRoomType(used_rooms);
   /*this while loop continues until all the rooms in used_rooms have at least 3 connections*/
   while(isGraphFull(used_rooms) == 0){
      addRandomConnection(used_rooms);
   }
   /*after all the connections are made then the data is outputted to the appropriate files*/
   printToFile(used_rooms);

}


























