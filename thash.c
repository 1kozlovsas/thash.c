/////comment
//Anotha one 
#include <stdio.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
void *threadFunction(void *arg);//function declaration
int i;//used to keep track of arguments.
int j;//used for nested for loop
/**flags to indicate which algorithms user has specified **/
int md5flag = 0;
int sha1flag = 0;
int 
sha256flag = 0;
int sha512flag = 0;
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


void *threadFunction(void *arg)
{
	//dequeing from the queue
	front1 = front;
 
    if (front1 == NULL)
    {
    	if(errFileName != NULL){
	   		fprintf(fpe, "\n Error: Trying to display elements from empty queue");
	   		printf("Something has failed, way to go, JERRY. See error file %s for more details.", errFileName);
	   		}
        printf("\n Error: You're trying to display elements from an empty queue. That would be Rick-Diculous!");
        return arg;
    }
    else
        if (front1->ptr != NULL)
        {
            front1 = front1->ptr;
            if(&md5 != NULL){
            MD5_Update(&md5, front->info, sizeof(node));	
    		MD5_Final(md5hash, &md5);
    			for(j = 0; j < sizeof(node); j++){
    				if(fileWriteName != NULL){
    					fprintf(fpw, "%s \n" , front->info);
    					fprintf(fpw, "%02x", md5hash[j]);
    				}
    				else{
    				printf (" %s\n", front->info);
    				printf("%02x", md5hash[j]);
    					}//end else
    				}//end for 
    		}//end if		
    		else if(&sha != NULL)
    		{
            SHA1_Update(&sha, front->info, sizeof(node));	
    		SHA1_Final(sha1hash, &sha);
    			for(j = 0; j < sizeof(node); j++){
    				if(fileWriteName != NULL){
    					fprintf(fpw, "%s \n" , front->info);
    					fprintf(fpw, "%02x", sha1hash[j]);
    				}
    				else{
    				printf (" %s\n", front->info);
    				printf("%02x", sha1hash[j]);
    				}
    			}
    		}
    		else if(&sha256 != NULL)
    		{
            SHA256_Update(&sha256, front->info, sizeof(node));	
    		SHA256_Final(sha256hash, &sha256);
    			for(j = 0; j < sizeof(node); j++){
    				if(fileWriteName != NULL){
    					fprintf(fpw, "%s \n" , front->info);
    					fprintf(fpw, "%02x", sha256hash[j]);
    				}
    				else{
    				printf (" %s\n", front->info);
    				printf("%02x", sha256hash[j]);
    			}}
    		}
    		else if(&sha512 != NULL)
    		{
            SHA512_Update(&sha512, front->info, sizeof(node));	
    		SHA512_Final(sha512hash, &sha512);
    			for(j = 0; j < sizeof(node); j++){
    				if(fileWriteName != NULL){
    					fprintf(fpw, "%s \n" , front->info);
    					fprintf(fpw, "%02x", sha512hash[j]);
    				}
    				else{
    				printf (" %s\n", front->info);
    				printf("%02x", sha256hash[j]);
    			}}
           }
           //printf("\n Dequed value : %s", front->info);
            free(front);
            front = front1;
        }
        else
        {
        	if(&md5 != NULL){
            MD5_Update(&md5, front->info, sizeof(node));	
    		MD5_Final(md5hash, &md5);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", md5hash[j]);
    			}
    		}
    		else if(&sha != NULL){
            SHA1_Update(&sha, front->info, sizeof(node));	
    		SHA1_Final(sha1hash, &sha);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", sha1hash[j]);
    			}
    		}
    		else if(&sha256 != NULL){
            SHA256_Update(&sha256, front->info, sizeof(node));	
    		SHA256_Final(sha256hash, &sha256);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", sha256hash[j]);
    			}
    		}
    		else if(&sha512 != NULL){
            SHA512_Update(&sha512, front->info, sizeof(node));	
    		SHA512_Final(sha512hash, &sha512);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", sha256hash[j]);
    			}
    		}
            printf("\n Dequed value : %s", front->info);
            free(front);
            front = NULL;
            rear = NULL;
        }
	return arg;
}
//End thread


