# include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *fptr;
    char *line;

    fptr = fopen("E:\\HCK\\L6S1\\hpc\\week2\\week_2_ex\\file_fputs.txt", "r");

    if (fptr == NULL) {
        printf("Error opening  file %s\n");
        exit(-1);
    }

    while (!feof(fptr))
    {
        fgets(line,100,fptr);//reads at most 99 characters from the file.(remaining one for null character)
        printf("%s",line);
    }
    fclose(fptr);
    return 0;
}