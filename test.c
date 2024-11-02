#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void concatenate_string(char* s, char* s1)
{
    int i;
 
    int j = strlen(s);
 
    for (i = 0; s1[i] != '\0'; i++) {
        s[i + j] = s1[i];
    }
 
    s[i + j] = '\0';
 
    return;
}


int main(int nb, char ** arg)
{
    //FILE *f = fopen("$temp$.###", "r");
    //int status = system(" > $temp$.###");


    for (int i = 0; i<10; i++){
        printf("%d\n", i);
        for (int j = 0; j<8; j++){
            float alpha = 0.845+((float)(j-5)/40);
            int n = i+3;

            char str[100] = "./tsp g50 1 ";
            char str2[100] = " ";
            char str3[100] = " >> $temp$.###";

            char snum[100];
            char buf[100];
            char c;

            sprintf(snum, "%d", n);


            gcvt(alpha, 3, buf); 
            //itoa(n, snum, 10);
            concatenate_string(str, buf);

            concatenate_string(str, str2);
            concatenate_string(str, snum);
            concatenate_string(str, str3);

            int status = system(str);
            //printf(str);
        }
    }
    return 0;
    



    /*

    return 0;

    while(!feof(f))
    {        
        return 0;
        c = fgetc(f); printf("%c", c);
    }
    
    fclose(f);
    remove ("$temp%.###");
    
    return 0;*/

}