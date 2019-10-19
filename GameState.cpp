#include <stdio.h>
#include "Player.h"
#include "Ball.h"
#include "Bullet.h"
#include "Object.h"
#include <iostream>
#include <list>
#include "GameState.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_primitives.h"

using namespace std;

float FPS = 60;
//ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
bool key[3] = { false, false ,false };  //Qui è dove memorizzeremo lo stato delle chiavi a cui siamo interessati.
bool redraw;
bool doexit;

Ball *b,*b2,*b3,*b4,*b5;
Bullet *bullet;

ALLEGRO_BITMAP * sfondo=NULL; //--> poi facciamo una lista di sfondi per ogni livello

GameState::GameState(ALLEGRO_DISPLAY * & d, ALLEGRO_EVENT_QUEUE * & e,int w,int h):State(d,e,w,h)
{
}

GameState::~GameState()
{
	al_destroy_timer(timer);
  al_destroy_bitmap(sfondo);
	//al_destroy_event_queue(event_queue);
  //al_destroy_display(display);
  delete b,b2,b3,b4,b5;
	delete player;
  delete bullet;
}

void GameState::tick()
{
   while(!doexit)
   {
      ALLEGRO_EVENT ev;
      al_wait_for_event(event_queue, &ev);
 
      if(ev.type == ALLEGRO_EVENT_TIMER) 
      {
         /*-------------------MOVIMENTO PLAYER---------------------*/
         if(key[KEY_LEFT] )  
            player->move(SCREEN_W,0);

         if(key[KEY_RIGHT])
             player->move(SCREEN_W,1);

         if(key[SPACE_BAR])
         {
            if (bulletDelay >= firerate)
            {
               bullet=new Bullet(1,BULLET,player->getBouncer_x(),player->getBouncer_y());
               object.push_back(bullet);
               al_set_target_bitmap(al_get_backbuffer(display));
               bulletDelay=0;
            }

            bulletDelay++;
         }

         /*--------------------------MOVIMENTO-------------------------*/
         for(list<DynamicObject*>::iterator it=object.begin();it!=object.end();)
         {
            (*it)->move(SCREEN_W,SCREEN_H);
         /*------------------------------------------------------------*/

            /*------------------COLLISIONI-----------------------*/
            if((*it)->getType()==BALL)
            {
                  if((*it)->collision(player->getBouncer_x(),player->getBouncer_y(),player->BOUNCER_SIZE))
                  {
                     gameOver();
                     it++; 
                  }
                  else if(checkCollision(it))
                  {
                     if((*it)->BOUNCER_SIZE/2>=6)
                     {
                        b4=new Ball(1,BALL,(*it)->BOUNCER_SIZE/2,(*it)->getBouncer_x(),(*it)->getBouncer_y(),(*it)->bouncer_dx,(*it)->bouncer_dy);
                        b5=new Ball(1,BALL,(*it)->BOUNCER_SIZE/2,(*it)->getBouncer_x(),(*it)->getBouncer_y(),-(*it)->bouncer_dx,(*it)->bouncer_dy);
                                                 // cout<<"POSIZIONE PALLA X:"<<(*it)->getBouncer_x()<<" Y:"<<(*it)->getBouncer_y()<<endl;

                        it=object.erase(it); //DISTRUGGO LA PALLA SE HA TOCCATO UN COLPO

                        object.push_back(b4);   
                        object.push_back(b5);
                     }
                     else
                      it=object.erase(it); //DISTRUGGO LA PALLA SE HA TOCCATO UN COLPO

                      al_set_target_bitmap(al_get_backbuffer(display));
                  }
                  else it++;
            }
            else it++; 
            /*----------------------------------------------------*/
         }

         redraw = true;
      }
    

      else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
         break;
      
      else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) //tasto premuto (lo stato del tasto nell'array si setta a true)
      {  
         switch(ev.keyboard.keycode) 
         {
            case ALLEGRO_KEY_LEFT: 
               key[KEY_LEFT] = true;
               break;

            case ALLEGRO_KEY_RIGHT:
               key[KEY_RIGHT] = true;
               break;

            case ALLEGRO_KEY_SPACE:
               key[SPACE_BAR]=true;
               break;
         }
      }

      else if(ev.type == ALLEGRO_EVENT_KEY_UP) //tasto rilasciato (si setta a false)
      {  
         switch(ev.keyboard.keycode) 
         {
            case ALLEGRO_KEY_LEFT: 
               key[KEY_LEFT] = false;
               break;

            case ALLEGRO_KEY_RIGHT:
               key[KEY_RIGHT] = false;
               break;

            case ALLEGRO_KEY_SPACE:
               key[SPACE_BAR]=false;
               break;

            case ALLEGRO_KEY_ESCAPE:
               doexit = true;
               break;

         }
      }
     
      for(list<DynamicObject*>::iterator it2=object.begin();it2!=object.end();)
      {
  
         if((*it2)->getType()==BULLET && (*it2)->getTtl()==0 )  //((*it2)->getType()==BULLET&&(*it2)->getBouncer_x() > SCREEN_W || (*it2)->getType()==BULLET&&(*it2)->getBouncer_y() > SCREEN_H ) 
           it2=object.erase(it2); 
          else 
          {
            (*it2)->decreaseTtl();
            it2++;
          }
     }
    // cout<<"SIZE: "<<object.size()<<endl;

     render();
      /*--------------------------------------------------------------*/
     if(object.size()!=0&&checkLevelOver()){ 
       cout<<"LEVEL OVER";
        doexit=true;
     }
   }
  

   finish=true;
  
}

