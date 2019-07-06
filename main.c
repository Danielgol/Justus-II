#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <math.h>



const float FPS = 60;
const int WORLD_W = 3000; // 13250 - 11000 - 3000
const int WORLD_H = 1500; // 7500 - 6250 - 1500
const int VELOCITY = 2;
const int DELAY = 6;

enum MYKEYS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_S, KEY_A, KEY_D, KEY_L, KEY_E
};

typedef struct{
    int width,height;
    ALLEGRO_BITMAP *img;
}SIMP_OBJECT;

typedef struct{
    int cur_Frame;
    int width,height;
    ALLEGRO_BITMAP **imgs;
}DYNF_OBJECT;

typedef struct{
    float x,y;
    int dir;
    int width,height;
    int cur_Frame;
    ALLEGRO_BITMAP **imgs;
}PLAYER;

typedef struct{
    int vida;
    float x,y;
    float forceX, forceY;
    int width,height;
    ALLEGRO_BITMAP *img;
}ASTEROID;

typedef struct{
    float x, y;
    int rotation;
    int controle;
    int repondoOxi;
    float forceX, forceY;
    int width, height;
    ALLEGRO_BITMAP *img;
}SHIP;

typedef struct{
    float x,y;
    int carga;
    int controle;
    int angle;
}GUN;

typedef struct{
    int ativo;
    float x,y;
    float velX,velY;
    int width, height;
    ALLEGRO_BITMAP *img;
}SHOT;



ALLEGRO_BITMAP** load_dynamic_bitmaps(int quant,char path[], char img[]){
    //VERIFICAR SE ESSE CÓDIGO ALOCA 2 ESPAÇOS, SE SIM, DEVOLVER ESSE CÓDIGO PARA ANTES DA CHAMADA DESSE MÉTODO.
    ALLEGRO_BITMAP** imagens = (ALLEGRO_BITMAP**) malloc(sizeof(ALLEGRO_BITMAP*)*quant);
    for(int i=0; i<quant; i++){
        char *imgi = (char*) malloc(strlen(img)+1);
        imgi[0] = '\0';
        strcat(imgi, img);
        for(int x=0; x<strlen(imgi); x++){
            if(imgi[x]=='#'){
                imgi[x] = (i/10)+'0';
                imgi[x+1] = i-((i/10)*10)+'0';
                break;
            }
        }
        char *result = (char*) malloc(strlen(path) + strlen(imgi) + 2);
        result[0] = '\0';
        strcat(result, path);
        strcat(result, imgi);
        imagens[i] = al_load_bitmap(result);
        free(result);
        free(imgi);
    }
    return imagens;
}

SIMP_OBJECT build_Simple_Object(char path[]){
    SIMP_OBJECT object;
    object.img = al_load_bitmap(path);
    object.width = al_get_bitmap_width(object.img);
    object.height = al_get_bitmap_height(object.img);
    return object;
}

DYNF_OBJECT build_Dynamic_Object(int quant, char path[], char img[], int cur_Frame){
    DYNF_OBJECT object;
    object.cur_Frame = cur_Frame;
    object.imgs = load_dynamic_bitmaps(quant, path, img);
    object.width = al_get_bitmap_width(object.imgs[0]);
    object.height = al_get_bitmap_height(object.imgs[0]);
    return object;
}

PLAYER build_player(float x, float y, int quant, char path[], char img[], int cur_Frame, int direction){
    PLAYER player;
    player.x = x;
    player.y = y;
    player.dir = direction;
    player.cur_Frame = cur_Frame;
    player.imgs = load_dynamic_bitmaps(quant, path, img);
    player.width = al_get_bitmap_width(player.imgs[0]);
    player.height = al_get_bitmap_height(player.imgs[0]);
    return player;
}

ASTEROID build_asteroid(float x, float y, float forceX, float forceY, int vida, char path[]){
    ASTEROID asteroid;
    asteroid.x = x;
    asteroid.y = y;
    asteroid.vida = vida;
    asteroid.forceX = forceX;
    asteroid.forceY = forceY;
    asteroid.img = al_load_bitmap(path);
    asteroid.width = al_get_bitmap_width(asteroid.img);
    asteroid.height = al_get_bitmap_height(asteroid.img);
    return asteroid;
}

SHIP build_ship(float x, float y, float forceX, float forceY, int rotation, char path[]){
    SHIP ship;
    ship.x = x;
    ship.y = y;
    ship.forceX = forceX;
    ship.forceY = forceY;
    ship.rotation = rotation;
    ship.controle = 0;
    ship.repondoOxi = 0;
    ship.img = al_load_bitmap(path);
    ship.width = al_get_bitmap_width(ship.img);
    ship.height = al_get_bitmap_height(ship.img);
    return ship;
}

GUN build_gun(float x, float y, int controle, int angle){
    GUN gun;
    gun.x = x;
    gun.y = y;
    gun.carga = 20;
    gun.controle = controle;
    gun.angle = angle;
    return gun;
}



void adaptarCamera(float *cameraX, float *cameraY, float x, float y, int width, int height, int SCREEN_W, int SCREEN_H){

    //ATUALIZA A CÂMERA AO OBJETO EM MOVIMENTO
    *cameraX = x - SCREEN_W/2 +width/2;
    *cameraY = y - SCREEN_H/2 +height/2;

    //LIMITA A CAMERA NAS BORDAS DO MAPA (PENSAR UM LIMITE DE ASTEROIDS E NÃO DA CÂMERA)
    if(*cameraX < 0){
        *cameraX = 0;
    }
    if(*cameraY < 0){
        *cameraY = 0;
    }
    if(*cameraX > WORLD_W - SCREEN_W){
        *cameraX = WORLD_W - SCREEN_W;
    }
    if(*cameraY > WORLD_H - SCREEN_H){
        *cameraY = WORLD_H - SCREEN_H;
    }
}

