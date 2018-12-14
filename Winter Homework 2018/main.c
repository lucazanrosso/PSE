#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 9

int start = -30;
int finish = 20;
int position = 0;
bool increment = true;

static void redraw(void);

int main(void)
{
    const int trigger   = (CLOCKS_PER_SEC * 50) / 1000;  // 500 ms in clocks.
    clock_t   prevClock = clock() - trigger;
	
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	fflush(stdout);
	
    while (1) {
		clock_t curClock = clock();

        if (curClock - prevClock >= trigger) {
            prevClock = curClock;
            redraw();
        }
		
    }
}

static void redraw(void) {

    /*const char prompt[] = "\
 -                                                                              - \n\
|                                                                                |\n\
|                                                                                |\n\
|                                                                                |\n\
|                                                                                |\n\
|                                                                                |\n\
|                                                                                |\n\
|                                                                                |\n\
|                                                                                |\n\
|                                                                                |\n\
 -                                                                              -";*/

 
	const char prompt[] = "";
	printf("\r%*s\r%s", WIDTH, "", prompt);
	for (int i = -WIDTH/2; i < WIDTH/2; i++) {
		if (i < start - 1 || i > finish + 1)
			printf(" ");
		else if (i == start - 1 || i == finish + 1)
			printf("|");
		else if (i == position)
			printf("O");
		else
			printf("-");		
	}
	fflush(stdout);
	
	if (position == finish)
		increment = false;
	else if (position == start)
		increment = true;
	
	if (increment)
		position++;
	else
		position--;
}
