/////comment
//Anotha one 
#include <stdio.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <getopt.h>
void *threadFunction(void *arg);//function declaration
int i;//used to keep track of arguments.
int j;//used for nested for loop
/**flags to indicate which algorithms user has specified **/
int md5flag = 1;
int sha1flag = 1;
int sha256flag = 1;
int sha512flag = 1;
/**Creating contexts and arrays to store hashes **/
MD5_CTX md5;
SHA_CTX sha;
SHA256_CTX sha256;
SHA512_CTX sha512;
unsigned char md5hash[MD5_DIGEST_LENGTH];
unsigned char sha1hash[SHA_DIGEST_LENGTH];
unsigned char sha256hash[SHA256_DIGEST_LENGTH];
unsigned char sha512hash[SHA512_DIGEST_LENGTH];
/**Variable declarations-see comments after each variable to determine purpose. **/
int errcheck;//variable used for error checking.
char *tokenPtr;//token pointer to temporarily hold tokenized hash specifications, to be placed into algorithms array.
char *algorithms[4];//array of strings entered after -a flag. //Ian: Should this not be the -a flag? <-Alex: yup there's no m-flag oops
char *fileReadName = NULL;//Pointer to file to read from, specified by user with -f flag.
char *errFileName = NULL;//Pointer to file to write errors to, specified by user with -e flag.
char *fileWriteName = NULL;//Pointer to file to write stdout to, specified by user with -o flag.
char buffer[1024];//buffer to hold each line of the file fgets obtains.
char dataBuffer[4096];//buffer to hold all data read from the files specified in buffer above to add to queue in order to hash.
int numOfThreads = 2;//Number of threads to complete, default is 2 but can be modified with -t flag. TODO: Add error checking so it cannot be > 1.
int queueCounter = 0;//counter to keep track of file names in queue.
int numofBytes = 0;//number of bytes for hash algos to check
FILE *fpr;//pointer to open file specified in -f flag
FILE *fprFromFile;//pointer to files listed in a specified file using the -f flag.
FILE *fpw;//used to write to a file using the -o flag.
FILE *fpe;//pointer to file used to print errors, specified using -e flag.
FILE *fpToHashData;//pointer to open files that were pushed to queue from fprFromFile.
pthread_t identifier;//thread identifier.
int fd;//file descriptor
pthread_mutex_t queue_mutex;//mutex declaration in order to lock down resource access.(in our case, the queue)
//defining a queue
struct node
{
    char *info;
    struct node *ptr;
}*front = NULL,*rear=NULL,*temp;//*front1;
typedef struct node node;//defining node to point to struct node so I don't have to declare struct node each time I want to use it.

//front = rear = NULL;//empty queue created

char * stringToLower(char *str, int n){ //Makes the first n chars of a string lowercase
    if(str == NULL)
        return NULL;
    const char *out = str;
    while(*str != '\0'&& n > 0){
        *str = tolower(*str);
        str++;
        n--;
    }
    return (char *)out;
}

void printHashes(int fd, struct node *front1){
	if((fpToHashData = fopen(front1->info, "r")) == NULL){
		if(errFileName != NULL){
	   					fprintf(fpe, "Error reading file %s from file list.\n", front1->info);
	   					printf("Something has failed, way to go, JERRY. See error file %s for more details.\n", errFileName);
	   				}
	   				else{
	   					printf("Error reading file %s from file list.\n", front1->info);
                        printf("String length is: %lu\n", strlen(front1->info));
	   				}
	}

    dprintf(fd, "%s\n" , front1->info);
    if(md5flag == 0){
    while((numofBytes = fread(dataBuffer, 1, 4096, fpToHashData))!= 0){//continue reading data from dataBuffer
    	MD5_Update(&md5, dataBuffer, numofBytes);
    }//end while    
    MD5_Final(md5hash, &md5);
    rewind(fpToHashData);//resetting file pointer to beginning for other hashing algos to use.
        dprintf(fd, "MD5\n");
        for(j = 0; j < MD5_DIGEST_LENGTH; j++){
            dprintf(fd, "%02x", md5hash[j]);
            }
            dprintf(fd, "\n"); 
    }      
    if(sha1flag == 0)
    {
    	while((numofBytes = fread(dataBuffer, 1, 4096, fpToHashData))!= 0){//continue reading data from dataBuffer
    SHA1_Update(&sha, dataBuffer, numofBytes);
    }//end while   
    SHA1_Final(sha1hash, &sha);
    rewind(fpToHashData);//resetting file pointer to beginning for other hashing algos to use.
        dprintf(fd, "SHA1\n");
        for(j = 0; j < SHA_DIGEST_LENGTH; j++){
                dprintf(fd, "%02x", sha1hash[j]);
        }
        dprintf(fd, "\n");
    }
    if(sha256flag == 0)
    {
    	while((numofBytes = fread(dataBuffer, 1, 4096, fpToHashData))!= 0){//continue reading data from dataBuffer
    SHA256_Update(&sha256, dataBuffer, numofBytes); 
    }//end while 
    SHA256_Final(sha256hash, &sha256);
    rewind(fpToHashData);//resetting file pointer to beginning for other hashing algos to use.
        dprintf(fd, "SHA256\n");
        for(j = 0; j < SHA256_DIGEST_LENGTH; j++){
            dprintf(fd, "%02x", sha256hash[j]);
        }
        dprintf(fd, "\n");
    }
    if(sha512flag == 0)
    {
    	while((numofBytes = fread(dataBuffer, 1, 4096, fpToHashData))!= 0){//continue reading data from dataBuffer
    SHA512_Update(&sha512, dataBuffer, numofBytes);
    }//end while  
    SHA512_Final(sha512hash, &sha512);
    rewind(fpToHashData);//resetting file pointer to beginning for other hashing algos to use.
        dprintf(fd, "SHA512\n");
        for(j = 0; j < SHA512_DIGEST_LENGTH; j++){
                dprintf(fd, "%02x", sha512hash[j]);
        }
        dprintf(fd, "\n");
    }
}

