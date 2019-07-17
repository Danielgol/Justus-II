#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <windows.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <math.h>





const float FPS = 60;
const int WORLD_W = 16000;
const int WORLD_H = 16000;
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
            if( (*x < 110) || (*y > 160) || (*x > 128 && *x < 160) || (*x > 165 && *y > 60) || (*x < 165 && *y <= 93) || (*x < 172 && *y <= 93) ){
                if((*y >= 93 && *y < 110 && *x < 212) || (*y < 93) || (*y >= 110)){
                    if(*x + VELOCITY <= 270){
                        *x += VELOCITY;
                }
            }}
        }
        *flag = 1;
    }
    if(LEFT){
        *dir = 2;
        if(*x - VELOCITY >=65 && *y>=40 && *y <=217 || *y >= 110 && *y <= 155 || *x - VELOCITY >=128 && *x - VELOCITY <=165 ){
            if( (*y > 160 && *x > 105) || (*x > 134 && *x < 165) || (*y < 198 && *x < 128) || (*y <= 93) || (*y >= 93 && *x > 188) ){
                 if((*y >= 93 && *y < 110 && *x > 85)|| (*y < 93) || (*y >= 110)){
                    if(*x - VELOCITY >= 23){
                        *x -= VELOCITY;
                }}
            }
        }
        *flag = 1;
    }
    if(UP){
        *dir = 3;
        if(*y - VELOCITY >= 40 && *x>=65 && *x<=232 || *x >= 128 && *x <= 165 || *y - VELOCITY >= 110 && *y - VELOCITY <= 155 ){
            if((*y > 163) || (*x < 116 && *y <= 163) || (*y > 93 && *y < 130)|| (*y <= 93 && *x <= 175) || (*y > 63 && *x > 178)){
               if((*y >= 93 && *y < 110 && *x > 80 && *x < 215)|| (*y < 93) || (*y >= 110)){
                    if(*y - VELOCITY >=7){
                        *y -= VELOCITY;
                }}
        }}
        *flag = 1;
    }
    if(DOWN){
        *dir = 4;
        if(*y + VELOCITY <= 217 && *x>=65 && *x<=232 || *x >= 128 && *x <= 165 || *y + VELOCITY >= 110 && *y + VELOCITY <= 155 ){
            if((*y <= 93) || (*y > 93 && *y < 124) || (*y > 161 && *x > 100)||(*x<=100 && *y < 193) || (*x < 116 && *y <=163) || (*x > 178)){
                if((*y > 93)|| (*x >128 && *x < 165)||(*x > 178)||(*x <116) || (*y < 93)){
                        if((*x > 80 && *x < 215)|| (*y < 91 && *x <= 80) || ((*y < 91 && *x >= 215)) || (*y >= 105)){
                            if(*y + VELOCITY <= 255){
                                *y += VELOCITY;
            }}}}
        }
        *flag = 1;
    }
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
}





//@OBSOLETO (ALTERAR OU REMOVER)
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

int atirar(SHOT *shots, bool UP, int *carga, float x, float y, int angle, int QUANT_SHOTS){
    if(UP && *carga == 20){
        for(int i=0; i<QUANT_SHOTS; i++){
            if((shots+i)->ativo == 0 ){
                (shots+i)->ativo = 1;
                (shots+i)->x = x;
                (shots+i)->y = y;
                (shots+i)->velX = -sin(angle*3.14159/180)*38;
                (shots+i)->velY = cos(angle*3.14159/180)*38;
                *carga = 0;
                return 1;
            }
        }
    }
    return 0;
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
        *rotation+=2;
        al_play_sample_instance(inst_rotacao_h);
    }else{
        al_stop_sample_instance(inst_rotacao_h);
    }
    if(RIGHT){
        *rotation-=2;
        al_play_sample_instance(inst_rotacao_a);
    }else{
        al_stop_sample_instance(inst_rotacao_a);
    }
}

float verificar_Colisao_SHIP_ASTEROID(SHIP *ship, ASTEROID *asteroid){
    float dist = sqrt((pow((ship->x + ship->width/2)-(asteroid->x + asteroid->width/2), 2)) + (pow((ship->y + ship->height/2)-(asteroid->y + asteroid->height/2), 2)));
    if(dist <= (ship->width/2)+(asteroid->width/2)){
        return 0;
    }
    return dist;
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
    }else if(player.y+player.height+35 > shipHeight){
        if(*DOWN_CONTROL == 0){
            *DOWN_CONTROL = playerID;
        }else if(*DOWN_CONTROL == playerID){
            *DOWN_CONTROL = 0;
        }
    }
}