void atualizarFramePlayer(int dir, int flag, int *cur_Frame){
    if(dir == 1){
        if(*cur_Frame < 0 || *cur_Frame > (7*DELAY)-2){
            *cur_Frame = 0;
        }else if(flag){
            *cur_Frame += 1;
        }else{
            *cur_Frame = 0;
        }
    }else if(dir == 2){
        if(*cur_Frame < 7*DELAY || *cur_Frame > (14*DELAY)-2){
            *cur_Frame = 7*DELAY;
        }else if(flag){
            *cur_Frame += 1;
        }else{
            *cur_Frame = 7*DELAY;
        }
    }else if(dir == 3){
        if(*cur_Frame < 14*DELAY || *cur_Frame > (18*DELAY)-2){
            *cur_Frame = 14*DELAY;
        }else if(flag){
            *cur_Frame += 1;
        }else{
            *cur_Frame = 15*DELAY;
        }
    }else if(dir == 4){
        if(*cur_Frame < 18*DELAY || *cur_Frame > (22*DELAY)-2){
            *cur_Frame = 18*DELAY;
        }else if(flag){
            *cur_Frame += 1;
        }else{
            *cur_Frame = 19*DELAY;
        }
    }
}

void moverPlayer(float *x, float *y, int *dir, int *flag, bool UP, bool LEFT, bool RIGHT, bool DOWN){

    if(RIGHT){
        *dir = 1;
        if(*x + VELOCITY <=232 && *y>=40 && *y <=217 || *y >= 110 && *y <= 155 || *x + VELOCITY >=128 && *x + VELOCITY <=165 ){
            if(*x + VELOCITY <= 270){
                *x += VELOCITY;
            }
        }
        *flag = 1;
    }
    if(LEFT){
        *dir = 2;
        if(*x - VELOCITY >=65 && *y>=40 && *y <=217 || *y >= 110 && *y <= 155 || *x - VELOCITY >=128 && *x - VELOCITY <=165 ){
            if(*x - VELOCITY >= 23){
                *x -= VELOCITY;
            }
        }
        *flag = 1;
    }
    if(UP){
        *dir = 3;
        if(*y - VELOCITY >= 40 && *x>=65 && *x<=232 || *x >= 128 && *x <= 165 || *y - VELOCITY >= 110 && *y - VELOCITY <= 155 ){
            if(*y - VELOCITY >=7){
                *y -= VELOCITY;
            }
        }
        *flag = 1;
    }
    if(DOWN){
        *dir = 4;
        if(*y + VELOCITY <= 217 && *x>=65 && *x<=232 || *x >= 128 && *x <= 165 || *y + VELOCITY >= 110 && *y + VELOCITY <= 155 ){
            if(*y + VELOCITY <= 265){
                *y += VELOCITY;
            }
        }
        *flag = 1;
    }

    //int flag = 0;
    //            if(key[KEY_RIGHT] && player1.x < ship.width - player1.width){
    //                player1.dir = 1;
    //                if(player1.x + VELOCITY <=255 && player1.y>=76 && player1.y <=244 || player1.y >= 140 && player1.y <= 182 || player1.x + VELOCITY >=155 && player1.x + VELOCITY <=195 ){
    //                  if((player1.x + VELOCITY < 141)||(player1.x + VELOCITY > 141 && player1.x + VELOCITY < 195 && player1.y < 182 && player1.y > 122)||(player1.y > 182)||(player1.y <= 122 && player1.y>90) || (player1.y <=90 && player1.x <= 200) || (player1.x + VELOCITY > 195 && player1.x + VELOCITY < 290 && player1.y >= 90))
    //                  {
    //                    if((player1.x + VELOCITY > 195 && player1.y>120 && player1.y < 142 && player1.x + VELOCITY <235)||(player1.x<=195)||(player1.x>195 && player1.y<120)||(player1.x>195 && player1.y>142)|| (player1.x>=255))
    //                    {
    //                       player1.x += VELOCITY;
    //                    }
    //                  }
    //                }
    //                flag = 1;
    //            }
    //            if(key[KEY_LEFT] && player1.x > 0){
    //                player1.dir = 2;
    //                if(player1.x - VELOCITY >=99 && player1.y>=76 && player1.y <=244 || player1.y >= 140 && player1.y <= 182 || player1.x - VELOCITY >=155 && player1.x - VELOCITY <=195 ){
    //                    if((player1.x - VELOCITY > 55 && player1.x - VELOCITY < 159 && player1.y < 230)||(player1.x-VELOCITY>159 && player1.x-VELOCITY<205 && player1.y <182 && player1.y > 122)||(player1.y > 182 && player1.x > 130)||(player1.y <= 122) || (player1.x - VELOCITY > 209))
    //                    {
    //                        if((player1.x - VELOCITY < 135 && player1.y>120 && player1.y<142 && player1.x - VELOCITY > 119)||(player1.x >=135)||(player1.x <135 && player1.y<120)||(player1.x<135 && player1.y > 142)||(player1.x<=99))
    //                        {
    //                            player1.x -= VELOCITY;
    //                        }
    //                    }
    //                }
    //                flag = 1;
    //            }
    //            if(key[KEY_UP] && player1.y > 0){
    //                player1.dir = 3;
    //                if(player1.y - VELOCITY >= 76 && player1.x>=99 && player1.x<=255 || player1.x >= 155 && player1.x <= 195 || player1.y - VELOCITY >= 140 && player1.y - VELOCITY <= 182 ){
    //                    if((player1.y - VELOCITY> 30 && player1.y - VELOCITY <= 76)||(player1.x >= 135 && player1.x <= 205 && player1.y - VELOCITY > 184)|| (player1.x > 205 && player1.y > 90 )||(player1.y<=90 && player1.y>=76 && player1.x<205)||(player1.x <= 140)||(player1.y-VELOCITY <= 155 && player1.y-VELOCITY>=76 && player1.x >=135 && player1.x<=205))
    //                    {
    //                        if((player1.x>50 && player1.y - VELOCITY>142)||(player1.y - VELOCITY <=142 && player1.x>110 && player1.x<242)||(player1.y-VELOCITY<=120))
    //                        {
    //                            player1.y -= VELOCITY;
    //                        }
    //                    }
    //                }
    //                flag = 1;
    //            }
    //            if(key[KEY_DOWN] && player1.y < ship.height - player1.height){
    //                player1.dir = 4;
    //                if(player1.y + VELOCITY <= 244 && player1.x>=99 && player1.x<=255 || player1.x >= 155 && player1.x <= 195 || player1.y + VELOCITY >= 140 && player1.y + VELOCITY <= 182 ){
    //                    if((player1.y + VELOCITY>= 244 && player1.y + VELOCITY < 295)||(player1.x >= 135 && player1.x <= 205 && player1.y + VELOCITY < 150)|| (player1.x > 205 )||(player1.x <= 140 && player1.y + VELOCITY < 230)||(player1.y + VELOCITY>= 230 && player1.x>115 && player1.x <=135)||(player1.x >= 135 && player1.x<=205 && player1.y + VELOCITY < 295 && player1.y>180))
    //                    {
    //                        if((player1.x>50 && player1.y + VELOCITY< 120)||(player1.y + VELOCITY >= 120 && player1.x>110 && player1.x<140 )||(player1.y + VELOCITY >= 120 && player1.x>159 && player1.x<195)||(player1.y + VELOCITY >= 120 && player1.x>209 && player1.x<242)||(player1.y+VELOCITY >= 142))
    //                            {
    //                            player1.y += VELOCITY;
    //                            }
    //                    }
    //                }
    //                flag = 1;
    //            }
}

