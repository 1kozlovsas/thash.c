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


int main(int argc, char *argv[]) {

int i;//used to keep track of arguments.
int j;//used for nested for loop
int md5flag = 0;
int sha1flag = 0;
int sha256flag = 0;
int sha512flag = 0;
unsigned char md5hash[MD5_DIGEST_LENGTH];
unsigned char sha1hash[SHA_DIGEST_LENGTH];
unsigned char sha256hash[SHA256_DIGEST_LENGTH];
unsigned char sha512hash[SHA512_DIGEST_LENGTH];
int errcheck;//variable used for error checking
char *algorithms[5];
char *fileReadName = NULL;
char *errFileName = NULL;
char *fileWriteName = NULL;
char buffer[1024];//buffer to hold each line from fgets in.
int numOfThreads;
FILE *fpr;
FILE *fpw;
FILE *fpe;//error writing file pointer.
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
            printf("Unrecognized option: -%c\n", optopt);
            break;
        default:
        	printf("Usage: thash -a algorithms file1...file2...");
        	exit(EXIT_FAILURE);

        }

    for(i = 0; i < 4; i++){
    	*algorithms[i] = tolower(algorithms[i]);
    }
    //printf("We getting to here?");
    for(i = 0; i < 4; i++){
    	if((md5flag = strcmp(algorithms[i], "md5")) == 0){
    		MD5_CTX md5;
			MD5_Init(&md5);
    		if(fileReadName != NULL){
    			while(!feof(fpr)){
    			fgets(buffer, 1024, fpr);
    			MD5_Update(&md5, buffer, sizeof(buffer));	
    			MD5_Final(md5hash, &md5);
    			for(j = 0; j < sizeof(buffer); j++) 
    				printf (" %s\n", buffer);
    				printf("%02x", md5hash[j]);
    			}
    		}
    		else{
    			while(errcheck = getopt(argc, argv, "t::") != 1){
    			MD5_Update(&md5, argv[argc -1], sizeof(argv[argc -1]));
    			MD5_Final(argv[argc], &md5);
    			for(j = 0; j < sizeof(argv[argc -1]); j++) 
    				printf (" %s\n", argv[argc]);
    				printf("%02x", md5hash[j]);
    			}
    			argc--;
    		}

    		}
    else if((sha1flag = strcmp(algorithms[i], "sha1")) == 0){
    		SHA_CTX sha;
    		SHA1_Init(&sha);
    		if(fileReadName != NULL){
    			while(!feof(fpr)){
    			fgets(buffer, 1024, fpr);
    			SHA1_Update(&sha, buffer, sizeof(buffer));	
    			SHA1_Final(sha1hash, &sha);
    			for(j = 0; j < sizeof(buffer); j++) 
    				printf (" %s\n", fileReadName);
    				printf("%02x", sha1hash[j]);
    			}
    		}
    		else{
    			while(errcheck = getopt(argc, argv, "t::") != 1){
    			SHA_Update(&sha, argv[argc -1], sizeof(argv[argc -1]));
    			SHA_Final(argv[argc], &sha);
    			for(j = 0; j < sizeof(argv[argc -1]); j++) 
    				printf (" %s\n", argv[argc]);
    				printf("%02x", sha1hash[j]);
    			}
    			argc--;
    		}

    }
    else if((sha256flag = strcmp(algorithms[i], "sha256")) == 0){
    		SHA256_CTX sha256;
    		SHA256_Init(&sha256);
    		if(fileReadName != NULL){
    			while(!feof(fpr)){
    			fgets(buffer, 1024, fpr);
    			SHA256_Update(&sha256, buffer, sizeof(buffer));	
    			SHA256_Final(sha256hash, &sha256);
    			for(j = 0; j < sizeof(buffer); j++) 
    				printf (" %s\n", fileReadName);
    				printf("%02x", sha256hash[j]);
    			}
    		}
    		else{
    			while(errcheck = getopt(argc, argv, "t::") != 1){
    			SHA256_Update(&sha256, argv[argc -1], sizeof(argv[argc -1]));
    			SHA256_Final(argv[argc], &sha256);
    			for(j = 0; j < sizeof(argv[argc -1]); j++) 
    				printf (" %s\n", argv[argc]);
    				printf("%02x", sha256hash[j]);
    			}
    			argc--;
    		}
    
}
else if((sha512flag = strcmp(algorithms[i], "sha512")) == 0){
    		SHA512_CTX sha512;
    		SHA512_Init(&sha512);
    		if(fileReadName != NULL){
    			while(!feof(fpr)){
    			fgets(buffer, 1024, fpr);
    			SHA512_Update(&sha512, buffer, sizeof(buffer));	
    			SHA512_Final(sha512hash, &sha512);
    			for(j = 0; j < sizeof(buffer); j++) 
    				printf (" %s\n", fileReadName);
    				printf("%02x", sha512hash[j]);
    			}
    		}
    		else{
    			while(errcheck = getopt(argc, argv, "t::") != 1){
    			SHA512_Update(&sha512, argv[argc -1], sizeof(argv[argc -1]));
    			SHA512_Final(argv[argc], &sha512);
    			for(j = 0; j < sizeof(argv[argc -1]); j++) 
    				printf (" %s\n", argv[argc]);
    				printf("%02x", sha512hash[j]);
    			}
    			argc--;
    		}






	}
}
}
}
