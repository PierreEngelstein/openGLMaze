/*******************************************************************************
 * Copyright (c) 2019 Pierre Engelstein, Alexis Goiset. All rights reserved.
 *  
 * You may not modify, redistribute, sell or use any part of this code without
 * the express permission of Pierre Engelstein and Alexis Goiset
 *******************************************************************************/

#include "maze.h"

Maze::Maze()
{
        textureWall = 0;
        textureGround = 0;
        textureStairDown = 0;
        textureDoorUnlocked = 0;
        textureDoorLocked = 0;
        textureChest = 0;
        textureChestOpen = 0;
        textureHeart = 0;
        textureKey = 0;
        textureVoid = 0;
        textureCheckpointValid = 0;
        textureCheckpointNoValid = 0;
}

void Maze::init()
{
        texture("res/wall.png", &textureWall);
        texture("res/ground.png", &textureGround);
        texture("res/stairDown.png", &textureStairDown);
        texture("res/door_unlocked.png", &textureDoorUnlocked);
        texture("res/door_locked.png", &textureDoorLocked);
        texture("res/chest.png", &textureChest);
        texture("res/chest_opened.png", &textureChestOpen);
        texture("res/heart.png", &textureHeart);
        texture("res/key.png", &textureKey);
        texture("res/void.png", &textureVoid);
        texture("res/checkpoint_valid.png", &textureCheckpointValid);
        texture("res/checkpoint_novalid.png", &textureCheckpointNoValid);
}

int Maze::countNbPathAround(int x, int y)
{
        int res = 0;
        for(int i = y - 1; i <= y + 1; i++)
        {
                for(int j = x - 1; j <= x + 1; j++)
                {
                        if(i >= 0 && i < width && j >= 0 && j < height)
                        {
                                //If the block was not discovered before, unveil it
                                if(tab[j+i*width]==3)
                                {
                                        res++;
                                }
                        }
                }
        }
        printf("There is %d path around %d %d\n", res, x, y);
        return res;
}

void Maze::generateHearts()
{
        printf("Generating %d hearts and voids\n", width/8);
        int nbHearts = width/8;
        while(nbHearts != 0)
        {
                int posX = rand()%height;
                int posY = rand()%width;
                if(tab[posY + posX * width] == 3) //If the chosen block is a path
                {
                        tab[posY + posX * width] = 24;
                        nbHearts--;
                }
        }
}

