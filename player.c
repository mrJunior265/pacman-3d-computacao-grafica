#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define valores para diferentes tipos de casa no mapa
#define CASA_LIVRE 1
#define CASA_VISITADA 2
#define JOGADOR_POSICAO 3
#define TIME_CABECA 10

float anguloCabeca = 0.0; // �ngulo da rota��o da cabe�a
float velocidadeRotacaoCabeca = 2.0; // Velocidade de rota��o da cabe�a (graus por frame)
int rotacaoCabecaAtiva = 1;

// Inicializa o jogador com valores padr�o
void initPlayer(Player* player) {
    player->theta[0] = 90.0;
    player->theta[1] = 0.0;
    player->theta[2] = 0.0;
    player->escala = 1.0;
    player->xCubo = 0.0;
    player->yCubo = 0.0;
    player->v = 0.2;
    player->pontuacao = 0;
}
void atualizarCabeca(int value) {
    if (rotacaoCabecaAtiva) { // S� atualiza se estiver ativo
        anguloCabeca += velocidadeRotacaoCabeca; // Incrementa o �ngulo
        if (anguloCabeca >= 360.0) {
            anguloCabeca -= 360.0; // Mant�m o �ngulo dentro de 0 a 360
        }

        glutPostRedisplay(); // Redesenhar a tela
    }

    glutTimerFunc(TIME_CABECA, atualizarCabeca, 0); // Atualiza continuamente
}


// Desenha um boneco estilizado para representar o jogador
void desenhar_boneco_player(Player* player) {
    
    glPushMatrix();

    glTranslatef(player->xCubo * 2.0, player->yCubo * 2.0, 2.0);//0.2 pa pol mas pa riba

    if (ladoVirar == FRENTE)
        glRotatef(0, 0.0, 0.0, 1.0);
    else if (ladoVirar == TRAS)
        glRotatef(180, 0.0, 0.0, 1.0);
    else if (ladoVirar == ESQUERDA)
        glRotatef(-90, 0.0, 0.0, 1.0);
    else if (ladoVirar == DIREITA)
        glRotatef(90, 0.0, 0.0, 1.0);

    // Rota��o do cubo
    glRotatef(player->theta[0], 1.0, 0.0, 0.0);
    glRotatef(player->theta[1], 0.0, 1.0, 0.0);
    glRotatef(player->theta[2], 0.0, 0.0, 1.0);


    

    
    
        // Cabe�a
        glPushMatrix();
        glColor3f(1.0, 0.8, 0.6); // Cor da pele
        glTranslatef(0.0, 0.8, 0.0); // Posiciona a cabe�a acima do corpo
        glRotatef(anguloCabeca, 0.0, 1.0, 0.0); // Rotaciona a cabe�a ao longo do eixo Y
        glScalef(0.2, 0.2, 0.2); // Escala para ajustar o tamanho da cabe�a
        glutSolidCube(1.0); // Desenha um cubo s�lido para a cabe�a
        glPopMatrix();

        // Corpo
        glPushMatrix();
        glColor3f(0.0, 0.0, 1.0); // Azul
        glTranslatef(0.0, 0.4, 0.0); // Posiciona o corpo abaixo da cabe�a
        glScalef(0.4, 0.6, 0.2); // Escala para formar o tronco
        glutSolidCube(1.0); // Desenha um cubo s�lido para o corpo
        glPopMatrix();

        // Bra�o esquerdo
        glPushMatrix();
        glColor3f(1.0, 0.8, 0.6); // Cor da pele
        glTranslatef(-0.25, 0.4, 0.0); // Posiciona o bra�o esquerdo ao lado do corpo
        glScalef(0.1, 0.4, 0.1); // Escala para formar o bra�o
        glutSolidCube(1.0); // Desenha um cubo s�lido para o bra�o
        glPopMatrix();

        // Bra�o direito
        glPushMatrix();
        glColor3f(1.0, 0.8, 0.6); // Cor da pele
        glTranslatef(0.25, 0.4, 0.0); // Posiciona o bra�o direito ao lado do corpo
        glScalef(0.1, 0.4, 0.1); // Escala para formar o bra�o
        glutSolidCube(1.0); // Desenha um cubo s�lido para o bra�o
        glPopMatrix();

        // Perna esquerda
        glPushMatrix();
        glColor3f(0.0, 0.0, 1.0); // Azul
        glTranslatef(-0.1, 0.1, 0.0); // Posiciona a perna esquerda abaixo do corpo
        glScalef(0.1, 0.4, 0.1); // Escala para formar a perna
        glutSolidCube(1.0); // Desenha um cubo s�lido para a perna
        glPopMatrix();

        // Perna direita
        glPushMatrix();
        glColor3f(0.0, 0.0, 1.0); // Azul
        glTranslatef(0.1, 0.1, 0.0); // Posiciona a perna direita abaixo do corpo
        glScalef(0.1, 0.4, 0.1); // Escala para formar a perna
        glutSolidCube(1.0); // Desenha um cubo s�lido para a perna
        glPopMatrix();
    glPopMatrix();
}
void atualizarPlayer(Player* player, int direcao) {
    switch (direcao) {
    case FRENTE:
        player->yCubo += 1.0;
        ladoVirar = FRENTE;
        break;
    case TRAS:
        player->yCubo -= 1.0;
        ladoVirar = TRAS;
        break;
    case ESQUERDA:
        player->xCubo -= 1.0;
        ladoVirar = ESQUERDA;
        break;
    case DIREITA:
        player->xCubo += 1.0;
        ladoVirar = DIREITA;
        break;
    }
    printf("Movimento: %d, Posi��o: (%f, %f)\n", ladoVirar, player->xCubo, player->yCubo);
}
// Define uma posi��o inicial para o jogador
void posicionarJogador(Player* player, char** map, int xTabSize, int yTabSize) {
    while (1) {
        int randX = rand() % xTabSize;
        int randY = rand() % yTabSize;

        if (map[randX][randY] == CASA_LIVRE) { // Se a posi��o � v�lida
            player->xCubo = randX;
            player->yCubo = randY;
            return;
        }
    }
}
// Verifica colis�o do jogador com fantasmas
int verificarColisaoComFantasmas(Player* player, int totalFantasmas, float* fantasmasX, float* fantasmasY) {
    for (int i = 0; i < totalFantasmas; i++) {
        if (fabs(player->xCubo - fantasmasX[i]) < 1.0 && fabs(player->yCubo - fantasmasY[i]) < 1.0) {
            return 1; // Colis�o detectada
        }
    }
    return 0;
}

