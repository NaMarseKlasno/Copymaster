#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include "options.h"

/**                         <RACCOON>
 *
 *                                    .                : :
 *                  _..----..__   __..:'.-'''-.-''    .  :
 *                .'      ,    '''    '    :   .'    /  '
 *               ',                  ( -=o):(o=-)   .  :
 *              :     ,               ''.  ;  .'  __:  :
 *              :          :      ,      '.0.''.-'.))  :  __..--
 *              :           :                ._.-'__| ':''.
 *               .           :   ,   ..  :.-' __.' /   ;    .
 *              .'       ,   :    _.'  '. '.''    /   /  '
 *            .:. .'.        :--:'_..--'''.))  .  ' -'    __.--'
 *          .''::'   '-.  .-''.  '.   .             __.--'
 *          :...:     __\  '.  '..))     '    __.--'
 *          ::'':.--''   '.)))          __.--'
 */



void FatalError(char c, const char* msg, int exit_status);
void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);

// === switches ===
void no_switches (struct CopymasterOptions cpm);
void fast_copy (struct CopymasterOptions cpm);
void slow_copy (struct CopymasterOptions cpm);
void create_copy (struct CopymasterOptions cpm);
void overwrite_copy (struct CopymasterOptions cpm);
void append_copy (struct CopymasterOptions cpm);
void lseek_copy (struct CopymasterOptions cpm);
void lseek_copy (struct CopymasterOptions cpm);
void directory_copy (struct CopymasterOptions cpm);
void delete_copy (struct CopymasterOptions cpm);
void chmod_copy (struct CopymasterOptions cpm);

// === switches ===


// === secondary functions ===
bool is_any_switch (struct CopymasterOptions cpm_options);
void check_errors (int file, char flag, int status);
// === secondary functions ===


int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);
    
    PrintCopymasterOptions(&cpm_options);

    if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }
    
    // TODO Nezabudnut dalsie kontroly kombinacii prepinacov ...

    if (is_any_switch(cpm_options))  no_switches(cpm_options);

    if (cpm_options.fast)            fast_copy(cpm_options);

    if (cpm_options.slow)            slow_copy(cpm_options);

    if (cpm_options.create)          create_copy(cpm_options);

    if (cpm_options.overwrite)       overwrite_copy(cpm_options);

    if (cpm_options.append)          append_copy(cpm_options);

    if (cpm_options.lseek)           lseek_copy(cpm_options);

    if (cpm_options.directory)       directory_copy(cpm_options);

    if (cpm_options.delete_opt)      delete_copy(cpm_options);

    if (cpm_options.chmod_mode)      chmod_copy(cpm_options);

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
    int in, out, tmp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'f', 21);

    /// open outfile
    out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_errors(out, 'f', 21);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    (tmp = read(in, &array, len)) > 0 ? write(out, &array, tmp) : FatalError('f', "INA CHYBA", 21);

    close(in);
    close(out);
}

void slow_copy (struct CopymasterOptions cpm)
{
    int in, out;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'f', 21);

    /// open outfile
    out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_errors(out, 'f', 21);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    for (int i = read(in, &array, 1); i > 0; write(out, &array, i), i = read(in, &array, 1));

    close(in);
    close(out);
}

void create_copy (struct CopymasterOptions cpm)
{
    int in, out = 0, tmp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'c', 23);

    /// open outfile
    if (cpm.create_mode > 777 || cpm.create_mode <= 0) FatalError('c', "ZLE PRAVA", 23);

    open(cpm.outfile, O_RDONLY) < 0 ? (out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, cpm.create_mode)) :  FatalError('c', "SUBOR EXISTUJE", 23);
    check_errors(out, 'c', 23);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    (tmp = read(in, &array, len)) > 0 ? write(out, &array, tmp) : FatalError('c', "INA CHYBA", 23);

    close(in);
    close(out);
}

void overwrite_copy (struct CopymasterOptions cpm)
{
    int in, out = 0, tmp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'o', 24);

    /// open outfile
    open(cpm.outfile, O_RDONLY) < 0 ? FatalError('o', "SUBOR NEEXISTUJE", 24) : (out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644));
    check_errors(out, 'o', 24);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    (tmp = read(in, &array, len)) > 0 ? write(out, &array, tmp) : FatalError('o', "INA CHYBA", 24);

    close(in);
    close(out);
}

