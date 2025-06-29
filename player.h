#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glut.h>

// Defini��es para dire��es de movimento
#define FRENTE 0
#define TRAS 1
#define ESQUERDA 2 
#define DIREITA 3

// Estrutura do jogador
typedef struct {
    float theta[3];  // �ngulos de rota��o
    float escala;     // Escala do boneco
    float xCubo;      // Posi��o X do jogador
    float yCubo;      // Posi��o Y do jogador
    float v;          // Velocidade do jogador
    int pontuacao;    // Pontua��o do jogador
} Player;

extern int ladoVirar; // Declara��o da vari�vel global


// Fun��es para o jogador
void initPlayer(Player* player);
void desenhar_boneco_player(Player* player);
void movimentarJogador(Player* player, int key, int* ladoVirar, char** map);
void posicionarJogador(Player* player, char** map, int xTabSize, int yTabSize);
int verificarColisaoComFantasmas(Player* player, int totalFantasmas, float* fantasmasX, float* fantasmasY);

#endif // PLAYER_H