// Movimenta o jogador conforme a tecla pressionada
void movimentarJogador(Player* player, int key, int* ladoVirar, char** map, float alfaLongitude) {
    float xAnterior = player->xCubo;
    float yAnterior = player->yCubo;
    float moveX = 0.0f;
    float moveY = 0.0f;

    // Aplica a l�gica de movimenta��o com base na c�mera (alfaLongitude)
    switch (key) {
    case GLUT_KEY_UP:
        if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
            moveY = 1.0f; *ladoVirar = FRENTE;
        }
        else if (alfaLongitude > 60 && alfaLongitude <= 150) {
            moveX = -1.0f; *ladoVirar = ESQUERDA;
        }
        else if (alfaLongitude > 150 && alfaLongitude <= 250) {
            moveY = -1.0f; *ladoVirar = TRAS;
        }
        else if (alfaLongitude < -60 && alfaLongitude >= -150) {
            moveX = 1.0f; *ladoVirar = DIREITA;
        }
        else if (alfaLongitude < -150 && alfaLongitude >= -240) {
            moveY = -1.0f; *ladoVirar = TRAS;
        }
        else if (alfaLongitude < -240 && alfaLongitude >= -330) {
            moveX = -1.0f; *ladoVirar = ESQUERDA;
        }
        break;

    case GLUT_KEY_DOWN:
        if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
            moveY = -1.0f; *ladoVirar = TRAS;
        }
        else if (alfaLongitude > 60 && alfaLongitude <= 150) {
            moveX = 1.0f; *ladoVirar = DIREITA;
        }
        else if (alfaLongitude > 150 && alfaLongitude <= 250) {
            moveY = 1.0f; *ladoVirar = FRENTE;
        }
        else if (alfaLongitude < -60 && alfaLongitude >= -150) {
            moveX = -1.0f; *ladoVirar = ESQUERDA;
        }
        else if (alfaLongitude < -150 && alfaLongitude >= -240) {
            moveY = 1.0f; *ladoVirar = FRENTE;
        }
        else if (alfaLongitude < -240 && alfaLongitude >= -330) {
            moveX = 1.0f; *ladoVirar = DIREITA;
        }
        break;

    case GLUT_KEY_LEFT:
        if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
            moveX = -1.0f; *ladoVirar = ESQUERDA;
        }
        else if (alfaLongitude > 60 && alfaLongitude <= 150) {
            moveY = -1.0f; *ladoVirar = TRAS;
        }
        else if (alfaLongitude > 150 && alfaLongitude <= 250) {
            moveX = 1.0f; *ladoVirar = DIREITA;
        }
        else if (alfaLongitude < -60 && alfaLongitude >= -150) {
            moveY = 1.0f; *ladoVirar = FRENTE;
        }
        else if (alfaLongitude < -150 && alfaLongitude >= -240) {
            moveX = 1.0f; *ladoVirar = DIREITA;
        }
        else if (alfaLongitude < -240 && alfaLongitude >= -330) {
            moveY = -1.0f; *ladoVirar = TRAS;
        }
        break;

    case GLUT_KEY_RIGHT:
        if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
            moveX = 1.0f; *ladoVirar = DIREITA;
        }
        else if (alfaLongitude > 60 && alfaLongitude <= 150) {
            moveY = 1.0f; *ladoVirar = FRENTE;
        }
        else if (alfaLongitude > 150 && alfaLongitude <= 250) {
            moveX = -1.0f; *ladoVirar = ESQUERDA;
        }
        else if (alfaLongitude < -60 && alfaLongitude >= -150) {
            moveY = -1.0f; *ladoVirar = TRAS;
        }
        else if (alfaLongitude < -150 && alfaLongitude >= -240) {
            moveX = -1.0f; *ladoVirar = ESQUERDA;
        }
        else if (alfaLongitude < -240 && alfaLongitude >= -330) {
            moveY = 1.0f; *ladoVirar = FRENTE;
        }
        break;
    }

    // Atualiza a posi��o do jogador
    player->xCubo += moveX;
    player->yCubo += moveY;

    // Verifica se a nova posi��o � v�lida
    if (map[(int)player->xCubo][(int)player->yCubo] == 0) {
        // Se for parede, restaura a posi��o anterior
        player->xCubo = xAnterior;
        player->yCubo = yAnterior;
    }
    else {
        // Se a casa n�o foi visitada antes, adiciona pontos
        if (map[(int)player->xCubo][(int)player->yCubo] == CASA_LIVRE) {
            player->pontuacao += 5;
            map[(int)player->xCubo][(int)player->yCubo] = CASA_VISITADA;
        }

        // Libera a posi��o anterior
        map[(int)xAnterior][(int)yAnterior] = CASA_VISITADA;
    }
}
