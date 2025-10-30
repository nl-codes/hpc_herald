#include <stdio.h>

struct employee {
   int age;
   float percent;
   char* name;
};

int main() {
   FILE* fp;
   struct employee emp[] = {
      {25, 65.5, "Ravi"},
      {21, 75.5, "Roshan"},
      {24, 60.5, "Reena"}
   };

   fp = fopen("E:\\HCK\\L6S1\\hpc\\week2\\week_2_ex\\file_fprintf.txt", "w");

   for (int i = 0; i < 3; i++) {
      fprintf(fp, "%d %f %s\n", emp[i].age, emp[i].percent, emp[i].name);
   }
   fclose(fp);
   return 0;
}
