#ifndef BAR_H
#define BAR_H

#include <GL/glut.h>

// Estrutura para a barra de informa��es
typedef struct {
    int x, y;
    char buffer[1024];
} StatusBar;

// Fun��o para definir a posi��o da barra de informa��es
void statusBarPosition(StatusBar* bar, int x, int y);

// Fun��o para atualizar o texto da barra de informa��es
void updateStatusBarText(StatusBar* bar, const char* txt);

// Fun��o para desenhar caracteres individuais na tela
void drawChars(const char* s);

// Fun��o para desenhar a barra de informa��es na tela
void drawStatusBar(StatusBar* bar);

// Fun��o para obter o texto atual da barra de informa��es
const char* getStatusBarText(const StatusBar* bar);

#endif