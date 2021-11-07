
void umask_copy (struct CopymasterOptions cpm)
{
    mode_t MASK;
    //newmask = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

    
    MASK = 0;
    umask(0046);

    
    
    printf("\nMASK:%d\n", MASK);


    char BUF;

    for (int i = 0; cpm.umask_options[i][0]; ++i, printf("MASK: %d\n", MASK)) {
        //printf("%d\n", i);
        if (cpm.umask_options[i][0] == 'o' && cpm.umask_options[i][2] == 'r') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IROTH);
            else if (BUF == '-') MASK = (MASK | S_IROTH);
        }
        else if (cpm.umask_options[i][0] == 'o' && cpm.umask_options[i][2] == 'w') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IWOTH);
            else if (BUF == '-') MASK = (MASK | S_IWOTH);
        }

        else if (cpm.umask_options[i][0] == 'o' && cpm.umask_options[i][2] == 'x') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IXOTH);
            else if (BUF == '-') MASK = (MASK | S_IXOTH);
        }

// -------------------------------------------------------------------------------------

        if (cpm.umask_options[i][0] == 'g' && cpm.umask_options[i][2] == 'r') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IRGRP);
            else if (BUF == '-') MASK = (MASK | S_IRGRP);
        }
        else if (cpm.umask_options[i][0] == 'g' && cpm.umask_options[i][2] == 'w') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IWGRP);
            else if (BUF == '-') MASK = (MASK | S_IWGRP);
        }
        else if (cpm.umask_options[i][0] == 'g' && cpm.umask_options[i][2] == 'x') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IXGRP);
            else if (BUF == '-') MASK = (MASK | S_IXGRP);
        }

// -------------------------------------------------------------------------------------

        if (cpm.umask_options[i][0] == 'u' && cpm.umask_options[i][2] == 'r') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IRUSR);
            else if (BUF == '-') MASK = (MASK | S_IRUSR);

        }
        else if (cpm.umask_options[i][0] == 'u' && cpm.umask_options[i][2] == 'w') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IWUSR);
            else if (BUF == '-') MASK = (MASK | S_IWUSR);
        }
        else if (cpm.umask_options[i][0] == 'u' && cpm.umask_options[i][2] == 'x') {
            if ((BUF = cpm.umask_options[i][1]) == '+') MASK = (MASK ^ S_IXUSR);
            else if (BUF == '-') MASK = (MASK | S_IXUSR);
        }
    }

    if ((int)MASK > 777 || (int)MASK <= 0) FatalError('u', "ZLE PRAVA", 32);
    
    /// ***** new
    umask(MASK);
    chmod(cpm.outfile, (cpm.create_mode) - MASK);

    
    
    //creat(cpm.outfile, MASK);
    int in, out, tmp;

    in = open(cpm.infile, O_RDONLY);
    check_errors(in, 'u', 32);

    /// open outfile
    out = open(cpm.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    check_errors(out, 'u', 32);

    //chmod(cpm.outfile, MASK);

    long int len = lseek(in, 0, SEEK_END);
    char array[len];
    lseek(in, 0, SEEK_SET);

    (tmp = read(in, &array, len)) != 0 ? write(out, &array, tmp) : FatalError('u', "INA CHYBA", 32);

    close(in);
    close(out);
}
