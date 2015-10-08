#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
extern "C" {
    #include "fonts.h"
}

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 10000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
    float x, y, z;
};

struct Shape {
    float width, height;
    float radius;
    Vec center;
};

struct Particle {
    Shape s;
    Vec velocity;
};

struct Game {
    Shape box[15];
    Shape circle;
    Particle particle[MAX_PARTICLES];
    int n;
    int lastMousex, lastMousey;
};


int keys[65536];
int yres = 300, xres = 500;

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);

int main(void)
{
    int done=0;
    srand(time(NULL));
    initXWindows();
    init_opengl();
    //declare game object
    Game game;
    game.n=0;

    //declare a box shape

    //First
    game.box[0].width = 100;
    game.box[0].height = 11;
    game.box[0].center.x = 65;
    game.box[0].center.y = 456;
    //Second
    game.box[1].width = 100;
    game.box[1].height = 11; 
    game.box[1].center.x = 175;
    game.box[1].center.y = 385;
    //Third
    game.box[2].width = 100;
    game.box[2].height = 11; 
    game.box[2].center.x = 285;
    game.box[2].center.y = 305;
    //Fourth
    game.box[3].width = 100;
    game.box[3].height = 11;  
    game.box[3].center.x = 385;
    game.box[3].center.y = 215;
    //Fifth 
    game.box[4].width = 100;
    game.box[4].height = 11; 
    game.box[4].center.x = 485;
    game.box[4].center.y = 155;
    //Source  (Sixth)
    game.box[5].width = 75;
    game.box[5].height = 15; 
    game.box[5].center.x = 0;
    game.box[5].center.y = 600;
    //Source 2 (Seventh)
    game.box[6].width = 70;
    game.box[6].height = 60; 
    game.box[6].center.x = 5;
    game.box[6].center.y = 515;
    //LS-Glasses (Eighth)
    game.box[7].width = 40;
    game.box[7].height = 20; 
    game.box[7].center.x = 650;
    game.box[7].center.y = 554;
    //Mid-Glasses (Ninth)
    game.box[8].width = 50;
    game.box[8].height = 8; 
    game.box[8].center.x = 740;
    game.box[8].center.y = 554;
    //RS-Glasses (Tenth)
    game.box[9].width = 40;
    game.box[9].height = 20; 
    game.box[9].center.x = 755;
    game.box[9].center.y = 554;
    //LS-Glasses Edge (Eleventh)
    game.box[10].width = 18;
    game.box[10].height = 8; 
    game.box[10].center.x = 593;
    game.box[10].center.y = 565;
    //RS-Glasses Edge (Twelfth)
    game.box[11].width = 18;
    game.box[11].height = 8; 
    game.box[11].center.x = 760;
    game.box[11].center.y = 565;
    





    game.circle.center.x = 720;
    game.circle.center.y = 546;
    game.circle.radius = 142;

    //SET WATER AT (1,580)
    
    //game.circle[1].center.x = ;
    //game.circle[1].center.y = ;
    //game.corcle[1].radius = 90;
    

    //start animation
    while(!done) {
        while(XPending(dpy)) {
            XEvent e;
            XNextEvent(dpy, &e);
            check_mouse(&e, &game);
            done = check_keys(&e, &game);
	    
        }
        movement(&game);
        render(&game);
        glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    cleanup_fonts();
    return 0;
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "CS335 Lab1 - Sunny Day ");
}

void cleanupXWindows(void) {
    //do not change
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void initXWindows(void) {
    //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        std::cout << "\n\tcannot connect to X server\n" << std::endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if(vi == NULL) {
        std::cout << "\n\tno appropriate visual found\n" << std::endl;
        exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask |
        PointerMotionMask |
        StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
            InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    //Set the screen background color
    glClearColor(0.4, 0.7, 0.7, 0.4);
    //Allow Fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
}

#define rnd() (float)rand() / (float)RAND_MAX

void makeParticle(Game *game, int x, int y) {
    if (game->n >= MAX_PARTICLES)
        return;
    //std::cout << "makeParticle() " << x << " " << y << std::endl;
    //position of particle
    Particle *p = &game->particle[game->n];
    p->s.center.x = x;
    p->s.center.y = y;
    p->velocity.y = rnd()*1.0 - 0.5;
    p->velocity.x =  1.0 + rnd()*0.1;
    game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
    static int savex = 1;
    static int savey = 580;
    //static int n = 0;

    if (e->type == ButtonRelease) {
        return;
    }
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) {
            //Left button was pressed
            int y = WINDOW_HEIGHT - e->xbutton.y;
	std::cout << " " << e->xbutton.x << "  " << y << std::endl;
            for (int i=0; i<10; i++)
                makeParticle(game, e->xbutton.x, y);
            return;
        }
        if (e->xbutton.button==3) {
            //Right button was pressed
            return;
        }
    }
    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        savex = e->xbutton.x;
        savey = e->xbutton.y;
        int y = WINDOW_HEIGHT - e->xbutton.y;
	std::cout << " " << savex << "  " << savey << std::endl;
        //for (int i=0; i<10; i++)
            //makeParticle(game, e->xbutton.x, y);
        //if (++n < 10)
        //  return;
        game->lastMousex = e->xbutton.x;
        game->lastMousey = y;
    }
}

