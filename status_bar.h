#pragma once
#ifndef STATUS_BAR_H
#define STATUS_BAR_H

// Tamanho m�ximo do buffer da barra de status
#define BUFFER_SIZE 1024

// Estrutura para representar a barra de status
typedef struct {
    int x;                      // Coordenada X da posi��o
    int y;                      // Coordenada Y da posi��o
    char buffer[BUFFER_SIZE]; // Texto exibido na barra de status
} StatusBar;

// Fun��es da barra de status
void statusBarInit(StatusBar* status, int x, int y, const char* initialText);
void statusBarSetPosition(StatusBar* status, int x, int y);
void statusBarSetText(StatusBar* status, const char* text);
const char* statusBarGetText(StatusBar* status);
void statusBarRender(StatusBar* status);

#endif // STATUS_BAR_H
