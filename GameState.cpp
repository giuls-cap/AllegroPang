#include <list>
#include "GameState.h"
#include <ctime>

//ALLEGRO_EVENT_QUEUE *event_queue = NULL;

bool key[3] = { false, false ,false };  //Qui è dove memorizzeremo lo stato delle chiavi a cui siamo interessati.
bool doexit;
int ran;

Ball *b,*b2,*b3,*b4,*b5;
Weapons *bullet;
Bonus * newBonus;

//VARIABILI CHE SERVONO PER ATTIVARE I BONUS PRESI 
bool orologio,arpione,machineGun;

ALLEGRO_BITMAP * sfondi[3];

//-------------------------------------BLOCCO FUNZIONI 1----------------------------------------
GameState::GameState(ALLEGRO_DISPLAY * & d,ALLEGRO_EVENT_QUEUE * &e,ALLEGRO_TIMER * &t,int w,int h):State(d,e,t,w,h)
{}

GameState::~GameState()
{
  delete b,b2,b3,b4,b5;
  delete player;
  delete bullet;
  delete newBonus;
}
//--------------------------------------------------------------------------------------------------------


//++++++++++++++++++++++++++++++++++BLOCCO FUNZIONI 2++++++++++++++++++++++++++++++++++++++++++++++++++++++
void GameState::init()
{
  orologio=false;
  arpione=false;
  machineGun=false;
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
     if(!al_init_image_addon()) {
         fprintf(stderr, "failed to create image!\n");
        return ;
     }
     al_init_font_addon(); al_init_ttf_addon();
     
    pangFont=al_load_ttf_font("pangFont.ttf",30,0 ); //il secondo paramentro è la size, il terzo il flag
    if(!pangFont)
      cout<<"NO FONT";
   
     player=new Player(PLAYER);
     generateBalls();


     sfondi[0]=al_load_bitmap("./resources/sfondo1.png");
     sfondi[1]=al_load_bitmap("./resources/sfondo2.png");
     sfondi[2]=al_load_bitmap("./resources/sfondo1.png");
 }
 else
 {
    reset();
 }

  al_set_target_bitmap(al_get_backbuffer(display));

  al_register_event_source(event_queue, al_get_keyboard_event_source());

  al_clear_to_color(al_map_rgb(0,0,0));

  al_flip_display();
}

void GameState::generateBalls()
{
  if(level==1)
  {
     b=new Ball(BALL,32,320,120,2,3);
     object.push_back(b);
     return;
  }
   else if(level==2)
  {
      b=new Ball(BALL,32,320,120,2,3),b2=new Ball(BALL,32,420,160,-2,4),b3=new Ball(BALL,32,120,220,2,-4);
     object.push_back(b);
     object.push_back(b2);
     object.push_back(b3);
     return;
  } 
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
            player->move(0);

         if(key[KEY_RIGHT])
             player->move(1);

         if(key[SPACE_BAR])
         {
            if (bulletDelay >= firerate)
            {
              if(!machineGun)
              {
                bullet=new Weapons(WEAPONS,player->getBouncer_x(),player->getBouncer_y());
              }
              else 
              {
                bullet=new MachineGun(WEAPONS,player->getBouncer_x(),player->getBouncer_y());
              }

              object.push_back(bullet);
              al_set_target_bitmap(al_get_backbuffer(display));
              bulletDelay=0;
            }

            bulletDelay++;
         }

         /*-------------------------MOVIMENTO Object-------------------------*/
         for(list<DynamicObject*>::iterator it=object.begin();it!=object.end();)
         {
            if((*it)->getType()==WEAPONS && !arpione && (*it)->dead)
            {
              object.erase(it); break;
            }
             //AGGIUNGERE UN TIMER CHE DOPO TOT SECONDI SBLOCCA LE PALLE
            else if((*it)->getType()!=BALL)  //se abbiamo il potere dell'orologio le palle devono restare ferme
              (*it)->move(SCREEN_W);
            else if(orologio==false)
              (*it)->move(SCREEN_W);
         /*------------------------------------------------------------*/
            
           /*------------------COLLISIONI OBJECT-----------------------*/
            if((*it)->getType()==BALL)
            { 
              BallCollision(it);
            }
            else it++; 
            /*----------------------------------------------------*/
         }

          //----------------COLLISIONI E MOVIMENTO BONUS---------------------
           for(list<Bonus*>::iterator it4=bonus.begin();it4!=bonus.end();)
            {
              (*it4)->move(0);
              //SE IL PLAYER PRENDE IL BONUS
               if((*it4)->collision(player->getBouncer_x(),player->getBouncer_y(),player->BOUNCER_SIZE))
              {
                findPower((*it4)->bonusType);
                it4=bonus.erase(it4);
              }
              else it4++;
            }
        //-------------------------------------------------------------


         redraw = true;
      }
    

      else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //esci=true;
         break;}
      
    setKey(ev);
    TtlManager(); 
    render();
      /*--------------------------------------------------------------*/
     if(object.size()!=0&&checkLevelOver())
     { 
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

         al_draw_scaled_bitmap(sfondi[level-1], 0, 0, al_get_bitmap_width(sfondi[level-1]), al_get_bitmap_height(sfondi[level-1]), 0, 0, gameAreaW, gameAreaH+20, 1);

         player->render();
      
         for(list<DynamicObject*>::iterator it=object.begin();it!=object.end();it++){ //cout<<"FOR 3:"<<i<<" "<<object.size()<<endl;
              (*it)->render();
         }

        for(list<Bonus*>::iterator it2=bonus.begin();it2!=bonus.end();it2++){ //cout<<"FOR 3:"<<i<<" "<<object.size()<<endl;
            (*it2)->render(); 
         }

      drawBar();

      al_flip_display();

      }
}

