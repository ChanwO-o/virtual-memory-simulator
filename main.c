#include <stdio.h>

void parsecmd(char * buf)
{
    char * cmd;

    cmd = strtok(buf, " \n");

    if (strcmp(cmd, "read") == 0)
    {
        printf("Inside read\n");
        cmd = strtok(buf, NULL);
    }
    else if (strcmp(cmd, "write") == 0)
    {
        printf("Write\n");
        cmd = strtok(buf, NULL);
    }
    else if (strcmp(cmd, "showmain") == 0)
    {
        printf("Showmain\n");
        cmd = strtok(buf, NULL);

    }
    else if (strcmp(cmd, "showdisk") == 0)
    {
        printf("Showdisk\n");
        cmd = strtok(buf, NULL);
  
    }
    else if (strcmp(cmd, "showptable") == 0)
    {
        printf("Showptable\n");
  
    }
}


int main()
{
    char buf[80];
    char * uinput;
    
    while (1)
    {
        printf("> ");

        if (fgets(buf, sizeof(buf), stdin) != NULL)
        { 
            if (strcmp(buf, "quit\n") == 0)
                break;

            parsecmd(buf);
     
        }
    }

    return 0;
}