#include <stdio.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(int argc, char *argv[]){

int i;//used to keep track of arguments.
int errcheck;//variable used for error checking
char algorithms[5];
char *fileReadName = NULL;
char *errFileName = NULL;
char *fileWriteName = NULL;
int numOfThreads;
FILE *fpr;
FILE *fpw;
FILE *fpe;//error writing file pointer.

while((errcheck = getopt(argc, argv, "a:f::e::o::t::")) != 1){
	switch(errcheck){
		case 'a':
			optind--;//external int used by getopt
			for(; optind < argc && *argv[optind] != '-'; optind++){
				algorithms[optind] = strtol(argv[optind], NULL, 10);
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
            printf(stderr,"Unrecognized option: -%c\n", optopt);
            break;
        default:
        	printf("Usage: thash -a algorithms file1...file2...");
        	exit(EXIT_FAILURE);

        	







	}
}

}