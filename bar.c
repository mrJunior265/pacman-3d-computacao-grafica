#include "bar.h"
#include <string.h>

// Fun��o para definir a posi��o da barra de informa��es
void statusBarPosition(StatusBar* bar, int x, int y) {
    if (bar) {
        bar->x = x;
        bar->y = y;
        strncpy(bar->buffer, "Status Bar", sizeof(bar->buffer) - 1);
        bar->buffer[sizeof(bar->buffer) - 1] = '\0';
    }
}

// Fun��o para atualizar o texto da barra de informa��es
void updateStatusBarText(StatusBar* bar, const char* txt) {
    if (bar && txt) {
        strncpy(bar->buffer, txt, sizeof(bar->buffer) - 1);
        bar->buffer[sizeof(bar->buffer) - 1] = '\0';
    }
}

// Fun��o para desenhar caracteres individuais na tela
void drawChars(const char* s) {
    if (s) {
        for (size_t i = 0; i < strlen(s); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
        }
    }
}

// Fun��o para desenhar a barra de informa��es na tela
void drawStatusBar(StatusBar* bar) {
    if (bar) {
        glColor3f(1.0, 1.0, 1.0); // Cor do texto
        glRasterPos2f(bar->x, bar->y); // Posi��o da barra
        drawChars(bar->buffer);
    }
}

// Fun��o para obter o texto atual da barra de informa��es
const char* getStatusBarText(const StatusBar* bar) {
    if (bar) {
        return bar->buffer;
    }
    return NULL;
}