int check_keys(XEvent *e, Game *game)
{
    //Was there input from the keyboard?
        static int water = 0;
        int key = XLookupKeysym(&e->xkey, 0);

	if(e->type == KeyPress){
	    if(key == XK_b)
	    std::cout << "Water: On" << std::endl;
	    water = 1;
	}
	if(e->type == KeyRelease){
	    if(key == XK_b)
		std::cout << "Water: Off" << std::endl;
		water = 0;
	}
	      if(water){
	  	makeParticle(game, 1, 580);
	  	makeParticle(game, 1, 579);
	  	makeParticle(game, 1, 578);
		makeParticle(game, 602, 557);
		makeParticle(game, 598, 553);
		makeParticle(game, 605, 531);
		makeParticle(game, 655, 547);
		makeParticle(game, 675, 524);
		makeParticle(game, 685, 490);
		makeParticle(game, 674, 587);
		makeParticle(game, 700, 490);
		makeParticle(game, 654, 486);
		makeParticle(game, 737, 474);
		makeParticle(game, 722, 485);
	      }


    return 0;
}

void movement(Game *game)
{
    Particle *p;

    if (game->n <= 0)
        return;

    for (int i=0; i<10; i++)
        makeParticle(game, game->lastMousex, game->lastMousey);

    for (int i=0; i<game->n; i++){
        p = &game->particle[i];
        p->s.center.x += p->velocity.x;
        p->s.center.y += p->velocity.y;
        p->velocity.y -= GRAVITY;

        //check for collision with shapes...
        for (int j=0; j<10; j++) {
            Shape *s = &game->box[j];
            if (p->s.center.y < s->center.y + s->height &&
                    p->s.center.y > s->center.y - s->height &&
                    p->s.center.x >= s->center.x - s->width &&
                    p->s.center.x <= s->center.x + s->width) {
                p->velocity.y *= -0.25;
                p->s.center.y = s->center.y + s->height + 0.01;
            }
        //check circle collision
        float d0,d1,dist;
        d0 = p->s.center.x - game->circle.center.x;
        d1 = p->s.center.y - game->circle.center.y;
        dist = sqrt(d0*d0 + d1*d1);
        if (dist <= game->circle.radius) {
            //p->velocity.y = 0.0;
            //float v[2];
            d0 /= dist;
            d1 /= dist;
            d0 *= game->circle.radius * 1.01;
            d1 *= game->circle.radius * 1.01;
            p->s.center.x = game->circle.center.x + d0;
            p->s.center.y = game->circle.center.y + d1;
            p->velocity.x += d0 * 0.02;
            p->velocity.y += d1 * 0.02;
	  }
	}

        //check for off-screen
        if (p->s.center.y < 0.0 || p->s.center.y > WINDOW_HEIGHT) {
            std::cout << " " << std::endl;
            memcpy(&game->particle[i], &game->particle[game->n-1],
                    sizeof(Particle));
            game->n--;
        }
    }
}

void render(Game *game)
{
    float w, h;
    Rect r;
    glClear(GL_COLOR_BUFFER_BIT);
    unsigned int cref = 0x00FFFFF00;
   
    //Draw shapes...

    static int firsttime=1;
    static int verts[60][2];
    static int n = 60;
    if (firsttime) {
        float angle = 0.0;
        float inc = (3.14159 * 2.0) / (float)n;
        for (int i=0; i<n; i++) {
            verts[i][0] = cos(angle) * game->circle.radius 
		          + game->circle.center.x;
            verts[i][1] = sin(angle) * game->circle.radius 
		          + game->circle.center.y;     
            angle += inc;
        }
	firsttime = 0;
    }

    glColor3ub(200,250,10);
    glPushMatrix();
    glBegin(GL_TRIANGLE_FAN);
        for (int i=0; i<n; i++){
            glVertex2i(verts[i][0], verts[i][1]);
	}
    glEnd();
    glPopMatrix();
     





    //draw box
    Shape *s;
    glColor3ub(0, 0, 0);
    for (int j=0; j<15; j++) {
        s = &game->box[j];
        glPushMatrix();
        glTranslatef(s->center.x, s->center.y, s->center.z);
        w = s->width;
        h = s->height;
        glBegin(GL_QUADS);
        glVertex2i(-w,-h);
        glVertex2i(-w, h);
        glVertex2i( w, h);
        glVertex2i( w,-h);
        glEnd();
        glPopMatrix();
    }
    //draw all particles here
    for (int i=0; i<game->n; i++) {
        glPushMatrix();
        glColor3ub(100,100,250);
        Vec *c = &game->particle[i].s.center;
        w = 1;
        h = 2.5;
        glBegin(GL_QUADS);
        glVertex2i(c->x-w, c->y-h);
        glVertex2i(c->x-w, c->y+h);
        glVertex2i(c->x+w, c->y+h);
        glVertex2i(c->x+w, c->y-h);
        glEnd();
        glPopMatrix();
    }

  //The Text Section
    r.bot = yres + 145;
    r.left = 15;
    r.center = 0;
    ggprint8b(&r, 25, cref, "Requirements");
    
    r.bot = yres + 75;
    r.left = 100;
    r.center = 0;
    ggprint8b(&r, 25, cref, "Design");

    r.bot = yres -5;
    r.left = 225;
    r.center = 0;
    ggprint8b(&r, 25, cref, "Coding");
    
    r.bot = yres - 95;
    r.left = 300;
    r.center = 0;
    ggprint8b(&r, 25, cref, "Testing");

    r.bot = yres - 155;
    r.center = 0;
    r.left = 400;
    ggprint8b(&r, 25, cref, "Maintenance");

  
}