void limitarAnguloNave(int *rotation){
    //LIMITA O VALOR DA ROTAÇÃO NO INTERVALO [0,359]
    if(*rotation >= 360){
        *rotation = 0;
    }else if(*rotation < 0){
        *rotation = 359;
    }
}

void limitarNaveMundo(float *x, float *y, float *forceX, float *forceY, int width, int height){

    //IMPEDE QUE A NAVE SAIA DOS LIMITES DO MUNDO (PENSAR EM TIRAR)

    if(*y < 0){
        *y = 0;
        *forceY = 0;
    }
    if(*y > WORLD_H - height){
        *y = WORLD_H - height;
        *forceY = 0;
    }
    if(*x < 0){
        *x = 0;
        *forceX = 0;
    }
    if(*x > WORLD_W - width){
        *x = WORLD_W - width;
        *forceX = 0;
    }
}

void limitarAsteroideMundo(float *x, float *y, float *forceX, float *forceY, int width, int height){

    //REPELE O ASTEROIDE NAS BORDAS (PENSAR EM TIRAR)
    if(*x <= 0){
        *forceX = -*forceX;
    }
    if(*x + width >= WORLD_W){
        *forceX = -*forceX;
    }
    if(*y <= 0){
        *forceY = -*forceY;
    }
    if(*y + height >= WORLD_H){
        *forceY = -*forceY;
    }

    //if(asteroid->x <= 0){
    //    asteroid->forceX = -asteroid->forceX;
    //}
    //if(asteroid->x + asteroid->width >= WORLD_W){
    //    asteroid->forceX = -asteroid->forceX;
    //}
    //if(asteroid->y <= 0){
    //    asteroid->forceY = -asteroid->forceY;
    //}
    //if(asteroid->y + asteroid->height >= WORLD_H){
    //    asteroid->forceY = -asteroid->forceY;
    //}
}

void generateAsteroids(int quant, ASTEROID *asteroids, int vida, char path[]){

    float Fx = (rand()%15)/10+0.5;
    float Fy = (rand()%15)/10+0.5;

    (asteroids[0]) = build_asteroid(0, 0, Fx, Fy, vida, path);

    int xA = rand()%(WORLD_W-(asteroids->width));
    int yA = rand()%(WORLD_H-(asteroids->height));

    (asteroids[0]).x = xA;
    (asteroids[0]).y = yA;

    for(int i=1; i<quant; i++){
        Fx = (rand()%15)/10+0.5;
        Fy = (rand()%15)/10+0.5;
        xA = rand()%(WORLD_W-(asteroids->width));
        yA = rand()%(WORLD_H-(asteroids->width));
        (asteroids[i]) = build_asteroid(xA, yA, Fx, Fy, vida, path);
    }
}

void aplicarForcas(float *x, float *y, float forceX, float forceY){
    *x += forceX;
    *y += forceY;
}

void limitarForcas(float *forceX, float *forceY, float limite){

    if(*forceY < -limite){
        *forceY = -limite;
    }
    if(*forceY > limite){
        *forceY = limite;
    }
    if(*forceX < -limite){
        *forceX = -limite;
    }
    if(*forceX > limite){
        *forceX = limite;
    }
}

void frearObjetos(float *forceX, float *forceY, float freio){
    if(*forceY < 0){
        *forceY += freio;
        if(*forceY>0){
            *forceY=0;
        }
    }else if(*forceY > 0){
        *forceY -= freio;
        if(*forceY < 0){
            *forceY = 0;
        }
    }
    if(*forceX < 0){
        *forceX += freio;
        if(*forceX>0){
            *forceX=0;
        }
    }else if(*forceX > 0){
        *forceX -= freio;
        if(*forceX < 0){
            *forceX = 0;
        }
    }
}

void impulsionarNave(float *forceX, float *forceY, float rotation){
    //IDEIA:
    //https://www.efunda.com/math/fourier_series/display.cfm?name=triangle
    //float angle = rotation*3.14159/180;
    //float senof = 0;
    //float cosef = 0;
    //if(-3.14159/2 <= angle && angle <= 3

    //    senof = 2*angle/3.14159;
    //    cosef = 1-senof;
    //}else{
    //    senof = (2*(3.14159-angle))/3.14159;
    //    cosef = 1-senof;
    //}
    //forceX -= senof*0.01;
    //forceY += cosef*0.01;
    *forceX -= sin(rotation*3.14159/180)*0.01;
    *forceY += cos(rotation*3.14159/180)*0.01;
}

void atirar(SHOT *shots, bool UP, int *carga, float x, float y, int angle, int QUANT_SHOTS){
    if(UP && *carga == 20){
        for(int i=0; i<QUANT_SHOTS; i++){
            if((shots+i)->ativo == 0 ){
                (shots+i)->ativo = 1;
                (shots+i)->x = x;
                (shots+i)->y = y;
                (shots+i)->velX = -sin(angle*3.14159/180)*38;
                (shots+i)->velY = cos(angle*3.14159/180)*38;
                break;
            }
        }
        *carga = 0;
    }
}

void controlarCanhao(bool LEFT, bool RIGHT, int *angle, int limite){
    if(LEFT){
        if(*angle - 2 > limite){
            *angle -= 2;
        }
    }
    if(RIGHT){
        if(*angle + 2 < limite+180){
            *angle += 2;
        }
    }
}

