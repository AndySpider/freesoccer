#include <GL/glut.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include "stdlib.h"
#include "global.h"
#include "coordinate.h"
#include "comm.h"

struct Ball ball;
struct Player players1[11];
struct Player players2[11];
int focuser = -1;
int holder = -1;

struct Team team1 = {0, 1, A442, BALANCED, players1};
struct Team team2 = {0, 2, A442, BALANCED, players2};

int nTick = 45;
struct Status sts;
struct Cmd c;
//struct Cmd pre_c;
//int cout_pre = 0;
//int cout_cmd = 0;

struct sockaddr_in sa;
socklen_t sa_len;
int sockfd;

int scale_ratio = 5;
float Lextent = 20.0;
float Wextent = 20.0;

GLuint spinx = 45;
GLfloat sdepth = 0.0;
GLuint spinz = 30;

void init_players(void)
{
    int i;
    for (i=0; i<11; i++)
    {
        players1[i].id = i;
        players1[i].attr.body = 90;
    }
    for (i=0; i<11; i++)
    {
        players2[i].id = i+11;
        players2[i].attr.body = 90;
    }

}

int init_sock()
{
    int sfd;
    sfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa_len = sizeof(sa);
    return sfd;
}

void drawball(struct Ball b)
{
    GLfloat x, y, z;
    x = b.pos.x*scale_ratio;
    y = b.pos.y*scale_ratio;
    z = b.pos.z*scale_ratio;
    glPointSize(5.0);
    glColor3f(1.0, 0, 0);
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void drawplayer(struct Player p)
{
    GLfloat x, y, z;
    x = p.pos.x*scale_ratio;
    y = p.pos.y*scale_ratio;
    z = p.pos.z*scale_ratio;
    glPointSize(9);
    if (p.id == holder)
        glColor3f(1.0, 1.0, 0.0);
    else if (p.id == focuser)
        glColor3f(1.0, 0.0, 1.0);
    else if(p.id <= 10)
        glColor3f(0.0, 0.0, 1.0);
    else
        glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}
void drawfield(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(3.0);
    // full_field
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0, 0.0);
    glVertex2f(Lfield*scale_ratio, 0.0);
    glVertex2f(Lfield*scale_ratio, Wfield*scale_ratio);
    glVertex2f(0.0, Wfield*scale_ratio);
    glEnd();
    // gate_field_1
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0, (Wfield*scale_ratio - Wgatearea*scale_ratio)/2.0);
    glVertex2f(Lgatearea*scale_ratio, (Wfield*scale_ratio - Wgatearea*scale_ratio)/2.0);
    glVertex2f(Lgatearea*scale_ratio, (Wfield*scale_ratio + Wgatearea*scale_ratio)/2.0);
    glVertex2f(0.0, (Wfield*scale_ratio + Wgatearea*scale_ratio)/2.0);
    glEnd();
    // penalty_field_1
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0, (Wfield*scale_ratio - Wpenaltyarea*scale_ratio)/2.0);
    glVertex2f(Lpenaltyarea*scale_ratio, (Wfield*scale_ratio - Wpenaltyarea*scale_ratio)/2.0);
    glVertex2f(Lpenaltyarea*scale_ratio, (Wfield*scale_ratio + Wpenaltyarea*scale_ratio)/2.0);
    glVertex2f(0.0, (Wfield*scale_ratio + Wpenaltyarea*scale_ratio)/2.0);
    glEnd();
    // middle line
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glVertex2f(Lfield*scale_ratio/2.0, 0.0);
    glVertex2f(Lfield*scale_ratio/2.0, Wfield*scale_ratio);
    glEnd();
    // gate_field_2
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(Lfield*scale_ratio, (Wfield*scale_ratio - Wgatearea*scale_ratio)/2.0);
    glVertex2f((Lfield - Lgatearea)*scale_ratio, (Wfield*scale_ratio - Wgatearea*scale_ratio)/2.0);
    glVertex2f((Lfield - Lgatearea)*scale_ratio, (Wfield*scale_ratio + Wgatearea*scale_ratio)/2.0);
    glVertex2f(Lfield*scale_ratio, (Wfield*scale_ratio + Wgatearea*scale_ratio)/2.0);
    glEnd();
    // penalty_field_2
    glBegin(GL_LINE_STRIP);
    glVertex2f(Lfield*scale_ratio, (Wfield*scale_ratio - Wpenaltyarea*scale_ratio)/2.0);
    glVertex2f((Lfield - Lpenaltyarea)*scale_ratio, (Wfield*scale_ratio - Wpenaltyarea*scale_ratio)/2.0);
    glVertex2f((Lfield - Lpenaltyarea)*scale_ratio, (Wfield*scale_ratio + Wpenaltyarea*scale_ratio)/2.0);
    glVertex2f(Lfield*scale_ratio, (Wfield*scale_ratio + Wpenaltyarea*scale_ratio)/2.0);
    glEnd();
    // 2 penalty point and center point
    glPointSize(4.0);
    glBegin(GL_POINTS);
    glVertex3f(Lpenaltypoint*scale_ratio, Wfield*scale_ratio/2.0, 0.0);
    glVertex3f((Lfield - Lpenaltypoint)*scale_ratio, Wfield*scale_ratio/2.0, 0.0);
    glVertex3f(Lfield*scale_ratio/2.0, Wfield*scale_ratio/2.0, 0.0);
    glEnd();
    // 2 gates
    glLineWidth(4.0);
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0, (Wfield*scale_ratio - Wgate*scale_ratio)/2.0, 0.0);
    glVertex3f(0.0, (Wfield*scale_ratio + Wgate*scale_ratio)/2.0, 0.0);
    glVertex3f(0.0, (Wfield*scale_ratio + Wgate*scale_ratio)/2.0, Hgate*scale_ratio);
    glVertex3f(0.0, (Wfield*scale_ratio - Wgate*scale_ratio)/2.0, Hgate*scale_ratio);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(Lfield*scale_ratio,(Wfield*scale_ratio - Wgate*scale_ratio)/2.0, 0.0);
    glVertex3f(Lfield*scale_ratio,(Wfield*scale_ratio + Wgate*scale_ratio)/2.0, 0.0);
    glVertex3f(Lfield*scale_ratio,(Wfield*scale_ratio + Wgate*scale_ratio)/2.0, Hgate*scale_ratio);
    glVertex3f(Lfield*scale_ratio,(Wfield*scale_ratio - Wgate*scale_ratio)/2.0, Hgate*scale_ratio);
    glEnd();
}
void init(void)
{
    struct Init_data idata;
    int i;

    init_players();
    sockfd = init_sock();

    idata.dif = EASY;
    idata.weath = RAIN;
    idata.nTick = nTick;
    idata.human = 1;
    idata.team1 = team1;
    idata.team2 = team2;
    for (i=0; i<11; i++)
    {
        idata.player1[i] = players1[i];
        idata.player2[i] = players2[i];
    }
    printf("begin send idata\n");
    sendto(sockfd, &idata, sizeof(idata), 0, (struct sockaddr *)&sa, sa_len);
    
    printf("begin recv cmd\n");
    recvfrom(sockfd ,&c, sizeof(c), 0, (struct sockaddr *)&sa, &sa_len);
    printf("recv cmd:%s\n", c.name);
    strcpy(c.name, "START");
    printf("begin send cmd:START\n");
    sendto(sockfd, &c, sizeof(c), 0, (struct sockaddr *)&sa, sa_len);
    
    strcpy(c.name, "NONE");
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h) 
      glOrtho (0.0 - Lextent, Lfield*scale_ratio + Lextent, 
         (0.0 - Wextent)*(GLfloat)h/(GLfloat)w, (Wfield*scale_ratio + Wextent)*(GLfloat)h/(GLfloat)w, -100.0,100.0);
   else 
      glOrtho ((0.0 - Lextent)*(GLfloat)w/(GLfloat)h, 
         (Lfield*scale_ratio + Lextent)*(GLfloat)w/(GLfloat)h , 0.0 - Wextent, Wfield*scale_ratio + Wextent, -100.0, 100.0);
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void clear_cmd(int val)
{
    if (val == 1)
        strcpy(c.name, "NONE");
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:  // esc
            strcpy(c.name, "END");
            sendto(sockfd, &c, sizeof(c), 0, (struct sockaddr *)&sa, sa_len);
            exit(0);
            break;
        case 32: // space
            strcpy(c.name, "SHOT");
            c.power = 15;
            break;
        case 'p': // shift 
            strcpy(c.name, "SPASS");
            break;
        case 'w':
            strcpy(c.name, "UP");
            c.power = 5;
            break;
        case 'a':
            strcpy(c.name, "LEFT");
            c.power = 5;
            break;
        case 's':
            strcpy(c.name, "DOWN");
            c.power = 5;
            break;
        case 'd':
            strcpy(c.name, "RIGHT");
            c.power = 5;
            break;
        case 'l':
            strcpy(c.name, "LPASS");
            c.power = 16;
            break;
        case 'k':
            strcpy(c.name, "MTOBALL");
            c.power = 7;
            break;
        case 'r':
            strcpy(c.name, "RESTART");
            break;
    }
}

