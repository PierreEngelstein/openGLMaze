/*******************************************************************************
 * Copyright (c) 2019 Pierre Engelstein, Alexis Goiset. All rights reserved.
 *  
 * You may not modify, redistribute, sell or use any part of this code without
 * the express permission of Pierre Engelstein and Alexis Goiset
 *******************************************************************************/

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#endif // _WIN32
#include <glm/glm.hpp>
#include <GL/gl.h>
#include <GL/glut.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include "lodePNG.h"
#include "lodePNG.h"
#include "maze.h"

/* Window size */
int width;
int height;
/* How big is a square on screen */
float squareSize = 0.5f;
/* Player characteristics */
int playerPosX = 0;
int playerPosY = 0;
int playerLife = 5;
float playerSize = 0.2f;
float zoomOnPlayer = 5.0f;
/* Current level object, name and ID */
Maze level;
int levelIndicator;
char levelName[64];
/* The texts shown on the HUD */
char helpText[256];
char lifeText[256];
/* The list of all keys of the player */
std::vector<int> keys;

/* Draws the player at the correct position on the maze */
void drawSolv(int posX, int posY, float squareSize, float playerSize)
{
        if(playerSize > squareSize)
        {
                printf("Error : can't have playerSize > squareSize");
                exit(-1);
        }
        int i = posX - level.getWidth()/2;
        int j = posY - level.getHeight()/2;
        glLoadIdentity();
        glBegin(GL_QUADS);
        float pad = (squareSize - playerSize) * 0.5f;
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(j * squareSize + pad,               -(i * squareSize + pad),             0.0);
        glVertex3f(squareSize + j * squareSize  - pad, -(i * squareSize + pad),             0.0);
        glVertex3f(squareSize + j * squareSize  - pad, -(squareSize + i* squareSize - pad), 0.0);
        glVertex3f(j * squareSize + pad,               -(squareSize + i* squareSize - pad), 0.0);
        glEnd();
}

/* Utility function to draw 2D string on the screen */
void drawString(const char *str, glm::vec2 pos, glm::vec3 color, GLfloat textScale)
{
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();
                glColor3f(color.x, color.y, color.z);
                glTranslatef(pos.x, pos.y, 0.0f);
                glScalef(textScale, textScale, textScale);
                for(int i = 0; i < strlen(str); i++)
                        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, str[i]);
        glPopMatrix();
}

void reshapeFunc(int w, int h)
{
        glViewport(0, 0, w, h);
}

/* Checks if the player has the key corresponding to the door at the given index */
void checkIfPlayerHasKey(int playerPos)
{
        std::vector<int>::iterator it = std::find(keys.begin(), keys.end(), level.doors[playerPos]);
        if(it != keys.end())
        {
                sprintf(helpText, "Player has the key, unlocking the door !");
                printf("Player has the key !\n");
                level.setTabIndex(playerPos, 7);
        }else
        {
                sprintf(helpText, "Player does not have the key ! Please get the key %d first", level.doors[playerPos]);
                printf("Player does not have the key !\n");
        }
        glutPostRedisplay();
}

/* Creates a new level from the levelIndicator */
void startLevel()
{
        level = Maze();
        level.init();
        sprintf(levelName, "res/maze%d.png", levelIndicator);
        level.loadMaze(levelName); //Load the maze from the image (.png) given
        /* Setup the player initial position */
        playerPosX = level.getStartPosX();
        playerPosY = level.getStartPosY();
        /* Discovers 8 blocks around the player to give him a start */
        level.discover(playerPosY, playerPosX);
        printf("Level with width=%d; height=%d\n", level.getWidth(), level.getHeight());
}


