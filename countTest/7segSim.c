#include <stdio.h>
#include <stdlib.h>

void leftDigit();

void rightDigit();

char printHexVal(int num);



int main()
{
   leftDigit();
   return 0;
}



void leftDigit()
{ 
   int num = 0, num2 = 0, newNum = 0;

   for (int i = 0; i < 40; i++)
   {
      num++;
      newNum = num % 16;
      
      if (num <=9)
      {
         printf("%d\n", num);
      }
      else if (newNum >= 10 && newNum <=15)
      {
         if (i >= 25)
         {

         }
         char val; 
         val = printHexVal(newNum);
         printf("%c\n", val);
      }
      else if ((num >= 16 && num < 26) || (num >= 26 && num < 36))
      {
         num2 = num - 6;
         printf("%d\n", num2);
      }
      else
      {

      }
   }
}

void rightDigit()
{
}

char printHexVal(int num)
{

   switch(num)
   {
      case 10:
         return 'a';
      break;

      case 11:
         return 'b';
      break;

      case 12:
         return 'c';
      break;

      case 13:
         return 'd';
      break;

      case 14:
         return 'e';
      break;

      case 15:
         return 'f';
      break;
   }
}