int Maze::loadMaze(const char *filename)
{
        /* Read the image */
        std::vector<unsigned char> image;
        unsigned int imgWidth, imgHeight;
        unsigned error = lodepng::decode(image, imgWidth, imgHeight, filename);
        width = imgWidth;
        height = imgHeight;
        if(error)
        {
                printf("Failed to read image %s. Lodepng error : %s\n", filename, lodepng_error_text(error));
                return -1;
        }
        if(width * height > MAXCAP) /* Make sure that our level is not too big compared to the size of the array */
        {
                printf("Can't read level, file too big !\n");
                exit(-2);
        }
        /* Go through each pixel and translate them into an array of integer */
        for(int i = 0;i<height;i++)
        {
                for (int j =0; j<width;j++)
                {
                        int index = j * 4; //one pixel is 4 integers in image array (RGBA)
                        /* Get the r g b values of each pixel */
                        unsigned char r = image[index   + i*width*4];
                        unsigned char g = image[index+1 + i*width*4];
                        unsigned char b = image[index+2 + i*width*4];
                        if (r!=255 || g!=255 || b!=255)
                        {
                                if(r == 250 && g == 250 && b == 250) //Spawn player
                                {
                                        if(startPosX == -1 && startPosY == -1)
                                        {
                                                startPosY = j;
                                                startPosX = i;
                                        }
                                        printf("Spawned player at %d %d\n", startPosX, startPosY);
                                        tab[j+ i*width]=3;
                                }else if(r == 200 && g == 200 && b == 200) // A stair
                                        tab[j+ i*width]=6;
                                else if(r == 0 && g == 255 && b == 0) //Unlocked door
                                        tab[j+ i*width]=9;
                                else if(r != 0 && g == 0 && b == 0) //Locked door
                                {
                                        tab[j+ i*width]=10;
                                        doors.insert(std::pair<int,int>(j+ i*width,r));
                                        printf("Added door of id %d at index %d\n", doors[j+ i*width], j+ i*width);
                                }
                                else if(r == 0 && g == 0 && b != 0)  //Key
                                {
                                        tab[j+ i*width]=13;
                                        keys.insert(std::pair<int,int>(j+ i*width,b));
                                        printf("Added key %d at index %d\n", keys[j+ i*width], j+ i*width);
                                }
                                else if(r == 150 && g == 150 && b == 150) //A chest
                                {
                                        int choice = rand()%3;
                                        switch(choice)
                                        {
                                                case 0: //Give a heart
                                                        tab[j+ i*width]=16;
                                                        break;
                                                case 1: //Discovers a part of the map
                                                        tab[j+ i*width]=17;
                                                        break;
                                                case 2: //Gives nothing
                                                        tab[j+ i*width]=20;
                                                        break;
                                        }
                                }
                                else if(r == 100 && g == 100 && b == 100) //The void
                                        tab[j+ i*width]=23;
                                else if(r == 70 && g == 70 && b == 70) //A heart
                                        tab[j+ i*width]=24;
                                else if(r == 25 && g == 25 && b == 25) //The end of the game
                                        tab[j + i * width] = 27;
                                else if(r == 80 && g == 80 && b == 80) //A checkpoint
                                        tab[j + i * width] = 30;
                                else
                                        tab[j+ i*width]=2; //A path
                        }
                        else
                                tab[j+ i*width]=3; //A wall
                }
        }
        Maze::generateHearts();
        return width * height;
}

void Maze::draw(GLdouble squareSize)
{
        glLoadIdentity();
        for(int i = -height/2; i < height/2; i++)
        {
                for(int j = -width/2; j < width/2; j++)
                {
                        //Offset the coordinates by -w/2; -h/2 to be centered on (0, 0) on screen
                        int index = (i + height/2) * width + (j + height/2);
                        GLuint texture;
                        switch(getTabIndex(index))
                        {
                                case 0:
                                        texture = textureWall;
                                        break;
                                case 1:
                                        texture = textureGround;
                                        break;
                                case 4:
                                case 25:
                                        texture = textureStairDown;
                                        break;
                                case 7:
                                        texture = textureDoorUnlocked;
                                        break;
                                case 8:
                                        texture = textureDoorLocked;
                                        break;
                                case 11:
                                        texture = textureKey;
                                        break;
                                case 14:
                                case 15:
                                case 18:
                                        texture = textureChest;
                                        break;
                                case 19:
                                        texture = textureChestOpen;
                                        break;
                                case 21:
                                        texture = textureVoid;
                                        break;
                                case 22:
                                        texture = textureHeart;
                                        break;
                                case 28:
                                        texture = textureCheckpointValid;
                                        break;
                                case 29:
                                        texture = textureCheckpointNoValid;
                                        break;
                                default:
                                        texture = 0;
                                        break;
                        }
                        if(texture == 0)
                                drawQuadColor(j + 1, -i, squareSize, 0.22f, 0.22f, 0.22f);
                        else
                                drawQuadTextured(j + 1, -i, squareSize, texture);
                }
        }
}

void Maze::discover(int x, int y)
{
        for(int i = y - 1; i <= y + 1; i++)
        {
                for(int j = x - 1; j <= x + 1; j++)
                {
                        if(i >= 0 && i < width && j >= 0 && j < height)
                        {
                                //If the block was not discovered before, unveil it
                                if(tab[j+i*width]==2 || tab[j+i*width]==3 || tab[j+i*width] ==  6 || tab[j+i*width] ==  9 || tab[j+i*width] ==  10 || tab[j+i*width] ==  13 || tab[j+i*width] ==  16 || tab[j+i*width] ==  17
                                || tab[j+i*width] ==  20 || tab[j+i*width] ==  23 || tab[j+i*width] ==  24 || tab[j+i*width] ==  27 || tab[j+i*width] ==  30)
                                {
                                        tab[j+i*width] -= 2;
                                }
                        }
                }
        }
}

