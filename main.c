#include <stdio.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>




//-----------------------------------------------
//TESTE CAIO COMETAS
//const int NUM_COMETA = 10;
//InitCometa(cometa, NUM_COMETA);


//void InitCometa(Cometa cometas[],int size)
//{
//  for(int i = 0;i < size;i++)
//    {
//      cometa[i].ID = ENEMY;
//      cometa[i].live = false;
//      cometa[i].speed = 5; VELOCIDADE DO COMETA, AJUSTAR DE ACORDO COM O JOGO
//      cometa[i].bondx = 18; LEMBRAR DE EXPLICAR ESSES COMANDOS DEPOIS
//      cometa[i].bondy = 18;  ''
//    };
//};


//DrawCometa(cometas,NUM_COMETA);
//void DrawCometa(Cometa cometas[],int size)
//{
// for(int i = 0;i < size;i++)
//   {
//     if(cometa[i].live)
//      {
//        al_draw_filled_circle(cometas[i].x,cometas[i].y,20,al_map_rgb(255,0,0));
//      }
//   }
//};
//void StartCometa(Cometa cometas[],int size);
//void UpdateCometa(Cometa cometas[],int size);
//-----------------------------------------------

//ISSUES:
//*PENSAR MAIS SOBRE O JOGO
//*VIDA(WEI)
//*FAZER OS BONECOS ANDAREM NO CAMINHO
//*ATIRAR (TIRO COLIDIR)(GABRIEL)
//*GERAR ASTEROIDES ALEATORIAMENTE NO MAPA(CAIO)
//*MOVIMENTAÇÃO DA NAVE (DANIEL)
//*COMETA AINDA SOME E VOLTA (IMPLANTAR PRECISAMENTE A COLISÃO NAVE-COMETA) (JOÃO)

//AO APAGAR QUALQUER OBJETO LEMBRAR:
//*APAGAR OS DADOS QUE CRIAM O OBJETO
//*APAGAR OS COMANDOS DO TECLADO
//*APAGAR O COMANDO DE DESENHO
//*APAGAR OS DESTROYERS

const float FPS = 60;
const int WORLD_W = 3000;
const int WORLD_H = 1500;
const int VELOCITY = 2;
const int DELAY = 6;

void MovimentoAsteroide(float dist, int shipWidth, int astWidth, int WORLD_W, int WORLD_H, int* dirAst, int* xAst, int* yAst)
{

        if(*dirAst == 1 && *xAst != astWidth && *yAst != astWidth)
        {
            *xAst -= 1;
            *yAst -= 1;
        }
        else if(*dirAst == 2 && *xAst != astWidth && *yAst != WORLD_H - astWidth)
        {
                *xAst -= 1;
                *yAst += 1;
        }
        else if(*dirAst == 3 && *xAst != WORLD_W - astWidth && *yAst != astWidth)
        {
                *xAst += 1;
                *yAst -= 1;
        }
        else if(*dirAst == 4 && *xAst != WORLD_W - astWidth && *yAst != WORLD_H - astWidth)
        {
                *xAst += 1;
                *yAst += 1;
        }
        else
        {
            *dirAst = rand()%4 + 1;

        }
}

void ColisaoNaveAst ()
{
    //if(dist > (shipWidth/2)+(astWidth/2))
   // {

    //}
}

enum MYKEYS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_A, KEY_S, KEY_D, KEY_L, KEY_E
};