void GameState:: drawBar()
{
  al_draw_text(pangFont, al_map_rgb(30, 80, 255), 400, 600,ALLEGRO_ALIGN_LEFT, "Time: "); //w h
  int lw=50;
  al_draw_text(pangFont, al_map_rgb(30, 80, 255), lw, 550,ALLEGRO_ALIGN_LEFT, "Life: ");
  for (int i=1;i<=player->getLife();i++)
    al_draw_text(pangFont, al_map_rgb(30, 80, 255), lw*i, 600,ALLEGRO_ALIGN_LEFT, "* ");
  
}

void GameState::setKey(ALLEGRO_EVENT ev)
{
  if(ev.type == ALLEGRO_EVENT_KEY_DOWN) //tasto premuto (lo stato del tasto nell'array si setta a true)
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
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//----------------------------BLOCCO FUNZIONI 3--------------------------------------------------------

void GameState::BallCollision(list<DynamicObject*>::iterator &it)
{
  int posX=(*it)->getBouncer_x();
  int posY=(*it)->getBouncer_y();

    if((*it)->collision(player->getBouncer_x(),player->getBouncer_y(),player->BOUNCER_SIZE))
    {
      //METTERE TIMER PURE QUI, SENNÒ PERDE TUTTE LE VITE INSIEME
      player->RemoveOneLife();
       gameOver();
       it++;
       return; 
    }
    else if(checkCollision(it)) //VEDE SE LA PALLA COLLIDE CON IL COLPO
    {
       if((*it)->BOUNCER_SIZE/2>=6) //CREA LE PALLE PICCOLE 
       {
          b4=new Ball(BALL,(*it)->BOUNCER_SIZE/2,(*it)->getBouncer_x(),(*it)->getBouncer_y(),(*it)->bouncer_dx,(*it)->bouncer_dy);
          b5=new Ball(BALL,(*it)->BOUNCER_SIZE/2,(*it)->getBouncer_x(),(*it)->getBouncer_y(),-(*it)->bouncer_dx,(*it)->bouncer_dy);
                                   // cout<<"POSIZIONE PALLA X:"<<(*it)->getBouncer_x()<<" Y:"<<(*it)->getBouncer_y()<<endl;

          it=object.erase(it); //DISTRUGGO LA PALLA SE HA TOCCATO UN COLPO

          object.push_back(b4);   
          object.push_back(b5);
       }
       else
        it=object.erase(it); //DISTRUGGO LA PALLA SE HA TOCCATO UN COLPO

      //---------------CREO I BONUS RANDOMICAMENTE-------------//
      srand(time(NULL));
      createBonus(posX,posY);
      //-------------------------------------------------------//
        al_set_target_bitmap(al_get_backbuffer(display));
      return;
    }
    else it++;
  return;
}

bool GameState::checkCollision(list<DynamicObject*>::iterator it )
{
  for(list<DynamicObject*>::iterator it2=object.begin();it2!=object.end();)
  {
     {      
           if((*it2)->getType()==WEAPONS) 
           {
              Object *o=(*it2);
              if((*it)->collision(o->getBouncer_x(),o->getBouncer_y(),o->BOUNCER_SIZE))
              {
                 it2=object.erase(it2); //DISTRUGGO IL COLPO SE HA TOCCATO UNA PALLA
                 return true;
              }
              if((*it)->getBouncer_x()<o->getBouncer_x()+5 && (*it)->getBouncer_x()>o->getBouncer_x()-5  && (*it)->getBouncer_y()>o->getBouncer_y())
              {
                 it2=object.erase(it2); //DISTRUGGO IL COLPO SE HA TOCCATO UNA PALLA
                 return true;
              }
           }
     }
     it2++;
  }
   return false;
}

void GameState::createBonus(int posX, int posY)
{
   //FACCIAMO CHE SE ESCE 5(I TIPI DI BONUS SONO 5) IL BONUS NON DEVE USCIRE
      //ALTRIMENTI GENERIAMO UN BONUS E GLI PASSIAMO IL ran CHE SARÀ IL TIPO DI BONUS
      ran=rand()%4;
     if( ran<=2 && bonus.size()<=4)
      {
        newBonus=new Bonus(BONUS,ran,posX,posY);
        bonus.push_back(newBonus);
      }
}

void GameState::findPower(int t)
{
  if(t==OROLOGIO){ 
    orologio=true;
  }
  else if(t==ARPIONE){ 
    arpione=true;
    machineGun=false;
  }
  else if(t==MACHINEGUN){ 
    machineGun=true;
    arpione=false;
  }
}

void GameState::TtlManager()
{
  for(list<DynamicObject*>::iterator it2=object.begin();it2!=object.end();)
      {
  
         if((*it2)->getType()==WEAPONS && (*it2)->getTtl()==0 )  //((*it2)->getType()==BULLET&&(*it2)->getBouncer_x() > SCREEN_W || (*it2)->getType()==BULLET&&(*it2)->getBouncer_y() > SCREEN_H ) 
           it2=object.erase(it2); 
          else 
          {
            (*it2)->decreaseTtl();
            it2++;
          }
      }
}



//++++++++++++++++++++++++++++BLOCCO FUNZIONI 4++++++++++++++++++++++++++++++++++++++++++++++++++++++

void GameState::gameOver()
{

  if(player->getLife()==0)
  cout<<"YOU LOOOOOOOOOOSE!"<<endl;

  /*al_clear_to_color(al_map_rgb(0,0,0));
  al_draw_bitmap(gameover1,  0,0, 0);
  al_flip_display();
  al_rest(0.5);*/
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

void GameState::reset()
{
  player->reset();

  object.clear();
  bonus.clear();
  generateBalls();
}




