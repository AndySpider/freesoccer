#include <GL/glut.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "global.h"
#include "actions.h"
#include "coordinate.h"
#include "comm.h"
#include "utilities.h"
#include "config.h"

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

GLfloat sdepth = 0.0;
GLuint spinz = 0.0;
GLfloat beginZ = 0.0;
GLfloat beginM = 0.0;
GLfloat lookX;
GLfloat lookY;
GLfloat lookZ;

GLfloat holder_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat focuser_diffuse[] = {1.0, 0.0, 1.0, 1.0};
GLfloat team1_diffuse[] = {0.0, 0.0, 1.0, 1.0};
GLfloat team2_diffuse[] = {0.0, 1.0, 0.0, 1.0};
GLfloat field_diffuse[] = {0.1, 0.2, 0.1, 1.0};
GLfloat line_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat ball_diffuse[] = {1.0, 1.0, 1.0, 1.0};

void init_players(void)
{
    int i;
    for (i=0; i<11; i++)
    {
        players1[i].id = i;
        if (i % 2 == 0)
            players1[i].attr.body = 100;
        else
            players2[i].attr.body = 90;
    }
    for (i=0; i<11; i++)
    {
        players2[i].id = i+11;
        if (i % 2 == 0)
            players1[i].attr.body = 100;
        else
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
    glPushMatrix();
    glPointSize(5.0);
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, ball_diffuse);
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(x, y, z);
    glutSolidSphere(0.3*scale_ratio, 15, 15);
    glPopMatrix();
}

void drawplayer(struct Player p)
{
    GLUquadricObj *quadObj = gluNewQuadric();
    GLfloat x, y, z, angle;
    x = p.pos.x*scale_ratio;
    y = p.pos.y*scale_ratio;
    z = p.pos.z*scale_ratio;
    //GLfloat player_ambient[];

    GLfloat radio = 0.4*scale_ratio, height = 1.8*scale_ratio;

    glPushMatrix();
    if (p.id == holder)
        //glMaterialfv(GL_FRONT, GL_DIFFUSE, holder_diffuse);
        glColor3f(1.0, 1.0, 0.0);
    else if (p.id == focuser)
        //glMaterialfv(GL_FRONT, GL_DIFFUSE, focuser_diffuse);
        glColor3f(1.0, 0.0, 1.0);
    else if(p.id <= 10)
        //glMaterialfv(GL_FRONT, GL_DIFFUSE, team1_diffuse);
        glColor3f(0.0, 0.0, 1.0);
    else
        //glMaterialfv(GL_FRONT, GL_DIFFUSE, team2_diffuse);
        glColor3f(0.0, 1.0, 0.0);

    glTranslatef(x, y, z);

    if (p.action.ac_type == TY_TUMBLE) 
    {
        glRotatef(p.direct*180/PI, 0, 0, 1.0);
        glRotatef(10.0*(FR_TUMBLE - p.action.frame), 0, 1, 0);
    }
    if (p.action.ac_type == TY_POUNCE)
    {
        if (p.action.spd.y > 0)
            glRotatef(-10.0*(FR_POUNCE - p.action.frame), 1.0, 0.0, 0.0);
        else if(p.action.spd.y < 0)
            glRotatef(10.0*(FR_POUNCE - p.action.frame), 1.0, 0.0, 0.0);
    }
    //gluQuadricDrawStyle(quadObj, GLU_FILL);
    //gluQuadricNormals(quadObj, GL_FLAT);
    //gluQuadricOrientation(quadObj, GLU_INSIDE);
    gluCylinder(quadObj, radio, radio, height, 15, 15);

    glBegin(GL_POLYGON);
    for (angle=0.0; angle<2*3.14; angle+=0.1)
    {
        x = radio*sin(angle);
        y = radio*cos(angle);
        glVertex3f(x, y, height);
    }
    glEnd();
    // Indicate direction
    glPointSize(5);
    glColor3f(1.0, 0.0, 0.1);
    glRotatef(p.direct*180/PI, 0.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    glVertex3f(radio, 0.0, height);
    glEnd();

    glPopMatrix();
    gluDeleteQuadric(quadObj);
}
void drawfield(void)
{
    float x, y, angle;
    // full_field
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, field_diffuse);
    glColor3f(1, 1, 1);
    glLineWidth(3.0);
    glColor3f(0.1, 0.2, 0.1);
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, 0.0);
    glVertex2f(Lfield*scale_ratio, 0.0);
    glVertex2f(Lfield*scale_ratio, Wfield*scale_ratio);
    glVertex2f(0.0, Wfield*scale_ratio);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0, 0.0);
    glVertex2f(Lfield*scale_ratio, 0.0);
    glVertex2f(Lfield*scale_ratio, Wfield*scale_ratio);
    glVertex2f(0.0, Wfield*scale_ratio);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.0, 0.0);
    glVertex3f(0.0, 0.0, 10.0);
    glVertex2f(Lfield*scale_ratio, 0.0);
    glVertex3f(Lfield*scale_ratio, 0.0, 10.0);
    glVertex2f(Lfield*scale_ratio, Wfield*scale_ratio);
    glVertex3f(Lfield*scale_ratio, Wfield*scale_ratio, 10.0);
    glVertex2f(0.0, Wfield*scale_ratio);
    glVertex3f(0.0, Wfield*scale_ratio, 10.0);
    glEnd();
    glPopMatrix();
    // center circle
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, line_diffuse);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (angle=0.0; angle<2*3.14; angle+=0.01)
    {
        x = Rcircle * sin(angle);
        y = Rcircle * cos(angle);
        glVertex2f((x+Lfield/2.0)*scale_ratio, (y+Wfield/2.0)*scale_ratio);
    }
    glEnd();
    // gate_field_1
    glColor3f(1.0, 1.0, 1.0);
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
    glPopMatrix();
    // 2 gates
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, line_diffuse);
    glLineWidth(4.0);
    glColor3f(1.0, 1.0, 1.0);
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
    glPopMatrix();
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
    glShadeModel(GL_SMOOTH);
    //GLfloat light_position[] = {Lfield*scale_ratio/2.0, Wfield*scale_ratio/2.0, 20.0*scale_ratio};
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(90, (GLfloat)w/(GLfloat)h, 1.0, 1000.0);
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
            c.power = 6;
            break;
        case 'a':
            strcpy(c.name, "LEFT");
            c.power = 6;
            break;
        case 's':
            strcpy(c.name, "DOWN");
            c.power = 6;
            break;
        case 'd':
            strcpy(c.name, "RIGHT");
            c.power = 6;
            break;
        case 'l':
            strcpy(c.name, "LPASS");
            c.power = 16;
            break;
        case 'k':
            strcpy(c.name, "MTOBALL");
            c.power = 8;
            break;
        case 'r':
            strcpy(c.name, "RESTART");
            spinz = 0.0;
            break;
        default:
            strcpy(c.name, "NONE");
    }
}

