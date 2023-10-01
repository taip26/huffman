
/* Function to get no of bits in binary
   representation of positive integer */
#include <stdio.h>         
     
unsigned int countBits(unsigned int n)
{
   unsigned int count = 0;
   while (n)
   {
        count++;
        n >>= 1;
    }
    return count;
}
   
/* Driver program*/
int main()
{
    int i = 65;
    printf("%d", countBits(i));
    return 0;
}