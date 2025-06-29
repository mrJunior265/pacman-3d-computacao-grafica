#ifndef BAR_H
#define BAR_H

#include <GL/glut.h>

// Estrutura para a barra de informações
typedef struct {
    int x, y;
    char buffer[1024];
} StatusBar;

// Função para definir a posição da barra de informações
void statusBarPosition(StatusBar* bar, int x, int y);

// Função para atualizar o texto da barra de informações
void updateStatusBarText(StatusBar* bar, const char* txt);

// Função para desenhar caracteres individuais na tela
void drawChars(const char* s);

// Função para desenhar a barra de informações na tela
void drawStatusBar(StatusBar* bar);

// Função para obter o texto atual da barra de informações
const char* getStatusBarText(const StatusBar* bar);

#endif