void controlarNave(bool BOOST, bool LEFT, bool RIGHT, float *forceX, float *forceY, int *rotation, int *fireFrame, ALLEGRO_SAMPLE_INSTANCE* inst_som_propulsor, ALLEGRO_SAMPLE_INSTANCE* inst_rotacao_h, ALLEGRO_SAMPLE_INSTANCE* inst_rotacao_a){
    if(BOOST){
        *fireFrame+=1;
        if(*fireFrame == 12){
            *fireFrame = 0;
        }
        impulsionarNave(&*forceX, &*forceY, *rotation);
        al_play_sample_instance(inst_som_propulsor);
    }else{
        al_stop_sample_instance(inst_som_propulsor);
    }
    if(LEFT){
        *rotation+=1;
        al_play_sample_instance(inst_rotacao_h);
    }else{
        al_stop_sample_instance(inst_rotacao_h);
    }
    if(RIGHT){
        *rotation-=1;
        al_play_sample_instance(inst_rotacao_a);
    }else{
        al_stop_sample_instance(inst_rotacao_a);
    }
}

int verificar_Colisao_SHIP_ASTEROID(SHIP *ship, ASTEROID *asteroid){
    float dist = sqrt((pow((ship->x + ship->width/2)-(asteroid->x + asteroid->width/2), 2)) + (pow((ship->y + ship->height/2)-(asteroid->y + asteroid->height/2), 2)));
    if(dist <= (ship->width/2)+(asteroid->width/2)){
        return 1;
    }
    return 0;
}

int verificar_Colisao_SHOT_ASTEROID(SHOT *shot, ASTEROID *asteroid){
    float dist = sqrt((pow((shot->x + shot->width/2)-(asteroid->x + asteroid->width/2), 2)) + (pow((shot->y + shot->height/2)-(asteroid->y + asteroid->height/2), 2)));
    if(dist <= (shot->width/2)+(asteroid->width/2)){
        return 1;
    }
    return 0;
}

int verificar_Colisao_ASTEROIDS(ASTEROID *asteroid_1, ASTEROID *asteroid_2){
    float dist = sqrt((pow((asteroid_1->x + asteroid_1->width/2)-(asteroid_2->x + asteroid_2->width/2), 2)) + (pow((asteroid_1->y + asteroid_1->height/2)-(asteroid_2->y + asteroid_2->height/2), 2)));
    if(dist <= (asteroid_1->width/2)+(asteroid_2->width/2)){
        return 1;
    }
    return 0;
}

void realizar_Colisao_SHIP_ASTEROID(SHIP *ship, ASTEROID *asteroid){

    float vx = (ship->x+(ship->width/2)) - (asteroid->x + (asteroid->width/2));
    float vy = (ship->y+(ship->height/2)) - (asteroid->y + (asteroid->height/2));

    float lenSq = (vx*vx + vy*vy);
    float len = sqrt(lenSq);

    vx /= len;
    vy /= len;

    asteroid->forceX += -vx;
    asteroid->forceY += -vy;

    limitarForcas(&asteroid->forceX, &asteroid->forceY, 1.5);

    ship->forceX -= asteroid->forceX/10;
    ship->forceY -= asteroid->forceY/10;
}

void realizar_Colisao_SHOT_ASTEROID(SHOT *shot, ASTEROID *asteroid){

    float vx = (shot->x+(shot->width/2)) - (asteroid->x + (asteroid->width/2));
    float vy = (shot->y+(shot->height/2)) - (asteroid->y + (asteroid->height/2));

    float lenSq = (vx*vx + vy*vy);
    float len = sqrt(lenSq);

    vx /= len;
    vy /= len;

    asteroid->forceX += -vx/2;
    asteroid->forceY += -vy/2;

    limitarForcas(&asteroid->forceX, &asteroid->forceY, 1.5);
}

void realizar_Colisao_ASTEROIDS(ASTEROID *asteroid_1, ASTEROID *asteroid_2){

    float vx = (asteroid_1->x+(asteroid_1->width/2)) - (asteroid_2->x + (asteroid_2->width/2));
    float vy = (asteroid_1->y+(asteroid_1->height/2)) - (asteroid_2->y + (asteroid_2->height/2));

    float lenSq = (vx*vx + vy*vy);
    float len = sqrt(lenSq);

    vx /= len;
    vy /= len;

    asteroid_2->forceX += -vx;
    asteroid_2->forceY += -vy;

    limitarForcas(&asteroid_2->forceX, &asteroid_2->forceY, 1.5);

    asteroid_1->forceX -= asteroid_2->forceX;
    asteroid_1->forceY -= asteroid_2->forceY;

    asteroid_2->forceX *= 0.6;
    asteroid_2->forceY *= 0.6;

    limitarForcas(&asteroid_1->forceX, &asteroid_1->forceY, 1.5);

    asteroid_1->forceX *= 0.6;
    asteroid_1->forceY *= 0.6;
}

void interagirGUNS(PLAYER player, int playerID, int shipWidth, int shipHeight, int *LEFT_CONTROL, int *RIGHT_CONTROL, int *UP_CONTROL, int *DOWN_CONTROL){
    if(player.x-25 < 0){
        if(*LEFT_CONTROL == 0){
                *LEFT_CONTROL = playerID;
        }else if(*LEFT_CONTROL == playerID){
                *LEFT_CONTROL = 0;
        }
    }else if(player.x+player.width+35 > shipWidth){
        if(*RIGHT_CONTROL == 0){
            *RIGHT_CONTROL = playerID;
            }else if(*RIGHT_CONTROL == playerID){
                *RIGHT_CONTROL = 0;
            }
    }else if(player.y-20 < 0){
        if(*UP_CONTROL == 0){
            *UP_CONTROL = playerID;
        }else if(*UP_CONTROL == playerID){
            *UP_CONTROL = 0;
        }
    }else if(player.y+player.height+30 > shipHeight){
        if(*DOWN_CONTROL == 0){
            *DOWN_CONTROL = playerID;
        }else if(*DOWN_CONTROL == playerID){
            *DOWN_CONTROL = 0;
        }
    }
}

