#include <stdio.h>

int main() {

   FILE *fp;
   char *s;
   int i,a;  
   float p;

   fp = fopen("E:\\HCK\\L6S1\\hpc\\week2\\week_2_ex\\file_fprintf.txt", "r");

   if (fp == NULL) {
      puts ("Cannot open file"); return 0;
   }

   while (!feof(fp)) {
    fscanf(fp, "%d %f %s", &a, &p, s);
    printf("Name: %s Age: %d Percent: %f\n", s, a, p);
   }
 
   fclose(fp);

   return 0;
}