void *threadFunction(void *arg)
{
	//Implementing file locking mechanism f_lock to lock writing to a file to one function call at a time.
	struct flock f_lock;
    f_lock.l_type = F_WRLCK;
 	f_lock.l_whence = SEEK_SET;
 	f_lock.l_start = 0;
 	f_lock.l_len = 0;
	//dequeing from the queue
    struct node *front1;
    pthread_mutex_lock(&queue_mutex);
	front1 = front;
    pthread_mutex_unlock(&queue_mutex);
 
    if (front1 == NULL)
    {
    	if(errFileName != NULL){
	   		fprintf(fpe, "\n Queue empty.\n");
	   		printf("Queue empty. A record of when the queue was emptied can be found at the end of %s.\n", errFileName);
	   	}
        printf("Queue empty, program terminating successfully. TINY RIIICK!\n");
        //exit(EXIT_SUCCESS);;
        return arg;
    }
    else{
       while(1){
        //Locking down front1 variable when it is being modified-no need to lock when it is being accessed
          pthread_mutex_lock(&queue_mutex);
          front1 = front;
          if(front!=NULL){
          front = front->ptr;
           }
          pthread_mutex_unlock(&queue_mutex);

  if (front1 == NULL)
  {

      if(errFileName != NULL){

              fprintf(fpe, "\n Queue empty.\n");
              printf("Queue empty. A record of when the queue was emptied can be found at the end of %s.\n", errFileName);
          }
      printf("Queue empty, program terminating successfully. TINY RIIICK!\n");
      //exit(EXIT_SUCCESS);;
      return arg;
  }
          if(fileWriteName != NULL){
              //f_lock.l_type = F_SETLKW;
              //fcntl(fd, F_SETLKW, &f_lock);
              pthread_mutex_lock(&queue_mutex);
              printHashes(fd, front1);
              //f_lock.l_type = F_UNLCK;
               //fcntl(fd, F_SETLK, &f_lock);
              pthread_mutex_unlock(&queue_mutex);
          }
          else{
            pthread_mutex_lock(&queue_mutex);    
              printHashes(fd, front1); //This has to be modified to take front1!!
            pthread_mutex_unlock(&queue_mutex);
          }
          printf("Dequeued value: %s\n", front1->info);
          free(front1);
}//end while
    }
	return arg;
}
//End thread