void interagirSHIP(PLAYER player, int playerID, int shipWidth, int shipHeight, int *controle){
    if(player.x <= shipWidth/2+5 && player.x >= shipWidth/2-30 && player.y <= shipHeight/2-20 && player.y >= shipHeight/2-40){
        if(*controle == 0){
            *controle = playerID;
        }else if(*controle == playerID){
            *controle = 0;
        }
    }
}

void interagirOXIG(PLAYER player, int playerID, int *repondoOxi){
    if(*repondoOxi == 0 && player.x < 110 && player.y > 180){
        *repondoOxi = playerID;
    }else if(*repondoOxi == playerID){
        *repondoOxi = 0;
    }
}

void add_new_asteroid(ASTEROID *asteroid, int cameraX, int cameraY, int SCREEN_W, int SCREEN_H){
    for(int i=0; i<1;){
        int x = rand()%(WORLD_W-(asteroid->width));
        int y = rand()%(WORLD_H-(asteroid->height));
        if(x > cameraX-200 && x < cameraX+SCREEN_W+200 && y > cameraY-200 && y < y+SCREEN_H+200){

        }else{
            asteroid->x = x;
            asteroid->y = y;
            i++;
        }
    }
}





int main(int argc, char **argv){

    ALLEGRO_DISPLAY *display = NULL;
    //ALLEGRO_DISPLAY_MODE   disp_data; //FULLSCREEN
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    //FAZER A CAMERA SEGUIR A NAVE ATÉ NAS BORDAS DO MAPA, PARA EVITAR O "LAG" DA NAVE.
    float cameraX = 0, cameraY = 0;
    bool key[10] = {false, false, false, false, false, false, false, false, false, false};
    bool redraw = true;

    al_init();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(20);
    //al_init_primitives_addon(); //FULLSCREEN
    al_install_keyboard();
    timer = al_create_timer(1.0 / FPS);

    //FULLSCREEN
    //al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    //display = al_create_display(disp_data.width, disp_data.height);
    //int SCREEN_W = al_get_display_width(display);
    //int SCREEN_H = al_get_display_height(display);
    int SCREEN_W = 1380;
    int SCREEN_H = 780;
    display = al_create_display(SCREEN_W, SCREEN_H);
    srand(time(NULL));



    SHIP ship;
    ship = build_ship(WORLD_W/2, WORLD_H/2, 0, 0, 0,"images/ship/shipWhite.png");

    SIMP_OBJECT propulsor;
    propulsor = build_Simple_Object("images/ship/propulsorWhite.png");

    PLAYER player1;
    player1 = build_player(ship.width/2+7,ship.height/2+10,22,"images/players/","a##.png",0,1);

    PLAYER player2;
    player2 = build_player(ship.width/2-35,ship.height/2+10,22,"images/players/","b##.png",0,2);

    DYNF_OBJECT fire;
    fire = build_Dynamic_Object(12,"images/effects/fire/","f##.png",0);

    DYNF_OBJECT faisca;
    faisca = build_Dynamic_Object(6,"images/effects/spark/","h##.png",0);
    int collide_shot_ast = 0;
    float xF, yF;

    DYNF_OBJECT explosion;
    explosion = build_Dynamic_Object(12,"images/effects/explosion/","e##.png",0);
    int asteroid_explode = 0;
    float xE, yE;



    int QUANT_ASTEROIDS = 30;
    ASTEROID asteroids[QUANT_ASTEROIDS];
    generateAsteroids(QUANT_ASTEROIDS, &asteroids, 4, "images/asteroids/Asteroid.png");
    //--------------------------------------------------------------
    adaptarCamera(&cameraX, &cameraY, ship.x, ship.y, ship.width, ship.height, SCREEN_W, SCREEN_H);
    for(int i=0; i<QUANT_ASTEROIDS; i++){
        add_new_asteroid(&asteroids[i], cameraX, cameraY, SCREEN_W, SCREEN_H);
    }
    //--------------------------------------------------------------



    int QUANT_SHOTS = 10;
    SHOT shots[QUANT_SHOTS];
    shots[0].ativo = 0;
    shots[0].img = al_load_bitmap("images/effects/shot/s07.png");
    shots[0].width = al_get_bitmap_width(shots[0].img);
    shots[0].height = al_get_bitmap_height(shots[0].img);
    for(int i=1; i<QUANT_SHOTS; i++){
        shots[i] = shots[0];
    }

    GUN gunUP;
    gunUP = build_gun(ship.x+ship.width/2+2, ship.y-6, 0, -90);

    GUN gunLEFT;
    gunLEFT = build_gun(ship.x-6, ship.y+ship.height/2-3, 0, 180);

    GUN gunRIGHT;
    gunRIGHT = build_gun(ship.x+ship.width+10, ship.y+ship.height/2-4, 0, 0);

    GUN gunDOWN;
    gunDOWN = build_gun(ship.x+ship.width/2+2, ship.y+ship.height+7, 0, 90);

    SIMP_OBJECT canhao;
    canhao = build_Simple_Object("images/ship/canhao.png");

    SIMP_OBJECT canhaoBase;
    canhaoBase = build_Simple_Object("images/ship/gunBase.png");

    SIMP_OBJECT barra;
    barra = build_Simple_Object("images/barra/barra.png");

    SIMP_OBJECT vida;
    vida = build_Simple_Object("images/barra/vida.png");
    float vidascale = 0;

    SIMP_OBJECT oxigenio;
    oxigenio = build_Simple_Object("images/barra/oxigenio.png");
    float oxigenioscale = 0;

    SIMP_OBJECT background;
    background = build_Simple_Object("images/background.png");
    float BGScale = 1.7;


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
    explosao1 = al_load_sample("sounds/explosao.wav");
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

    ALLEGRO_SAMPLE *intro;
    ALLEGRO_SAMPLE_INSTANCE *inst_intro;
    intro = al_load_sample("sounds/intro.ogg");
    inst_intro = al_create_sample_instance(intro);
    al_attach_sample_instance_to_mixer(inst_intro,al_get_default_mixer());
    al_set_sample_instance_playmode(inst_intro, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_intro,0.8);


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
    int fechar = 0;


    //MENU
    while(1){

        ALLEGRO_EVENT ev2;
        al_wait_for_event(event_queue, &ev2);

        //EVENTOS RELACIONADO AO MOUSE (MENU INICIAL E FINAL)
        if(ev2.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            break;
        }
        /*INTERAÇÃO DO MOUSE NO MENU
        if(ev2.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            ----------------
        }
        if(ev2.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if(ev2.mouse.button & 1)
            {
                --------------------
            }
        }
        --------------------------------------------*/

        /*
        -------------------------
        -------------------------
        ---MENU INICIAL(DIEGO)---
        -------------------------
        -------------------------
        */

        //JOGO
        while((vidascale != vida.width)&&(fechar==0)){

            ALLEGRO_EVENT ev;
            al_wait_for_event(event_queue, &ev);

            //EVENTOS REAIS (TEMPO)
            if(ev.type == ALLEGRO_EVENT_TIMER) {

                //EVENTOS RELACIONADOS A VIDA
                if(oxigenioscale == oxigenio.width){
                    if(vidascale < vida.width){
                        vidascale += 0.1;
                    }else{
                        vidascale = vida.width;
                    }
                }

                //EVENTOS RELACIONADOS AO OXIGENIO
                if(oxigenioscale < oxigenio.width){
                    oxigenioscale += 0.2;
                }else{
                    oxigenioscale = oxigenio.width;
                }

                //NAVE / OXIGENIO / CANHOES / PLAYER 1
                if(gunLEFT.controle == 1){
                    player1.cur_Frame = 7*DELAY;
                    controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunLEFT.angle, 90);
                    atirar(&shots, key[KEY_UP], &gunLEFT.carga, ship.x-6, ship.y+ship.height/2-10, gunLEFT.angle-90, QUANT_SHOTS);
                }else if(gunRIGHT.controle == 1){
                    player1.cur_Frame = 0;
                    controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunRIGHT.angle, -90);
                    atirar(&shots, key[KEY_UP], &gunRIGHT.carga, ship.x+ship.width+6, ship.y+ship.height/2-10, gunRIGHT.angle-90, QUANT_SHOTS);
                }else if(gunUP.controle == 1){
                    player1.cur_Frame = 15*DELAY;
                    controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunUP.angle, -180);
                    atirar(&shots, key[KEY_UP], &gunUP.carga, ship.x+ship.width/2-6, ship.y-10, gunUP.angle-90, QUANT_SHOTS);
                }else if(gunDOWN.controle == 1){
                    player1.cur_Frame = 19*DELAY;
                    controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunDOWN.angle, 0);
                    atirar(&shots, key[KEY_UP], &gunDOWN.carga, ship.x+ship.width/2-6, ship.y+ship.height-5, gunDOWN.angle-90, QUANT_SHOTS);
                }else if(ship.repondoOxi == 1){
                    if(oxigenioscale <= oxigenio.width && oxigenioscale > 0){
                        oxigenioscale -= 1;
                    }else{
                        oxigenioscale = 0;
                    }
                }else if(ship.controle == 1){
                    player1.cur_Frame = 19*DELAY;
                    controlarNave(key[KEY_UP], key[KEY_LEFT], key[KEY_RIGHT], &ship.forceX, &ship.forceY, &ship.rotation, &fire.cur_Frame, inst_som_propulsor, inst_rotacao_h, inst_rotacao_a);
                    limitarForcas(&ship.forceX, &ship.forceY, 2);
                }else{
                    int flag = 0;
                    moverPlayer(&player1.x, &player1.y, &player1.dir, &flag, key[KEY_UP], key[KEY_LEFT], key[KEY_RIGHT], key[KEY_DOWN]);
                    atualizarFramePlayer(player1.dir, flag, &player1.cur_Frame);
                }

                //NAVE / OXIGENIO / CANHOES / PLAYER 2
                if(gunLEFT.controle == 2){
                    player2.cur_Frame = 7*DELAY;
                    controlarCanhao(key[KEY_A], key[KEY_D], &gunLEFT.angle, 90);
                    atirar(&shots, key[KEY_W], &gunLEFT.carga, ship.x-6, ship.y+ship.height/2-10, gunLEFT.angle-90, QUANT_SHOTS);
                }else if(gunRIGHT.controle == 2){
                    player2.cur_Frame = 0;
                    controlarCanhao(key[KEY_A], key[KEY_D], &gunRIGHT.angle, -90);
                    atirar(&shots, key[KEY_W], &gunRIGHT.carga, ship.x+ship.width+6, ship.y+ship.height/2-10, gunRIGHT.angle-90, QUANT_SHOTS);
                }else if(gunUP.controle == 2){
                    player2.cur_Frame = 15*DELAY;
                    controlarCanhao(key[KEY_A], key[KEY_D], &gunUP.angle, -180);
                    atirar(&shots, key[KEY_W], &gunUP.carga, ship.x+ship.width/2-6, ship.y-10, gunUP.angle-90, QUANT_SHOTS);
                }else if(gunDOWN.controle == 2){
                    player2.cur_Frame = 19*DELAY;
                    controlarCanhao(key[KEY_A], key[KEY_D], &gunDOWN.angle, 0);
                    atirar(&shots, key[KEY_W], &gunDOWN.carga, ship.x+ship.width/2-6, ship.y+ship.height-5, gunDOWN.angle-90, QUANT_SHOTS);
                }else if(ship.repondoOxi == 2){
                    if(oxigenioscale <= oxigenio.width && oxigenioscale > 0){
                        oxigenioscale -= 1;
                    }else{
                        oxigenioscale = 0;
                    }
                }else if(ship.controle == 2){
                    player2.cur_Frame = 19*DELAY;
                    controlarNave(key[KEY_W], key[KEY_A], key[KEY_D], &ship.forceX, &ship.forceY, &ship.rotation, &fire.cur_Frame, inst_som_propulsor, inst_rotacao_h, inst_rotacao_a);
                    limitarForcas(&ship.forceX, &ship.forceY, 2);
                }else{
                    int flag = 0;
                    moverPlayer(&player2.x, &player2.y, &player2.dir, &flag, key[KEY_W], key[KEY_A], key[KEY_D], key[KEY_S]);
                    atualizarFramePlayer(player2.dir, flag, &player2.cur_Frame);
                }


                if(gunLEFT.carga < 20){
                    gunLEFT.carga += 1;
                }
                if(gunRIGHT.carga < 20){
                    gunRIGHT.carga += 1;
                }
                if(gunUP.carga < 20){
                    gunUP.carga += 1;
                }
                if(gunDOWN.carga < 20){
                    gunDOWN.carga += 1;
                }

                for(int i=0; i<QUANT_SHOTS; i++){
                    if(shots[i].ativo != 0){
                        shots[i].ativo += 1;
                        if(shots[i].ativo == 100){
                            shots[i].ativo = 0;
                        }else{
                            shots[i].x += shots[i].velX;
                            shots[i].y += shots[i].velY;
                        }
                    }
                }

                for(int i=0; i<QUANT_ASTEROIDS; i++){
                    int colidiu = verificar_Colisao_SHIP_ASTEROID(&ship, &asteroids[i]);
                    if(colidiu){
                        if(asteroids[i].vida > 0){
                            realizar_Colisao_SHIP_ASTEROID(&ship, &asteroids[i]);
                        }
                    }
                }

                for(int i=0; i<QUANT_ASTEROIDS; i++){
                    for(int x=0; x<QUANT_ASTEROIDS; x++){
                        if(asteroids[i].vida > 0){
                            if(i != x && asteroids[x].vida > 0){
                                int colidiu = verificar_Colisao_ASTEROIDS(&asteroids[i], &asteroids[x]);
                                if(colidiu){
                                    realizar_Colisao_ASTEROIDS(&asteroids[i], &asteroids[x]);
                                }
                            }
                        }else{
                            add_new_asteroid(&asteroids[i], cameraX, cameraY, SCREEN_W, SCREEN_H);
                            asteroids[i].vida = 4;
                            break;
                        }
                    }
                }

                //FAÍSCA/EXPLOSAO--------------------------------------------------------------
                if(collide_shot_ast){
                    faisca.cur_Frame += 1;
                    if(faisca.cur_Frame >= 5*2){
                        faisca.cur_Frame = 0;
                        collide_shot_ast = 0;
                    }
                }
                if(asteroid_explode){
                    explosion.cur_Frame += 1;
                    if(explosion.cur_Frame >= 11*5){
                        explosion.cur_Frame = 0;
                        asteroid_explode = 0;
                    }
                }
                //----------------------------------------------------------------------------

                for(int i=0; i<QUANT_SHOTS; i++){
                    for(int x=0; x<QUANT_ASTEROIDS; x++){
                        if(shots[i].ativo != 0){
                            if(asteroids[x].vida > 0){
                                int colidiu = verificar_Colisao_SHOT_ASTEROID(&shots[i], &asteroids[x]);
                                if(colidiu){
                                    realizar_Colisao_SHOT_ASTEROID(&shots[i], &asteroids[x]);
                                    shots[i].ativo = 0;

                                    //FAÍSCA------------------------------------------------------------------------
                                    xF = ((shots[i].x+shots[i].width/2)+(asteroids[x].x+asteroids[x].width/4))/2;
                                    yF = ((shots[i].y+shots[i].height/2)+(asteroids[x].y+asteroids[x].height/4))/2;
                                    collide_shot_ast = 1;
                                    //------------------------------------------------------------------------------

                                    asteroids[x].vida -= 1;

                                    //EXPLOSÃO----------------------------------------------------------------------
                                    if(asteroids[x].vida <= 0){
                                       asteroid_explode = 1;
                                       xE = asteroids[x].x;
                                       yE = asteroids[x].y;
                                       al_play_sample_instance(inst_explosao1);
                                    }
                                    //------------------------------------------------------------------------------

                                }
                            }
                        }else{
                            break;
                        }
                    }
                }

                limitarAnguloNave(&ship.rotation);
                limitarForcas(&ship.forceX, &ship.forceY, 2);//TESTE - (EVITAR QUE O CONTATO COM ASTEROIDES EXCEDA 2)
                limitarNaveMundo(&ship.x, &ship.y, &ship.forceX, &ship.forceY, ship.width, ship.height);
                aplicarForcas(&ship.x, &ship.y, ship.forceX, ship.forceY);

                for(int i=0; i<QUANT_ASTEROIDS; i++){
                    if(asteroids[i].vida > 0){
                        limitarAsteroideMundo(&asteroids[i].x, &asteroids[i].y, &asteroids[i].forceX, &asteroids[i].forceY, asteroids[i].width, asteroids[i].height);
                    }
                }
                for(int i=0; i<QUANT_ASTEROIDS; i++){
                     if(asteroids[i].vida > 0){
                        aplicarForcas(&asteroids[i].x, &asteroids[i].y, asteroids[i].forceX, asteroids[i].forceY);
                     }
                }

                adaptarCamera(&cameraX, &cameraY, ship.x, ship.y, ship.width, ship.height, SCREEN_W, SCREEN_H);
                redraw = true;

            }else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
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
                        interagirGUNS(player1, 1, ship.width, ship.height, &gunLEFT.controle, &gunRIGHT.controle, &gunUP.controle, &gunDOWN.controle);
                        interagirSHIP(player1, 1, ship.width, ship.height, &ship.controle);
                        interagirOXIG(player1, 1, &ship.repondoOxi);
                        break;
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
                        interagirGUNS(player2, 2, ship.width, ship.height, &gunLEFT.controle, &gunRIGHT.controle, &gunUP.controle, &gunDOWN.controle);
                        interagirSHIP(player2, 2, ship.width, ship.height, &ship.controle);
                        interagirOXIG(player2, 2, &ship.repondoOxi);
                        break;
                }
            }else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
                switch(ev.keyboard.keycode) {
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
                fechar = 1;
                break;
            }

            if(redraw && al_is_event_queue_empty(event_queue)) {

                redraw = false;
                al_draw_scaled_bitmap(background.img,0,0,background.width,background.height,0-(cameraX/2.5),0-(cameraY/2.5),background.width*BGScale, background.height*BGScale, 0);

                if((key[KEY_UP] && ship.controle==1) || (key[KEY_W] && ship.controle==2) ){
                    al_draw_rotated_bitmap(fire.imgs[fire.cur_Frame],25, -200,ship.x-cameraX+ship.width/2, ship.y-cameraY+ship.height/2,((ship.rotation+180)*3.14159/180),0);
                }

                al_draw_rotated_bitmap(propulsor.img,propulsor.height/2, propulsor.height/2,ship.x-cameraX+ship.width/2, ship.y-cameraY+ship.height/2,((ship.rotation-90)*3.14159/180),0);
                al_draw_scaled_bitmap(ship.img,0,0,ship.width,ship.height,ship.x-cameraX,ship.y-cameraY,ship.width, ship.height, 0);

                al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX+ship.width/2+2, ship.y-cameraY-6,(gunUP.angle*3.14159/180), 0);
                al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX+ship.width+10, ship.y-cameraY+ship.height/2-4,(gunRIGHT.angle*3.14159/180), 0);
                al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX-6, ship.y-cameraY+ship.height/2-3,(gunLEFT.angle*3.14159/180), 0);
                al_draw_rotated_bitmap(canhao.img,12, canhao.height/2, ship.x-cameraX+ship.width/2+2, ship.y-cameraY+ship.height+7,(gunDOWN.angle*3.14159/180), 0);

                al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX+ship.width/2-12, ship.y-cameraY-10,0, 0);
                al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX+ship.width+15, ship.y-cameraY+ship.height/2-17,(90*3.14159/180), 0);
                al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX-12, ship.y-cameraY+ship.height/2+12,(-90*3.14159/180), 0);
                al_draw_rotated_bitmap(canhaoBase.img,12, canhaoBase.height/2, ship.x-cameraX+ship.width/2+16, ship.y-cameraY+ship.height+12,180*3.14159/180, 0);

                if(player1.y >= player2.y){
                    al_draw_scaled_bitmap(player2.imgs[player2.cur_Frame/DELAY],0,0,player2.width,player2.height,player2.x+ship.x-cameraX,player2.y+ship.y-cameraY,player2.width, player2.height, 0);
                    al_draw_scaled_bitmap(player1.imgs[player1.cur_Frame/DELAY],0,0,player1.width,player1.height,player1.x+ship.x-cameraX,player1.y+ship.y-cameraY,player1.width, player1.height, 0);
                }else{
                    al_draw_scaled_bitmap(player1.imgs[player1.cur_Frame/DELAY],0,0,player1.width,player1.height,player1.x+ship.x-cameraX,player1.y+ship.y-cameraY,player1.width, player1.height, 0);
                    al_draw_scaled_bitmap(player2.imgs[player2.cur_Frame/DELAY],0,0,player2.width,player2.height,player2.x+ship.x-cameraX,player2.y+ship.y-cameraY,player2.width, player2.height, 0);
                }

                for(int i=0; i<QUANT_ASTEROIDS; i++){
                    if(asteroids[i].vida > 0){
                        al_draw_scaled_bitmap(asteroids[i].img,0,0,asteroids[i].width,asteroids[i].height,asteroids[i].x-cameraX,asteroids[i].y-cameraY,asteroids[i].width, asteroids[i].height, 0);
                    }
                }

                if(collide_shot_ast){
                    al_draw_scaled_bitmap(faisca.imgs[faisca.cur_Frame/2],0,0,faisca.width,faisca.height,xF-cameraX,yF-cameraY,faisca.width, faisca.height, 0);
                }
                if(asteroid_explode){
                    al_draw_scaled_bitmap(explosion.imgs[explosion.cur_Frame/5],0,0,explosion.width,explosion.height,xE-cameraX,yE-cameraY,explosion.width*2.5, explosion.height*2.5, 0);
                }

                for(int i=0; i<QUANT_SHOTS; i++){
                    if(shots[i].ativo != 0){
                        al_draw_scaled_bitmap(shots[i].img,0,0, shots[i].width, shots[i].height, shots[i].x-cameraX, shots[i].y-cameraY, shots[i].width*0.7, shots[i].height*0.7, 0);
                    }
                }

                al_draw_scaled_bitmap(vida.img,0,0,vida.width,vida.height,(SCREEN_W/2)-(vida.width/2)-15,SCREEN_H-43,vida.width-vidascale,vida.height, 0);
                al_draw_scaled_bitmap(oxigenio.img,0,0,oxigenio.width,oxigenio.height,SCREEN_W/2-oxigenio.width/2-15,SCREEN_H-36,oxigenio.width-oxigenioscale,oxigenio.height, 0);
                al_draw_scaled_bitmap(barra.img,0,0,barra.width,barra.height,SCREEN_W/2-barra.width/2,SCREEN_H-100,barra.width,barra.height, 0);

                al_flip_display();

            }

        }//JOGO


        if(fechar){
            break;
        }

        al_stop_sample_instance(inst_theme);
        al_stop_sample_instance(inst_tiro);
        al_stop_sample_instance(inst_explosao1);
        al_stop_sample_instance(inst_explosao2);
        al_stop_sample_instance(inst_explosao3);
        al_stop_sample_instance(inst_som_propulsor);
        al_stop_sample_instance(inst_rotacao_a);
        al_stop_sample_instance(inst_rotacao_h);
        al_stop_sample_instance(inst_alerta);
        al_flip_display();

        /*
        ------------------------------------
        ---------MENU ENCECRRAMENTO---------
        ---------------WEI------------------
        ------------------------------------
        */

    }//MENUS

    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_bitmap(background.img);
    al_destroy_bitmap(propulsor.img);
    al_destroy_bitmap(ship.img);
    al_destroy_bitmap(barra.img);
    al_destroy_bitmap(vida.img);
    al_destroy_bitmap(oxigenio.img);
    al_destroy_bitmap(canhao.img);
    al_destroy_bitmap(canhaoBase.img);

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

    for(int i=0; i<6; i++){
        al_destroy_bitmap(faisca.imgs[i]);
    }
    for(int i=0; i<12; i++){
        al_destroy_bitmap(explosion.imgs[i]);
    }

    for(int i=0; i<QUANT_SHOTS; i++){
        al_destroy_bitmap(shots[i].img);
    }
    for(int i=0; i<QUANT_ASTEROIDS; i++){
        al_destroy_bitmap(asteroids[i].img);
    }
    for(int i=0; i<12; i++){
        al_destroy_bitmap(fire.imgs[i]);
    }
    for(int i=0; i<22; i++){
        al_destroy_bitmap(player1.imgs[i]);
    }
    for(int i=0; i<22; i++){
        al_destroy_bitmap(player2.imgs[i]);
    }
    al_destroy_event_queue(event_queue);

    return 0;
}
