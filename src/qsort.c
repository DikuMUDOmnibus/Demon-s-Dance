/*      qsort.c                 John Nelson             2-Dec-1994
 *
 *      Demonstrate the quicksort function
 */
/****************************************************************************
*	Demon's Dance MUD, and source code are property of Eric Goetschalckx	*
*	By compiling this code, you agree to include the following in your		*
*	login screen:															*
*	    Derivative of Demon's Dance, by Enzo/Stan							*
***************************************************************************/
#include <stdio.h>

void quicksort(int arr[], const int lo, const int hi);
void printTable(const int arr[], const int lo, const int hi);


int main_sort(void)
{
   int table[]= {3,14,12,6,10,9,2,18,11,21,5,13};
   int tableSize = sizeof(table)/sizeof(int);
   printf("Quicksort test program\n\n");
   quicksort(table, 0, tableSize-1);

   printTable(table, 0, tableSize-1);

   return 0;
}


/* quicksort                    A recursive quicksort function for integer*/
void quicksort(int arr[], const int lo, const int hi)
{
   int low, high, pivot, temp;
   low = lo;
   high = hi;
   printf("Quicksort Partition:  %2d to %2d\n", lo, hi);
   if (low < high)
     {
     printTable(arr, low, high);
     pivot = arr[high];
     do
       {
       while (low < high && arr[low] <= pivot)
          low++;
       while (high > low && arr[high] >= pivot)
          high--;
       if (low < high)                  /* Elements in wrong partition */
          {
          temp = arr[low];  arr[low] = arr[high];  arr[high] = temp;
          }
       }
     while (low < high);
     temp = arr[low];  arr[low] = arr[hi];  arr[hi] = temp;
     quicksort(arr, lo, low-1);
     quicksort(arr, low+1, hi);
     }
}

/* printTable                                                           */

void printTable(const int arr[], const int lo, const int hi)
{
   int i;
   for(i=lo; i <= hi; i++)
      printf("%3d ", arr[i]);
   putchar('\n');
}

