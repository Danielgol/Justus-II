#include <stdio.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <math.h>



//ISSUES:
//*PENSAR MAIS SOBRE O JOGO
//*VIDA
//*FAZER OS BONECOS ANDAREM NO CAMINHO
//*ATIRAR (TIRO COLIDIR)
//*GERAR ASTEROIDES ALEATORIAMENTE NO MAPA
//*MOVIMENTAÇÃO DA NAVE (ADAPTAR)
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

enum MYKEYS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_A, KEY_S, KEY_D, KEY_L
};

int main(int argc, char **argv){

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
    ship = al_load_bitmap("images/ship/shipBig.png");
    float xShip = WORLD_W/2, yShip = WORLD_H/2;
    int shipWidth = al_get_bitmap_width(ship), shipHeight = al_get_bitmap_height(ship);
    int controle = 0;
    float forceX = 0, forceY = 0;
    //PROPULSOR
    ALLEGRO_BITMAP *propulsor;
    propulsor = al_load_bitmap("images/ship/Propulsor.png");
    int propWidth = al_get_bitmap_width(propulsor), propHeight = al_get_bitmap_height(propulsor);
    int rotation = 90;

    //MEGAMEN
    ALLEGRO_BITMAP  *mega[14];
    mega[0] = al_load_bitmap("images/mega/mr1.png");
    mega[1] = al_load_bitmap("images/mega/mr2.png");
    mega[2] = al_load_bitmap("images/mega/mr3.png");
    mega[3] = al_load_bitmap("images/mega/mr4.png");
    mega[4] = al_load_bitmap("images/mega/mr5.png");
    mega[5] = al_load_bitmap("images/mega/mr6.png");
    mega[6] = al_load_bitmap("images/mega/mr7.png");
    mega[7] = al_load_bitmap("images/mega/ml1.png");
    mega[8] = al_load_bitmap("images/mega/ml2.png");
    mega[9] = al_load_bitmap("images/mega/ml3.png");
    mega[10] = al_load_bitmap("images/mega/ml4.png");
    mega[11] = al_load_bitmap("images/mega/ml5.png");
    mega[12] = al_load_bitmap("images/mega/ml6.png");
    mega[13] = al_load_bitmap("images/mega/ml7.png");
    int xMega = shipWidth/2, yMega = shipHeight/2, curMega = 0;
    int megaWidth = al_get_bitmap_width(mega[0]), megaHeight = al_get_bitmap_height(mega[0]);

    //SONIC
    ALLEGRO_BITMAP  *sonic[10];
    sonic[0] = al_load_bitmap("images/sonic/r1.png");
    sonic[1] = al_load_bitmap("images/sonic/r2.png");
    sonic[2] = al_load_bitmap("images/sonic/r3.png");
    sonic[3] = al_load_bitmap("images/sonic/r4.png");
    sonic[4] = al_load_bitmap("images/sonic/r5.png");
    sonic[5] = al_load_bitmap("images/sonic/l1.png");
    sonic[6] = al_load_bitmap("images/sonic/l2.png");
    sonic[7] = al_load_bitmap("images/sonic/l3.png");
    sonic[8] = al_load_bitmap("images/sonic/l4.png");
    sonic[9] = al_load_bitmap("images/sonic/l5.png");
    int xSonic = shipWidth/2, ySonic = shipHeight/2, curSonic = 0;
    int sonicWidth = al_get_bitmap_width(sonic[0]), sonicHeight = al_get_bitmap_height(sonic[0]);

    //COMPUTADOR
    ALLEGRO_BITMAP *comp;
    comp = al_load_bitmap("images/ship/comp.png");
    float compScale = 0.3;
    int compWidth = al_get_bitmap_width(comp), compHeight = al_get_bitmap_height(comp);

    //COMPUTADOR
    ALLEGRO_BITMAP *compShot[4];
    compShot[0] = al_load_bitmap("images/ship/compShot.png");
    compShot[1] = al_load_bitmap("images/ship/compShot.png");
    compShot[2] = al_load_bitmap("images/ship/compShot.png");
    compShot[3] = al_load_bitmap("images/ship/compShot.png");
    int compShotWidth = al_get_bitmap_width(compShot[0]);
    int compShotHeight = al_get_bitmap_height(compShot[0]);

    ALLEGRO_BITMAP *asteroid;
    asteroid = al_load_bitmap("images/asteroids/Asteroid.png");
    int xAst = (WORLD_W/2)-200, yAst = (WORLD_H/2)-200;
    int astWidth = al_get_bitmap_width(asteroid);
    int astHeight = al_get_bitmap_height(asteroid);
    float dist;

    //PLANO DE FUNDO
    ALLEGRO_BITMAP *background;
    background = al_load_bitmap("images/background.jpg");
    int xBG = 0, yBG = 0;
    float BGScale = 1.7;
    int BGWidth = al_get_bitmap_width(background);
    int BGHeight = al_get_bitmap_height(background);




    //CONFIGURA/INICIALIZA EVENTOS E LOCAL DE DESENHO
    al_set_target_bitmap(al_get_backbuffer(display));
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_flip_display();
    al_start_timer(timer);




    while(1){

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        //EVENTOS REAIS (TEMPO)
        if(ev.type == ALLEGRO_EVENT_TIMER) {

            //MOVIMENTOS NAVE
            if(controle == 1){
                if(key[KEY_UP]){

                    //IDEIA:
                    //https://www.efunda.com/math/fourier_series/display.cfm?name=triangle
                    //float angle = rotation*3.14159/180;
                    //float senof = 0;
                    //float cosef = 0;
                    //if(-3.14159/2 <= angle && angle <= 3.14159/2){
                    //    senof = 2*angle/3.14159;
                    //    cosef = 1-senof;
                    //}else{
                    //    senof = (2*(3.14159-angle))/3.14159;
                    //    cosef = 1-senof;
                    //}
                    //forceX -= senof*0.01;
                    //forceY += cosef*0.01;

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
                }
                if(key[KEY_LEFT]){
                    rotation+=1;
                }
                if(key[KEY_RIGHT]){
                    rotation-=1;
                }
            }else{
                //MOVIMENTOS MEGAMEN
                if(key[KEY_UP] && yMega > 0){
                    yMega -= VELOCITY;
                }
                if(key[KEY_DOWN] && yMega < shipHeight - megaHeight){
                    yMega += VELOCITY;
                }
                if(key[KEY_LEFT] && xMega > 0){
                    xMega -= VELOCITY;
                    curMega++;
                }
                if(key[KEY_RIGHT] && xMega < shipWidth - megaWidth){
                    xMega += VELOCITY;
                    curMega--;
                }

            }
            //ATUALIZA OS FRAMES MEGAMEN
            if(curMega == 0){
                curMega = 6*DELAY;
            }else if(curMega == 14*DELAY){
                curMega = 8*DELAY;
            }

            //FAZ COM QUE A NAVE FREIE AOS POUCOS
            if(forceY < 0){
                    forceY += 0.002;
                    if(forceY>0){
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

            if(rotation >= 360){
                rotation = 0;
            }else if(rotation < 0){
                rotation = 359;
            }

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

            //MOVIMENTOS SONIC
            if(key[KEY_W] && ySonic > 0){
                ySonic -= VELOCITY;
            }
            if(key[KEY_S] && ySonic < shipHeight - sonicHeight){
                ySonic += VELOCITY;
            }
            if(key[KEY_A] && xSonic > 0){
                xSonic -= VELOCITY;
                curSonic++;
            }
            if(key[KEY_D] && xSonic < shipWidth - sonicWidth){
                xSonic += VELOCITY;
                curSonic--;
            }
            //ATUALIZA OS FRAMES SONIC
            if(curSonic == 0){
                curSonic = 4*DELAY;
            }else if(curSonic == 10*DELAY){
                curSonic = 6*DELAY;
            }

            //ATUALIZA A CÂMERA AO OBJETO EM MOVIMENTO
            cameraX = xShip - SCREEN_W/2 +shipWidth/2;
            cameraY = yShip - SCREEN_H/2 +shipHeight/2;

            dist = sqrt((pow((xShip+shipWidth/2)-(xAst+astWidth/2), 2))+(pow((yShip+shipWidth/2)-(yAst+astHeight/2), 2)));

            //LIMITA A CAMERA NAS BORDAS DO MAPA
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
                //MEGAMEN
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
                    if(controle == 1){
                        controle = 0;
                    }else{
                        controle = 1;
                    }
                    break;
                //SONIC
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
            }

        //DESATIVA AS TECLAS PRESSIONADAS
        }else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch(ev.keyboard.keycode) {
                //MEGAMEN
                case ALLEGRO_KEY_UP:
                    key[KEY_UP] = false;
                    break;
                case ALLEGRO_KEY_DOWN:
                    key[KEY_DOWN] = false;
                    break;
                case ALLEGRO_KEY_LEFT:
                    key[KEY_LEFT] = false;
                    curMega=7*DELAY; //RETORNA PARA O FRAME DO PERSONAGEM PARADO
                    break;
                case ALLEGRO_KEY_RIGHT:
                    key[KEY_RIGHT] = false;
                    curMega=6*DELAY; //RETORNA PARA O FRAME DO PERSONAGEM PARADO
                    break;
                //SONIC
                case ALLEGRO_KEY_W:
                    key[KEY_W] = false;
                    break;
                case ALLEGRO_KEY_S:
                    key[KEY_S] = false;
                    break;
                case ALLEGRO_KEY_A:
                    key[KEY_A] = false;
                    curSonic=5*DELAY; //RETORNA PARA O FRAME DO PERSONAGEM PARADO
                    break;
                case ALLEGRO_KEY_D:
                    curSonic=4*DELAY; //RETORNA PARA O FRAME DO PERSONAGEM PARADO
                    key[KEY_D] = false;
                    break;
            }
        }else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        //DESENHOS
        if(redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            //al_clear_to_color(al_map_rgb(50,50,50)); //LIMPA A TELA (NECESSÁRIO QUANDO NÃO TEM BACKGROUND)
            //###TALVEZ NA HORA DAS COLISÕES DÊ ERRO POR CONTA DO LUGAR DOS OBJETOS SEREM DIFERENTES DE ONDE ELES ESTÃO
            //###TESTAR MUDAR REALMENTE A VARIAVEL DE POSIÇÃO DOS OBJETOS, AO INVES DE SÓ COLOCAR ONDE ELES DEVEM SER DESENHADOS.
            //###EX: xSonic -= cameraX; al_draw_scaled_bitmap(...,xSonic, ySonic,...);
            al_draw_scaled_bitmap(background,0,0,BGWidth,BGHeight,xBG-(cameraX/2),yBG-(cameraY/2),BGWidth*BGScale, BGHeight*BGScale, 0);
            al_draw_rotated_bitmap(propulsor,propHeight/2, propHeight/2,xShip-cameraX+shipWidth/2, yShip-cameraY+shipHeight/2,((rotation-90)*3.14159/180),0);
            al_draw_scaled_bitmap(ship,0,0,shipWidth,shipHeight,xShip-cameraX,yShip-cameraY,shipWidth, shipHeight, 0);
            al_draw_scaled_bitmap(comp,0,0,compWidth,compHeight,xShip-cameraX+shipWidth/2-10,yShip-cameraY+shipHeight/2-15,compWidth*compScale, compHeight*compScale, 0);
            al_draw_scaled_bitmap(compShot[0],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2-140,yShip-cameraY+shipHeight/2-15,compShotWidth*compScale, compShotHeight*compScale, 0);
            al_draw_scaled_bitmap(compShot[1],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2+110,yShip-cameraY+shipHeight/2-20,compShotWidth*compScale, compShotHeight*compScale, 0);
            al_draw_scaled_bitmap(compShot[2],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2+15,yShip-cameraY+shipHeight/2-150,compShotWidth*compScale, compShotHeight*compScale, 0);
            al_draw_scaled_bitmap(compShot[3],0,0,compShotWidth,compShotHeight,xShip-cameraX+shipWidth/2-5,yShip-cameraY+shipHeight/2+100,compShotWidth*compScale, compShotHeight*compScale, 0);
            al_draw_scaled_bitmap(sonic[curSonic/DELAY],0,0,sonicWidth,sonicHeight,xSonic+xShip-cameraX,ySonic+yShip-cameraY,sonicWidth, sonicHeight, 0);
            al_draw_scaled_bitmap(mega[curMega/DELAY],0,0,megaWidth,megaHeight,xMega+xShip-cameraX,yMega+yShip-cameraY,megaWidth, megaHeight, 0);

            if(dist > (shipWidth/2)+(astWidth/2)){
                al_draw_scaled_bitmap(asteroid,0,0,astWidth,astHeight,xAst-cameraX,yAst-cameraY,astWidth, astHeight, 0);
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
    al_destroy_bitmap(comp);
    al_destroy_bitmap(compShot);
    al_destroy_bitmap(asteroid);
    for(int i=0; i<14; i++){
        al_destroy_bitmap(mega[i]);
    }
    for(int i=0; i<10; i++){
        al_destroy_bitmap(sonic[i]);
    }
    al_destroy_event_queue(event_queue);

    return 0;
}