void set_positions(struct Status *sts)
{
    int i;
    for (i=0; i<11; i++)
    {
        players1[i] = sts->players[i];
        players2[i] = sts->players[i+11];
    }
    ball = sts->ball;
    focuser = sts->focus_id;
    holder = sts->holder_id;
}
    
void display()
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_PROJECTION);
    //glLoadIdentity();  // !!!!!!!!!!!!!!
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef ((GLfloat) spinx, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, -sdepth);
    glRotatef ((GLfloat) spinz, 0.0, 0.0, 1.0);

    //glColor3f(1.0, 1.0, 1.0);
    glOrtho(0.0 - Lextent, Lfield*scale_ratio + Lextent, 0.0-Wextent, Wfield*scale_ratio + Wextent, -100.0, 100.0); 
    drawfield();
    printf("bpos: %f, %f, %f\n", ball.pos.x, ball.pos.y, ball.pos.z);
    for (i=0; i<11; i++)
    {
        //printf("x:%f, y:%f, z:%f ;", players1[i].pos.x,players1[i].pos.y,players1[i].pos.z); 

        drawplayer(players1[i]);
        drawplayer(players2[i]);
        printf("player[%d], ac_type: %d, frame: %d\n", i, players1[i].action.ac_type, players1[i].action.frame);
        printf("player[%d], ac_type: %d, frame: %d\n", i+11, players2[i].action.ac_type, players2[i].action.frame);
    }
    printf("\n");
    drawball(ball);

    glFlush();
    glutSwapBuffers();
}

