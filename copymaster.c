#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "options.h"


void FatalError(char c, const char* msg, int exit_status);
void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);

// === switches ===
void no_switches (struct CopymasterOptions cpm);
void fast_copy (struct CopymasterOptions cpm);
//void slow_copy (struct CopymasterOptions cpm);
// === switches ===


// === secondary functions ===
bool is_any_switch (struct CopymasterOptions cpm_options);
void check_errors (int file, char flag, int status);
// === secondary functions ===


int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);


    //-------------------------------------------------------------------
    // Kontrola hodnot prepinacov



    //if (cpm_options.fast) fast_copy(cpm_options);


    //-------------------------------------------------------------------

    // Vypis hodnot prepinacov odstrante z finalnej verzie
    
    PrintCopymasterOptions(&cpm_options);
    
    //-------------------------------------------------------------------
    // Osetrenie prepinacov pred kopirovanim
    //-------------------------------------------------------------------
    
    if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }
    
    // TODO Nezabudnut dalsie kontroly kombinacii prepinacov ...

    if (is_any_switch(cpm_options)) no_switches(cpm_options);

    if (cpm_options.fast) fast_copy(cpm_options);

    //if (cpm_options.slow) slow_copy(cpm_options);


    //-------------------------------------------------------------------
    
    // TODO Implementovat kopirovanie suborov
    
    // cpm_options.infile
    // cpm_options.outfile
    
    //-------------------------------------------------------------------
    // Vypis adresara
    //-------------------------------------------------------------------
    
    if (cpm_options.directory) {
        // TODO Implementovat vypis adresara
    }
        
    //-------------------------------------------------------------------
    // Osetrenie prepinacov po kopirovani
    //-------------------------------------------------------------------
    
    // TODO Implementovat osetrenie prepinacov po kopirovani
    
    return 0;
}


void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno); 
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}

void PrintCopymasterOptions(struct CopymasterOptions* cpm_options)
{
    if (cpm_options == 0)
        return;
    
    printf("infile:        %s\n", cpm_options->infile);
    printf("outfile:       %s\n", cpm_options->outfile);
    
    printf("fast:          %d\n", cpm_options->fast);
    printf("slow:          %d\n", cpm_options->slow);
    printf("create:        %d\n", cpm_options->create);
    printf("create_mode:   %o\n", (unsigned int)cpm_options->create_mode);
    printf("overwrite:     %d\n", cpm_options->overwrite);
    printf("append:        %d\n", cpm_options->append);
    printf("lseek:         %d\n", cpm_options->lseek);
    
    printf("lseek_options.x:    %d\n", cpm_options->lseek_options.x);
    printf("lseek_options.pos1: %ld\n", cpm_options->lseek_options.pos1);
    printf("lseek_options.pos2: %ld\n", cpm_options->lseek_options.pos2);
    printf("lseek_options.num:  %lu\n", cpm_options->lseek_options.num);
    
    printf("directory:     %d\n", cpm_options->directory);
    printf("delete_opt:    %d\n", cpm_options->delete_opt);
    printf("chmod:         %d\n", cpm_options->chmod);
    printf("chmod_mode:    %o\n", (unsigned int)cpm_options->chmod_mode);
    printf("inode:         %d\n", cpm_options->inode);
    printf("inode_number:  %lu\n", cpm_options->inode_number);
    
    printf("umask:\t%d\n", cpm_options->umask);
    for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options->umask_options[i][0] == 0) {
            // dosli sme na koniec zoznamu nastaveni umask
            break;
        }
        printf("umask_options[%u]: %s\n", i, cpm_options->umask_options[i]);
    }
    
    printf("link:          %d\n", cpm_options->link);
    printf("truncate:      %d\n", cpm_options->truncate);
    printf("truncate_size: %ld\n", cpm_options->truncate_size);
    printf("sparse:        %d\n", cpm_options->sparse);
}




// =======================================
// ===========  PREPINACE ================
// =======================================
void no_switches (struct CopymasterOptions cpm)
{
    int in, out, temp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'b', 21);

    /// open outfile
    out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_errors(out, 'b', 21);


    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    temp = read(in, &array, len);
    if (temp > 0) temp = write(out, &array, temp);
    if (temp < 0) FatalError('b', "INA CHYBA", 21);

    close(in);
    close(out);
}

void fast_copy(struct CopymasterOptions cpm)
{
    int in, out, temp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'f', 21);

    /// open outfile
    out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_errors(out, 'f', 21);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    temp = read(in, &array, len);
    if (temp > 0) temp = write(out, &array, temp);
    if (temp < 0) FatalError('b', "INA CHYBA", 21);

    close(in);
    close(out);
}

// =======================================







bool is_any_switch (struct CopymasterOptions cpm)
{
    if (cpm.fast == 0 && cpm.slow == 0 && cpm.create == 0 && cpm.overwrite == 0 &&
        cpm.append == 0 && cpm.lseek == 0 && cpm.directory == 0 && cpm.delete_opt == 0 &&
        cpm.chmod == 0 && cpm.inode == 0 && cpm.umask == 0 && cpm.link == 0 && cpm.truncate == 0 && cpm.sparse == 0)
        return true;

    return false;
}

void check_errors (int file, char flag, int status)
{
    if (file == ENOENT) FatalError(flag, "SUBOR NEEXISTUJE", status);
    else if (file == -1 || file == -2) FatalError(flag, "INA CHYBA", status);
}