int main(int argc, char *argv[]) {
printf("Hi, I'm Mr Meseeks, look at me! I heard you want to hash some files. CAAAN DO! \n");



while((errcheck = getopt(argc, argv, "a:f::e::o::t::")) != -1) {
	printf("Value of errcheck is %d", errcheck);
	switch(errcheck){
		case 'a':
			//optind--;//external int used by getopt
			//for(; optind < argc - 1 && *argv[optind] != '-'; optind++){
			algorithms[0] = strtok(argv[optind], ",");
			for(int i = 1; i<=3 && (algorithms[i] = strtok(NULL, ","))!=NULL; i++); //This tokenizes the algorithms
			//algorithms[optind] = argv[optind];
			//}
			break;
		case 'f':
			fileReadName = optarg;
			if ( ( fpr = fopen(fileReadName, "r" ) ) == NULL ) {
      			printf( "File %s could not be opened", fileReadName);
      			exit(EXIT_FAILURE);
      		}
      		break;
      	case 'e':
      		errFileName = optarg;
      		if ( ( fpe = fopen(errFileName, "w" ) ) == NULL ) {
      			printf( "File %s could not be opened", errFileName);
      			exit(EXIT_FAILURE);
      		}
      		break;
      	case 'o':
      		fileWriteName = optarg;
      		if ( ( fpw = fopen(fileWriteName, "w" ) ) == NULL ) {
      			printf( "File %s could not be opened", fileWriteName);
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
        	printf("You really squanched it up. The usage for this program is: thash -a algorithms file1...file2...");
        	exit(EXIT_FAILURE);

        }//end switch
    	}//end while loop

		//obtaining a list of file names.
        if(fileReadName != NULL){
    		while(!feof(fpr)){

	   			if((fprFromFile = fopen(fgets(buffer, 1024, fpr), "r")) == NULL){
	   				if(errFileName != NULL){
	   					fprintf(fpe, "Error reading file names from file specified");
	   					printf("Something has failed, way to go, JERRY. See error file %s for more details.", errFileName);
	   				}
	   				else{
	   					printf("Error reading file names from file specified.");
	   					exit(EXIT_FAILURE);
	   				}

	   			}
	   	else{
	   		//populating queue with values read from file specified with -f flag as well as comamnd line args.
	   		 if (rear == NULL) {
        		rear = (struct node *)malloc(1*sizeof(struct node));
        		rear->ptr = NULL;
        		rear->info = fileNamefromFileFlag;
        		front = rear;
    			}
    		 else {
        		temp=(struct node *)malloc(1*sizeof(struct node));
        		rear->ptr = temp;
        		temp->info = fileNamefromFileFlag;
        		temp->ptr = NULL;
        		rear = temp;
	   			}
	   			queueCounter++;
			}//end else statement
			}//end while
			}//end if fileReadName != NULL check.


		argc--;//decrement argc by 1 since number of args starts at 0.
/*		int counter = argc;//assiging argc to a placeholder counter for readability  //This line is disabling a lot of code to isolate the segfault
		//This while loop is used to locate the last valid file names to populate queue with.
		while(strcmp(argv[counter], "-f") != 1 || strcmp(argv[counter], "-a") != 1 || strcmp(argv[counter], "-o") != 1 || strcmp(argv[counter], "-e") != 1 || strcmp(argv[counter], "-t") != 1){
			counter--;
		}//end while
		counter++;//incrementing counter by 1 to account for bogus argument.
		while(counter <= argc){
			//queue populating with file names taken from command line.
			if (rear == NULL) {
        		rear = (struct node *)malloc(1*sizeof(struct node));
        		rear->ptr = NULL;
        		rear->info = argv[counter];
        		front = rear;
    			}
    		 else {
        		temp=(struct node *)malloc(1*sizeof(struct node));
        		rear->ptr = temp;
        		temp->info = argv[counter];
        		temp->ptr = NULL;
        		rear = temp;
	   			}
	   		queueCounter++;
			counter++;
		}
			
		//re-run while loop above, only looking for -a flag. After the while loop counter position is oriented, Take the argument after -a and tokenize(because comma makes it one argument), then populate *algorithms with the tokenized strings.
		while(strcmp(argv[counter], "-a") != 1){
			counter --;
		}

		//oriented counter
		counter++;
		//tokenize strings from argument
		while( tokenPtr != NULL){
		tokenPtr = strtok(argv[counter], ",");
		algorithms[i] = tokenPtr;
		printf("Algos entered: %s", algorithms[i]);
		i++;
		}
//    for(i = 0; i < 4; i++){//converts array of algorithms you made above to lowercase.
   
//    	*algorithms[i] = tolower(algorithms[i]);
//    }                                                           //Ian: This won't work, tolower works on chars, not strings

/*    for(i = 0; i < 4; i++){                                    
    	if((md5flag = strcmp(algorithms[i], "md5")) == 0){
			MD5_Init(&md5);
    		}
    	else if((sha1flag = strcmp(algorithms[i], "sha1")) == 0){    		
    		SHA1_Init(&sha);
    		}

    	else if((sha256flag = strcmp(algorithms[i], "sha256")) == 0){
    		SHA256_Init(&sha256);
			}
		else if((sha512flag = strcmp(algorithms[i], "sha512")) == 0){
    		SHA512_Init(&sha512);
    	}
    	
		}//end for loop.
*/
    
    //TODO: Finish implementing threading. You have written the function for the thread, you've created the thread below. Now finish it.
    for(j = 0; j <= numOfThreads; j++){
    pthread_create(&identifier, NULL, &threadFunction, "I'm Mr Threadseeks, look at me!");
	}
	pthread_join(identifier, NULL);
    


    
		printf("If you got to here, the program executed successfully. TINY RIIICK!");
	}
