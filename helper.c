#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>


char* file_name = NULL;                                                                     // The file we are checking
char* path = NULL;                                                                          // Path to prepend
char* file_log = "./deleter_log.txt";                                                       // File log
int   dry_run = 0;                                                                          // DRY RUN mode    
int   help = 0;                                                                             // Just display help
int   auto_accept= 0;                                                                       // No user confirmation


int pError(char* msg,char* s) {
    printf("**** %s",msg);
    if (s != NULL) printf("%s", s);
    printf(". Use -h to view help.\n");
    return 0;
}

int checkFlag(char* flag, char* value) {
    if (flag[0] != '-') return pError("Unknown command: ", flag);

    if (flag[2] == '\0' && (flag[1] == 'f' || flag[1] == 'p' || flag[1] == 'l') ) {
        if (value == NULL) return pError(flag," requires a value");
        if (flag[1] == 'f') file_name = value;
        if (flag[1] == 'p') path = value;
        if (flag[1] == 'l') file_log = value;
        return 1;
    }
    if (flag[1] == 'h' && flag[2] == '\0' ) { help= 1;return 0;}
    if (flag[1] == 'd' && flag[2] == '\0') {dry_run= 1;return 0;}
    if (flag[1] == 'y' && flag[2] == '\0') { auto_accept= 1; return 0; }
    return pError("Unknown flag: %s\n", flag);
}

int showHelp() {
    printf("Mass deleter 2024-April\n"); 
    printf("deleter -f file [-l log] [-p path_value] [-h] [-d]\n\n");
    printf("  -f       files with list of items to delete. item format: <path>,<size>\n");
    printf("  -p       if <path> to delete is not an absolute path then prepend path_value to the file to delete\n");
    printf("  -l       log file. Default is ./deleter_log.txt\n");
    printf("  -d       dry-run mode, nothing gets deleted\n");
    printf("  -h       display this help\n");
    printf("  -y       no user confirmation\n");
    printf("\n");
    return 0;
}


// https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
int key_press() { 
    struct termios term;
    tcgetattr(0, &term);
    
    term.c_lflag &= ~(ICANON | ECHO);                                                                       // turn off line buffering and echoing
    tcsetattr(0, TCSANOW, &term);
    int nbbytes;
    ioctl(0, FIONREAD, &nbbytes); // 0 is STDIN
    while (!nbbytes) {
        sleep(0.01);
        fflush(stdout);
        ioctl(0, FIONREAD, &nbbytes); // 0 is STDIN
    }
    int key = (int)getchar();
    term.c_lflag |= (ICANON | ECHO); // turn on line buffering and echoing
    tcsetattr(0, TCSANOW, &term);
    return key;
}

int confirm() {
    printf("Items file:      %s\n", file_name);
    printf("Log file:        %s\n", file_log);
    printf("Path prefix:     %s\n", path == NULL ? "None" : path);
    printf("Dry-Run:         %s\n", dry_run == 1 ? "Yes" : "No");
    if (auto_accept) return 1;
    printf("\nPress [Y] to proceed: ");
    if (key_press() == 121) {
        printf("Y\n");
        return 1;
    }
    printf("\n");
    return pError("User aborted", NULL);
}



int parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) i = i + checkFlag(argv[i], (i < argc - 1) ? argv[i + 1] : NULL);
    if (help) return showHelp();
    if (file_name == NULL) return pError("Missing file with items to delete", NULL);
    return 1;
}

char* getItems() { return file_name;  }
char* getPath() { return path; }
char* getLog() { return file_log; }
int   getDryRun() { return dry_run; }







