#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <time.h>

struct env
{
  int screenWidth;
  int screenHeight;
  double gravity;
  double airmass;
};

struct Vector2
{
  double x;
  double y;
};

struct Vector2* newVector2(double x, double y)
{
  struct Vector2* vector2 = (struct Vector2*)malloc(sizeof(struct Vector2));

  vector2->x = x;
  vector2->y = y;

  return vector2;
}

struct Particle
{
  struct Vector2* pos;
  double angle;
  double speed;
  int mass;
};

struct Particle* newParticle(struct Vector2* pos, int mass, double speed, double angle)
{
  struct Particle* particle = (struct Particle*)malloc(sizeof(struct Particle));

  particle->pos = pos;
  particle->mass = mass;
  particle->speed = speed;
  particle->angle = angle;

  return particle;
}

struct Vector2* addVectors(struct Vector2* vec1, struct Vector2* vec2)
{
  double x = sin(vec1->x) * vec1->y + sin(vec2->x) * vec2->y;
  double y = cos(vec1->x) * vec1->y + cos(vec2->x) * vec2->y;
  double nx = 0.5f * M_PI - atan2(y, x);
  double ny = hypot(x, y);

  return newVector2(nx, ny);
};

void moveParticle(struct Particle* p, struct env* e, double dT)
{
  p->pos->y += sin(p->angle) * p->speed * dT;
  p->pos->x += cos(p->angle) * p->speed * dT;

  p->speed *= e->airmass;

  if (p->pos->y >= e->screenHeight-1)
    p->pos->y = e->screenHeight-1;
  if (p->pos->x >= e->screenWidth-1)
    p->pos->x = e->screenWidth-1;
  if (p->pos->y <= 1)
    p->pos->y = 1;
  if (p->pos->x <= 1)
    p->pos->x = 1;
}

void applyForce(struct Particle* p, struct Vector2* a, double dT)
{
  struct Vector2* added_grav = addVectors(newVector2(p->angle,p->speed),a);
  p->angle = added_grav->x;
  p->speed = added_grav->y;
}

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR)
    {
        ungetch(ch);
        return 1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char *argv[])
{
  struct env e;
  e.gravity = 0.000f;
  e.airmass = 0.9999f;
  struct Particle* p = newParticle(newVector2(1.0f, 50.0f), 1, 0.0f, M_PI/2);

  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  scrollok(stdscr, TRUE);
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);
  e.screenHeight = max_y;
  e.screenWidth = max_x;

  char screen[e.screenHeight * e.screenWidth];

  clock_t tp1, tp2;
  tp1 = clock();

  while (1)
  {
    tp2 = clock();
    double dT = (double)(tp2 - tp1)/CLOCKS_PER_SEC;
    tp1 = tp2;

    if (kbhit())
    {
      int asciival = getch();
      switch (asciival)
      {
      case 97:
        applyForce(p,newVector2(M_PI,50.0f), dT);
        break;
      case 100:
        applyForce(p,newVector2(0,50.0f),dT);
        break;
      case 119:
        applyForce(p,newVector2(M_PI/2+1*M_PI, 50.0f),dT);
        break;
      case 115:
        applyForce(p,newVector2(M_PI/2, 50.0f),dT);
        break;
      }

    }

    applyForce(p,newVector2(M_PI/2, e.gravity),dT);

    moveParticle(p, &e, dT);

    for (double y = 0; y < e.screenHeight; y++)
    {
      for (double x = 0; x < e.screenWidth; x++)
      {
        char screenitem = ' ';
        if ((int)p->pos->x == x && (int)p->pos->y == y)
        {
          screenitem = '0';
        }
        screen[(int)y*e.screenWidth+(int)x] = screenitem;
      }
    }

    refresh();

    mvprintw(0,0,screen);
  }

  return 0;
}
