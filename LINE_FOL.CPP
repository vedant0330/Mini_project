#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <dos.h>
#include <stdlib.h>

#define MAXP 20
#define GRID 10

int x[MAXP], y[MAXP];
int n;

int cell = 40;
int ox = 60;
int oy = 440;

/* Convert grid coordinates to screen coordinates */
int screenX(int gx)
{
    return ox + gx * cell;
}

int screenY(int gy)
{
    return oy - gy * cell;
}

/* Draw coordinate grid */
void drawGrid(void)
{
    int i;
    char label[10];

    setcolor(DARKGRAY);

    for (i = 0; i <= GRID; i++)
    {
        line(screenX(i), screenY(0),
             screenX(i), screenY(GRID));

        line(screenX(0), screenY(i),
             screenX(GRID), screenY(i));

        sprintf(label, "%d", i);

        outtextxy(screenX(i) - 3, oy + 8, label);
        outtextxy(ox - 20, screenY(i) - 3, label);
    }
}

/* Draw complete route */
void drawRoute(void)
{
    int i;

    setcolor(YELLOW);
    setlinestyle(SOLID_LINE, 0, THICK_WIDTH);

    for (i = 0; i < n - 1; i++)
    {
        line(screenX(x[i]), screenY(y[i]),
             screenX(x[i + 1]), screenY(y[i + 1]));
    }

    /* Start marker */
    setcolor(GREEN);
    circle(screenX(x[0]), screenY(y[0]), 6);
    outtextxy(screenX(x[0]) + 8,
              screenY(y[0]) - 8, "START");

    /* Destination marker */
    setcolor(RED);
    circle(screenX(x[n - 1]), screenY(y[n - 1]), 6);
    outtextxy(screenX(x[n - 1]) + 8,
              screenY(y[n - 1]) - 8, "DEST");
}

/* Draw robot */
void drawRobot(int rx, int ry)
{
    /* Body */
    setcolor(CYAN);
    setfillstyle(SOLID_FILL, CYAN);
    bar(rx - 8, ry - 6, rx + 8, ry + 6);

    /* Wheels */
    setcolor(WHITE);
    line(rx - 10, ry - 5, rx - 10, ry + 5);
    line(rx + 10, ry - 5, rx + 10, ry + 5);

    /* Front sensor */
    setcolor(RED);
    circle(rx, ry - 7, 2);
}

/* Animate robot along a segment */
void animateSegment(int x1, int y1, int x2, int y2)
{
    int step;
    int steps = 40;
    int rx, ry;
    int oldx, oldy;

    oldx = screenX(x1);
    oldy = screenY(y1);

    for (step = 0; step <= steps; step++)
    {
        rx = screenX(x1)
             + ((x2 - x1) * cell * step) / steps;

        ry = screenY(y1)
             - ((y2 - y1) * cell * step) / steps;

        /*
           Restore the small area behind the robot:
           redraw grid lines and route in that area.
        */
        setfillstyle(SOLID_FILL, BLACK);
        bar(oldx - 12, oldy - 10,
            oldx + 12, oldy + 10);

        /* Restore grid lines crossing the erased area */
        setcolor(DARKGRAY);

        line(oldx - 12, screenY(0),
             oldx - 12, screenY(GRID));

        line(screenX(0), oldy,
             screenX(GRID), oldy);

        /* Redraw route segments */
        setcolor(YELLOW);

        {
            int i;

            for (i = 0; i < n - 1; i++)
            {
                line(screenX(x[i]), screenY(y[i]),
                     screenX(x[i + 1]), screenY(y[i + 1]));
            }
        }

        /* Draw robot at current position */
        drawRobot(rx, ry);

        oldx = rx;
        oldy = ry;

        delay(40);
    }
}

int main(void)
{
    int gd = DETECT, gm;
    int errorcode;
    int i;

    clrscr();

    printf("LINE FOLLOWER ROBOT - PATH PLANNER\n");
    printf("----------------------------------\n");
    printf("Enter coordinates from 0 to 10.\n\n");

    printf("Enter number of waypoints (2-%d): ", MAXP);
    scanf("%d", &n);

    if (n < 2 || n > MAXP)
    {
        printf("Invalid number of waypoints.");
        getch();
        return 0;
    }

    for (i = 0; i < n; i++)
    {
        printf("Enter waypoint %d (x y): ", i + 1);
        scanf("%d %d", &x[i], &y[i]);

        if (x[i] < 0 || x[i] > GRID ||
            y[i] < 0 || y[i] > GRID)
        {
            printf("Coordinates must be from 0 to 10.");
            getch();
            return 0;
        }
    }

    /* Start graphics */
    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");

    errorcode = graphresult();

    if (errorcode != grOk)
    {
        printf("\nGraphics error: %s",
               grapherrormsg(errorcode));
        printf("\nCheck the BGI folder path.");
        getch();
        return 1;
    }

    /* Draw graph and route only once */
    setbkcolor(BLACK);
    cleardevice();

    setcolor(WHITE);
    outtextxy(60, 15, "LINE FOLLOWER ROBOT SIMULATION");

    drawGrid();
    drawRoute();

    /* Show robot at starting point */
    drawRobot(screenX(x[0]), screenY(y[0]));
    delay(800);

    /* Move robot through each waypoint */
    for (i = 0; i < n - 1; i++)
    {
        animateSegment(x[i], y[i],
                       x[i + 1], y[i + 1]);
    }

    setcolor(LIGHTGREEN);
    outtextxy(60, 470, "Robot reached destination!");

    setcolor(WHITE);
    outtextxy(60, 455, "Press any key to close.");

    getch();
    closegraph();

    return 0;
}