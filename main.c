#include <stdio.h>

#define PGNUM 8
#define DISKSIZE 8
#define MMSIZE 4

struct pt_entry
{
    int entry;
    int valid;
    int dirty;
    int pgnum;
};

struct page
{
    int data[4];
    int originaddr;
};

struct pt_entry ptable[PGNUM];

struct page diskmem[DISKSIZE];

struct page mainmem[MMSIZE];

int addrtrans(int addr)
{
    return addr / 4;
}

int addrtransoff(int addr)
{
    return addr % 4;
}

//accesses the address and offset in ptable
//and does relevant calculations.
void accessptable(int addr, int offset)
{
    struct pt_entry * entry = ptable[addr];
 
    //if not valid then access disk
    if (entry->valid == 0)
    {
        //access disk due to page fault
    
    }
    //if valid access main memory
    else
    {
        //access main memory
    }
}

//copy index address in disk into main memory
void copypage(int addr)
{
    struct page * pg = diskmem[addr];
    pg->originaddr = addr;


}





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