void append_copy (struct CopymasterOptions cpm)
{
    int in, out = 0, tmp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'a', 22);

    /// open outfile
    open(cpm.outfile, O_RDONLY) < 0 ? FatalError('a', "SUBOR NEEXISTUJE", 22) : (out = open(cpm.outfile, O_WRONLY | O_CREAT | O_APPEND, 0644));
    check_errors(out, 'a', 22);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    (tmp = read(in, &array, len)) > 0 ? write(out, &array, tmp) : FatalError('a', "INA CHYBA", 22);

    close(in);
    close(out);
}

void lseek_copy (struct CopymasterOptions cpm)
{
    int in, out = 0, tmp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'l', 33);

    /// open outfile
    open(cpm.outfile, O_RDONLY) < 0 ? FatalError('l', "SUBOR NEEXISTUJE", 33) : (out = open(cpm.outfile, O_WRONLY, 0644));
    check_errors(out, 'l', 33);

    int len = cpm.lseek_options.num;
    char array[len];

    lseek(in,cpm.lseek_options.pos1,SEEK_SET);

    switch (cpm.lseek_options.x)
    {
        case 0:
            lseek(out, cpm.lseek_options.pos2, SEEK_SET);
            break;
        case 1:
            lseek(out, cpm.lseek_options.pos2, SEEK_CUR);
            break;
        case 2:
            lseek(out, cpm.lseek_options.pos2, SEEK_END);
            break;
        default:
            break;
    }

    (tmp = read(in, &array, len)) > 0 ? write(out, &array, tmp) : FatalError('l', "INA CHYBA", 33);

    close(in);
    close(out);
}

void directory_copy (struct CopymasterOptions cpm)
{
    DIR *dp;

    char buff[20], buf[1024], buf1[1024];;

    struct dirent *entry;
    struct stat statbuf;
    struct passwd pwent, *pwentp;
    struct group grp, *grpt;


    if ((dp = opendir(".")) == NULL) {
        fprintf(stderr,"cannot open directory\n");
        return;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        //if (strcmp(entry->d_name, cpm.infile) != 0) continue;
        lstat(entry->d_name,&statbuf);

        strftime(buff, sizeof(buff), "%b %d %H:%M", localtime(&statbuf.st_atime));

        /// Found a directory, but ignore . and ..
        if (strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0) continue;


        FILE *out = fopen(cpm.outfile, "a");
        if (out == NULL) FatalError('D', "VYSTUPNY SUBOR - CHYBA", 28);

        fputs((S_ISDIR(statbuf.st_mode)) ? "d" : "-", out);
        fputs((statbuf.st_mode & S_IRUSR) ? "r" : "-", out);
        fputs((statbuf.st_mode & S_IWUSR) ? "w" : "-", out);
        fputs((statbuf.st_mode & S_IXUSR) ? "x" : "-", out);
        fputs((statbuf.st_mode & S_IRGRP) ? "r" : "-", out);
        fputs((statbuf.st_mode & S_IWGRP) ? "w" : "-", out);
        fputs((statbuf.st_mode & S_IXGRP) ? "x" : "-", out);
        fputs((statbuf.st_mode & S_IROTH) ? "r" : "-", out);
        fputs((statbuf.st_mode & S_IWOTH) ? "w" : "-", out);
        fputs((statbuf.st_mode & S_IXOTH) ? "x" : "-", out);


        getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp);
        getgrgid_r (statbuf.st_gid, &grp, buf1, sizeof(buf1), &grpt);

        fprintf(out, "%3d %s %s %5lld %10s %s\n", statbuf.st_nlink,pwent.pw_name, grp.gr_name, statbuf.st_size, buff, entry->d_name);

        //closedir(dp);

        //return;
    }
    //FatalError('D', "VSTUPNY SUBOR NIE JE ADRESAR", 28);

    closedir(dp);
}

void delete_copy (struct CopymasterOptions cpm)
{
    int in, out, tmp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'd', 26);

    /// open outfile
    out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_errors(out, 'd', 26);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    (tmp = read(in, &array, len)) > 0 ? write(out, &array, tmp) : FatalError('f', "INA CHYBA", 26);

    remove(cpm.infile);

    in = open(cpm.infile, O_RDONLY);
    in >= 0 ? FatalError('D', "SUBOR NEBOL ZMAZANY", 26) : close(in), close(out);
}

void chmod_copy (struct CopymasterOptions cpm)
{
    if (cpm.create_mode > 777 || cpm.create_mode <= 0) FatalError('m', "ZLE PRAVA", 34);

    int in, out, tmp;

    /// open infile
    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'm', 34);

    /// open outfile
    out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, cpm.create_mode);
    check_errors(out, 'm', 34);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    (tmp = read(in, &array, len)) > 0 ? write(out, &array, tmp) : FatalError('m', "INA CHYBA", 34);

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