int main(int argc, char **argv){
    srand(time(NULL));
    //---------------------------------------------
    //TESTE CAIO COMETAS
    //Cometa cometas[NUM_COMETA]
   // struct cometa
    //{
    //int ID;
    //int x;
    //int y;
    //bool live;
    //int speed;
    //int boundx;
    //int boundy;
    //};
    //---------------------------------------------

    ALLEGRO_DISPLAY *display = NULL;
    //ALLEGRO_DISPLAY_MODE   disp_data; //FULLSCREEN
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    //FAZER A CAMERA SEGUIR A NAVE ATÉ NAS BORDAS DO MAPA, PARA EVITAR O "LAG" DA NAVE.
    float cameraX = 0, cameraY = 0;
    bool key[9] = { false, false, false, false, false, false, false, false, false };
    //###PENSAR EM CRIAR OUTRO KEY (UM PARA MEGAMEN E OUTRO PARA SONIC)
    //###PARA CRIAR UMA FUNÇÃO DE MOVIMENTO, ONDE CADA KEY SERIA PASSADO
    bool redraw = true;

    //INSTALA AS FUNCIONALIDADES NECESSARIAS
    al_init();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(20); // MOVIMENTO ASTEROIDE
    //al_init_primitives_addon(); //FULLSCREEN
    al_install_keyboard();
    timer = al_create_timer(1.0 / FPS);

    //FULLSCREEN
    //al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    //display = al_create_display(disp_data.width, disp_data.height);
    //int SCREEN_W = al_get_display_width(display);
    //int SCREEN_H = al_get_display_height(display);
    int SCREEN_W = 1380; //APAGAR (FULLSCREEN)
    int SCREEN_H = 780; //APAGAR (FULLSCREEN)
    display = al_create_display(SCREEN_W, SCREEN_H); //APAGAR (FULLSCREEN)

    //NAVE
    ALLEGRO_BITMAP *ship;
    ship = al_load_bitmap("images/ship/shipWhite.png");
    float xShip = WORLD_W/2, yShip = WORLD_H/2;
    int shipWidth = al_get_bitmap_width(ship), shipHeight = al_get_bitmap_height(ship);
    int controle = 0;
    float forceX = 0, forceY = 0;
    ALLEGRO_BITMAP *propulsor;
    propulsor = al_load_bitmap("images/ship/propulsorWhite.png");
    int propWidth = al_get_bitmap_width(propulsor), propHeight = al_get_bitmap_height(propulsor);
    int rotation = 90;

    //COMPUTADOR
    ALLEGRO_BITMAP *fire[12];
    fire[0] = al_load_bitmap("images/ship/fire/f1.png");
    fire[1] = al_load_bitmap("images/ship/fire/f2.png");
    fire[2] = al_load_bitmap("images/ship/fire/f3.png");
    fire[3] = al_load_bitmap("images/ship/fire/f4.png");
    fire[4] = al_load_bitmap("images/ship/fire/f5.png");
    fire[5] = al_load_bitmap("images/ship/fire/f6.png");
    fire[6] = al_load_bitmap("images/ship/fire/f7.png");
    fire[7] = al_load_bitmap("images/ship/fire/f8.png");
    fire[8] = al_load_bitmap("images/ship/fire/f9.png");
    fire[9] = al_load_bitmap("images/ship/fire/f10.png");
    fire[10] = al_load_bitmap("images/ship/fire/f11.png");
    fire[11] = al_load_bitmap("images/ship/fire/f12.png");
    int curFire = 0;
    int fireWidth = al_get_bitmap_width(fire[0]);
    int fireHeight = al_get_bitmap_height(fire[0]);

    //PLAYER1
    ALLEGRO_BITMAP  *player1[20];
    player1[0] = al_load_bitmap("images/players/a00.png");
    player1[1] = al_load_bitmap("images/players/a01.png");
    player1[2] = al_load_bitmap("images/players/a02.png");
    player1[3] = al_load_bitmap("images/players/a03.png");
    player1[4] = al_load_bitmap("images/players/a04.png");
    player1[5] = al_load_bitmap("images/players/a05.png");
    player1[6] = al_load_bitmap("images/players/a06.png");
    player1[7] = al_load_bitmap("images/players/a07.png");
    player1[8] = al_load_bitmap("images/players/a08.png");
    player1[9] = al_load_bitmap("images/players/a09.png");
    player1[10] = al_load_bitmap("images/players/a10.png");
    player1[11] = al_load_bitmap("images/players/a11.png");
    player1[12] = al_load_bitmap("images/players/a12.png");
    player1[13] = al_load_bitmap("images/players/a13.png");
    player1[14] = al_load_bitmap("images/players/a14.png");
    player1[15] = al_load_bitmap("images/players/a15.png");
    player1[16] = al_load_bitmap("images/players/a16.png");
    player1[17] = al_load_bitmap("images/players/a17.png");
    player1[18] = al_load_bitmap("images/players/a18.png");
    player1[19] = al_load_bitmap("images/players/a19.png");
    int xplayer1 = shipWidth/2+30, yplayer1 = shipHeight/2+10, curplayer1 = 0;
    int player1Width = al_get_bitmap_width(player1[0]), player1Height = al_get_bitmap_height(player1[0]);
    int dir = 0;

    //PLAYER2
    ALLEGRO_BITMAP  *player2[20];
    player2[0] = al_load_bitmap("images/players/b00.png");
    player2[1] = al_load_bitmap("images/players/b01.png");
    player2[2] = al_load_bitmap("images/players/b02.png");
    player2[3] = al_load_bitmap("images/players/b03.png");
    player2[4] = al_load_bitmap("images/players/b04.png");
    player2[5] = al_load_bitmap("images/players/b05.png");
    player2[6] = al_load_bitmap("images/players/b06.png");
    player2[7] = al_load_bitmap("images/players/b07.png");
    player2[8] = al_load_bitmap("images/players/b08.png");
    player2[9] = al_load_bitmap("images/players/b09.png");
    player2[10] = al_load_bitmap("images/players/b10.png");
    player2[11] = al_load_bitmap("images/players/b11.png");
    player2[12] = al_load_bitmap("images/players/b12.png");
    player2[13] = al_load_bitmap("images/players/b13.png");
    player2[14] = al_load_bitmap("images/players/b14.png");
    player2[15] = al_load_bitmap("images/players/b15.png");
    player2[16] = al_load_bitmap("images/players/b16.png");
    player2[17] = al_load_bitmap("images/players/b17.png");
    player2[18] = al_load_bitmap("images/players/b18.png");
    player2[19] = al_load_bitmap("images/players/b19.png");
    int xplayer2 = shipWidth/2-50, yplayer2 = shipHeight/2+10, curplayer2 = 0;
    int player2Width = al_get_bitmap_width(player2[0]), player2Height = al_get_bitmap_height(player2[0]);
    int dir2 = 0;

    //COMPUTADOR NAVE
    //ALLEGRO_BITMAP *comp;
    //comp = al_load_bitmap("images/ship/comp.png");
    //float compScale = 0.3;
    //int compWidth = al_get_bitmap_width(comp), compHeight = al_get_bitmap_height(comp);

    //COMPUTADORES DE TIRO
    //ALLEGRO_BITMAP *compShot[4];
    //compShot[0] = al_load_bitmap("images/ship/compShot.png");
    //compShot[1] = al_load_bitmap("images/ship/compShot.png");
    //compShot[2] = al_load_bitmap("images/ship/compShot.png");
    //compShot[3] = al_load_bitmap("images/ship/compShot.png");
    //int compShotWidth = al_get_bitmap_width(compShot[0]);
    //int compShotHeight = al_get_bitmap_height(compShot[0]);

    //ASTEROIDE
    ALLEGRO_BITMAP *asteroid;
    asteroid = al_load_bitmap("images/asteroids/Asteroid.png");
    int xAst = (WORLD_W/2)-200, yAst = (WORLD_H/2)-200;
    int astWidth = al_get_bitmap_width(asteroid);
    int astHeight = al_get_bitmap_height(asteroid);
    float dist;
    int dirAst = 1;

    //PLANO DE FUNDO
    ALLEGRO_BITMAP *background;
    background = al_load_bitmap("images/background.jpg");
    int xBG = 0, yBG = 0;
    float BGScale = 1.7;
    int BGWidth = al_get_bitmap_width(background);
    int BGHeight = al_get_bitmap_height(background);

    //BARRA
    ALLEGRO_BITMAP *barra;
    barra=al_load_bitmap("images/barra/barra.png");
    float xbarra = cameraX;
    float ybarra = cameraY;
    int barraWidth = al_get_bitmap_width(barra);
    int barraHeight = al_get_bitmap_height(barra);

    //BARRA DE VIDA
    ALLEGRO_BITMAP *vida;
    vida=al_load_bitmap("images/barra/vida.png");
    float xvida = cameraX;
    float yvida = cameraY;
    int vidaWidth = al_get_bitmap_width(vida);
    int vidaHeight = al_get_bitmap_height(vida);
    float vidascale = 0;

    //BARRA DE OXIGENIO
    ALLEGRO_BITMAP *oxigenio;
    oxigenio=al_load_bitmap("images/barra/oxigenio.png");
    float xoxigenio = cameraX;
    float yoxigenio = cameraY;
    int oxigenioWidth = al_get_bitmap_width(oxigenio);
    int oxigenioHeight = al_get_bitmap_height(oxigenio);
    float oxigenioscale = 0;
    int oxi = 0;

    //GALAO
    ALLEGRO_BITMAP *galao;
    galao=al_load_bitmap("images/barra/galao.png");
    float xgalao = cameraX;
    float ygalao = cameraY;
    int galaoWidth = al_get_bitmap_width(galao);
    int galaoHeight = al_get_bitmap_height(galao);

    //GASOLINA
    ALLEGRO_BITMAP *gasolina;
    gasolina=al_load_bitmap("images/barra/gasolina.png");
    float xgasolina = cameraX;
    float ygasolina = cameraY;
    int gasolinaWidth = al_get_bitmap_width(gasolina);
    int gasolinaHeight = al_get_bitmap_height(gasolina);

    //SONS
    ALLEGRO_SAMPLE *theme;
    ALLEGRO_SAMPLE_INSTANCE *inst_theme;
    theme = al_load_sample("sounds/theme.ogg");
    inst_theme = al_create_sample_instance(theme);
    al_attach_sample_instance_to_mixer(inst_theme,al_get_default_mixer());
    al_set_sample_instance_playmode(inst_theme, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_theme,0.8);

    ALLEGRO_SAMPLE *tiro;
    ALLEGRO_SAMPLE_INSTANCE *inst_tiro;
    tiro = al_load_sample("sounds/tiro.wav");
    inst_tiro = al_create_sample_instance(tiro);
    al_attach_sample_instance_to_mixer(inst_tiro,al_get_default_mixer());
    al_set_sample_instance_gain(inst_tiro,0.9);

    ALLEGRO_SAMPLE *explosao1;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao1;
    explosao1 = al_load_sample("sounds/explosao1.wav");
    inst_explosao1 = al_create_sample_instance(explosao1);
    al_attach_sample_instance_to_mixer(inst_explosao1,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao1,1.0);

    ALLEGRO_SAMPLE *explosao2;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao2;
    explosao2 = al_load_sample("sounds/explosao2.wav");
    inst_explosao2 = al_create_sample_instance(explosao2);
    al_attach_sample_instance_to_mixer(inst_explosao2,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao2,1.0);

    ALLEGRO_SAMPLE *explosao3;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao3;
    explosao3 = al_load_sample("sounds/explosao3.wav");
    inst_explosao3 = al_create_sample_instance(explosao3);
    al_attach_sample_instance_to_mixer(inst_explosao3,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao3,1.0);

    ALLEGRO_SAMPLE *som_propulsor;
    ALLEGRO_SAMPLE_INSTANCE *inst_som_propulsor;
    som_propulsor = al_load_sample("sounds/propulsor.ogg");
    inst_som_propulsor = al_create_sample_instance(som_propulsor);
    al_attach_sample_instance_to_mixer(inst_som_propulsor,al_get_default_mixer());
    al_set_sample_instance_gain(inst_som_propulsor,1.0);

    ALLEGRO_SAMPLE *rotacao_a;
    ALLEGRO_SAMPLE_INSTANCE *inst_rotacao_a;
    rotacao_a = al_load_sample("sounds/rotacaoa.ogg");
    inst_rotacao_a = al_create_sample_instance(rotacao_a);
    al_attach_sample_instance_to_mixer(inst_rotacao_a,al_get_default_mixer());
    al_set_sample_instance_gain(inst_rotacao_a,1.4);

    ALLEGRO_SAMPLE *rotacao_h;
    ALLEGRO_SAMPLE_INSTANCE *inst_rotacao_h;
    rotacao_h = al_load_sample("sounds/rotacaoh.ogg");
    inst_rotacao_h = al_create_sample_instance(rotacao_h);
    al_attach_sample_instance_to_mixer(inst_rotacao_h,al_get_default_mixer());
    al_set_sample_instance_gain(inst_rotacao_h,0.8);

    ALLEGRO_SAMPLE *alerta;
    ALLEGRO_SAMPLE_INSTANCE *inst_alerta;
    alerta = al_load_sample("sounds/alerta.wav");
    inst_alerta = al_create_sample_instance(alerta);
    al_attach_sample_instance_to_mixer(inst_alerta,al_get_default_mixer());
    al_set_sample_instance_gain(inst_alerta,1.0);



    //CONFIGURA/INICIALIZA EVENTOS E LOCAL DE DESENHO
    al_set_target_bitmap(al_get_backbuffer(display));
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_flip_display();
    al_start_timer(timer);



    //INICIALIZAÇÃO DA TRILHA SONORA
    al_play_sample_instance(inst_theme);

    //FORÇA ASTEROIDE
    float forceAX = 0;
    float forceAY = 0;

    while(1){

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);



        //EVENTOS RELACIONADO A VIDA
        if(oxigenioscale==oxigenioWidth)
        {
            if(vidascale < vidaWidth)
            {
                vidascale+=0.1;
            }
        }

        //EVENTOS REAIS (TEMPO)
        if(ev.type == ALLEGRO_EVENT_TIMER) {

            //MOVIMENTOS NAVE
            if(controle == 1){
                if(key[KEY_UP]){

                    curFire+=1;
                    if(curFire == 12){
                        curFire = 0;
                    }

                    //IDEIA:
                    //https://www.efunda.com/math/fourier_series/display.cfm?name=triangle
                    //float angle = rotation*3.14159/180;
                    //float senof = 0;
                    //float cosef = 0;
                    //if(-3.14159/2 <= angle && angle <= 3  if(dist > (shipWidth/2)+(astWidth/2))

                    //    senof = 2*angle/3.14159;
                    //    cosef = 1-senof;
                    //}else{
                    //    senof = (2*(3.14159-angle))/3.14159;
                    //    cosef = 1-senof;
                    //}
                    //forceX -= senof*0.01;
                    //forceY += cosef*0.01;

                    al_play_sample_instance(inst_som_propulsor);

                    forceX -= sin(rotation*3.14159/180)*0.01;
                    forceY += cos(rotation*3.14159/180)*0.01;

                    if(forceY < -2){
                        forceY = -2;
                    }
                    if(forceY > 2){
                        forceY = 2;
                    }
                    if(forceX < -2){
                        forceX = -2;
                    }
                    if(forceX > 2){
                        forceX = 2;
                    }
                }else{
                    al_stop_sample_instance(inst_som_propulsor);
                }

                if(key[KEY_LEFT]){
                    rotation+=1;

                    al_play_sample_instance(inst_rotacao_h);
                }else{if(dist > (shipWidth/2)+(astWidth/2))
                    al_stop_sample_instance(inst_rotacao_h);
                }
                if(key[KEY_RIGHT]){
                    rotation-=1;
                    al_play_sample_instance(inst_rotacao_a);
                }else{
                    al_stop_sample_instance(inst_rotacao_a);
                }

            }else{
                //MOVIMENTOS PLAYER1
                int flag = 0;
                if(key[KEY_RIGHT] && xplayer1 < shipWidth - player1Width){
                    dir = 1;
                    xplayer1 += VELOCITY;
                    flag = 1;
                }
                if(key[KEY_LEFT] && xplayer1 > 0){
                    dir = 2;
                    xplayer1 -= VELOCITY;
                    flag = 1;
                }
                if(key[KEY_UP] && yplayer1 > 0){
                    dir = 3;if(dist > (shipWidth/2)+(astWidth/2))
                    yplayer1 -= VELOCITY;
                    flag = 1;
                }
                if(key[KEY_DOWN] && yplayer1 < shipHeight - player1Height){
                    dir = 4;
                    yplayer1 += VELOCITY;
                    flag = 1;
                }

                if(dir == 1){
                    if(curplayer1 < 0 || curplayer1 > (7*DELAY)-2){
                        curplayer1 = 0;
                    }else if(flag){
                        curplayer1++;
                    }else{
                        curplayer1 = 0;
                    }
                }else if(dir == 2){
                    if(curplayer1 < 7*DELAY || curplayer1 > (14*DELAY)-2){
                        curplayer1 = 7*DELAY;
                    }else if(flag){
                        curplayer1++;
                    }else{
                        curplayer1 = 7*DELAY;
                    }
                }else if(dir == 3){
                    if(curplayer1 < 14*DELAY || curplayer1 > (17*DELAY)-2){
                        curplayer1 = 14*DELAY;
                    }else if(flag){
                        curplayer1++;
                    }else{
                        curplayer1 = 15*DELAY;
                    }
                }else if(dir == 4){
                    if(curplayer1 < 17*DELAY || curplayer1 > (20*DELAY)-2){
                        curplayer1 = 17*DELAY;
                    }else if(flag){
                        curplayer1++;
                    }else{
                        curplayer1 = 18*DELAY;
                    }
                }
            }

            //FAZ COM QUE A NAVE FREIE AOS POUCOS (PENSAR EM TIRAR)
            if(forceY < 0){
                    forceY += 0.002;
                    if(forceY>0){if(dist > (shipWidth/2)+(astWidth/2))
                        forceY=0;
                    }
            }else if(forceY > 0){
                    forceY -= 0.002;
                    if(forceY < 0){
                        forceY = 0;
                    }
            }
            if(forceX < 0){
                    forceX += 0.002;
                    if(forceX>0){
                        forceX=0;
                    }
            }else if(forceX > 0){
                    forceX -= 0.002;
                    if(forceX < 0){
                        forceX = 0;
                    }
            }

            //LIMITA O VALOR DA ROTAÇÃO NO INTERVALO [0,359]
            if(rotation >= 360){
                rotation = 0;
            }else if(rotation < 0){
                rotation = 359;
            }

             // MOVIMENTO ASTEROIDE
            MovimentoAsteroide(dist, shipWidth, astWidth, WORLD_W, WORLD_H, &dirAst, &xAst, &yAst);

            dist = sqrt((pow((xShip+shipWidth/2)-(xAst+astWidth/2), 2))+(pow((yShip+shipWidth/2)-(yAst+astHeight/2), 2)));
            if(dist <= (shipWidth/2)+(astWidth/2))
            {
                float vx = xShip - xAst;
                float vy = yShip - yAst;

                float lenSq = (vx*vx + vy*vy);
                float len = sqrt(lenSq);

                vx /= len;
                vy /= len;

                forceAX += -vx*0.1;
                forceAY += -vy*0.1;
            }
            if(forceAY < -1.5){
                forceAY = -1.5;
            }
            if(forceAY > 1.5){
                forceAY = 1.5;
            }
            if(forceAX < -1.5){
                forceAX = -1.5;
            }
            if(forceAX > 1.5){
                forceAX = 1.5;
            }

            xAst += forceAX;
            yAst += forceAY;

            //APLICA AS FORÇAS NA NAVE
            yShip += forceY;
            xShip += forceX;

            //IMPEDE QUE A NAVE SAIA DOS LIMITES DO MUNDO
            if(yShip < 0){
                yShip = 0;
                forceY=0;
            }
            if(yShip > WORLD_H - shipHeight){
                yShip = WORLD_H - shipHeight;
                forceY=0;
            }
            if(xShip < 0){
                xShip = 0;
                forceX=0;
            }
            if(xShip > WORLD_W - shipWidth){
                xShip = WORLD_W - shipWidth;
                forceX=0;
            }

            //EVENTOS RELACIONADO AO OXIGENIO
            if (oxigenioscale < oxigenioWidth)
            {
                oxigenioscale += 0.2;
            }
            else
            {
                oxigenioscale=oxigenioWidth;
            }

            if(oxi == 1)
            {
                if (oxigenioscale <= oxigenioWidth && oxigenioscale>0)
                {
                    oxigenioscale -= 1;
                }
                else
                {
                    oxigenioscale = 0;
                }
            }else{
                //MOVIMENTOS PLAYER2
                int flag = 0;
                if(key[KEY_D] && xplayer2 < shipWidth - player2Width){
                    dir2 = 1;
                    xplayer2 += VELOCITY;
                    flag = 1;
                }
                if(key[KEY_A] && xplayer2 > 0){
                    dir2 = 2;
                    xplayer2 -= VELOCITY;
                    flag = 1;
                }
                if(key[KEY_W] && yplayer2 > 0){
                    dir2 = 3;
                    yplayer2 -= VELOCITY;
                    flag = 1;
                }
                if(key[KEY_S] && yplayer2 < shipHeight - player2Height){
                    dir2 = 4;
                    yplayer2 += VELOCITY;
                    flag = 1;
                }

                if(dir2 == 1){
                    if(curplayer2 < 0 || curplayer2 > (7*DELAY)-2){
                        curplayer2 = 0;
                    }else if(flag){
                        curplayer2++;
                    }else{
                        curplayer2 = 0;
                    }
                }else if(dir2 == 2){
                    if(curplayer2 < 7*DELAY || curplayer2 > (14*DELAY)-2){
                        curplayer2 = 7*DELAY;
                    }else if(flag){
                        curplayer2++;
                    }else{
                        curplayer2 = 7*DELAY;
                    }
                }else if(dir2 == 3){
                    if(curplayer2 < 14*DELAY || curplayer2 > (17*DELAY)-2){
                        curplayer2 = 14*DELAY;
                    }else if(flag){
                        curplayer2++;
                    }else{
                        curplayer2 = 15*DELAY;
                    }
                }else if(dir2 == 4){
                    if(curplayer2 < 17*DELAY || curplayer2 > (20*DELAY)-2){
                        curplayer2 = 17*DELAY;
                    }else if(flag){
                        curplayer2++;
                    }else{
                        curplayer2 = 18*DELAY;
                    }
                }
            }

            //ATUALIZA A CÂMERA AO OBJETO EM MOVIMENTO
            cameraX = xShip - SCREEN_W/2 +shipWidth/2;
            cameraY = yShip - SCREEN_H/2 +shipHeight/2;

            //CALCULA DISTÂNCIA ENTRE O ASTEROID E A NAVE
          //  dist = sqrt((pow((xShip+shipWidth/2)-(xAst+astWidth/2), 2))+(pow((yShip+shipWidth/2)-(yAst+astHeight/2), 2)));

            //LIMITA A CAMERA NAS BORDAS DO MAPA (PENSAR UM LIMITE DE ASTEROIDS E NÃO DA CÂMERA)
            if(cameraX < 0){
                cameraX = 0;
            }
            if (cameraY < 0){
                cameraY = 0;
            }
            if (cameraX > WORLD_W - SCREEN_W){
                cameraX = WORLD_W - SCREEN_W;
            }
            if (cameraY > WORLD_H - SCREEN_H){
                cameraY = WORLD_H - SCREEN_H;
            }

            redraw = true;

        //ATIVA AS TECLAS PRESSIONADAS
        }else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch(ev.keyboard.keycode) {
                //PLAYER1
                case ALLEGRO_KEY_UP:if(dist > (shipWidth/2)+(astWidth/2))
                    key[KEY_UP] = true;
                    break;
                case ALLEGRO_KEY_DOWN:
                    key[KEY_DOWN] = true;
                    break;
                case ALLEGRO_KEY_LEFT:
                    key[KEY_LEFT] = true;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    key[KEY_RIGHT] = true;
            break;
                case ALLEGRO_KEY_L:
                    if(controle == 1){
                        controle = 0;
                    }else{
                        controle = 1;
                    }
                    break;
                //PLAYER2
                case ALLEGRO_KEY_W:
                    key[KEY_W] = true;
                    break;
                case ALLEGRO_KEY_S:
                    key[KEY_S] = true;
                    break;
                case ALLEGRO_KEY_A:
                    key[KEY_A] = true;
                    break;
                case ALLEGRO_KEY_D:
                    key[KEY_D] = true;
                    break;
                case ALLEGRO_KEY_E:
                    if(oxi == 1){
                        oxi = 0;
                    }else{
                        oxi = 1;
                    }
                    break;
            }

        //DESATIVA AS TECLAS PRESSIONADAS
        }else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch(ev.keyboard.keycode) {
                //PLAYER1
                case ALLEGRO_KEY_UP:
                    key[KEY_UP] = false;
                    break;
                case ALLEGRO_KEY_DOWN:
                    key[KEY_DOWN] = false;
                    break;
                case ALLEGRO_KEY_LEFT:
                    key[KEY_LEFT] = false;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    key[KEY_RIGHT] = false;
                    break;
                //PLAYER2
                case ALLEGRO_KEY_W:
                    key[KEY_W] = false;
                    break;
                case ALLEGRO_KEY_S:
                    key[KEY_S] = false;
                    break;
                case ALLEGRO_KEY_A:
                    key[KEY_A] = false;
                    break;
                case ALLEGRO_KEY_D:
                    key[KEY_D] = false;
                    break;

            }
        }else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        //DESENHOS
        if(redraw && al_is_event_queue_empty(event_queue)) {

            redraw = false;

            al_draw_scaled_bitmap(background,0,0,BGWidth,BGHeight,xBG-(cameraX/2),yBG-(cameraY/2),BGWidth*BGScale, BGHeight*BGScale, 0);
            if(key[KEY_UP] && controle){
                al_draw_rotated_bitmap(fire[curFire],25, -200,xShip-cameraX+shipWidth/2, yShip-cameraY+shipHeight/2,((rotation+180)*3.14159/180),0);
            }

            al_draw_rotated_bitmap(propulsor,propHeight/2, propHeight/2,xShip-cameraX+shipWidth/2, yShip-cameraY+shipHeight/2,((rotation-90)*3.14159/180),0);
            al_draw_scaled_bitmap(ship,0,0,shipWidth,shipHeight,xShip-cameraX,yShip-cameraY,shipWidth, shipHeight, 0);
            //al_draw_scaled_bitmap(comp,0,0,compWidth,compHeight,xShip-cameraX+shipWidth/2-10,yShip-cameraY+shipHeight/2-15,compWidth*compScale, compHeight*compScale, 0);
            //al_draw_scaled_bitmap(compShot[0],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2-140,yShip-cameraY+shipHeight/2-15,compShotWidth*compScale, compShotHeight*compScale, 0);
            //al_draw_scaled_bitmap(compShot[1],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2+110,yShip-cameraY+shipHeight/2-20,compShotWidth*compScale, compShotHeight*compScale, 0);
            //al_draw_scaled_bitmap(compShot[2],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2+15,yShip-cameraY+shipHeight/2-150,compShotWidth*compScale, compShotHeight*compScale, 0);
            //al_draw_scaled_bitmap(compShot[3],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2-5,yShip-cameraY+shipHeight/2+100,compShotWidth*compScale, compShotHeight*compScale, 0);
            al_draw_scaled_bitmap(player2[curplayer2/DELAY],0,0,player2Width,player2Height,xplayer2+xShip-cameraX,yplayer2+yShip-cameraY,player2Width, player2Height, 0);
            al_draw_scaled_bitmap(player1[curplayer1/DELAY],0,0,player1Width,player1Height,xplayer1+xShip-cameraX,yplayer1+yShip-cameraY,player1Width, player1Height, 0);
            al_draw_scaled_bitmap(vida,0,0,vidaWidth,vidaHeight,(SCREEN_W/2)-(vidaWidth/2)-15,SCREEN_H-43,vidaWidth-vidascale,vidaHeight, 0);
            al_draw_scaled_bitmap(oxigenio,0,0,oxigenioWidth,oxigenioHeight,SCREEN_W/2-oxigenioWidth/2-15,SCREEN_H-36,oxigenioWidth-oxigenioscale,oxigenioHeight, 0);
            al_draw_scaled_bitmap(barra,0,0,barraWidth,barraHeight,SCREEN_W/2-barraWidth/2,SCREEN_H-100,barraWidth,barraHeight, 0);
            //al_draw_scaled_bitmap(gasolina,0,0,gasolinaWidth,gasolinaHeight,xgasolina+400,ygasolina+650,gasolinaWidth,gasolinaHeight, 0);
            //al_draw_scaled_bitmap(galao,0,0,galaoWidth,galaoHeight,xgalao+400,ygalao+650,galaoWidth,galaoHeight, 0);
            al_draw_scaled_bitmap(asteroid,0,0,astWidth,astHeight,xAst-cameraX,yAst-cameraY,astWidth, astHeight, 0);

            //FORMULA COLISÃO NAVE - ASTEROIDE
            if(dist > (shipWidth/2)+(astWidth/2))
            {

            }

            //AO DESENHAR QUALQUER COISA (FORA O OBJETO FOCO DA CAMERA) COLOCAR AS POSIÇÕES DE DESENHO "X_OBJETO - CAMERAX" E "Y_OBJETO - CAMERAY"
            al_flip_display();
        }

    }

    //DESTROI TUDO DA MEMORIA
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_bitmap(background);
    al_destroy_bitmap(propulsor);
    al_destroy_bitmap(ship);
    //al_destroy_bitmap(comp);
    al_destroy_bitmap(asteroid);
    al_destroy_bitmap(barra);
    al_destroy_bitmap(vida);
    al_destroy_bitmap(oxigenio);
    al_destroy_bitmap(galao);
    al_destroy_bitmap(gasolina);
    al_destroy_sample(theme);
    al_destroy_sample(tiro);
    al_destroy_sample(explosao1);
    al_destroy_sample(explosao2);
    al_destroy_sample(explosao3);
    al_destroy_sample(som_propulsor);
    al_destroy_sample(rotacao_a);
    al_destroy_sample(rotacao_h);
    al_destroy_sample(alerta);
    al_destroy_sample_instance(inst_theme);
    al_destroy_sample_instance(inst_tiro);
    al_destroy_sample_instance(inst_explosao1);
    al_destroy_sample_instance(inst_explosao2);
    al_destroy_sample_instance(inst_explosao3);
    al_destroy_sample_instance(inst_som_propulsor);
    al_destroy_sample_instance(inst_rotacao_a);
    al_destroy_sample_instance(inst_rotacao_h);
    al_destroy_sample_instance(inst_alerta);

    for(int i=0; i<12; i++){
        al_destroy_bitmap(fire[i]);
    }
    //for(int i=0; i<4; i++){
    //    al_destroy_bitmap(compShot[i]);
    //}
    for(int i=0; i<20; i++){
        al_destroy_bitmap(player1[i]);
    }
    for(int i=0; i<20; i++){
        al_destroy_bitmap(player2[i]);
    }
    al_destroy_event_queue(event_queue);

    return 0;
}
