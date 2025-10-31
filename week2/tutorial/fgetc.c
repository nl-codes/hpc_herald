#include <stdio.h>

int main(){

   FILE *fp ;
   char ch ;
   fp = fopen("E:\\HCK\\L6S1\\hpc\\week2\\week_2_ex\\file_fputc.txt", "r");

   while(1) {
      ch = fgetc (fp);
      if (ch == EOF)
         break;
      printf ("%c", ch);
   }
   printf ("\n");
   fclose (fp);
}