int main(int argc, char *argv[]) {
printf("Hi, I'm Mr Meseeks, look at me! I heard you want to hash some files. CAAAN DO! \n");
static struct option long_options[] ={
     {"algorithms", required_argument, 0, 'a'},
     {"file", required_argument, 0, 'f'},
     {"error", required_argument, 0, 'e'},           //This is how the long flags are associated with the short flags
     {"output", required_argument, 0, 'o'},
     {"threads", required_argument, 0, 't'},
     {0,0,0,0}
 };
 int option_index = 0;


while((errcheck = getopt(argc, argv, "a:f:e:o:t:")) != -1) {
	opterr = 0;
	printf("Value of errcheck is %c\n", errcheck);
	switch(errcheck){
		case 'a':
			optind--;//external int used by getopt
			algorithms[0] = stringToLower(strtok(argv[optind], ","), 6);
			printf("Algorithm read: %s\n", algorithms[0]);
			for(int i = 1; i<=3 && (algorithms[i] = stringToLower(strtok(NULL, ","), 6))!=NULL; i++){
				printf("Algorithm read: %s\n", algorithms[i]);
			} //This tokenizes the algorithms
			optind++;

			break;
		case 'f':

			fileReadName = optarg;
			if ( ( fpr = fopen(fileReadName, "r" ) ) == NULL ) {
      			printf( "File %s could not be opened\n", optarg);
      			exit(EXIT_FAILURE);
      		}
      		break;
      	case 'e':

      		errFileName = optarg;
      		if ( ( fpe = fopen(errFileName, "w" ) ) == NULL ) {
      			printf( "File %s could not be opened\n", optarg);
      			exit(EXIT_FAILURE);
      		}
      		break;
      	case 'o':
      		fileWriteName = optarg;
      		if ( ( fd = open(fileWriteName, O_WRONLY | O_CREAT, 0744 ) ) < 0 ) {
      			printf( "File %s could not be opened\n", fileWriteName);
      			exit(EXIT_FAILURE);
      		}
      		break;
      	case 't':
      		numOfThreads = strtol(optarg, NULL, 10);
      		break;
      	case ':':                           
        	printf("Option -%c requires a plumb-I mean, argument.\n", optopt);//optopt is an external int.
 		break;
        case '?':                           
        	printf("Morty, you're not using it right. You can't put -%c into this program. \n ", optopt);
        	break;
        default:
        	printf("You really squanched it up. The usage for this program is: thash -a algorithms file1...file2...\n");
        	exit(EXIT_FAILURE);

        }//end switch
    	}//end while loop

		//obtaining a list of file names.
        if(fileReadName != NULL){		//If the file of filenames exists
    		while(fgets(buffer, 1024, fpr)!=NULL){		//Continue reading filenames until EOF
                buffer[strlen(buffer)-1] = '\0';          //Delimit the file name
	   			if((fprFromFile = fopen(buffer, "r")) == NULL){
	   				if(errFileName != NULL){
	   					fprintf(fpe, "Error reading file %s from file list.\n", buffer);
	   					printf("Something has failed, way to go, JERRY. See error file %s for more details.\n", errFileName);
                        exit(EXIT_FAILURE);
	   				}
	   				else{
	   					printf("Error reading file %s from file list.\n", buffer);
                        printf("String length is: %lu\n", strlen(buffer));
                        //exit(EXIT_FAILURE);
	   				}

	   			}
	   			else{
	   			//populating queue with values read from file specified with -f flag as well as comamnd line args.
	   		 		if (rear == NULL) {
        				rear = (struct node *)malloc(sizeof(struct node));
        				rear->ptr = NULL; 
                        rear->info = (char *)malloc(strlen(buffer));
        				strncpy(rear->info, buffer, strlen(buffer));
        				front = rear;
    					}//end if
    		 			else {
        				temp = (struct node *)malloc(sizeof(struct node));
        				rear->ptr = temp;
        				while(fread(dataBuffer, 1, 4096, fprFromFile)!= 0){//continue reading data from dataBuffer 
                        temp->info = (char *)malloc(strlen(buffer));
        				strncpy(temp->info, buffer, strlen(buffer));
        				}
        				temp->ptr = NULL;
        				rear = temp;
	   				}
	   			queueCounter++;
				}//end else statement
		}//end while
	}//end if fileReadName != NULL check.
	for(int i = optind; i < argc; i++){
		printf("File Detected: %s\n", argv[i]);
        if((fprFromFile = fopen(argv[i], "r")) == NULL){//Alex: fprFromFile is meant to be a file pointer to files inside file spcified in -f flag.
                    if(errFileName != NULL){
                        fprintf(fpe, "Error reading file %s : File does not exist.\n", argv[i]);
                        printf("Something has failed, way to go, JERRY. See error file %s for more details.\n", errFileName);
                    }
                    else{
                        printf("Error reading file %s.\n", argv[i]);
                    }

                }
                else{
                //populating queue with values read from file specified with -f flag as well as comamnd line args.
                    if (rear == NULL) {
                        rear = (struct node *)malloc(sizeof(struct node));
                        rear->ptr = NULL;
                        rear->info = argv[i];
                        front = rear;
                        }
                        else {
                        temp = (struct node *)malloc(sizeof(struct node));
                        rear->ptr = temp;
                        temp->info = argv[i];
                        temp->ptr = NULL;
                        rear = temp;
                    }
                queueCounter++;
                }//end else statement
	}

    for(i = 0; i < 4 && algorithms[i]!=NULL; i++){                                    
    	if(strcmp(algorithms[i], "md5") == 0){
            md5flag = 0;
			MD5_Init(&md5);
    		}
    	else if(strcmp(algorithms[i], "sha1") == 0){ 
            sha1flag = 0;   		
    		SHA1_Init(&sha);
    		}

    	else if(strcmp(algorithms[i], "sha256") == 0){
            sha256flag = 0;
    		SHA256_Init(&sha256);
			}
		else if(strcmp(algorithms[i], "sha512") == 0){
            sha512flag = 0;
    		SHA512_Init(&sha512);
    	}
    	
		}//end for loop.
    
    //TODO: Finish implementing threading. You have written the function for the thread, you've created the thread below. Now finish it.
    printf("Number of threads is %d ", numOfThreads);
    for(j = 0; j < numOfThreads; j++){
    pthread_create(&identifier, NULL, &threadFunction, "I'm Mr Threadseeks, look at me!\n");
	//sleep(1);
    }

	pthread_join(identifier, NULL);
    
	}

