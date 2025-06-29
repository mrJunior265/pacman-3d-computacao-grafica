#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glut.h>

// Definições para direções de movimento
#define FRENTE 0
#define TRAS 1
#define ESQUERDA 2 
#define DIREITA 3

// Estrutura do jogador
typedef struct {
    float theta[3];  // Ângulos de rotação
    float escala;     // Escala do boneco
    float xCubo;      // Posição X do jogador
    float yCubo;      // Posição Y do jogador
    float v;          // Velocidade do jogador
    int pontuacao;    // Pontuação do jogador
} Player;

extern int ladoVirar; // Declaração da variável global


// Funções para o jogador
void initPlayer(Player* player);
void desenhar_boneco_player(Player* player);
void movimentarJogador(Player* player, int key, int* ladoVirar, char** map);
void posicionarJogador(Player* player, char** map, int xTabSize, int yTabSize);
int verificarColisaoComFantasmas(Player* player, int totalFantasmas, float* fantasmasX, float* fantasmasY);

#endif // PLAYER_H