void GameState::render()
{
      if(redraw && al_is_event_queue_empty(event_queue)) 
      {
         redraw = false;

         al_clear_to_color(al_map_rgb(0,0,0));

         player->render();
      
         for(list<DynamicObject*>::iterator it=object.begin();it!=object.end();it++){ //cout<<"FOR 3:"<<i<<" "<<object.size()<<endl;
            (*it)->render();
         }

         al_flip_display();

      }
}

void GameState::generateBalls()
{
  if(level==1)
  {
     b=new Ball(1,BALL,32,320,120,2,3);
     object.push_back(b);
     return;
  }
   else if(level==2)
  {
      b=new Ball(1,BALL,32,320,120,2,3),b2=new Ball(1,BALL,32,420,160,-2,4),b3=new Ball(1,BALL,32,120,220,2,-4);
     object.push_back(b);
     object.push_back(b2);
     object.push_back(b3);
     return;
  }	
}

bool GameState::checkCollision(list<DynamicObject*>::iterator it )
{
	for(list<DynamicObject*>::iterator it2=object.begin();it2!=object.end();)
  {
     {      
           if((*it2)->getType()==BULLET) 
           {
              Object *o=(*it2);
              if((*it)->collision(o->getBouncer_x(),o->getBouncer_y(),o->BOUNCER_SIZE))
              {
                 it2=object.erase(it2); //DISTRUGGO IL COLPO SE HA TOCCATO UNA PALLA
                 return true;
              }
              if((*it)->getBouncer_x()<o->getBouncer_x()+5 && (*it)->getBouncer_x()>o->getBouncer_x()-5  && (*it)->getBouncer_y()>o->getBouncer_y())
              {
                //cout<<(*it)->getBouncer_x()<<" = "<<o->getBouncer_x()<<" e "<<(*it)->getBouncer_y()<<" > "<<o->getBouncer_y()<<endl;
                 it2=object.erase(it2); //DISTRUGGO IL COLPO SE HA TOCCATO UNA PALLA
                 return true;
              }
           }
     }
     it2++;
  }
   return false;
}


void GameState::init()
{
  finish=false;
  redraw = true;
  doexit = false;
  bulletDelay=10.0f;
  firerate=10.0f;

  if(level==1)
  {
     if(!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return;
     }

     timer = al_create_timer(1.0 / FPS);

     if(!timer) {
        fprintf(stderr, "failed to create timer!\n");
        return;
     }
  //
     if(!al_init_image_addon()) {
         fprintf(stderr, "failed to create image!\n");
        return ;
     }
   
     player=new Player(3,PLAYER);
     player->setBouncer_x(player->getGameAreaW() / 2.0 - player->BOUNCER_SIZE / 2.0);
     player->setBouncer_y(player->getGameAreaH() - player->BOUNCER_SIZE );

     generateBalls();

     sfondo=al_load_bitmap("./resources/sfondo1.bmp"); 
 }
 else
 {
    reset();
 }

   al_set_target_bitmap(al_get_backbuffer(display));

     al_register_event_source(event_queue, al_get_timer_event_source(timer));

     al_register_event_source(event_queue, al_get_keyboard_event_source());

     al_clear_to_color(al_map_rgb(0,0,0));
   
     al_flip_display();
   
     al_start_timer(timer); 
}

void GameState::reset()
{
  al_destroy_timer(timer);
  timer = al_create_timer(1.0 / FPS);

     if(!timer) {
        fprintf(stderr, "failed to create timer!\n");
        return;
     }

  player->reset();
  player->setBouncer_x(player->getGameAreaW() / 2.0 - player->BOUNCER_SIZE / 2.0);
  player->setBouncer_y(player->getGameAreaH() - player->BOUNCER_SIZE );

  object.clear();
  generateBalls();
}

void GameState::gameOver()
{
	cout<<"YOU LOOOOOOOOOOSE!"<<endl;
   return;
}

bool GameState::checkLevelOver()
{
   int numbBalls=0;
   if(object.size()!=0){
   for(list<DynamicObject*>::iterator it2=object.begin();it2!=object.end();it2++)
      if((*it2)->getType()==BALL) numbBalls++;
   if(numbBalls==0) return true;
   }
   return false;
}
