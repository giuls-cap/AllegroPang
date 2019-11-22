#include "Ball.h"
#include<iostream>
#include <random>
#include <cstdlib>
#include <time.h>
using namespace std;

Ball::Ball(Type t,int s,float initialPosX, float initialPosY, float dx,float dy ):DynamicObject(t)
{
	BOUNCER_SIZE=s;
   
   bouncer_dx=dx;
   bouncer_dy=dy;
	bouncer_x=initialPosX;
   bouncer_y=initialPosY;

   if(s==32)
   image=al_load_bitmap("./resources/ballGrande.bmp"); 
   else if(s==16)
      image=al_load_bitmap("./resources/ballMedia.bmp"); 
   else
      image=al_load_bitmap("./resources/ballPiccola.bmp"); 
   if(!image)
      cout<<"ERROR TO LOAD IMMAGE";
}

Ball::~Ball()
{ 
}


void Ball::move(int SCREEN_W)  //togliere
{
   float multiplier=0;
   if(BOUNCER_SIZE>=32)multiplier=2.0;
   else if(BOUNCER_SIZE<=16 && BOUNCER_SIZE>8) multiplier =1.5;
   else multiplier =1;
	 
    if(bouncer_x < BOUNCER_SIZE || bouncer_x > (gameAreaW-40) - BOUNCER_SIZE) {  //invertire la direzione. Inseriamo la logica nell'evento timer, in modo che la bitmap che rimbalza 
            bouncer_dx = -bouncer_dx;          //^il 40 è per non fargli oltrepassare il muro         si sposti alla stessa velocità su qualsiasi computer.
         }

         if(bouncer_y < 0 || bouncer_y > (gameAreaH-20) - BOUNCER_SIZE) {
            bouncer_dy = -bouncer_dy;     //^il 20 è per non fargli oltrepassare il muro
         }

         bouncer_x += bouncer_dx;
         bouncer_y += bouncer_dy;   
         if(bouncer_dy!=24)
         if(bouncer_dy>6.0*multiplier) bouncer_dy-=0.3; // se aumenti il 24 aumenta anche la grandezza del salto
         else
         bouncer_dy+=0.3;
         
         //cout<<"X: "<<bouncer_dx<<" Y:"<<bouncer_dy<<endl;
}
void Ball::render()
{   
   //cout<<"POSIZIONE PALLA X:"<<bouncer_x<<" Y:"<<bouncer_y<<endl;
	al_draw_bitmap(image, bouncer_x, bouncer_y, 0);
}   