void Maze::DiscoverZone(int x, int y)
{
        int size=rand()%(width/8);
        // printf("Discovering a zone of %d by %d", size, size);
        for(int i = y - size; i <= y + size; i++)
        {
                for(int j = x - size; j <= x + size; j++)
                {
                        if(i >= 0 && i < width && j >= 0 && j < height)
                        {
                                //If the block was not discovered before, unveil it
                                if(tab[j+i*width]==2 || tab[j+i*width]==3 || tab[j+i*width] ==  6 || tab[j+i*width] ==  9 || tab[j+i*width] ==  10 || tab[j+i*width] ==  13 || tab[j+i*width] ==  16 || tab[j+i*width] ==  17
                                || tab[j+i*width] ==  20 || tab[j+i*width] ==  23 || tab[j+i*width] ==  24 || tab[j+i*width] ==  27 || tab[j+i*width] ==  30)
                                {
                                        tab[j+i*width] -= 2;
                                }
                        }
                }
        }
}

void Maze::drawQuadColor(int x, int y, GLfloat scale, GLfloat r, GLfloat g, GLfloat b)
{
        glPushMatrix();
                glScalef(scale, scale, scale);
                glTranslatef(x, y, 0);
                glDisable(GL_TEXTURE_2D);
                glColor3f(r, g, b);
                glBegin(GL_QUADS);
                        glVertex3f(0, 0, 0);
                        glVertex3f(-1, 0, 0);
                        glVertex3f(-1, -1, 0);
                        glVertex3f(0, -1, 0);
                glEnd();
        glPopMatrix();
}

void Maze::drawQuadTextured(int x, int y, GLfloat scale, GLuint texture)
{
        glPushMatrix();
                glScalef(scale, scale, scale);
                glTranslatef(x, y, 0);
                //Enables and binds the texture
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture);
                glBegin(GL_QUADS);
                        glTexCoord2f(1.0f, 0.0f);
                        glVertex3f(0, 0, 0);
                        glTexCoord2f(0.0f, 0.0f);
                        glVertex3f(-1, 0, 0);
                        glTexCoord2f(0.0f, 1.0f);
                        glVertex3f(-1, -1, 0);
                        glTexCoord2f(1.0f, 1.0f);
                        glVertex3f(0, -1, 0);
                //Unbind the texture, to make sure that the next drawn element is correctly colored / textured
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_TEXTURE_2D);
                glEnd();
        glPopMatrix();
}

int Maze::texture(const char *filename, GLuint *textureID)
{
        unsigned width, height;
        /* Generates and binds in memory a texture unit */
        glGenTextures(1, textureID);
        glBindTexture(GL_TEXTURE_2D, *textureID);
        /* Read the PNG image */
        std::vector<unsigned char> image;
        unsigned error = lodepng::decode(image, width, height, filename);
        if(error)
        {
                printf("Failed to read image %s. Lodepng error : %s\n", filename, lodepng_error_text(error));
                return 1;
        }
        /* Creates the 2D texture from the image array */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
        /* Make sure we have indeed pixel-per-pixel texture (no linear interpolation that would blur the texture) */
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        printf("Loaded texture %s\n", filename);
        return 0;
}

int Maze::getWidth()
{
        return width;
}
int Maze::getHeight()
{
        return height;
}
int Maze::getStartPosX()
{
        return startPosX;
}
int Maze::getStartPosY()
{
        return startPosY;
}
void Maze::setStartPos(int x, int y)
{
        startPosX = x;
        startPosY = y;
}
int Maze::getTabIndex2D(int i, int j)
{
        return tab[i * width + j];
}
int Maze::getTabIndex(int index)
{
        return tab[index];
}

void Maze::setTabIndex(int index, int value)
{
        tab[index] = value;
}

Maze::~Maze()
{

}