void playing(int val)
{
    if (val == 0)
    {
        printf("begin recv sts\n");
        recvfrom(sockfd, &sts, sizeof(sts), 0, (struct sockaddr *)&sa, &sa_len);
        set_positions(&sts);
        printf("focuser: %d, holder: %d\n", focuser, holder); 
        printf("begin send cmd:%s\n",c.name);
        sendto(sockfd, &c, sizeof(c), 0, (struct sockaddr *)&sa, sa_len);
        if (strcmp(c.name, "RESTART") == 0)
        {
            strcpy(c.name, "START");
            sendto(sockfd, &c, sizeof(c), 0, (struct sockaddr *)&sa, sa_len);
        }
        strcpy(c.name, "NONE");
        display();
        glutTimerFunc (22, playing, 0);
    }
}
    
void mouse(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            switch (state) {
                case GLUT_DOWN:
                    spinx = (spinx + 5) % 360; 
                    glutPostRedisplay();
                    break;
                default:
                    break;
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            switch (state) {
                case GLUT_DOWN:
                    sdepth += 0.1; 
                    glutPostRedisplay();
                    break;
                default:
                    break;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            switch (state) {
                case GLUT_UP:
                    spinz = (spinz + 5) % 360; 
                    glutPostRedisplay();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  color index display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE);
   glutInitWindowSize (Lfield*scale_ratio, Wfield*scale_ratio);
   glutCreateWindow (argv[0]);
   init();
   glutReshapeFunc (reshape);
   glutMouseFunc(mouse);
   glutKeyboardFunc (keyboard);
   glutTimerFunc (17, playing, 0);
   glutMainLoop();
   return 0;
}

