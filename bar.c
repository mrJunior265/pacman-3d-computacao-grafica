#include "bar.h"
#include <string.h>

// Função para definir a posição da barra de informações
void statusBarPosition(StatusBar* bar, int x, int y) {
    if (bar) {
        bar->x = x;
        bar->y = y;
        strncpy(bar->buffer, "Status Bar", sizeof(bar->buffer) - 1);
        bar->buffer[sizeof(bar->buffer) - 1] = '\0';
    }
}

// Função para atualizar o texto da barra de informações
void updateStatusBarText(StatusBar* bar, const char* txt) {
    if (bar && txt) {
        strncpy(bar->buffer, txt, sizeof(bar->buffer) - 1);
        bar->buffer[sizeof(bar->buffer) - 1] = '\0';
    }
}

// Função para desenhar caracteres individuais na tela
void drawChars(const char* s) {
    if (s) {
        for (size_t i = 0; i < strlen(s); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
        }
    }
}

// Função para desenhar a barra de informações na tela
void drawStatusBar(StatusBar* bar) {
    if (bar) {
        glColor3f(1.0, 1.0, 1.0); // Cor do texto
        glRasterPos2f(bar->x, bar->y); // Posição da barra
        drawChars(bar->buffer);
    }
}

// Função para obter o texto atual da barra de informações
const char* getStatusBarText(const StatusBar* bar) {
    if (bar) {
        return bar->buffer;
    }
    return NULL;
}
