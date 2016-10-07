/////comment
//Anotha one 
#include <stdio.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

int i;//used to keep track of arguments.
int j;//used for nested for loop
int md5flag = 0;
int sha1flag = 0;
int sha256flag = 0;
int sha512flag = 0;
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
char *algorithms[5];//array of strings entered after -m flag.
char *fileReadName = NULL;//specified with -f flag.
char *errFileName = NULL;//specified with -e flag.
char *fileWriteName = NULL;//specified with -o flag.
char buffer[1024];//buffer to hold each line from fgets in.
int numOfThreads;//supplied with -t flag.
int queueCounter = 0;//counter to keep track of file names in queue.
FILE *fpr;//pointer to open file specified in -f flag
FILE *fprFromFile;//pointer to files listed in a specified file using the -f flag.
FILE *fpw;//used to write to a file using the -o flag.
FILE *fpe;//pointer to file used to print errors, specified using -e flag.
pthread_t identifier;//thread identifier
front = rear = NULL;//empty queue created

void *threadFunction(void *arg);
{
	//dequeing from the queue
	front1 = front;
 
    if (front1 == NULL)
    {
    	if(errFileName != NULL){
	   		fprintf(errFileName, "\n Error: Trying to display elements from empty queue");
	   		printf("Something has failed, way to go, JERRY. See error file %s for more details.", errFileName);
	   		}
        printf("\n Error: You're trying to display elements from an empty queue. That would be Rick-Diculous!");
        return;
    }
    else
        if (front1->ptr != NULL)
        {
            front1 = front1->ptr;
            if(&md5 != NULL)
            MD5_Update(&md5, front->info, sizeof(node));	
    		MD5_Final(md5hash, &md5);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", md5hash[j]);
    			}
    		else if(&sha != NULL)
            SHA1_Update(&sha, front->info, sizeof(node));	
    		SHA1_Final(sha1hash, &sha);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", sha1hash[j]);
    			}
    		else if(&sha256 != NULL)
            SHA256_Update(&sha256, front->info, sizeof(node));	
    		SHA256_Final(sha256hash, &sha256);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", sha256hash[j]);
    			}
    		else if(&sha512 != NULL)
            SHA512_Update(&sha512, front->info, sizeof(node));	
    		SHA512_Final(sha512hash, &sha512);
    			for(j = 0; j < sizeof(node); j++){
    				printf (" %s\n", front->info);
    				printf("%02x", sha256hash[j]);
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
        count--;

}
//defining a queue
struct node
{
    int info;
    struct node *ptr;
}*front,*rear,*temp,*front1;

int main(int argc, char *argv[]) {
printf("Hi, I'm Mr Meseeks, look at me! I heard you want to hash some files. CAAAN DO!");



while((errcheck = getopt(argc, argv, "a:f::e::o::t::")) != 1) {
	switch(errcheck){
		case 'a':
			optind--;//external int used by getopt
			for(; optind < argc - 1 && *argv[optind] != '-'; optind++){
				algorithms[optind] = argv[optind];
			}
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
            printf("Option -%c requires an argument.\n", optopt);//optopt is an external int.
            break;
        case '?':                           
            printf("Morty, you're not using it right. You can't put -%c into this program. \n ", optopt);
            break;
        default:
        	printf("You really squanched it up. The usage for this is: thash -a algorithms file1...file2...");
        	exit(EXIT_FAILURE);

        }

		//obtaining a list of file names.
        if(fileReadName != NULL){
    		while(!feof(fpr)){
	   	if(fprFromFile = fopen(fgets(buffer, 1024, fpr), "r") == NULL){
	   		if(errFileName != NULL){
	   		fprintf(errFileName, "Error reading file names from file specified");
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
        		rear->info = fprFromFile;
        		front = rear;
    			}
    		 else {
        		temp=(struct node *)malloc(1*sizeof(struct node));
        		rear->ptr = temp;
        		temp->info = fprFromFile;
        		temp->ptr = NULL;
        		rear = temp;
	   			}
	   			queueCounter++;
}
}//end while
}//end if fileReadName != NULL check.
		argc--;//decrement argc by 1 since number of args starts at 0.
		int counter = argc;//assiging argc to a placeholder counter for readability
		//This while loop is used to locate the last valid file names to populate queue with.
		while(strcmp(argv[counter], "-f") != 1 || strcmp(argv[counter], "-a") != 1 || strcmp(argv[counter], "-o") != 1 || strcmp(argv[counter], "-e") != 1 || strcmp(argv[counter], "-t") != 1){
			counter--;
		}

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
			
//TODO: re-run while loop above, only looking for -a flag. After the while loop counter position is oriented, Take the argument after -a and tokenize(because comma makes it one argument), then populate *algorithms with the tokenized strings.
		while(strcmp(argv[counter], "-a") != 1){
			counter --;
		}
		//oriented counter
		counter++;
		//tokenize strings from argument
		while( token != NULL){
		token = strtok(argv[counter], ",");
		*algorithms[i] = token;
		printf("Algos entered: %s", *algorithms[i]);
		i++;
		}
    for(i = 0; i < 4; i++){//converts array of algorithms you made above to lowercase.
   
    	*algorithms[i] = tolower(algorithms[i]);
    }



    //printf("We getting to here?");
    //TODO: CLEAN UP HASHING FUNCTIONS. Write one perfect for md5, then write the rest using the md5 as a template.
    //TODO: Implement threading.
    for(j = 0; j <= numOfThreads; j++){
    pthread_create(&identifier, NULL, threadFunction, "I'm Mr Threadseeks, look at me!");
	}
    


    for(i = 0; i < 4; i++){
    	if((md5flag = strcmp(algorithms[i], "md5")) == 0){
    		
			MD5_Init(&md5);
    		
    		}
    		

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
    	printf("If you got to here, the program executed successfully. TINY RIIICK!");
}

