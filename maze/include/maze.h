/*******************************************************************************
 * Copyright (c) 2019 Pierre Engelstein, Alexis Goiset. All rights reserved.
 *  
 * You may not modify, redistribute, sell or use any part of this code without
 * the express permission of Pierre Engelstein and Alexis Goiset
 *******************************************************************************/
#ifndef MAZE_H
#define MAZE_H

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <vector>
#include <GL/gl.h>
#include <map>
#include "lodePNG.h"

#define MAXCAP 5000

/**
 * The class managing the maze.
 * Allows to create and draw the maze.
 **/
class Maze
{
        public:
                Maze();
                virtual ~Maze();
                /* Initializes the maze by creating all the textures we need. */
                void init();

                /* Loads the maze into tab.
                 * @param filename the path to the image describing the maze
                 * @return -1 when error with lodepng
                 * @return w*h the size of the maze
                 */
                int loadMaze(const char *filename);

                /* Draws the maze on screen.
                 * @param squareSize the size of one quad on screen
                 */
                void draw(GLdouble squareSize);

                /* Unveils every block around the given coordinate (radius of 1).
                 * @param x the x coordinate (on the maze coordinate system)
                 * @param y the y coordinate (on the maze coordinate system)
                 */
                void discover(int x, int y);

                /* Unveils every block around the given coordinate (random radius).
                 * @param x the x coordinate (on the maze coordinate system)
                 * @param y the y coordinate (on the maze coordinate system)
                 */
                void DiscoverZone(int x, int y);

                /* Getters */
                int getWidth();
                int getHeight();
                int getStartPosX();
                int getStartPosY();
                int getTabIndex2D(int i, int j);
                int getTabIndex(int index);
                void setTabIndex(int index, int value);
                void setStartPos(int x, int y);
                std::map<int,int> doors;  //Position in array // id of corresponding key
                std::map<int,int> keys;   //Position in array // id of this key
        private:
                /* Draws a colored quad on screen.
                 * @param x & y the coordinate (maze coordinate system) of the quad
                 * @param scale the scale of the rendered quad on screen
                 * @param r & g & b the color of the quad
                 */
                void drawQuadColor(int x, int y, GLfloat scale, GLfloat r, GLfloat g, GLfloat b);

                /* Draws a colored quad on screen.
                 * @param x & y the coordinate (maze coordinate system) of the quad
                 * @param scale the scale of the rendered quad on screen
                 * @param texture the GLuint texture to be drawn on the quad
                 */
                void drawQuadTextured(int x, int y, GLfloat scale, GLuint texture);

                /* Creates an openGL texture from a PNG image 
                 * @param filename the path to the PNG texture
                 * @param textureID the pointer to the texture id to be created
                 */
                int texture(const char *filename, GLuint *textureID);

                void generateHearts();

                /* Counts how many path is there around the given coordinates 
                 * @param x y the coordinates of the block to check
                 */
                int countNbPathAround(int x, int y);
                
                /* Size of the maze */
                int width;
                int height;
                /* Initial position of player on the maze */
                int startPosX;
                int startPosY;
                /* All the texture ID needed */
                GLuint textureWall;
                GLuint textureGround;
                GLuint textureStairDown;
                GLuint textureDoorUnlocked;
                GLuint textureDoorLocked;
                GLuint textureChest;
                GLuint textureChestOpen;
                GLuint textureKey;
                GLuint textureHeart;
                GLuint textureVoid;
                GLuint textureCheckpointValid;
                GLuint textureCheckpointNoValid;
                /* The maze itself, each block being described as an integer */
                int tab[MAXCAP];
        protected:
};

#endif  //MAZE_H