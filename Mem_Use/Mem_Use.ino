#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

#include "malloc.h"


int heapSize(){
    return mallinfo().uordblks;
}
 
extern char _end;
extern "C" char *sbrk(int i);
char *ramstart=(char *)0x20070000;
char *ramend=(char *)0x20088000;
Time t;

void ShowMemory(void)
{
  struct mallinfo mi=mallinfo();

  char *heapend=sbrk(0);
  register char * stack_ptr asm("sp");

  printf("    arena=%d\n",mi.arena);
  printf("  ordblks=%d\n",mi.ordblks);
  printf(" uordblks=%d\n",mi.uordblks);
  printf(" fordblks=%d\n",mi.fordblks);
  printf(" keepcost=%d\n",mi.keepcost);
  
  printf("RAM Start %lx\n", (unsigned long)ramstart);
  printf("Data/Bss end %lx\n", (unsigned long)&_end);
  printf("Heap End %lx\n", (unsigned long)heapend);
  printf("Stack Ptr %lx\n",(unsigned long)stack_ptr);
  printf("RAM End %lx\n", (unsigned long)ramend);

  printf("Heap RAM Used: %d\n",mi.uordblks);
  printf("Program RAM Used %d\n",&_end - ramstart);
  printf("Stack RAM Used %d\n",ramend - stack_ptr);

  printf("Estimated Free RAM: %d\n\n",stack_ptr - heapend + mi.fordblks);
}




void setup()
{
  Serial.begin(9600);
  rtc.begin();
  Time t = rtc.getTime();
}


void loop()
{
  Serial.println("-------------------------------");
  ShowMemory();
  for (int i=0; i<10000; i++) {
    t = rtc.getTime();
  }
  ShowMemory();
}
