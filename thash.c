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
int errcheck;//variable used for error checking
char *tokenPtr;//token pointer to temporarily hold tokenized hash specifications, to be placed into algorithms array
char *algorithms[4];//array of strings entered after -m flag. //Ian: Should this not be the -a flag?
char *fileReadName = NULL;//Pointer to file to read from, specified by user with -f flag.
char *errFileName = NULL;//Pointer to file to write errors to, specified by user with -e flag.
char *fileWriteName = NULL;//Pointer to file to write stdout to, specified by user with -o flag.
char *fileNamefromFileFlag = NULL;
char buffer[1024];//buffer to hold each line of the file fgets obtains.
int numOfThreads = 2;//Number of threads to complete, default is 2 but can be modified with -t flag.
int queueCounter = 0;//counter to keep track of file names in queue.
FILE *fpr;//pointer to open file specified in -f flag
FILE *fprFromFile;//pointer to files listed in a specified file using the -f flag.
FILE *fpw;//used to write to a file using the -o flag.
FILE *fpe;//pointer to file used to print errors, specified using -e flag.
pthread_t identifier;//thread identifier
//defining a queue
struct node
{
    char *info;
    struct node *ptr;
}*front = NULL,*rear=NULL,*temp,*front1;
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

void printHashes(FILE *fp){
    fprintf(fp, "%s\n" , front->info);
    if(md5flag == 0){
    MD5_Update(&md5, front->info, sizeof(node));    
    MD5_Final(md5hash, &md5);
        fprintf(fp, "MD5\n");
        for(j = 0; j < MD5_DIGEST_LENGTH; j++){
            fprintf(fp, "%02x", md5hash[j]);
            }
            fprintf(fp, "\n"); 
    }      
    if(sha1flag == 0)
    {
    SHA1_Update(&sha, front->info, sizeof(node));   
    SHA1_Final(sha1hash, &sha);
        fprintf(fp, "SHA1\n");
        for(j = 0; j < SHA_DIGEST_LENGTH; j++){
                fprintf(fp, "%02x", sha1hash[j]);
        }
        fprintf(fp, "\n");
    }
    if(sha256flag == 0)
    {
    SHA256_Update(&sha256, front->info, sizeof(node));  
    SHA256_Final(sha256hash, &sha256);
        fprintf(fp, "SHA256\n");
        for(j = 0; j < SHA256_DIGEST_LENGTH; j++){
            fprintf(fp, "%02x", sha256hash[j]);
        }
        fprintf(fp, "\n");
    }
    if(sha512flag == 0)
    {
    SHA512_Update(&sha512, front->info, sizeof(node));  
    SHA512_Final(sha512hash, &sha512);
        fprintf(fp, "SHA512\n");
        for(j = 0; j < SHA512_DIGEST_LENGTH; j++){
                fprintf(fp, "%02x", sha512hash[j]);
        }
        fprintf(fp, "\n");
    }
}

void *threadFunction(void *arg)
{
	//dequeing from the queue
	front1 = front;
 
    if (front1 == NULL)
    {
    	if(errFileName != NULL){
	   		fprintf(fpe, "\n Error: Trying to display elements from empty queue\n");
	   		printf("Something has failed, way to go, JERRY. See error file %s for more details.\n", errFileName);
	   	}
        printf("Error: You're trying to display elements from an empty queue. That would be Rick-Diculous!\n");
        return arg;
    }
    else{
        if (front1->ptr != NULL)
        {
            front1 = front1->ptr;
            if(fileWriteName != NULL){
                printHashes(fpw);
            }
            else{
                printHashes(stdout);
            }
            printf("Dequeued value: %s\n", front->info);
            free(front);
            front = front1;
        }
        else
        {
            if(fileWriteName != NULL){
                printHashes(fpw);
            }
            else{
                printHashes(stdout);
            }
        printf("Dequeued value: %s\n", front->info);
        free(front);
        front = NULL;
        rear = NULL;
        }
    }
	return arg;
}
//End thread


int main(int argc, char *argv[]) {
printf("Hi, I'm Mr Meseeks, look at me! I heard you want to hash some files. CAAAN DO! \n");


opterr = 0;
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
      		if ( ( fpw = fopen(fileWriteName, "w" ) ) == NULL ) {
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
	   				}
	   				else{
	   					printf("Error reading file %s from file list.\n", buffer);
                        printf("String length is: %lu\n", strlen(buffer));
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
    					}
    		 			else {
        				temp = (struct node *)malloc(sizeof(struct node));
        				rear->ptr = temp;
                        temp->info = (char *)malloc(strlen(buffer));
        				strncpy(temp->info, buffer, strlen(buffer));
        				temp->ptr = NULL;
        				rear = temp;
	   				}
	   			queueCounter++;
				}//end else statement
		}//end while
	}//end if fileReadName != NULL check.
	for(int i = optind; i < argc; i++){
		printf("File Detected: %s\n", argv[i]);
        if((fprFromFile = fopen(argv[i], "r")) == NULL){
                    if(errFileName != NULL){
                        fprintf(fpe, "Error reading file %s.\n", argv[i]);
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
    for(j = 0; j <= numOfThreads; j++){
    pthread_create(&identifier, NULL, &threadFunction, "I'm Mr Threadseeks, look at me!\n");
	}
	pthread_join(identifier, NULL);
    


    
		printf("If you got to here, the program executed successfully. TINY RIIICK!\n");
	}