void interagirSHIP(PLAYER player, int playerID, int shipWidth, int shipHeight, int *controle, ALLEGRO_SAMPLE_INSTANCE* inst_som_propulsor , ALLEGRO_SAMPLE_INSTANCE* inst_rotacaoh , ALLEGRO_SAMPLE_INSTANCE* inst_rotacaoa){
    if(player.x <= shipWidth/2+5 && player.x >= shipWidth/2-30 && player.y <= shipHeight/2-20 && player.y >= shipHeight/2-40){
        if(*controle == 0){
            *controle = playerID;
        }else if(*controle == playerID){
            *controle = 0;
            al_stop_sample_instance(inst_som_propulsor);
            al_stop_sample_instance(inst_rotacaoa);
            al_stop_sample_instance(inst_rotacaoh);
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

        int x = rand()%(SCREEN_W+1000)+(cameraX-500);
        int y = rand()%(SCREEN_H+1000)+(cameraY-500);

        float Fx = rand()%(40)/10.0 - 2.0;
        float Fy = rand()%(40)/10.0 - 2.0;

        if(Fx < 0.5 && Fx >= 0){
            Fx = 0.5;
        }else if(Fx < 0 && Fx > -0.5){
            Fx = -0.5;
        }

        if(Fy < 0.5 && Fy >= 0){
            Fy = 0.5;
        }else if(Fy < 0 && Fy > -0.5){
            Fy = -0.5;
        }

        if((x > cameraX-100 && x < cameraX+SCREEN_W+100 && y > cameraY-100 && y < cameraY+SCREEN_H+100) || (x <= 0 || y <= 0 || x+asteroid->width > WORLD_W || y+asteroid->height > WORLD_H)) {

        }else{
            asteroid->x = x;
            asteroid->y = y;
            asteroid->forceX = Fx;
            asteroid->forceY = Fy;
            i++;
        }
    }
}

void add_new_especial_asteroid(ASTEROID *asteroid, int cameraX, int cameraY, int SCREEN_W, int SCREEN_H){
    for(int i=0; i<1;){

        int x = rand()%(WORLD_W-(asteroid->width));
        int y = rand()%(WORLD_H-(asteroid->height));

        float Fx = rand()%(40)/10.0 - 2.0;
        float Fy = rand()%(40)/10.0 - 2.0;

        if(Fx < 0.5 && Fx >= 0){
            Fx = 0.5;
        }else if(Fx < 0 && Fx > -0.5){
            Fx = -0.5;
        }

        if(Fy < 0.5 && Fy >= 0){
            Fy = 0.5;
        }else if(Fy < 0 && Fy > -0.5){
            Fy = -0.5;
        }

        if((x > cameraX-100 && x < cameraX+SCREEN_W+100 && y > cameraY-100 && y < cameraY+SCREEN_H+100) || (x <= 0 || y <= 0 || x+asteroid->width > WORLD_W || y+asteroid->height > WORLD_H)) {

        }else{
            asteroid->x = x;
            asteroid->y = y;
            asteroid->forceX = Fx;
            asteroid->forceY = Fy;
            i++;
        }
    }
}

void throw_oxi_ball(ASTEROID *ball, float x, float y){

    float Fx = rand()%(40)/10.0 - 2.0;
    float Fy = rand()%(40)/10.0 - 2.0;

    if(Fx < 0.5 && Fx >= 0){
        Fx = 0.5;
    }else if(Fx < 0 && Fx > -0.5){
        Fx = -0.5;
    }

    if(Fy < 0.5 && Fy >= 0){
        Fy = 0.5;
    }else if(Fy < 0 && Fy > -0.5){
        Fy = -0.5;
    }

    ball->x = x;
    ball->y = y;
    ball->vida = 800;
    ball->forceX = Fx;
    ball->forceY = Fy;
}







void reset_players(bool key[10], PLAYER *player1, PLAYER *player2, int shipWidth, int shipHeight){
    key[KEY_UP] = false;
    key[KEY_DOWN] = false;
    key[KEY_LEFT] = false;
    key[KEY_RIGHT] = false;
    key[KEY_W] = false;
    key[KEY_S] = false;
    key[KEY_A] = false;
    key[KEY_D] = false;

    player1->x = shipWidth/2+7;
    player1->y = shipHeight/2+10;
    player2->x = shipWidth/2-35;
    player2->y = shipHeight/2+10;

    player1->cur_Frame = 0;
    player2->cur_Frame = 0;

    player1->dir = 1;
    player2->dir = 2;
}

void reset_ship(SHIP *ship, GUN *gunLEFT, GUN *gunRIGHT, GUN *gunUP, GUN *gunDOWN, int QUANT_SHOTS, SHOT shots[QUANT_SHOTS], float *vidascale, float *oxigenioscale, float *oxigenioscale2, int oxigenio2Width){
    ship->x = WORLD_W/2;
    ship->y = WORLD_H/2;
    ship->forceX = 0;
    ship->forceY = 0;
    ship->rotation = 0;

    *vidascale = 0;
    *oxigenioscale = 0;
    *oxigenioscale2 = oxigenio2Width;

    gunLEFT->controle = 0;
    gunRIGHT->controle = 0;
    gunUP->controle = 0;
    gunDOWN->controle = 0;

    gunLEFT->angle = 180;
    gunRIGHT->angle = 0;
    gunUP->angle = -90;
    gunDOWN->angle = 90;

    ship->repondoOxi = 0;
    ship->controle = 0;

    for(int i=0; i<QUANT_SHOTS; i++){
        shots[i].ativo = 0;
    }
}

void reset_asteroids(int QUANT_ASTEROIDS, ASTEROID asteroids[QUANT_ASTEROIDS], int *asteroid_explode, int *explosion_cur_Frame){
    *asteroid_explode = 0;
    *explosion_cur_Frame = 0;
    for(int i=0; i<QUANT_ASTEROIDS; i++){
        asteroids[i].vida = 0;
    }
}

void reset_especial_asteroids(int QUANT_ESPECIAL, ASTEROID especiais[QUANT_ESPECIAL], int QUANT_OXI, ASTEROID oxigen_balls[QUANT_OXI]){
    for(int i=0; i<QUANT_ESPECIAL; i++){
        especiais[i].vida = 0;
    }
    for(int i=0; i<QUANT_OXI; i++){
        oxigen_balls[i].vida = 0;
    }
}

void show_intro(DYNF_OBJECT intro, ALLEGRO_SAMPLE_INSTANCE* inst_intro, int SCREEN_W, int SCREEN_H){

    Sleep(2000);
    al_play_sample_instance(inst_intro);
    Sleep(5370);

    al_draw_scaled_bitmap(intro.imgs[0],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(5000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(2000);

    al_draw_scaled_bitmap(intro.imgs[1],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(9000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1000);

    al_draw_scaled_bitmap(intro.imgs[2],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(3000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(400);

    al_draw_scaled_bitmap(intro.imgs[3],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(2700);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(400);

    al_draw_scaled_bitmap(intro.imgs[4],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(2700);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(600);

    al_draw_scaled_bitmap(intro.imgs[5],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(4400);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(4500);

    al_draw_scaled_bitmap(intro.imgs[6],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(11000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1400);

    al_draw_scaled_bitmap(intro.imgs[0],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(3500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(800);

    al_draw_scaled_bitmap(intro.imgs[7],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(7000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(800);

    al_draw_scaled_bitmap(intro.imgs[8],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(10000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[9],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(11000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[0],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(3500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[10],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(10000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1000);

    al_draw_scaled_bitmap(intro.imgs[11],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(7000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(400);

    al_draw_scaled_bitmap(intro.imgs[12],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(7000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(400);

    al_draw_scaled_bitmap(intro.imgs[13],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(7000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[0],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(3500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[14],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(7000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[15],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(7000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[16],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(2500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[17],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(2000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[18],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(3000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[19],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(3500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1000);

    al_draw_scaled_bitmap(intro.imgs[20],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(7000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[21],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(2000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[22],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[23],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[24],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(500);

    al_draw_scaled_bitmap(intro.imgs[25],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(700);

    al_draw_scaled_bitmap(intro.imgs[26],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1700);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1000);

    al_draw_scaled_bitmap(intro.imgs[27],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(5000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1500);

    al_draw_scaled_bitmap(intro.imgs[28],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(3000);

    al_draw_scaled_bitmap(intro.imgs[29],0,0,intro.width,intro.height,0,0,SCREEN_W,SCREEN_H, 0);
    al_flip_display();
    Sleep(1000);
}

void switchNumbers(int *new_ranking, int index_Menor, int i){

    int maior = (new_ranking[i]);
    int menor = (new_ranking[index_Menor]);

    new_ranking[i] = menor;
    new_ranking[index_Menor] = maior;
}















int main(int argc, char **argv){

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_DISPLAY_MODE   disp_data;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    float cameraX = 0, cameraY = 0;
    bool key[10] = {false, false, false, false, false, false, false, false, false, false};
    bool redraw = true;

    al_init();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(20);
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_mouse();
    al_install_keyboard();
    timer = al_create_timer(1.0 / FPS);

    //FULLSCREEN
    al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);
    al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    display = al_create_display(disp_data.width, disp_data.height);
    int SCREEN_W = al_get_display_width(display);
    int SCREEN_H = al_get_display_height(display);
    srand(time(NULL));
















    int pontuacao = 0;
    int ranking[5];
    FILE *score;
    score = fopen("score.txt", "r");
    fscanf(score, "%i %i %i %i %i" , &ranking[0], &ranking[1], &ranking[2], &ranking[3], &ranking[4]);
    fclose(score);


    SHIP ship;
    ship = build_ship(WORLD_W/2, WORLD_H/2, 0, 0, 0,"images/ship/shipWhite.png");

    SIMP_OBJECT propulsor;
    propulsor = build_Simple_Object("images/ship/propulsorWhite.png");

    SIMP_OBJECT objetos_nave;
    objetos_nave = build_Simple_Object("images/ship/objetos1.png");

    PLAYER player1;
    player1 = build_player(ship.width/2+7,ship.height/2+10,22,"images/players/","a##.png",0,1);

    PLAYER player2;
    player2 = build_player(ship.width/2-35,ship.height/2+10,22,"images/players/","b##.png",0,2);

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

    DYNF_OBJECT explosion_especial;
    explosion_especial = build_Dynamic_Object(9,"images/effects/explosion/","f##.png",0);
    int especial_explode = 0;
    float xE_E, yE_E;

    //FLASHES DO TIRO-------------------------------------------------------------------------------------------------------------------------------------
    DYNF_OBJECT flash;
    flash = build_Dynamic_Object(2,"images/effects/shot/","s##.png",0);
    int flash_p1 = 0;

    DYNF_OBJECT flash2;
    flash2 = build_Dynamic_Object(2,"images/effects/shot/","s##.png",0);
    int flash_p2 = 0;
    //FLASHES DO TIRO-------------------------------------------------------------------------------------------------------------------------------------

    int QUANT_ASTEROIDS = 30;
    ASTEROID asteroids[QUANT_ASTEROIDS];
    generateAsteroids(QUANT_ASTEROIDS, &asteroids, 4, "images/asteroids/Asteroid.png");
    adaptarCamera(&cameraX, &cameraY, ship.x, ship.y, ship.width, ship.height, SCREEN_W, SCREEN_H);
    for(int i=0; i<QUANT_ASTEROIDS; i++){
        add_new_asteroid(&asteroids[i], cameraX, cameraY, SCREEN_W, SCREEN_H);
    }

    int QUANT_ESPECIAL_ASTEROIDS = 50;
    ASTEROID especiais[QUANT_ESPECIAL_ASTEROIDS];
    generateAsteroids(QUANT_ESPECIAL_ASTEROIDS, &especiais, 1, "images/asteroids/asteroid_esp.png");
    for(int i=0; i<QUANT_ESPECIAL_ASTEROIDS; i++){
        especiais[i].vida = 0;
    }

    int QUANT_OXI_BALLS = 10;
    ASTEROID oxigen_balls[QUANT_OXI_BALLS];
    generateAsteroids(QUANT_OXI_BALLS, &oxigen_balls, 0, "images/asteroids/oxiball.png");

    int QUANT_SHOTS = 10;
    SHOT shots[QUANT_SHOTS];
    shots[0].ativo = 0;
    shots[0].img = al_load_bitmap("images/effects/shot/shot.png");
    shots[0].width = al_get_bitmap_width(shots[0].img);
    shots[0].height = al_get_bitmap_height(shots[0].img);
    for(int i=1; i<QUANT_SHOTS; i++){
        shots[i] = shots[0];
    }

    SIMP_OBJECT barra;
    barra = build_Simple_Object("images/barra/barra.png");

    SIMP_OBJECT vida;
    vida = build_Simple_Object("images/barra/vida.png");
    float vidascale = 0;

    SIMP_OBJECT oxigenio;
    oxigenio = build_Simple_Object("images/barra/oxigenio.png");
    float oxigenioscale = 0;

    SIMP_OBJECT barra2;
    barra2 = build_Simple_Object("images/barra/barra2.png");

    SIMP_OBJECT oxigenio2;
    oxigenio2 = build_Simple_Object("images/barra/oxigenio2.png");
    float oxigenioscale2 = oxigenio2.width;

    SIMP_OBJECT background;
    background = build_Simple_Object("images/fundo/background.png");
    float BGScale = 1.7;

    SIMP_OBJECT planeta;
    planeta = build_Simple_Object("images/fundo/planet.png");

    SIMP_OBJECT beltside;
    beltside = build_Simple_Object("images/asteroids/belt.png");
    float beltScale = 2.3;

    SIMP_OBJECT belttop;
    belttop = build_Simple_Object("images/asteroids/beltup.png");

    SIMP_OBJECT dangerzoneSide;
    dangerzoneSide = build_Simple_Object("images/asteroids/dangerzoneSide.png");

    SIMP_OBJECT dangerzoneTop;
    dangerzoneTop = build_Simple_Object("images/asteroids/dangerzoneTop.png");



    //MENU----------------------------------------------------------------------------

    SIMP_OBJECT menu_background;
    menu_background = build_Simple_Object("images/menu/menu_background.png");

    SIMP_OBJECT logo;
    logo = build_Simple_Object("images/menu/logo.png");

    DYNF_OBJECT iniciar;
    iniciar = build_Dynamic_Object(2,"images/menu/","iniciar##.png", 0);

    DYNF_OBJECT story;
    story = build_Dynamic_Object(2,"images/menu/","storymode##.png", 0);

    DYNF_OBJECT continuar;
    continuar = build_Dynamic_Object(2,"images/menu/","continuar##.png", 0);

    DYNF_OBJECT close_X;
    close_X = build_Dynamic_Object(2,"images/menu/","x##.png", 0);

    SIMP_OBJECT creditos;
    creditos = build_Simple_Object("images/menu/creditos.png");

    DYNF_OBJECT pauseButton;
    pauseButton = build_Dynamic_Object(2,"images/menu/","pause##.png", 0);

    SIMP_OBJECT tela_pausa;
    tela_pausa = build_Simple_Object("images/menu/tela_pausa.png");

    DYNF_OBJECT video_intro;
    video_intro = build_Dynamic_Object(30,"images/intro/","##.jpg", 0);

    DYNF_OBJECT creditosButton;
    creditosButton = build_Dynamic_Object(2,"images/menu/","creditos##.png",0);

    DYNF_OBJECT sair;
    sair = build_Dynamic_Object(2,"images/menu/","sair##.png", 0);

    SIMP_OBJECT game_over;
    game_over = build_Simple_Object("images/menu/game_over.png");

    DYNF_OBJECT reiniciar;
    reiniciar = build_Dynamic_Object(2,"images/menu/","reiniciar##.png",0);

    DYNF_OBJECT rankingButton;
    rankingButton = build_Dynamic_Object(2,"images/menu/","ranking##.png",0);

    SIMP_OBJECT ranking_tela;
    ranking_tela = build_Simple_Object("images/menu/ranking_tela.png");

    DYNF_OBJECT menuButton;
    menuButton = build_Dynamic_Object(2,"images/menu/","menu##.png",0);

    SIMP_OBJECT black;
    black = build_Simple_Object("images/menu/black.png");

    SIMP_OBJECT deadnaut;
    deadnaut = build_Simple_Object("images/menu/deadnaut.png");

    //--------------------------------------------------------------------------------



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
    tiro = al_load_sample("sounds/tiro.ogg");
    inst_tiro = al_create_sample_instance(tiro);
    al_attach_sample_instance_to_mixer(inst_tiro,al_get_default_mixer());
    al_set_sample_instance_gain(inst_tiro,0.6);

    ALLEGRO_SAMPLE *explosao1;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao1;
    explosao1 = al_load_sample("sounds/explosao.wav");
    inst_explosao1 = al_create_sample_instance(explosao1);
    al_attach_sample_instance_to_mixer(inst_explosao1,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao1,1.2);

    ALLEGRO_SAMPLE *explosao2;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao2;
    explosao2 = al_load_sample("sounds/explosao2.ogg");
    inst_explosao2 = al_create_sample_instance(explosao2);
    al_attach_sample_instance_to_mixer(inst_explosao2,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao2,0.6);

    ALLEGRO_SAMPLE *explosao3;
    ALLEGRO_SAMPLE_INSTANCE *inst_explosao3;
    explosao3 = al_load_sample("sounds/explosao3.ogg");
    inst_explosao3 = al_create_sample_instance(explosao3);
    al_attach_sample_instance_to_mixer(inst_explosao3,al_get_default_mixer());
    al_set_sample_instance_gain(inst_explosao3,0.6);

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
    al_set_sample_instance_gain(inst_rotacao_a,1.6);

    ALLEGRO_SAMPLE *rotacao_h;
    ALLEGRO_SAMPLE_INSTANCE *inst_rotacao_h;
    rotacao_h = al_load_sample("sounds/rotacaoh.ogg");
    inst_rotacao_h = al_create_sample_instance(rotacao_h);
    al_attach_sample_instance_to_mixer(inst_rotacao_h,al_get_default_mixer());
    al_set_sample_instance_gain(inst_rotacao_h,1.6);

    ALLEGRO_SAMPLE *alerta;
    ALLEGRO_SAMPLE_INSTANCE *inst_alerta;
    alerta = al_load_sample("sounds/alerta.ogg");
    inst_alerta = al_create_sample_instance(alerta);
    al_attach_sample_instance_to_mixer(inst_alerta,al_get_default_mixer());
    al_set_sample_instance_gain(inst_alerta,0.4);

    ALLEGRO_SAMPLE *crash;
    ALLEGRO_SAMPLE_INSTANCE *inst_crash;
    crash = al_load_sample("sounds/crash.ogg");
    inst_crash = al_create_sample_instance(crash);
    al_attach_sample_instance_to_mixer(inst_crash,al_get_default_mixer());
    al_set_sample_instance_gain(inst_crash,1.0);

    ALLEGRO_SAMPLE *intro;
    ALLEGRO_SAMPLE_INSTANCE *inst_intro;
    intro = al_load_sample("sounds/intro.ogg");
    inst_intro = al_create_sample_instance(intro);
    al_attach_sample_instance_to_mixer(inst_intro,al_get_default_mixer());
    al_set_sample_instance_gain(inst_intro,0.8);

    ALLEGRO_SAMPLE *menu_music;
    ALLEGRO_SAMPLE_INSTANCE *inst_menu_music;
    menu_music = al_load_sample("sounds/menu_music.ogg");
    inst_menu_music = al_create_sample_instance(menu_music);
    al_attach_sample_instance_to_mixer(inst_menu_music,al_get_default_mixer());
    al_set_sample_instance_playmode(inst_menu_music, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_menu_music,0.8);

    ALLEGRO_SAMPLE *menu_death;
    ALLEGRO_SAMPLE_INSTANCE *inst_menu_death;
    menu_death = al_load_sample("sounds/menu_death.ogg");
    inst_menu_death = al_create_sample_instance(menu_death);
    al_attach_sample_instance_to_mixer(inst_menu_death,al_get_default_mixer());
    al_set_sample_instance_playmode(inst_menu_death, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_menu_death,0.8);

    ALLEGRO_SAMPLE *button;
    ALLEGRO_SAMPLE_INSTANCE *inst_button;
    button = al_load_sample("sounds/button.ogg");
    inst_button = al_create_sample_instance(button);
    al_attach_sample_instance_to_mixer(inst_button,al_get_default_mixer());
    al_set_sample_instance_gain(inst_button,1.0);

    ALLEGRO_SAMPLE *pass_button;
    ALLEGRO_SAMPLE_INSTANCE *inst_pass_button;
    pass_button = al_load_sample("sounds/pass_button.ogg");
    inst_pass_button = al_create_sample_instance(pass_button);
    al_attach_sample_instance_to_mixer(inst_pass_button,al_get_default_mixer());
    al_set_sample_instance_gain(inst_pass_button,1.0);

    ALLEGRO_SAMPLE *som_oxigenio;
    ALLEGRO_SAMPLE_INSTANCE *inst_som_oxigenio;
    som_oxigenio = al_load_sample("sounds/som_oxigenio.ogg");
    inst_som_oxigenio = al_create_sample_instance(som_oxigenio);
    al_attach_sample_instance_to_mixer(inst_som_oxigenio,al_get_default_mixer());
    al_set_sample_instance_gain(inst_som_oxigenio,1.0);


    ALLEGRO_FONT *font = al_load_font("images/font/superstar.ttf", 42, 0);


    //CONFIGURA/INICIALIZA EVENTOS E LOCAL DE DESENHO
    al_set_target_bitmap(al_get_backbuffer(display));
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_flip_display();
    al_start_timer(timer);

    //--------------------------------------------------------------------------------
    int pos_x = 0, pos_y = 0; //MOUSE
    int fechar = 0; // FECHAR O JOGO NO [X] DO DISPLAY
    int show_Menu = 1; // MOSTRAR (NORMAL) OU PULAR O MENU (REINICIAR)
    int show_Menu_Death = 1;// MOSTRAR MENU DE GAME OVER
    //--------------------------------------------------------------------------------














    while(1){


        int show_ranking = 0;
        int show_credits = 0;
        int button_flag = 0;
        int play_intro = 0;
        al_play_sample_instance(inst_menu_music);

        //MENU INICIAL
        while(show_Menu && fechar == 0){

            ALLEGRO_EVENT ev2;
            al_wait_for_event(event_queue, &ev2);
            int redrawMenu = 0;

            if(ev2.type == ALLEGRO_EVENT_TIMER) {
                if(play_intro){
                    break;
                }
                redrawMenu = 1;
            }else if(ev2.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(ev2.mouse.button & 1){
                    if(show_credits == 0 && show_ranking == 0){
                        if(pos_x >= SCREEN_W/2-(iniciar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(iniciar.width*0.9)/2)+iniciar.width*0.9 && pos_y >= SCREEN_H/2+(iniciar.height*0.9)/2-100 && pos_y <= (SCREEN_H/2+(iniciar.height*0.9)/2-100)+iniciar.height*0.9){
                            al_play_sample_instance(inst_button);
                            break;
                        }else if(pos_x >= SCREEN_W/2-(sair.width*0.9)/2 && pos_x <= (SCREEN_W/2-(sair.width*0.9)/2)+sair.width*0.9 && pos_y >= SCREEN_H/2+(sair.height*0.9)/2+260 && pos_y <= (SCREEN_H/2+(sair.height*0.9)/2+260)+sair.height*0.9){
                            al_play_sample_instance(inst_button);
                            fechar = 1;
                            break;
                        }else if(pos_x >= SCREEN_W/2-(story.width*0.9)/2 && pos_x <= (SCREEN_W/2-(story.width*0.9)/2)+story.width*0.9 && pos_y >= SCREEN_H/2+(story.height*0.9)/2+20 && pos_y <= (SCREEN_H/2+(story.height*0.9)/2+20)+story.height*0.9){
                            al_play_sample_instance(inst_button);
                            play_intro = 1;
                        }else if(pos_x >= SCREEN_W/2-(rankingButton.width*0.9)/2 && pos_x <= (SCREEN_W/2-(rankingButton.width*0.9)/2)+rankingButton.width*0.9 && pos_y >= SCREEN_H/2+(rankingButton.height*0.9)/2+140 && pos_y <= (SCREEN_H/2+(rankingButton.height*0.9)/2+140)+rankingButton.height*0.9){
                            al_play_sample_instance(inst_button);
                            show_ranking = 1;
                        }else if(pos_x >= 20 && pos_x <= 20+creditosButton.width && pos_y >= SCREEN_H-creditosButton.height-20 && pos_y <= SCREEN_H-20){
                            al_play_sample_instance(inst_button);
                            show_credits = 1;
                        }
                    }else{

                        if(show_ranking){
                            if(pos_x >= (SCREEN_W/2+(ranking_tela.width)/2)-(close_X.width)-10 && pos_x <= (SCREEN_W/2+(ranking_tela.width)/2)-(close_X.width)+close_X.width-10 && pos_y >= (SCREEN_H/2-(ranking_tela.height)/2)+10 && pos_y <= (SCREEN_H/2-(ranking_tela.height)/2)+close_X.height+10){
                                al_play_sample_instance(inst_button);
                                show_ranking = 0;
                            }
                        }else if(show_credits){
                            if(pos_x >= (SCREEN_W/2+(creditos.width)/2)-(close_X.width)-10 && pos_x <= (SCREEN_W/2+(creditos.width)/2)-(close_X.width)+close_X.width-10 && pos_y >= (SCREEN_H/2-(creditos.height)/2)+10 && pos_y <= (SCREEN_H/2-(creditos.height)/2)+close_X.height+10){
                                al_play_sample_instance(inst_button);
                                show_credits = 0;
                            }
                        }

                    }
                }
            }else if(ev2.type == ALLEGRO_EVENT_MOUSE_AXES){
                pos_x = ev2.mouse.x;
                pos_y = ev2.mouse.y;
            }else if(ev2.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                fechar = 1;
                break;
            }

            //DESENHA
            if(redrawMenu && al_is_event_queue_empty(event_queue)){
                redrawMenu = 0;

                al_draw_scaled_bitmap(menu_background.img,0,0,menu_background.width,menu_background.height,0,0,SCREEN_W, SCREEN_H, 0);

                if(show_credits == 0 && show_ranking == 0){

                    al_draw_scaled_bitmap(logo.img,0,0,logo.width,logo.height,SCREEN_W/2-logo.width*0.5/2, 80 ,logo.width*0.5, logo.height*0.5, 0);
                    if(pos_x >= SCREEN_W/2-(iniciar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(iniciar.width*0.9)/2)+iniciar.width*0.9 && pos_y >= SCREEN_H/2+(iniciar.height*0.9)/2-100 && pos_y <= (SCREEN_H/2+(iniciar.height*0.9)/2-100)+iniciar.height*0.9){
                        al_draw_scaled_bitmap(iniciar.imgs[1],0,0,iniciar.width,iniciar.height,SCREEN_W/2-(iniciar.width)/2,SCREEN_H/2+(iniciar.height)/2-100,iniciar.width, iniciar.height, 0);
                        if(button_flag != 1){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 1;
                        }
                    }else{
                        al_draw_scaled_bitmap(iniciar.imgs[0],0,0,iniciar.width,iniciar.height,SCREEN_W/2-(iniciar.width*0.9)/2,SCREEN_H/2+(iniciar.height*0.9)/2-100,iniciar.width*0.9, iniciar.height*0.9, 0);
                        if(button_flag == 1){
                            button_flag = 0;
                        }
                    }

                    if(pos_x >= SCREEN_W/2-(story.width*0.9)/2 && pos_x <= (SCREEN_W/2-(story.width*0.9)/2)+story.width*0.9 && pos_y >= SCREEN_H/2+(story.height*0.9)/2+20 && pos_y <= (SCREEN_H/2+(story.height*0.9)/2+20)+story.height*0.9){
                        al_draw_scaled_bitmap(story.imgs[1],0,0,story.width,story.height,SCREEN_W/2-(story.width)/2,SCREEN_H/2+(story.height)/2+20,story.width, story.height, 0);
                        if(button_flag != 2){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 2;
                        }
                    }else{
                        al_draw_scaled_bitmap(story.imgs[0],0,0,story.width,story.height,SCREEN_W/2-(story.width*0.9)/2,SCREEN_H/2+(story.height*0.9)/2+20,story.width*0.9, story.height*0.9, 0);
                        if(button_flag == 2){
                            button_flag = 0;
                        }
                    }

                    if(pos_x >= SCREEN_W/2-(rankingButton.width*0.9)/2 && pos_x <= (SCREEN_W/2-(rankingButton.width*0.9)/2)+rankingButton.width*0.9 && pos_y >= SCREEN_H/2+(rankingButton.height*0.9)/2+140 && pos_y <= (SCREEN_H/2+(rankingButton.height*0.9)/2+140)+rankingButton.height*0.9){
                        al_draw_scaled_bitmap(rankingButton.imgs[1],0,0,rankingButton.width,rankingButton.height,SCREEN_W/2-(rankingButton.width)/2,SCREEN_H/2+(rankingButton.height)/2+140,rankingButton.width, rankingButton.height, 0);
                        if(button_flag != 3){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 3;
                        }
                    }else{
                        al_draw_scaled_bitmap(rankingButton.imgs[0],0,0,rankingButton.width,rankingButton.height,SCREEN_W/2-(rankingButton.width*0.9)/2,SCREEN_H/2+(rankingButton.height*0.9)/2+140,rankingButton.width*0.9, rankingButton.height*0.9, 0);
                        if(button_flag == 3){
                            button_flag = 0;
                        }
                    }

                    if(pos_x >= SCREEN_W/2-(sair.width*0.9)/2 && pos_x <= (SCREEN_W/2-(sair.width*0.9)/2)+sair.width*0.9 && pos_y >= SCREEN_H/2+(sair.height*0.9)/2+260 && pos_y <= (SCREEN_H/2+(sair.height*0.9)/2+260)+sair.height*0.9){
                        al_draw_scaled_bitmap(sair.imgs[1],0,0,sair.width,sair.height,SCREEN_W/2-(sair.width)/2,SCREEN_H/2+(sair.height)/2+260,sair.width, sair.height, 0);
                        if(button_flag != 4){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 4;
                        }
                    }else{
                        al_draw_scaled_bitmap(sair.imgs[0],0,0,sair.width,sair.height,SCREEN_W/2-(sair.width*0.9)/2,SCREEN_H/2+(sair.height*0.9)/2+260,sair.width*0.9, sair.height*0.9, 0);
                        if(button_flag == 4){
                            button_flag = 0;
                        }
                    }

                    if(pos_x >= 20 && pos_x <= 20+creditosButton.width && pos_y >= SCREEN_H-creditosButton.height-20 && pos_y <= SCREEN_H-20){
                        al_draw_scaled_bitmap(creditosButton.imgs[1],0,0,creditosButton.width,creditosButton.height, 20 , SCREEN_H-creditosButton.height-20 , creditosButton.width , creditosButton.height, 0);
                    }else{
                        al_draw_scaled_bitmap(creditosButton.imgs[0],0,0,creditosButton.width,creditosButton.height, 20 , SCREEN_H-creditosButton.height-20 , creditosButton.width , creditosButton.height, 0);
                    }

                }else{

                    if(show_ranking){

                        score = fopen("score.txt", "r");
                        fscanf(score, "%i %i %i %i %i" , &ranking[0], &ranking[1], &ranking[2], &ranking[3], &ranking[4]);
                        fclose(score);

                        al_draw_scaled_bitmap(ranking_tela.img,0,0,ranking_tela.width,ranking_tela.height, SCREEN_W/2-(ranking_tela.width)/2, SCREEN_H/2-(ranking_tela.height)/2 , ranking_tela.width*1, ranking_tela.height*1, 0);
                        al_draw_textf(font , al_map_rgb(255 , 255 , 255) , SCREEN_W/2 , SCREEN_H/2-80 , ALLEGRO_ALIGN_CENTRE   , "%d" , ranking[4] );
                        al_draw_textf(font , al_map_rgb(255 , 255 , 255) , SCREEN_W/2 , SCREEN_H/2-10 , ALLEGRO_ALIGN_CENTRE   , "%d" , ranking[3] );
                        al_draw_textf(font , al_map_rgb(255 , 255 , 255) , SCREEN_W/2 , SCREEN_H/2+60 , ALLEGRO_ALIGN_CENTRE   , "%d" , ranking[2] );
                        al_draw_textf(font , al_map_rgb(255 , 255 , 255) , SCREEN_W/2 , SCREEN_H/2+130 , ALLEGRO_ALIGN_CENTRE   , "%d" , ranking[1] );
                        al_draw_textf(font , al_map_rgb(255 , 255 , 255) , SCREEN_W/2 , SCREEN_H/2+200 , ALLEGRO_ALIGN_CENTRE   , "%d" , ranking[0] );

                        if(pos_x >= (SCREEN_W/2+(ranking_tela.width)/2)-(close_X.width)-10 && pos_x <= (SCREEN_W/2+(ranking_tela.width)/2)-(close_X.width)+close_X.width-10 && pos_y >= (SCREEN_H/2-(ranking_tela.height)/2)+10 && pos_y <= (SCREEN_H/2-(ranking_tela.height)/2)+close_X.height+10){
                            al_draw_scaled_bitmap(close_X.imgs[1] ,0,0, close_X.width, close_X.height,  (SCREEN_W/2+(ranking_tela.width)/2)-(close_X.width)-10  ,  (SCREEN_H/2-(ranking_tela.height)/2)+10 , close_X.width, close_X.height, 0);
                            if(button_flag != 5){
                                button_flag = 5;
                            }
                        }else{
                            al_draw_scaled_bitmap(close_X.imgs[0] ,0,0, close_X.width, close_X.height,  (SCREEN_W/2+(ranking_tela.width)/2)-(close_X.width)-10  ,  (SCREEN_H/2-(ranking_tela.height)/2)+10 , close_X.width, close_X.height, 0);
                            if(button_flag == 5){
                                button_flag = 0;
                            }
                        }
                    }else if(show_credits){
                        al_draw_scaled_bitmap(creditos.img,0,0,creditos.width,creditos.height, SCREEN_W/2-(creditos.width)/2, SCREEN_H/2-(creditos.height)/2 , creditos.width*1, creditos.height*1, 0);
                        if(pos_x >= (SCREEN_W/2+(creditos.width)/2)-(close_X.width)-10 && pos_x <= (SCREEN_W/2+(creditos.width)/2)-(close_X.width)+close_X.width-10 && pos_y >= (SCREEN_H/2-(creditos.height)/2)+10 && pos_y <= (SCREEN_H/2-(creditos.height)/2)+close_X.height+10){
                            al_draw_scaled_bitmap(close_X.imgs[1] ,0,0, close_X.width, close_X.height,  (SCREEN_W/2+(creditos.width)/2)-(close_X.width)-10  ,  (SCREEN_H/2-(creditos.height)/2)+10 , close_X.width, close_X.height, 0);
                            if(button_flag != 5){
                                button_flag = 5;
                            }
                        }else{
                            al_draw_scaled_bitmap(close_X.imgs[0] ,0,0, close_X.width, close_X.height,  (SCREEN_W/2+(creditos.width)/2)-(close_X.width)-10  ,  (SCREEN_H/2-(creditos.height)/2)+10 , close_X.width, close_X.height, 0);
                            if(button_flag == 5){
                                button_flag = 0;
                            }
                        }
                    }

                }

                if(play_intro){
                    al_draw_scaled_bitmap(video_intro.imgs[29],0,0,video_intro.width,video_intro.height,0,0,SCREEN_W,SCREEN_H, 0);
                }

                al_flip_display();
            }
        }

        show_Menu = 1;
        button_flag = 0;
        al_stop_sample_instance(inst_menu_music);

        if(fechar){
            break;
        }





















        al_stop_timer(timer);
        if(play_intro){
            al_draw_scaled_bitmap(video_intro.imgs[29],0,0,video_intro.width,video_intro.height,0,0,SCREEN_W,SCREEN_H, 0);
            al_flip_display();
            show_intro(video_intro, inst_intro, SCREEN_W, SCREEN_H);
        }
        pontuacao = 0;
        int fadeIn = 1;
        float jogo_acabou = 0.98;
        reset_players(key, &player1, &player2, ship.width, ship.height);
        reset_ship(&ship, &gunLEFT, &gunRIGHT, &gunUP, &gunDOWN, QUANT_SHOTS, shots, &vidascale, &oxigenioscale, &oxigenioscale2, oxigenio2.width);
        adaptarCamera(&cameraX, &cameraY, ship.x, ship.y, ship.width, ship.height, SCREEN_W, SCREEN_H);
        reset_asteroids(QUANT_ASTEROIDS, asteroids, &asteroid_explode, &explosion.cur_Frame);
        reset_especial_asteroids(QUANT_ESPECIAL_ASTEROIDS, especiais, QUANT_OXI_BALLS, oxigen_balls);
        al_play_sample_instance(inst_theme);
        al_start_timer(timer);
        int pausado = 0;



        while(jogo_acabou < 1){

            ALLEGRO_EVENT ev;
            al_wait_for_event(event_queue, &ev);

            //PAUSA----------------------------------------------------------------------------------------------------------------------
            if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(ev.mouse.button & 1){
                    if(pos_x >= SCREEN_W-(pauseButton.width)-20 && pos_x <= (SCREEN_W)-20 && pos_y >= 20 && pos_y <= (pauseButton.height)+20 && jogo_acabou == 0){
                        if(pausado){

                        }else{
                            pausado = 1;
                            al_play_sample_instance(inst_button);
                            al_stop_timer(timer);
                        }
                    }
                    if(pausado){

                        if(pos_x >= SCREEN_W/2-(iniciar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(iniciar.width*0.9)/2)+iniciar.width*0.9 && pos_y >= SCREEN_H/2+(iniciar.height*0.9)/2-100 && pos_y <= (SCREEN_H/2+(iniciar.height*0.9)/2-100)+iniciar.height*0.9){
                            al_play_sample_instance(inst_button);
                            pausado = 0;
                            al_start_timer(timer);
                        }else if(pos_x >= SCREEN_W/2-(sair.width*0.9)/2 && pos_x <= (SCREEN_W/2-(sair.width*0.9)/2)+sair.width*0.9 && pos_y >= SCREEN_H/2+(sair.height*0.9)/2+260 && pos_y <= (SCREEN_H/2+(sair.height*0.9)/2+260)+sair.height*0.9){
                            al_play_sample_instance(inst_button);
                            fechar = 1;
                            break;
                        }else if(pos_x >= SCREEN_W/2-(reiniciar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(reiniciar.width*0.9)/2)+reiniciar.width*0.9 && pos_y >= SCREEN_H/2+(reiniciar.height*0.9)/2+20 && pos_y <= (SCREEN_H/2+(reiniciar.height*0.9)/2+20)+reiniciar.height*0.9){
                            al_play_sample_instance(inst_button);
                            al_start_timer(timer);
                            show_Menu_Death = 0;
                            show_Menu = 0;
                            break;
                        }else if(pos_x >= SCREEN_W/2-(menuButton.width*0.9)/2 && pos_x <= (SCREEN_W/2-(menuButton.width*0.9)/2)+menuButton.width*0.9 && pos_y >= SCREEN_H/2+(menuButton.height*0.9)/2+140 && pos_y <= (SCREEN_H/2+(menuButton.height*0.9)/2+140)+menuButton.height*0.9){
                            al_play_sample_instance(inst_button);
                            al_start_timer(timer);
                            show_Menu_Death = 0;
                            break;
                        }

                    }
                }
            }else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
                pos_x = ev.mouse.x;
                pos_y = ev.mouse.y;
            }

            if(pausado){
                redraw = true;
            }
            //PAUSA-----------------------------------------------------------------------------------------------------------------------


            //EVENTOS REAIS (TEMPO)
            if(ev.type == ALLEGRO_EVENT_TIMER) {

                if(fadeIn){
                    jogo_acabou -= 0.005;
                    if(jogo_acabou <= 0){
                        jogo_acabou = 0;
                        fadeIn = 0;
                    }
                }

                //EVENTOS RELACIONADOS A VIDA
                if(oxigenioscale == oxigenio.width){
                    vidascale += 0.5;
                    al_play_sample_instance(inst_alerta);
                }

                //EVENTOS RELACIONADOS AO OXIGENIO
                if(oxigenioscale < oxigenio.width){
                    oxigenioscale += 0.1;
                }else{
                    oxigenioscale = oxigenio.width;
                }

                if(vidascale >= vida.width){
                    vidascale = vida.width;
                    ship.controle = 0;
                    jogo_acabou += 0.005;
                }else{

                    //NAVE / OXIGENIO / CANHOES / PLAYER 1
                    if(gunLEFT.controle == 1){
                        player1.cur_Frame = 7*DELAY;
                        controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunLEFT.angle, 90);
                        int atirou = atirar(&shots, key[KEY_UP], &gunLEFT.carga, ship.x-6, ship.y+ship.height/2-10, gunLEFT.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p1 = 1;
                        }
                    }else if(gunRIGHT.controle == 1){
                        player1.cur_Frame = 0;
                        controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunRIGHT.angle, -90);
                        int atirou = atirar(&shots, key[KEY_UP], &gunRIGHT.carga, ship.x+ship.width+6, ship.y+ship.height/2-10, gunRIGHT.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p1 = 1;
                        }
                    }else if(gunUP.controle == 1){
                        player1.cur_Frame = 15*DELAY;
                        controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunUP.angle, -180);
                        int atirou = atirar(&shots, key[KEY_UP], &gunUP.carga, ship.x+ship.width/2-6, ship.y-10, gunUP.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p1 = 1;
                        }
                    }else if(gunDOWN.controle == 1){
                        player1.cur_Frame = 19*DELAY;
                        controlarCanhao(key[KEY_LEFT], key[KEY_RIGHT], &gunDOWN.angle, 0);
                        int atirou = atirar(&shots, key[KEY_UP], &gunDOWN.carga, ship.x+ship.width/2-6, ship.y+ship.height-5, gunDOWN.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p1 = 1;
                        }
                    }else if(ship.repondoOxi == 1){
                        if(oxigenioscale <= oxigenio.width && oxigenioscale > 0 && oxigenioscale2 < oxigenio2.width){
                            oxigenioscale -= 1;
                            oxigenioscale2 += 0.3;
                        }else{
                            //oxigenioscale = 0;
                            ship.repondoOxi = 0;
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
                        int atirou = atirar(&shots, key[KEY_W], &gunLEFT.carga, ship.x-6, ship.y+ship.height/2-10, gunLEFT.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p2 = 1;
                        }
                    }else if(gunRIGHT.controle == 2){
                        player2.cur_Frame = 0;
                        controlarCanhao(key[KEY_A], key[KEY_D], &gunRIGHT.angle, -90);
                        int atirou = atirar(&shots, key[KEY_W], &gunRIGHT.carga, ship.x+ship.width+6, ship.y+ship.height/2-10, gunRIGHT.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p2 = 1;
                        }
                    }else if(gunUP.controle == 2){
                        player2.cur_Frame = 15*DELAY;
                        controlarCanhao(key[KEY_A], key[KEY_D], &gunUP.angle, -180);
                        int atirou = atirar(&shots, key[KEY_W], &gunUP.carga, ship.x+ship.width/2-6, ship.y-10, gunUP.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p2 = 1;
                        }
                    }else if(gunDOWN.controle == 2){
                        player2.cur_Frame = 19*DELAY;
                        controlarCanhao(key[KEY_A], key[KEY_D], &gunDOWN.angle, 0);
                        int atirou = atirar(&shots, key[KEY_W], &gunDOWN.carga, ship.x+ship.width/2-6, ship.y+ship.height-5, gunDOWN.angle-90, QUANT_SHOTS);
                        if(atirou){
                            al_stop_sample_instance(inst_tiro);
                            al_play_sample_instance(inst_tiro);
                            flash_p2 = 1;
                        }
                    }else if(ship.repondoOxi == 2){
                        if(oxigenioscale <= oxigenio.width && oxigenioscale > 0 && oxigenioscale2 < oxigenio2.width){
                            oxigenioscale -= 1;
                            oxigenioscale2 += 0.3;
                        }else{
                            //oxigenioscale = 0;
                            ship.repondoOxi = 0;
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
                    float distancia = verificar_Colisao_SHIP_ASTEROID(&ship, &asteroids[i]);
                    if(distancia == 0){
                        if(asteroids[i].vida > 0){
                            realizar_Colisao_SHIP_ASTEROID(&ship, &asteroids[i]);
                            vidascale+=1;
                            al_stop_sample_instance(inst_crash);
                            al_play_sample_instance(inst_crash);
                        }
                    }else if(distancia >= 2000){
                        asteroids[i].vida = 0;
                    }
                }

                //ESPECIAIS-----------------------------------------------------------------------------------------
                for(int i=0; i<QUANT_ESPECIAL_ASTEROIDS; i++){
                    float distancia = 0;
                    if(especiais[i].vida > 0){
                        distancia = verificar_Colisao_SHIP_ASTEROID(&ship, &especiais[i]);
                        if(distancia == 0){
                            especiais[i].vida = 0;
                            vidascale+=0.5;
                            al_play_sample_instance(inst_explosao1);
                        }
                    }else{
                        add_new_especial_asteroid(&especiais[i], cameraX, cameraY, SCREEN_W, SCREEN_H);
                        especiais[i].vida = 1;
                    }
                    if((especiais[i].x > 14400 || especiais[i].x < 1850 || especiais[i].y > 15000 || especiais[i].y < 1400) && distancia >= 2000){
                        especiais[i].vida = 0;
                    }
                }

                for(int i=0; i<QUANT_OXI_BALLS; i++){
                    if(oxigen_balls[i].vida > 0){
                        float distancia = verificar_Colisao_SHIP_ASTEROID(&ship, &oxigen_balls[i]);
                        if(distancia == 0){

                            pontuacao += 25;
                            oxigen_balls[i].vida = 0;

                            al_play_sample_instance(inst_som_oxigenio);

                            oxigenioscale2 -= 24;
                            if(oxigenioscale2 <= 0){
                                oxigenioscale2 = 0;
                            }

                        }
                    }
                }
                //--------------------------------------------------------------------------------------------------

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
                if(especial_explode){
                    explosion_especial.cur_Frame += 1;
                    if(explosion_especial.cur_Frame >= 9*5){
                        explosion_especial.cur_Frame = 0;
                        especial_explode = 0;
                    }
                }


                //FLASHES DO TIRO-------------------------------------------------------------------------------------------------------------------------------------
                if(flash_p1){
                    flash.cur_Frame += 1;
                    if(flash.cur_Frame >= 2*2){
                        flash.cur_Frame = 0;
                        flash_p1 = 0;
                    }
                }

                if(flash_p2){
                    flash2.cur_Frame += 1;
                    if(flash2.cur_Frame >= 2*2){
                        flash2.cur_Frame = 0;
                        flash_p2 = 0;
                    }
                }
                //FLASHES DO TIRO-------------------------------------------------------------------------------------------------------------------------------------


                //ESPECIAIS-----------------------------------------------------------------------------------------
                for(int i=0; i<QUANT_SHOTS; i++){
                    for(int x=0; x<QUANT_ESPECIAL_ASTEROIDS; x++){
                        if(shots[i].ativo != 0){
                            if(especiais[x].vida > 0){
                                int colidiu = verificar_Colisao_SHOT_ASTEROID(&shots[i], &especiais[x]);
                                if(colidiu){

                                    pontuacao += 150;

                                    xE_E = especiais[x].x;
                                    yE_E = especiais[x].y;

                                    shots[i].ativo = 0;
                                    especiais[x].vida = 0;
                                    especial_explode = 1;

                                    xF = ((shots[i].x+shots[i].width/2)+(especiais[x].x+especiais[x].width/4))/2;
                                    yF = ((shots[i].y+shots[i].height/2)+(especiais[x].y+especiais[x].height/4))/2;
                                    collide_shot_ast = 1;
                                    al_play_sample_instance(inst_explosao1);

                                    int created = 0;
                                    int quant_balls = (rand()%3) + 1;

                                    for(int i=0; i<QUANT_OXI_BALLS; i++){
                                        if(oxigen_balls[i].vida == 0){
                                            throw_oxi_ball(&oxigen_balls[i], especiais[x].x, especiais[x].y);
                                            created += 1;
                                        }
                                        if(created == quant_balls){
                                            break;
                                        }
                                    }

                                }
                            }
                        }else{
                            break;
                        }
                    }
                }
                //--------------------------------------------------------------------------------------------------

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

                                        pontuacao += 50;

                                        asteroid_explode = 1;
                                        xE = asteroids[x].x;
                                        yE = asteroids[x].y;

                                        int tempo = rand()%3;
                                        if(tempo == 0){
                                            al_stop_sample_instance(inst_explosao1);
                                            al_play_sample_instance(inst_explosao1);
                                        }
                                        if(tempo == 1){
                                            al_stop_sample_instance(inst_explosao2);
                                            al_play_sample_instance(inst_explosao2);
                                        }
                                        if(tempo == 2){
                                            al_stop_sample_instance(inst_explosao3);
                                            al_play_sample_instance(inst_explosao3);
                                        }

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

                if(ship.x >= 14400 || ship.x <= 1850 || ship.y >= 14400 || ship.y <= 1400){
                    vidascale += 0.5;
                    frearObjetos(&ship.forceX, &ship.forceY, 0.005);
                }

                aplicarForcas(&ship.x, &ship.y, ship.forceX, ship.forceY);

                for(int i=0; i<QUANT_ESPECIAL_ASTEROIDS; i++){
                    if(especiais[i].vida > 0){
                        limitarAsteroideMundo(&especiais[i].x, &especiais[i].y, &especiais[i].forceX, &especiais[i].forceY, especiais[i].width, especiais[i].height);
                        aplicarForcas(&especiais[i].x, &especiais[i].y, especiais[i].forceX, especiais[i].forceY);
                    }
                }

                for(int i=0; i<QUANT_ASTEROIDS; i++){
                    if(asteroids[i].vida > 0){
                        limitarAsteroideMundo(&asteroids[i].x, &asteroids[i].y, &asteroids[i].forceX, &asteroids[i].forceY, asteroids[i].width, asteroids[i].height);
                        aplicarForcas(&asteroids[i].x, &asteroids[i].y, asteroids[i].forceX, asteroids[i].forceY);
                    }
                }

                for(int i=0; i<QUANT_OXI_BALLS; i++){
                    if(oxigen_balls[i].vida > 0){
                        limitarAsteroideMundo(&oxigen_balls[i].x, &oxigen_balls[i].y, &oxigen_balls[i].forceX, &oxigen_balls[i].forceY, oxigen_balls[i].width, oxigen_balls[i].height);
                        aplicarForcas(&oxigen_balls[i].x, &oxigen_balls[i].y, oxigen_balls[i].forceX, oxigen_balls[i].forceY);
                        frearObjetos(&oxigen_balls[i].forceX, &oxigen_balls[i].forceY, 0.006);
                        oxigen_balls[i].vida -= 1;
                    }
                }

                adaptarCamera(&cameraX, &cameraY, ship.x, ship.y, ship.width, ship.height, SCREEN_W, SCREEN_H);
                redraw = true;

            }else if(ev.type == ALLEGRO_EVENT_KEY_DOWN && pausado != 1) {
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
                        interagirSHIP(player1, 1, ship.width, ship.height, &ship.controle , inst_som_propulsor , inst_rotacao_h , inst_rotacao_a);
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
                        interagirSHIP(player2, 2, ship.width, ship.height, &ship.controle , inst_som_propulsor, inst_rotacao_h , inst_rotacao_a);
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

            if(redraw && al_is_event_queue_empty(event_queue) && jogo_acabou < 1) {

                redraw = false;
                al_draw_scaled_bitmap(background.img,0,0,background.width,background.height,0-(cameraX/2.5),0-(cameraY/2.5),background.width*BGScale, background.height*BGScale, 0);

                al_draw_scaled_bitmap(beltside.img,0,0,beltside.width,beltside.height,1300-(cameraX/1.5),0-(cameraY/1.5),beltside.width*beltScale, beltside.height*beltScale, 0);//LEFT
                al_draw_scaled_bitmap(beltside.img,0,0,beltside.width,beltside.height,(WORLD_W-7500)-(cameraX/1.5),0-(cameraY/1.5),beltside.width*beltScale, beltside.height*beltScale, 0);//RIGTH
                al_draw_scaled_bitmap(belttop.img,0,0,belttop.width,belttop.height,1300-(cameraX/1.5),900-(cameraY/1.5),belttop.width*beltScale, belttop.height*beltScale, 0);//UP
                al_draw_scaled_bitmap(belttop.img,0,0,belttop.width,belttop.height,1300-(cameraX/1.5),(WORLD_H-7000)-(cameraY/1.5),belttop.width*beltScale, belttop.height*beltScale, 0);//DOWN

                al_draw_scaled_bitmap(planeta.img,0,0,planeta.width,planeta.height,(WORLD_W/2)-(cameraX/2.2)-4000,(WORLD_H/2)-(cameraY/2.2)-4000,planeta.width*3, planeta.height*3, 0);

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


                //FLASHES DO TIRO-------------------------------------------------------------------------------------------------------------------------------------
                if(flash_p1){
                    if(player1.x-100 < 0){
                        al_draw_rotated_bitmap(flash.imgs[flash.cur_Frame/2],12, flash.height/2+50, ship.x-cameraX-6, ship.y-cameraY+ship.height/2-3, ((gunLEFT.angle+90)*3.14159/180), 0);
                    }
                    if(player1.x+100 > ship.width){
                        al_draw_rotated_bitmap(flash.imgs[flash.cur_Frame/2],12, flash.height/2+50, ship.x-cameraX+ship.width+10, ship.y-cameraY+ship.height/2-4 , ((gunRIGHT.angle+90)*3.14159/180), 0);
                    }
                    if(player1.y-100 < 0){
                        al_draw_rotated_bitmap(flash.imgs[flash.cur_Frame/2],12, flash.height/2+50, ship.x-cameraX+ship.width/2+2, ship.y-cameraY-6 , ((gunUP.angle+90)*3.14159/180), 0);
                    }
                    if(player1.y+100 > ship.height){
                        al_draw_rotated_bitmap(flash.imgs[flash.cur_Frame/2],12, flash.height/2+50, ship.x-cameraX+ship.width/2+2, ship.y-cameraY+ship.height+7 , ((gunDOWN.angle+90)*3.14159/180), 0);
                    }
                }

                if(flash_p2){
                    if(player2.x-100 < 0){
                        al_draw_rotated_bitmap(flash2.imgs[flash2.cur_Frame/2],12, flash2.height/2+50, ship.x-cameraX-6, ship.y-cameraY+ship.height/2-3, ((gunLEFT.angle+90)*3.14159/180), 0);
                    }
                    if(player2.x+100 > ship.width){
                        al_draw_rotated_bitmap(flash2.imgs[flash2.cur_Frame/2],12, flash2.height/2+50, ship.x-cameraX+ship.width+10, ship.y-cameraY+ship.height/2-4 , ((gunRIGHT.angle+90)*3.14159/180), 0);
                    }
                    if(player2.y-100 < 0){
                        al_draw_rotated_bitmap(flash2.imgs[flash2.cur_Frame/2],12, flash2.height/2+50, ship.x-cameraX+ship.width/2+2, ship.y-cameraY-6 , ((gunUP.angle+90)*3.14159/180), 0);
                    }
                    if(player2.y+100 > ship.height){
                        al_draw_rotated_bitmap(flash2.imgs[flash2.cur_Frame/2],12, flash2.height/2+50, ship.x-cameraX+ship.width/2+2, ship.y-cameraY+ship.height+7 , ((gunDOWN.angle+90)*3.14159/180), 0);
                    }
                }
                //FLASHES DO TIRO-------------------------------------------------------------------------------------------------------------------------------------


                if(player1.y >= player2.y){
                    al_draw_scaled_bitmap(player2.imgs[player2.cur_Frame/DELAY],0,0,player2.width,player2.height,player2.x+ship.x-cameraX,player2.y+ship.y-cameraY,player2.width, player2.height, 0);
                    al_draw_scaled_bitmap(player1.imgs[player1.cur_Frame/DELAY],0,0,player1.width,player1.height,player1.x+ship.x-cameraX,player1.y+ship.y-cameraY,player1.width, player1.height, 0);
                }else{
                    al_draw_scaled_bitmap(player1.imgs[player1.cur_Frame/DELAY],0,0,player1.width,player1.height,player1.x+ship.x-cameraX,player1.y+ship.y-cameraY,player1.width, player1.height, 0);
                    al_draw_scaled_bitmap(player2.imgs[player2.cur_Frame/DELAY],0,0,player2.width,player2.height,player2.x+ship.x-cameraX,player2.y+ship.y-cameraY,player2.width, player2.height, 0);
                }

                al_draw_scaled_bitmap(objetos_nave.img,0,0,objetos_nave.width,objetos_nave.height, ship.x-cameraX, ship.y-cameraY, objetos_nave.width, objetos_nave.height, 0);

                al_draw_scaled_bitmap(dangerzoneSide.img,0,0,dangerzoneSide.width,dangerzoneSide.height, 0-(cameraX/1.5)-600 , 0-(cameraY/1.5) ,dangerzoneSide.width*beltScale, dangerzoneSide.height*beltScale, 0);//LEFT
                al_draw_scaled_bitmap(dangerzoneSide.img,0,0,dangerzoneSide.width,dangerzoneSide.height, (WORLD_W-6200)-(cameraX/1.5) , 0-(cameraY/1.5) ,dangerzoneSide.width*beltScale, dangerzoneSide.height*beltScale, 0);//RIGHT
                al_draw_scaled_bitmap(dangerzoneTop.img,0,0,dangerzoneTop.width,dangerzoneTop.height, 0-(cameraX/1.5)-600 , 0-(cameraY/1.5)-1200 ,dangerzoneTop.width*beltScale, dangerzoneTop.height*beltScale, 0);//UP
                al_draw_scaled_bitmap(dangerzoneTop.img,0,0,dangerzoneTop.width,dangerzoneTop.height, 0-(cameraX/1.5)-600 , (WORLD_H-6200)-(cameraY/1.5) ,dangerzoneTop.width*beltScale, dangerzoneTop.height*beltScale, 0);//DOWN

                for(int i=0; i<QUANT_ASTEROIDS; i++){
                    if(asteroids[i].vida > 0){
                        al_draw_scaled_bitmap(asteroids[i].img,0,0,asteroids[i].width,asteroids[i].height,asteroids[i].x-cameraX,asteroids[i].y-cameraY,asteroids[i].width, asteroids[i].height, 0);
                    }
                }
                for(int i=0; i<QUANT_ESPECIAL_ASTEROIDS; i++){
                    if(especiais[i].vida > 0){
                        al_draw_scaled_bitmap(especiais[i].img,0,0,especiais[i].width,especiais[i].height,especiais[i].x-cameraX,especiais[i].y-cameraY,especiais[i].width, especiais[i].height, 0);
                    }
                }
                for(int i=0; i<QUANT_OXI_BALLS; i++){
                    if(oxigen_balls[i].vida > 0){
                        if(oxigen_balls[i].vida > 200 || (oxigen_balls[i].vida/10)%2 == 0 ){
                            al_draw_scaled_bitmap(oxigen_balls[i].img,0,0,oxigen_balls[i].width,oxigen_balls[i].height,oxigen_balls[i].x-cameraX,oxigen_balls[i].y-cameraY,oxigen_balls[i].width, oxigen_balls[i].height, 0);
                        }
                    }
                }

                if(collide_shot_ast){
                    al_draw_scaled_bitmap(faisca.imgs[faisca.cur_Frame/2],0,0,faisca.width,faisca.height,xF-cameraX,yF-cameraY,faisca.width, faisca.height, 0);
                }
                if(asteroid_explode){
                    al_draw_scaled_bitmap(explosion.imgs[explosion.cur_Frame/5],0,0,explosion.width,explosion.height,xE-cameraX,yE-cameraY,explosion.width*2.5, explosion.height*2.5, 0);
                }
                if(especial_explode){
                    al_draw_scaled_bitmap(explosion_especial.imgs[explosion_especial.cur_Frame/5],0,0,explosion_especial.width,explosion_especial.height,xE_E-cameraX-50,yE_E-cameraY-50,explosion_especial.width*0.5, explosion_especial.height*0.5, 0);
                }

                for(int i=0; i<QUANT_SHOTS; i++){
                    if(shots[i].ativo != 0){
                        al_draw_scaled_bitmap(shots[i].img,0,0, shots[i].width, shots[i].height, shots[i].x-cameraX, shots[i].y-cameraY, shots[i].width*0.7, shots[i].height*0.7, 0);
                    }
                }

                //al_draw_text(font, al_map_rgb(255, 255, 255), 80, 30, ALLEGRO_ALIGN_CENTRE, "SCORE:");
                al_draw_textf(font , al_map_rgb(255 , 255 , 255) , SCREEN_W/2 , 20 , ALLEGRO_ALIGN_CENTRE   , "%d" , pontuacao );

                al_draw_scaled_bitmap(vida.img,0,0,vida.width,vida.height,(SCREEN_W/2)-(vida.width/2)-15,SCREEN_H-43,vida.width-vidascale,vida.height, 0);
                al_draw_scaled_bitmap(oxigenio.img,0,0,oxigenio.width,oxigenio.height,SCREEN_W/2-oxigenio.width/2-15,SCREEN_H-36,oxigenio.width-oxigenioscale,oxigenio.height, 0);
                al_draw_scaled_bitmap(barra.img,0,0,barra.width,barra.height,SCREEN_W/2-barra.width/2,SCREEN_H-100,barra.width,barra.height, 0);
                al_draw_scaled_bitmap(oxigenio2.img,0,0,oxigenio2.width,oxigenio2.height, SCREEN_W/2-oxigenio2.width/2+335 , SCREEN_H-44 ,oxigenio2.width-oxigenioscale2, oxigenio2.height, 0);
                al_draw_scaled_bitmap(barra2.img,0,0,barra2.width,barra2.height, SCREEN_W/2-barra2.width/2+340, SCREEN_H-85 ,barra2.width,barra2.height, 0);


                //PAUSADO---------------------------------------------------------------------------------------------------------------------
                if(pausado){

                    al_draw_scaled_bitmap(tela_pausa.img,0,0,tela_pausa.width,tela_pausa.height, SCREEN_W/2-(tela_pausa.width)/2, SCREEN_H/2-(tela_pausa.height)/2 , tela_pausa.width*1, tela_pausa.height*1, 0);
                    al_draw_scaled_bitmap(logo.img,0,0,logo.width,logo.height, SCREEN_W/2-logo.width*0.38/2 , (SCREEN_H/2-(tela_pausa.height)/2)+40 , logo.width*0.38 , logo.height*0.38, 0);

                    if(pos_x >= SCREEN_W/2-(continuar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(continuar.width*0.9)/2)+continuar.width*0.9 && pos_y >= SCREEN_H/2+(continuar.height*0.9)/2-100 && pos_y <= (SCREEN_H/2+(continuar.height*0.9)/2-100)+continuar.height*0.9){
                        al_draw_scaled_bitmap(continuar.imgs[1],0,0,continuar.width,continuar.height,SCREEN_W/2-(continuar.width)/2,SCREEN_H/2+(continuar.height)/2-100,continuar.width, continuar.height, 0);
                        if(button_flag != 1){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 1;
                        }
                    }else{
                        al_draw_scaled_bitmap(continuar.imgs[0],0,0,continuar.width,continuar.height,SCREEN_W/2-(continuar.width*0.9)/2,SCREEN_H/2+(continuar.height*0.9)/2-100,continuar.width*0.9, continuar.height*0.9, 0);
                        if(button_flag == 1){
                            button_flag = 0;
                        }
                    }

                    if(pos_x >= SCREEN_W/2-(reiniciar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(reiniciar.width*0.9)/2)+reiniciar.width*0.9 && pos_y >= SCREEN_H/2+(reiniciar.height*0.9)/2+20 && pos_y <= (SCREEN_H/2+(reiniciar.height*0.9)/2+20)+reiniciar.height*0.9){
                        al_draw_scaled_bitmap(reiniciar.imgs[1],0,0,reiniciar.width,reiniciar.height,SCREEN_W/2-(reiniciar.width)/2,SCREEN_H/2+(reiniciar.height)/2+20,reiniciar.width, reiniciar.height, 0);
                        if(button_flag != 2){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 2;
                        }
                    }else{
                        al_draw_scaled_bitmap(reiniciar.imgs[0],0,0,reiniciar.width,reiniciar.height,SCREEN_W/2-(reiniciar.width*0.9)/2,SCREEN_H/2+(reiniciar.height*0.9)/2+20,reiniciar.width*0.9, reiniciar.height*0.9, 0);
                        if(button_flag == 2){
                            button_flag = 0;
                        }
                    }

                    if(pos_x >= SCREEN_W/2-(menuButton.width*0.9)/2 && pos_x <= (SCREEN_W/2-(menuButton.width*0.9)/2)+menuButton.width*0.9 && pos_y >= SCREEN_H/2+(menuButton.height*0.9)/2+140 && pos_y <= (SCREEN_H/2+(menuButton.height*0.9)/2+140)+menuButton.height*0.9){
                        al_draw_scaled_bitmap(menuButton.imgs[1],0,0,menuButton.width,menuButton.height,SCREEN_W/2-(menuButton.width)/2,SCREEN_H/2+(menuButton.height)/2+140,menuButton.width, menuButton.height, 0);
                        if(button_flag != 3){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 3;
                        }
                    }else{
                        al_draw_scaled_bitmap(menuButton.imgs[0],0,0,menuButton.width,menuButton.height,SCREEN_W/2-(menuButton.width*0.9)/2,SCREEN_H/2+(menuButton.height*0.9)/2+140,menuButton.width*0.9, menuButton.height*0.9, 0);
                        if(button_flag == 3){
                            button_flag = 0;
                        }
                    }

                    if(pos_x >= SCREEN_W/2-(sair.width*0.9)/2 && pos_x <= (SCREEN_W/2-(sair.width*0.9)/2)+sair.width*0.9 && pos_y >= SCREEN_H/2+(sair.height*0.9)/2+260 && pos_y <= (SCREEN_H/2+(sair.height*0.9)/2+260)+sair.height*0.9){
                        al_draw_scaled_bitmap(sair.imgs[1],0,0,sair.width,sair.height,SCREEN_W/2-(sair.width)/2,SCREEN_H/2+(sair.height)/2+260,sair.width, sair.height, 0);
                        if(button_flag != 4){
                            al_stop_sample_instance(inst_pass_button);
                            al_play_sample_instance(inst_pass_button);
                            button_flag = 4;
                        }
                    }else{
                        al_draw_scaled_bitmap(sair.imgs[0],0,0,sair.width,sair.height,SCREEN_W/2-(sair.width*0.9)/2,SCREEN_H/2+(sair.height*0.9)/2+260,sair.width*0.9, sair.height*0.9, 0);
                        if(button_flag == 4){
                            button_flag = 0;
                        }
                    }
                }else{
                    if(jogo_acabou == 0){
                        if(pos_x >= SCREEN_W-(pauseButton.width)-20 && pos_x <= (SCREEN_W)-20 && pos_y >= 20 && pos_y <= (pauseButton.height)+20){
                            al_draw_scaled_bitmap(pauseButton.imgs[1] ,0,0, pauseButton.width, pauseButton.height,  SCREEN_W-(pauseButton.width)-20 , 20 , pauseButton.width, pauseButton.height, 0);
                        }else{
                            al_draw_scaled_bitmap(pauseButton.imgs[0] ,0,0, pauseButton.width, pauseButton.height,  SCREEN_W-(pauseButton.width)-20 , 20 , pauseButton.width, pauseButton.height, 0);
                        }
                    }
                }
                //PAUSADO---------------------------------------------------------------------------------------------------------------------

                al_draw_tinted_bitmap(video_intro.imgs[29], al_map_rgba_f(1, 1, 1, jogo_acabou), 0, 0, 0);

                al_flip_display();

            }

        }//JOGO


        button_flag = 0;
        al_stop_sample_instance(inst_theme);
        al_stop_sample_instance(inst_tiro);
        al_stop_sample_instance(inst_explosao1);
        al_stop_sample_instance(inst_explosao2);
        al_stop_sample_instance(inst_explosao3);
        al_stop_sample_instance(inst_som_propulsor);
        al_stop_sample_instance(inst_rotacao_a);
        al_stop_sample_instance(inst_rotacao_h);
        al_stop_sample_instance(inst_alerta);


        if(show_Menu_Death){
            int new_ranking[6];
            for(int i=0; i<5; i++){
                new_ranking[i] = ranking[i];
            }
            new_ranking[5] = pontuacao;
            int index_Menor = 0;
            for(int i=0; i<6; i++){
                index_Menor = i;
                for(int x=i; x<6; x++){
                    if(new_ranking[index_Menor] > new_ranking[x]){
                        index_Menor = x;
                    }
                }
                switchNumbers(new_ranking, index_Menor, i);
            }
            score = fopen("score.txt", "w");
            fprintf(score, "%i %i %i %i %i" , new_ranking[1], new_ranking[2], new_ranking[3], new_ranking[4], new_ranking[5]);
            fclose(score);
        }













        if(fechar){
            break;
        }

        int subindo = 1; //MOVIMENTO DEADNAUT
        float y_astron = 0; //MOVIMENTO DEADNAUT
        al_play_sample_instance(inst_menu_death);

        //MENU GAME OVER
        while(show_Menu_Death){

            ALLEGRO_EVENT ev2;
            al_wait_for_event(event_queue, &ev2);
            int redrawMenu = 0;

            if(ev2.type == ALLEGRO_EVENT_TIMER) {
                if(subindo){
                    y_astron -= 0.2;
                    if(y_astron <= -20){
                        subindo = 0;
                    }
                }else{
                    y_astron += 0.2;
                    if(y_astron >= 0){
                        subindo = 1;
                    }
                }
                redrawMenu = 1;
            }else if(ev2.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(ev2.mouse.button & 1)
                {
                    if(pos_x >= SCREEN_W/2-(reiniciar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(reiniciar.width*0.9)/2)+reiniciar.width*0.9 && pos_y >= SCREEN_H/2+(reiniciar.height*0.9)/2 +20 && pos_y <= (SCREEN_H/2+(reiniciar.height*0.9)/2)+reiniciar.height*0.9 +20)
                    {
                        al_play_sample_instance(inst_button);
                        show_Menu = 0;
                        break;
                    }else if(pos_x >= SCREEN_W/2-(menuButton.width*0.9)/2 && pos_x <= (SCREEN_W/2-(menuButton.width*0.9)/2)+menuButton.width*0.9 && pos_y >= SCREEN_H/2+(menuButton.height*0.9)/2+ 140 && pos_y <= (SCREEN_H/2+(menuButton.height*0.9)/2+ 140)+menuButton.height*0.9){
                        al_play_sample_instance(inst_button);
                        break;
                    } else if(pos_x >= SCREEN_W/2-(sair.width*0.9)/2 && pos_x <= (SCREEN_W/2-(sair.width*0.9)/2)+sair.width*0.9 && pos_y >= SCREEN_H/2+(sair.height*0.9)/2+260 && pos_y <= (SCREEN_H/2+(sair.height*0.9)/2+260)+sair.height*0.9){
                        al_play_sample_instance(inst_button);
                        fechar = 1;
                        break;
                    }
                }
            }else if(ev2.type == ALLEGRO_EVENT_MOUSE_AXES){
                pos_x = ev2.mouse.x;
                pos_y = ev2.mouse.y;
            }else if(ev2.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                fechar = 1;
                break;
            }

            //DESENHA
            if(redrawMenu && al_is_event_queue_empty(event_queue)){
                redrawMenu = 0;
                al_draw_scaled_bitmap(black.img,0,0, black.width, black.height,0,0, SCREEN_W, SCREEN_H, 0);
                al_draw_scaled_bitmap(game_over.img,0,0,game_over.width,game_over.height,SCREEN_W/2-game_over.width*1.5/2, 100 ,game_over.width*1.5, game_over.height*1.5, 0);
                al_draw_scaled_bitmap(deadnaut.img,0,0,deadnaut.width,deadnaut.height, SCREEN_W/2 - deadnaut.width*2/2 , SCREEN_H/2 - deadnaut.height/2 - 100 + y_astron ,deadnaut.width*2, deadnaut.height*2, 0);

                if(pos_x >= SCREEN_W/2-(reiniciar.width*0.9)/2 && pos_x <= (SCREEN_W/2-(reiniciar.width*0.9)/2)+reiniciar.width*0.9 && pos_y >= SCREEN_H/2+(reiniciar.height*0.9)/2 +20 && pos_y <= (SCREEN_H/2+(reiniciar.height*0.9)/2)+reiniciar.height*0.9 +20){
                    al_draw_scaled_bitmap(reiniciar.imgs[1],0,0,reiniciar.width,reiniciar.height,SCREEN_W/2-(reiniciar.width)/2,SCREEN_H/2+(reiniciar.height)/2 +20,reiniciar.width, reiniciar.height, 0);
                    if(button_flag != 1){
                        al_stop_sample_instance(inst_pass_button);
                        al_play_sample_instance(inst_pass_button);
                        button_flag = 1;
                    }
                }else{
                    al_draw_scaled_bitmap(reiniciar.imgs[0],0,0,reiniciar.width,reiniciar.height,SCREEN_W/2-(reiniciar.width*0.9)/2,SCREEN_H/2+(reiniciar.height*0.9)/2 +20,reiniciar.width*0.9, reiniciar.height*0.9, 0);
                    if(button_flag == 1){
                        button_flag = 0;
                    }
                }

                if(pos_x >= SCREEN_W/2-(menuButton.width*0.9)/2 && pos_x <= (SCREEN_W/2-(menuButton.width*0.9)/2)+menuButton.width*0.9 && pos_y >= SCREEN_H/2+(menuButton.height*0.9)/2 + 140 && pos_y <= (SCREEN_H/2+(menuButton.height*0.9)/2)+menuButton.height*0.9 + 140){
                    al_draw_scaled_bitmap(menuButton.imgs[1],0,0,menuButton.width,menuButton.height, SCREEN_W/2-(menuButton.width)/2 , SCREEN_H/2+(menuButton.height)/2 + 140 ,menuButton.width, menuButton.height, 0);
                    if(button_flag != 2){
                        al_stop_sample_instance(inst_pass_button);
                        al_play_sample_instance(inst_pass_button);
                        button_flag = 2;
                    }
                }else{
                    al_draw_scaled_bitmap(menuButton.imgs[0],0,0,menuButton.width,menuButton.height, SCREEN_W/2-(menuButton.width*0.9)/2 , SCREEN_H/2+(menuButton.height*0.9)/2 + 140 ,menuButton.width*0.9, menuButton.height*0.9, 0);
                    if(button_flag == 2){
                        button_flag = 0;
                    }
                }

                if(pos_x >= SCREEN_W/2-(sair.width*0.9)/2 && pos_x <= (SCREEN_W/2-(sair.width*0.9)/2)+sair.width*0.9 && pos_y >= SCREEN_H/2+(sair.height*0.9)/2+260 && pos_y <= (SCREEN_H/2+(sair.height*0.9)/2+260)+sair.height*0.9){
                    al_draw_scaled_bitmap(sair.imgs[1],0,0,sair.width,sair.height,SCREEN_W/2-(sair.width)/2,SCREEN_H/2+(sair.height)/2+260,sair.width, sair.height, 0);
                    if(button_flag != 3){
                        al_stop_sample_instance(inst_pass_button);
                        al_play_sample_instance(inst_pass_button);
                        button_flag = 3;
                    }
                }else{
                    al_draw_scaled_bitmap(sair.imgs[0],0,0,sair.width,sair.height,SCREEN_W/2-(sair.width*0.9)/2,SCREEN_H/2+(sair.height*0.9)/2+260,sair.width*0.9, sair.height*0.9, 0);
                    if(button_flag == 3){
                        button_flag = 0;
                    }
                }

                al_flip_display();
            }
        }

        al_stop_sample_instance(inst_menu_death);
        button_flag = 0;
        show_Menu_Death = 1;



        if(fechar){
            break;
        }



    }

    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_bitmap(background.img);
    al_destroy_bitmap(propulsor.img);
    al_destroy_bitmap(ship.img);
    al_destroy_bitmap(barra.img);
    al_destroy_bitmap(vida.img);
    al_destroy_bitmap(oxigenio.img);
    al_destroy_bitmap(barra2.img);
    al_destroy_bitmap(oxigenio2.img);
    al_destroy_bitmap(canhao.img);
    al_destroy_bitmap(canhaoBase.img);
    al_destroy_bitmap(belttop.img);
    al_destroy_bitmap(beltside.img);
    al_destroy_bitmap(logo.img);
    al_destroy_bitmap(menu_background.img);
    al_destroy_bitmap(planeta.img);
    al_destroy_bitmap(objetos_nave.img);
    al_destroy_bitmap(dangerzoneSide.img);
    al_destroy_bitmap(dangerzoneTop.img);
    al_destroy_bitmap(iniciar.imgs[0]);
    al_destroy_bitmap(iniciar.imgs[1]);
    al_destroy_bitmap(sair.imgs[0]);
    al_destroy_bitmap(sair.imgs[1]);
    al_destroy_bitmap(story.imgs[0]);
    al_destroy_bitmap(story.imgs[1]);
    al_destroy_bitmap(creditosButton.imgs[0]);
    al_destroy_bitmap(creditosButton.imgs[1]);
    al_destroy_bitmap(deadnaut.img);
    al_destroy_bitmap(black.img);
    al_destroy_bitmap(reiniciar.imgs[0]);
    al_destroy_bitmap(reiniciar.imgs[1]);
    al_destroy_bitmap(menuButton.imgs[0]);
    al_destroy_bitmap(menuButton.imgs[1]);
    al_destroy_bitmap(close_X.imgs[0]);
    al_destroy_bitmap(close_X.imgs[1]);
    al_destroy_bitmap(pauseButton.imgs[0]);
    al_destroy_bitmap(pauseButton.imgs[1]);
    al_destroy_bitmap(continuar.imgs[0]);
    al_destroy_bitmap(continuar.imgs[1]);
    al_destroy_bitmap(game_over.img);
    al_destroy_bitmap(creditos.img);
    al_destroy_bitmap(tela_pausa.img);
    al_destroy_bitmap(rankingButton.imgs[0]);
    al_destroy_bitmap(rankingButton.imgs[1]);
    al_destroy_bitmap(ranking_tela.img);

    al_destroy_sample(theme);
    al_destroy_sample(tiro);
    al_destroy_sample(explosao1);
    al_destroy_sample(explosao2);
    al_destroy_sample(explosao3);
    al_destroy_sample(som_propulsor);
    al_destroy_sample(rotacao_a);
    al_destroy_sample(rotacao_h);
    al_destroy_sample(alerta);
    al_destroy_sample(intro);
    al_destroy_sample(crash);
    al_destroy_sample(menu_music);
    al_destroy_sample(menu_death);
    al_destroy_sample(button);
    al_destroy_sample(pass_button);
    al_destroy_sample_instance(inst_theme);
    al_destroy_sample_instance(inst_tiro);
    al_destroy_sample_instance(inst_explosao1);
    al_destroy_sample_instance(inst_explosao2);
    al_destroy_sample_instance(inst_explosao3);
    al_destroy_sample_instance(inst_som_propulsor);
    al_destroy_sample_instance(inst_rotacao_a);
    al_destroy_sample_instance(inst_rotacao_h);
    al_destroy_sample_instance(inst_alerta);
    al_destroy_sample_instance(inst_intro);
    al_destroy_sample_instance(inst_crash);
    al_destroy_sample_instance(inst_menu_music);
    al_destroy_sample_instance(inst_menu_death);
    al_destroy_sample_instance(inst_button);
    al_destroy_sample_instance(inst_pass_button);

    for(int i=0; i<2; i++){
        al_destroy_bitmap(flash.imgs[i]);
        al_destroy_bitmap(flash2.imgs[i]);
    }

    for(int i=0; i<9; i++){
        al_destroy_bitmap(explosion_especial.imgs[i]);
    }
    for(int i=0; i<QUANT_ESPECIAL_ASTEROIDS; i++){
        al_destroy_bitmap(especiais[i].img);
    }
    for(int i=0; i<QUANT_OXI_BALLS; i++){
        al_destroy_bitmap(oxigen_balls[i].img);
    }
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
    for(int i=0; i<30; i++){
        al_destroy_bitmap(video_intro.imgs[i]);
    }

    al_destroy_event_queue(event_queue);

    return 0;
}