void endDisplayFunc()
{
        glClearColor(0.22f, 0.22f, 0.22f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if(playerLife > 0)
        {
                drawString("                    You Won !", glm::vec2(100.0, height - 100), glm::vec3(1.0, 1.0, 1.0), 0.1);
                drawString("You successfully found your way out of this maze !", glm::vec2(100.0, height - 140), glm::vec3(1.0, 1.0, 1.0), 0.1);
                drawString("       You are now free to go.", glm::vec2(100.0, height - 160), glm::vec3(1.0, 1.0, 1.0), 0.1);
                drawString("              To exit, press space.", glm::vec2(100.0, height - 200), glm::vec3(1.0, 1.0, 1.0), 0.1);
        }else
        {
                drawString("                   You failed !", glm::vec2(100.0, height - 100), glm::vec3(1.0, 1.0, 1.0), 0.1);
                drawString("You were not able to get out of here and died on your way.", glm::vec2(100.0, height - 140), glm::vec3(1.0, 1.0, 1.0), 0.1);
                drawString("       I'm sorry but you will have to try again.", glm::vec2(100.0, height - 160), glm::vec3(1.0, 1.0, 1.0), 0.1);
                drawString("              To exit, press space.", glm::vec2(100.0, height - 200), glm::vec3(1.0, 1.0, 1.0), 0.1);

        }
        glutSwapBuffers();
}

void endKeyboardFunc(unsigned char key, int x, int y)
{
        if(key == ' ')
        {
                exit(1);
        }
}

/* Actions performed when  */
void checkPlayerCurrentPos()
{
        switch(level.getTabIndex(playerPosY + (playerPosX) * level.getWidth()))
        {
                case 4: /* The player is on a stair : jump to the next level */
                        levelIndicator++;
                        sprintf(helpText, "Jumped to level %d !", levelIndicator);
                        keys.clear();
                        startLevel();
                        glutPostRedisplay();
                        break;
                case 14: /* Chest where it adds one heart to the player */
                        playerLife++;
                        sprintf(helpText, "Added one heart to player !");
                        level.setTabIndex(playerPosY + (playerPosX) * level.getWidth(), 19);
                        break;
                case 15: /* Chest where it discovers a part of the maze */
                        sprintf(helpText, "Discovered a part of the maze !");
                        level.DiscoverZone(playerPosY, playerPosX);
                        level.setTabIndex(playerPosY + (playerPosX) * level.getWidth(), 19);
                        glutPostRedisplay();
                        break;
                case 18: /* Empty chest */
                        sprintf(helpText, "Sorry, nothing here !\n");
                        level.setTabIndex(playerPosY + (playerPosX) * level.getWidth(), 19);
                        break;
                case 21: /* The void : the player loses one heart, all his keys and goes back to the start */
                        sprintf(helpText, "You felt into the void ! The anger of Daedalus sends you back to the start !");
                        startLevel();
                        keys.clear();
                        playerLife--;
                        glutPostRedisplay();
                        break;
                case 22: /* A heart : +1 life for the player */
                        sprintf(helpText, "Given back the player 1 heart !");
                        playerLife++;
                        level.setTabIndex(playerPosY + (playerPosX) * level.getWidth(), 1);
                        glutPostRedisplay();
                        break;
                case 25: /* The player has reached the end of the maze */
                        glutDisplayFunc(endDisplayFunc);
                        glutKeyboardFunc(endKeyboardFunc);
                        glutPostRedisplay();
                case 11: /* Add a key to player's backpack (only if he does not have it) */
                        std::vector<int>::iterator it = std::find(keys.begin(), keys.end(), level.keys[playerPosY + (playerPosX) * level.getWidth()]);
                        if (it != keys.end())
                        {
                                sprintf(helpText, "Player already has the key %d", level.keys[playerPosY + (playerPosX) * level.getWidth()]);
                                printf("Already found this key !\n");
                        }
                        else
                        {
                                keys.insert(keys.begin(), level.keys[playerPosY + (playerPosX) * level.getWidth()]);
                                printf("Have not found this key yet !\n");
                                sprintf(helpText, "Added key %d to player's backpack", level.keys[playerPosY + (playerPosX) * level.getWidth()]);
                                printf("Added key %d to player !\n", keys.back());
                                level.setTabIndex(playerPosY + (playerPosX) * level.getWidth(), 1);
                        }
                        glutPostRedisplay();
                        break;
                
        }
}

void keyboardFunc(unsigned char key, int x, int y)
{
        switch(key)
        {
                case 'k':
                        zoomOnPlayer -= 0.5f;
                        break;
                case 'l':
                        zoomOnPlayer += 0.5f;
                        break;
                case 'z':
                        if(playerPosX > 0 && level.getTabIndex(playerPosY + (playerPosX-1) * level.getWidth()) != 0)
                        {
                                if(level.getTabIndex(playerPosY + (playerPosX-1) * level.getWidth()) != 8)
                                        playerPosX--;
                                else
                                        checkIfPlayerHasKey(playerPosY + (playerPosX-1) * level.getWidth());
                                        
                        }
                        break;
                case 's':
                        if(playerPosX < level.getHeight()-1 && level.getTabIndex(playerPosY + (playerPosX+1) * level.getWidth()) != 0)
                        {
                                if(level.getTabIndex(playerPosY + (playerPosX+1) * level.getWidth()) != 8)
                                        playerPosX++;
                                else
                                        checkIfPlayerHasKey(playerPosY + (playerPosX+1) * level.getWidth());
                        }
                        break;
                case 'q':
                        if(playerPosY > 0 && level.getTabIndex(playerPosY-1 + (playerPosX) * level.getWidth()) != 0)
                        {
                                if(level.getTabIndex(playerPosY-1 + (playerPosX) * level.getWidth()) != 8)
                                        playerPosY--;
                                else
                                        checkIfPlayerHasKey(playerPosY-1 + (playerPosX) * level.getWidth());
                        }
                        break;
                case 'd':
                        if(playerPosY < level.getWidth()-1 && level.getTabIndex(playerPosY+1 + (playerPosX) * level.getWidth()) != 0)
                        {
                                if(level.getTabIndex(playerPosY+1 + (playerPosX) * level.getWidth()) != 8)
                                        playerPosY++;
                                else
                                        checkIfPlayerHasKey(playerPosY+1 + (playerPosX) * level.getWidth());
                        }
                        break;
                default:
                        break;
        }
        level.discover(playerPosY, playerPosX);
        glutPostRedisplay();
}

void specialFunc(int key, int x, int y)
{
}

void displayFunc()
{
        glClearColor(0.22f, 0.22f, 0.22f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Setup the projection mode (center the screen on the player with a zoom coefficient) */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        int i = playerPosX - level.getWidth()/2;
        int j = playerPosY - level.getHeight()/2;
        float playerCenterX = squareSize * (2 * j + 1) * 0.5;
        float playerCenterY = -squareSize * (2 * i + 1) * 0.5;
        glLoadIdentity();
        gluOrtho2D(playerCenterX - zoomOnPlayer, playerCenterX + zoomOnPlayer, playerCenterY - zoomOnPlayer, playerCenterY + zoomOnPlayer);
        glMatrixMode(GL_MODELVIEW);

        /* Render all that is necessary */
        level.draw(squareSize);
        drawSolv(playerPosX, playerPosY, squareSize, 0.3f);
        checkPlayerCurrentPos();

        /* Draw the HUD */
        drawString(helpText, glm::vec2(0.0, height - 20), glm::vec3(1.0, 1.0, 1.0), 0.1);
        sprintf(lifeText, "Player has %d hearts", playerLife);
        drawString(lifeText, glm::vec2(0.0, height - 40), glm::vec3(1.0, 1.0, 1.0), 0.1);

        /* If the player has lost */
        if(playerLife == 0)
        {
                printf("You Lost !");
                glutDisplayFunc(endDisplayFunc);
                glutKeyboardFunc(endKeyboardFunc);
                glutPostRedisplay();
        }
        glutSwapBuffers();
}

void startDisplayFunc()
{
        glClearColor(0.22f, 0.22f, 0.22f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawString("             Welcome to the maze.", glm::vec2(100.0, height - 100), glm::vec3(1.0, 1.0, 1.0), 0.1);
        drawString("Your objective is to go through all the level and not die.", glm::vec2(100.0, height - 140), glm::vec3(1.0, 1.0, 1.0), 0.1);
        drawString("       You can move with the arrows z q s d.", glm::vec2(100.0, height - 160), glm::vec3(1.0, 1.0, 1.0), 0.1);
        drawString("              To start, press space.", glm::vec2(100.0, height - 200), glm::vec3(1.0, 1.0, 1.0), 0.1);
        
        glutSwapBuffers();
}

void startKeyboardFunc(unsigned char key, int x, int y)
{
        if(key == ' ')
        {
                        printf("Starting the game !\n");
                        /* Level initialisation */
                        levelIndicator = 0;
                        startLevel();
                        /* Set the main keyboard and display func for the game */
                        glutKeyboardFunc(keyboardFunc);
                        glutDisplayFunc(displayFunc);
                        glutPostRedisplay();
        }
}

int main(int argc, char** argv)
{
        glutInit(&argc, argv);
        srand(time(NULL));

/* Disable the console when running on Windows */
#ifdef _WIN32
		FreeConsole();
#endif // _WIN32

        /* Window initialization */
        width = 800;
        height = 800;
        int wx = (glutGet(GLUT_SCREEN_WIDTH) - width) / 4.0;
        int wy = (glutGet(GLUT_SCREEN_HEIGHT) - height) / 2.0;
        glutInitWindowPosition(wx, wy);
        glutInitWindowSize(width, height);
        gluPerspective(90, width/height, 0.0f, 1000.0f);
        GLuint window = glutCreateWindow("The Amazing Daedalus");

        glClearColor(0.0, 0.0, 0.0, 0.0);

        /* Callbacks setup */
        glutReshapeFunc(reshapeFunc);
        glutKeyboardFunc(startKeyboardFunc);
        glutSpecialFunc(specialFunc);
        glutDisplayFunc(startDisplayFunc);

        /* Jump to the main loop */
        glutMainLoop();
        return 0;
}