void keyup(unsigned char key, int x, int y)
{
    strcpy(c.name, "NONE");
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
    glPushMatrix();
    glTranslatef(0.0, 0.0, - sdepth);
    glRotatef ((GLfloat) spinz, 0.0, 0.0, 1.0);
    lookX = ball.pos.x*scale_ratio;
    lookY = (ball.pos.y-15)*scale_ratio;
    lookZ = (30.0)*scale_ratio;
    gluLookAt(lookX, lookY, lookZ, ball.pos.x*scale_ratio, ball.pos.y*scale_ratio, 0.0, 0.0, 1.0, 0.5);

    drawfield();
    for (i=0; i<11; i++)
    {
        drawplayer(players1[i]);
        drawplayer(players2[i]);
        printf("player[%d], ac_type: %d, frame: %d\n", i, players1[i].action.ac_type, players1[i].action.frame);
        printf("player[%d], ac_type: %d, frame: %d\n", i+11, players2[i].action.ac_type, players2[i].action.frame);
    }
    printf("\n");
    drawball(ball);
    glPopMatrix();
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
        //strcpy(c.name, "NONE");
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
                    beginZ = (float)x;
                    beginM = (float)0;
                    break;
                default:
                    break;
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            switch (state) {
                case GLUT_DOWN:
                    beginM = (float)y;
                    beginZ = (float)0;
                    //sdepth -= 4.0; 
                    //glutPostRedisplay();
                    break;
                default:
                    break;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            switch (state) {
                case GLUT_UP:
                    beginZ = (float)0;
                    beginM = (float)0;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void motion(int x, int y)
{
    if (beginZ != 0)
        spinz += (float)(beginZ - y) / 50.0;
    if (beginM != 0)
        sdepth -= (float)(beginM - y) / 10.0;
    glutPostRedisplay();
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  color index display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize (Lfield*scale_ratio, Wfield*scale_ratio);
   glutCreateWindow (PACKAGE_STRING);
   init();
   glutReshapeFunc (reshape);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutKeyboardFunc (keyboard);
   glutKeyboardUpFunc (keyup);
   glutTimerFunc (17, playing, 0);
   glutFullScreen();
   glutMainLoop();
   return 0;
}

