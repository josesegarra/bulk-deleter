#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#define BUFFER_SIZE 4096

int parseCommandLine(int argc, char* argv[]);                                                   // This one is defined in helper.c (not using a .h for the time being)
int confirm();                                                                                  // This one is defined in helper.c (not using a .h for the time being)
char* getItems();                                                                               // This one is defined in helper.c (not using a .h for the time being)
char* getPath();                                                                                // This one is defined in helper.c (not using a .h for the time being)
char* getLog();                                                                                 // This one is defined in helper.c (not using a .h for the time being)
int   getDryRun();                                                                              // This one is defined in helper.c (not using a .h for the time being)    
int delete_file(char* path, char* name, long int size, FILE* log);                              // This is just a forwared declaration

int rError(char* msg) {
    printf("**** %s\n", msg);
    return 1;
}

int main(int argc, char* argv[]) {
    if (parseCommandLine(argc, argv) == 0) return 0;                                            // Parse command line
    if (confirm() != 1) return 0;                                                               // Confirm

    FILE* ptr = fopen(getItems(), "r");                                                         // Open ITEMS file
    if (NULL == ptr) return rError("Items file can't be opened");

    FILE* log = fopen(getLog(), "w");                                                           // Create LOG FILE
    if (NULL == log) return rError("Log file can't be opened");

    char ch;
    char* path = getPath();
    char buffer[BUFFER_SIZE] = { '\0' };
    char* placeholder;
    int pos = 0, pos_comma = 0, total_files = 0, total_deleted = 0;
    long int total_size = 0;

    do {
        if (pos >= BUFFER_SIZE) return rError("Line too long");                                 // If we ever get to buffer overrun

        ch = fgetc(ptr);                                                                        // Read char from file    
        buffer[pos] = ch;                                                                       // Store it in buffer
        pos++;                                                                                  // Move to next pos
        if (ch == ',') pos_comma = pos;                                                         // If char was a comma then store current position (right after the comma)
        if (ch == '\n' && pos_comma == 0) pos = 0;                                              // If found an end of line but not a comma, then reset buffer (ie: skip empty lines)        

        if (ch == '\n' && pos_comma != 0) {                                                     // If founf end of line and we had a comma in the line
            buffer[pos_comma - 1] = '\0';                                                       // Set where the comma is located as terminator for the path 
            buffer[pos] = '\0';                                                                 // And set this as end of size
            long size = strtol(&buffer[pos_comma], &placeholder, 10);                           // Compute size
            if (delete_file(path, buffer, size, log)){                                          // If the file is deleted        
                total_deleted++;                                                                //   then increase counter of deleted files
                total_size = total_size + size;                                                 //   then increase size of deleted bytes
            }
            total_files++;                                                                      // In any case increase total number of lines (aka files) processed
            pos = 0;                                                                            // Move to beginning of buffer
            pos_comma = 0;                                                                      // and reset the marker for comma
        }   
    } while (ch != EOF);                                                                        // Continue until end of file    

    fclose(ptr);                                                                                // Close file
    fclose(log);                                                                                // Close log
    printf("Files processed: %i\n", total_files);                                               // Print statistics    
    printf("Files deleted:   %i\n", total_deleted);
    printf("Bytes deleted:   %li\n", total_size);
    return 0;
}


int delete_msg(FILE* log, char* msg,int c) {
    fprintf(log, c==0?" ****error: %s\n":" %s\n",msg);
    return c;
}

int delete_file(char* path,char* name, long int size, FILE* log) {
    struct stat st;

    if (path != NULL && name[0] != '/') {                                                       // If we have a PATH name is not an absolute path
        if (name[0] == '.' && name[1] == '/') name = name + 2;                                  // simplify special case ./whatever to whatever    
        char destination[2*BUFFER_SIZE];                                                        // Stack buffer
        strcpy(destination, path);                                                              // Copy path
        strcat(destination, name);                                                              // Copy destination
        name = destination;                                                                     // New name
    }
    fprintf(log,"%s    ", name);                                                                // Log file name
    if (!access(name, F_OK) == 0) return delete_msg(log,"could not be found", 0);               // If file doesn't exists log error
    stat(name, &st);
    if (st.st_size != size) return delete_msg(log, "has unexpected size",0);                    // If file site is different than expected log error
    if (getDryRun()) return delete_msg(log, "deleted !! (DRY_RUN)", 1);                         // If in DRY RUN mode log a success (DRY RUN)
    if (remove(name) != 0) return delete_msg(log, "failed delete",0);                           // If delete fails log error
    return delete_msg(log, "deleted !!", 1);                                                    // Log deletion
}



