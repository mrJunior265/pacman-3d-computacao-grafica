
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "bar.h"
#include "player.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "pm-maps.h"





#define TIME 1500
#define TIME_CABECA 10
#define NCubos 2
#define JOGADOR_POSICAO_MAPA 2 
#define M_PI 3.14
#define FRENTE 0
#define TRAS 1
#define ESQUERDA 2 
#define DIREITA 3
#define INTELIGENTE 1
#define NORMAL 2
#define LIVRE 0
#define OCUPADA 1


GLfloat vertices[][3] = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat normals[][3] = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat colors[][3] = { {0.0,0.0,0.0},{1.0,0.0,0.0},
{1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
{1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0} };




// Variáveis Globais


struct player {
	float theta[3];
	float escala;
	float xCubo;
	float yCubo;
	float v;
};


StatusBar statusBar;


int tela = 1;
int venceu = 0;  // 0: jogando, 1: venceu.
int perdeu = 0; // 0: jogando, 1: perdeu
int fimDoJogo = 0;       // 0: jogando, 1: venceu, 2: perdeu
int tempoExibicaoFim = 5000; // Tempo para exibir a mensagem de fim (em milissegundos)

int jogoAbortado = 0;
int total_pos_livre = 0;     // Contador de posições válidas
int fantamas_total;

float distCamara = 3;
float dDistCamara = 0;
float alfaLongitude = 0;
float alfaLatitude = 135;
float dAlfa = 2;
float xRato, yRato;   // guarda a última posição do rato (para calcular o deslocamento na Lat. e Long.)
float angRato = 0.75;  // precisão do rato: número de angulos por pixel.
float x, y, z;
int ladoVirar = 0;
float firstTouch = 0;
int pausa = 0;
char buffer[4096];
int point = 0;
int total_pos_livre;





struct player players[NCubos];
float v = 0.5;				// velocidade (units/second)


// PACMAN - MAP (Alterar a posição do visualizador)
static GLdouble viewer[] = { 0.0, -3.0, 1.5 }; /* Localização inicial do visualizador -3.0 1.5*/

char** map = NULL;
//Posicao
typedef struct {
	float x;        // Coordenada x
	float y;        // Coordenada y
	int livre;    // 1 se a posição está livre, 0 se está ocupada
} Posicao;

Posicao* posicoesValidas = NULL;  // Ponteiro para posições válidas


//Fantasma
typedef struct {
	float x;
	float y;
	int tipo; //1 inteligente, 0 normal
	float theta[3];
}ghost;

ghost* fantasmas = NULL;



void init_player() {
	players[0].theta[0] = 90.0;
	players[0].theta[1] = 0.0;
	players[0].theta[2] = 0.0;
	players[0].escala = 1.0;
	players[0].xCubo = -1.5;
	players[0].yCubo = 0.0;
	players[0].v = -0.2;
}
void int_ghost() {
	int total = numGhosts() + numSmartGhosts();

	// Alocação de memória para todos os fantasmas
	fantasmas = (ghost*)malloc(total * sizeof(ghost));
	if (fantasmas == NULL) {
		printf("Erro ao alocar memória para fantasmas!\n");
		exit(1);
	}

	for (int i = 0; i < total; i++) {
		fantasmas[i].y = 0.0;
		fantasmas[i].theta[0] = 90.0;
		fantasmas[i].theta[1] = 0.0;
		fantasmas[i].theta[2] = 0.0;

		if (i < numGhosts()){
			printf("Fantasma Normal\n");
			fantasmas[i].x = 1.5;
			fantasmas[i].tipo = NORMAL;
		}
		else {
			printf("Fantasma Inteligente\n");
			fantasmas[i].x = -1.5;
			fantasmas[i].tipo = INTELIGENTE;
		}
	}
}

void PosicoesValidas() {
	int x = xTabSize();
	int y = yTabSize();
	total_pos_livre = 0;
	// Contar posições válidas
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			if (map[i][j] == 1) {
				total_pos_livre++;
			}
		}
	}

	// Alocação de memória para posições válidas
	posicoesValidas = (Posicao*)malloc(total_pos_livre * sizeof(Posicao));
	if (posicoesValidas == NULL) {
		printf("Erro ao alocar memória para posições válidas.\n");
		exit(1);
	}

	// Preenchendo as posições válidas
	int indice = 0;
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			if (map[i][j] == 1) {
				posicoesValidas[indice++] = (Posicao){ i, j, LIVRE };
			}
		}
	}
}

Posicao* posicaoAleatoria() {
	int livres = 0;

	// Aloca memória dinamicamente para armazenar índices livres
	int* indicesLivres = (int*)malloc(total_pos_livre * sizeof(int));
	if (indicesLivres == NULL) {
		printf("Erro ao alocar memória para índices livres.\n");
		return NULL;
	}

	// Coletar índices das posições livres
	for (int i = 0; i < total_pos_livre; i++) {
		if (posicoesValidas[i].livre == LIVRE) {
			indicesLivres[livres++] = i;
		}
	}

	// Se não houver posições livres, libera memória e retorna NULL
	if (livres == 0) {
		free(indicesLivres);
		return NULL;
	}

	// Escolhe uma posição aleatória entre as livres
	int indiceEscolhido = indicesLivres[rand() % livres];
	posicoesValidas[indiceEscolhido].livre = OCUPADA;

	free(indicesLivres);  // Libera a memória antes de retornar
	return &posicoesValidas[indiceEscolhido];
}

// Libera uma posição ocupada
void PosicaoLivre(int x, int y) {
	for (int i = 0; i < total_pos_livre; i++) {
		if (posicoesValidas[i].x == x && posicoesValidas[i].y == y) {
			posicoesValidas[i].livre = LIVRE;
			return;
		}
	}
}


void polygon(int a, int b, int c, int d)
{

	/* draw a polygon via list of vertices */

	glBegin(GL_POLYGON);
	//glColor3fv(colors[a]);
	glNormal3fv(normals[a]);
	glVertex3fv(vertices[a]);
	//glColor3fv(colors[b]);
	glNormal3fv(normals[b]);
	glVertex3fv(vertices[b]);
	//glColor3fv(colors[c]);
	glNormal3fv(normals[c]);
	glVertex3fv(vertices[c]);
	//glColor3fv(colors[d]);
	glNormal3fv(normals[d]);
	glVertex3fv(vertices[d]);
	glEnd();
}


void colorcube(void)
{

	/* map vertices to faces */

	//glColor3fv(colors[1]);
	polygon(0, 3, 2, 1);
	//glColor3fv(colors[2]);
	polygon(2, 3, 7, 6);
	//glColor3fv(colors[3]);
	polygon(0, 4, 7, 3);
	//glColor3fv(colors[4]);
	polygon(1, 2, 6, 5);
	//glColor3fv(colors[5]);
	polygon(4, 5, 6, 7);
	//glColor3fv(colors[7]);
	polygon(0, 1, 5, 4);
}



void colorcube2(float r, float g, float b)
{

	/* map vertices to faces */
	glColor3f(r, g, b);
	polygon(0, 3, 2, 1);
	polygon(2, 3, 7, 6);
	polygon(0, 4, 7, 3);
	polygon(1, 2, 6, 5);
	polygon(4, 5, 6, 7);
	polygon(0, 1, 5, 4);
}

void desenhaBoneco2() {
	// Cabeça
	glPushMatrix();
	glColor3f(1.0, 0.8, 0.6); // Cor da pele
	glTranslatef(0.0, 0.8, 0.0); // Posiciona a cabeça acima do corpo
	glRotatef(anguloCabeca, 0.0, 1.0, 0.0); // Rotaciona a cabeça ao longo do eixo Y
	glScalef(0.2, 0.2, 0.2); // Escala para ajustar o tamanho da cabeça
	glutSolidCube(1.0); // Desenha um cubo sólido para a cabeça
	glPopMatrix();

	// Corpo
	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0); // Azul
	glTranslatef(0.0, 0.4, 0.0); // Posiciona o corpo abaixo da cabeça
	glScalef(0.4, 0.6, 0.2); // Escala para formar o tronco
	glutSolidCube(1.0); // Desenha um cubo sólido para o corpo
	glPopMatrix();

	// Braço esquerdo
	glPushMatrix();
	glColor3f(1.0, 0.8, 0.6); // Cor da pele
	glTranslatef(-0.25, 0.4, 0.0); // Posiciona o braço esquerdo ao lado do corpo
	glScalef(0.1, 0.4, 0.1); // Escala para formar o braço
	glutSolidCube(1.0); // Desenha um cubo sólido para o braço
	glPopMatrix();

	// Braço direito
	glPushMatrix();
	glColor3f(1.0, 0.8, 0.6); // Cor da pele
	glTranslatef(0.25, 0.4, 0.0); // Posiciona o braço direito ao lado do corpo
	glScalef(0.1, 0.4, 0.1); // Escala para formar o braço
	glutSolidCube(1.0); // Desenha um cubo sólido para o braço
	glPopMatrix();

	// Perna esquerda
	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0); // Azul
	glTranslatef(-0.1, 0.1, 0.0); // Posiciona a perna esquerda abaixo do corpo
	glScalef(0.1, 0.4, 0.1); // Escala para formar a perna
	glutSolidCube(1.0); // Desenha um cubo sólido para a perna
	glPopMatrix();

	// Perna direita
	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0); // Azul
	glTranslatef(0.1, 0.1, 0.0); // Posiciona a perna direita abaixo do corpo
	glScalef(0.1, 0.4, 0.1); // Escala para formar a perna
	glutSolidCube(1.0); // Desenha um cubo sólido para a perna
	glPopMatrix();
}


void Fantasma(float r, float g, float b) {
	glPushMatrix();
	glTranslatef(0.0, 0.5, 0.0); // Elevação do corpo
	glScalef(0.6, 0.8, 0.6);    // Escala do corpo
	glColor3f(r, b, g);   // Cor principal do fantasma (roxo, exemplo)
	glutSolidSphere(1.0, 50, 50); // Corpo arredondado
	glPopMatrix();

	// Base do fantasma (ondulada)
	glPushMatrix();
	glTranslatef(0.0, -0.1, 0.0); // Posição abaixo do corpo
	glScalef(0.6, 0.2, 0.6);      // Escala da base
	glColor3f(r, g, b);     // Mesma cor do corpo
	for (int i = 0; i < 360; i += 45) {
		glPushMatrix();
		glRotatef(i, 0.0, 1.0, 0.0);
		glTranslatef(0.8, 0.0, 0.0); // Posição ondulada
		glutSolidSphere(0.2, 20, 20); // Esferas onduladas na base
		glPopMatrix();
	}
	glPopMatrix();

	// Olhos do fantasma
	// Olho esquerdo
	glPushMatrix();
	glTranslatef(-0.2, 0.8, 0.6); // Posição do olho esquerdo
	glScalef(0.15, 0.2, 0.1);     // Escala do olho
	glColor3f(1.0, 1.0, 1.0);     // Contorno branco
	glutSolidSphere(1.0, 30, 30);
	glColor3f(0.0, 0.0, 1.0);     // Pupila azul
	glTranslatef(0.0, 0.0, 0.5);  // Posição da pupila
	glutSolidSphere(0.5, 20, 20); // Esfera menor preta
	glPopMatrix();

	// Olho direito
	glPushMatrix();
	glTranslatef(0.2, 0.8, 0.6); // Posição do olho direito
	glScalef(0.15, 0.2, 0.1);    // Escala do olho
	glColor3f(1.0, 1.0, 1.0);    // Contorno branco
	glutSolidSphere(1.0, 30, 30);
	glColor3f(0.0, 0.0, 1.0);    // Pupila azul
	glTranslatef(0.0, 0.0, 0.5); // Posição da pupila
	glutSolidSphere(0.5, 20, 20); // Esfera menor preta
	glPopMatrix();

	// Boca do fantasma
	glPushMatrix();
	glTranslatef(0.0, 0.6, 0.8); // Posição da boca
	glColor3f(0.0, 0.0, 0.0);    // Preto para a boca
	glBegin(GL_POLYGON);
	for (int i = 0; i <= 180; i += 10) {
		float angle = i * M_PI / 180.0;
		glVertex3f(0.3 * cos(angle), -0.1 * sin(angle), 0.0);
	}
	glEnd();
	glPopMatrix();
}



void eixos() {
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f(-2, 0, 0);
	glVertex3f(2, 0, 0);

	glVertex3f(2, 0, 0);
	glVertex3f(1.7, 0.3, 0);
	glVertex3f(2, 0, 0);
	glVertex3f(1.7, -0.3, 0);


	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, -2, 0);
	glVertex3f(0, 2, 0);

	glVertex3f(0, 2, 0);
	glVertex3f(0.3, 1.7, 0);
	glVertex3f(0, 2, 0);
	glVertex3f(-0.3, 1.7, 0);


	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, -2);
	glVertex3f(0, 0, 2);

	glVertex3f(0, 0, 2);
	glVertex3f(0.3, 0, 1.7);
	glVertex3f(0, 0, 2);
	glVertex3f(-0.3, 0, 1.7);

	glEnd();
}

void desenharFantasma(int i) {
	glPushMatrix();

	glTranslatef(fantasmas[i].x * 2.0, fantasmas[i].y * 2.0, 2.0);

	glRotatef(fantasmas[i].theta[0], 1.0, 0.0, 0.0);
	glRotatef(fantasmas[i].theta[1], 0.0, 1.0, 0.0);
	glRotatef(fantasmas[i].theta[2], 0.0, 0.0, 1.0);

	if (fantasmas[i].tipo == 1) {
		Fantasma(1.0, 1.0, 1.0); 
	}
	else {
		Fantasma(0.6, 0.1, 0.8);
	}

	

	glPopMatrix();
}

void posicionarFantasma() {
	if (total_pos_livre > 0) {
		for (int i = 0; i < fantamas_total; i++) {
			Posicao* pos = posicaoAleatorio();

			if (pos != NULL) {
				fantasmas[i].x = pos->x;
				fantasmas[i].y = pos->y;
				printf(" Fantasma posicionado em: %.2f, %.2f\n", fantasmas[i].x, fantasmas[i].y);
			}
		}
	}
	else {
		printf("Nenhuma posição válida encontrada no mapa.\n");
	}
}


void desenhar_boneco_player(int i) {

	glPushMatrix();

	glTranslatef(players[i].xCubo * 2.0, players[i].yCubo * 2.0, 2.0);//0.2 pa pol mas pa riba

	if (ladoVirar == FRENTE)
		glRotatef(0, 0.0, 0.0, 1.0);
	else if (ladoVirar == TRAS)
		glRotatef(180, 0.0, 0.0, 1.0);
	else if (ladoVirar == ESQUERDA)
		glRotatef(-90, 0.0, 0.0, 1.0);
	else if (ladoVirar == DIREITA)
		glRotatef(90, 0.0, 0.0, 1.0);

	// Rotação do cubo
	glRotatef(players[i].theta[0], 1.0, 0.0, 0.0);
	glRotatef(players[i].theta[1], 0.0, 1.0, 0.0);
	glRotatef(players[i].theta[2], 0.0, 0.0, 1.0);


	desenhaBoneco2();

	glPopMatrix();
}

void posicionarJogador() {
	
	if (total_pos_livre > 0) {
		Posicao* pos = posicaoAleatorio();

		if (pos != NULL) {
			players[0].xCubo = pos->x;
			players[0].yCubo = pos->y;

			map[(int)players[0].xCubo][(int)players[0].yCubo] = JOGADOR_POSICAO_MAPA;

			printf("Jogador posicionado em: %.2f, %.2f\n", players[0].xCubo, players[0].yCubo);
		}
		//}
	}
	else {
		printf("Nenhuma posição válida encontrada no mapa.\n");
	}
}

void posicionarEntidades() {
	posicionarFantasma();
	posicionarJogador();
	
}

// PACMAN - MAP
void drawRawMap(char** m) {
	int i, j;
	int maxSize = xTabSize() > yTabSize() ? xTabSize() : yTabSize();
	glPushMatrix();

	for (i = 0; i < xTabSize(); i++) {
		for (j = 0; j < yTabSize(); j++) {
			glPushMatrix();
			glTranslatef(i * 2.0, j * 2.0, 0.0); // Posição no tabuleiro
			glScalef(0.8, 0.8, 0.8);           // Tamanho do cubo

			// Verificar a posição do jogador
			if ((int)players[0].xCubo == i && (int)players[0].yCubo == j) {
				glColor3f(1.0, 0.0, 0.0); // Vermelho para a casa onde está o jogador
			}
			else if (m[i][j] == 1) {
				glColor3f(0.6, 0.8, 1.0); // Azul claro para casas não visitadas
			}
			else if (m[i][j] == 2) {
				glColor3fv(colors[3]);    // Verde para casas já visitadas
			}
			else if (m[i][j] == 3) {
				glColor3fv(colors[2]);    // Amarelo para casas de perda
			}

			if (m[i][j] != 0) { // Desenha cubos apenas para casas válidas
				colorcube();
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
}


void restaurarPosicoesValidas() {///todas as posicoes
	for (int i = 0; i < total_pos_livre; i++) {
		int x = (int)posicoesValidas[i].x;
		int y = (int)posicoesValidas[i].y;
		map[x][y] = 1; // Restaura as posições válidas
	}
}

int verificarColisao(float jogadorX, float jogadorY) {
	for (int i = 0; i < fantamas_total; i++) {
		float diferencaX = jogadorX - fantasmas[i].x;
		float diferencaY = jogadorY - fantasmas[i].y;

		// Calcular o valor absoluto de forma explícita
		if (diferencaX < 0) {
			diferencaX = -diferencaX;
		}
		if (diferencaY < 0) {
			diferencaY = -diferencaY;
		}

		// Verificar colisão
		if (diferencaX <= 1.0 && diferencaY <= 1.0) {
			return 1; // Colisão detectada
		}
	}
	return 0;
}

void reiniciarJogo(int value) {
	// Reseta os estados globais
	venceu = 0;
	perdeu = 0;
	fimDoJogo = 0;
	jogoAbortado = 0; // Remove estado de jogo abortado
	pausa = 0;

	// Pontuação inicial
	point = 0;

	// Mensagem de debug no console
	printf("Reiniciando o jogo...\n");

	// Limpa todas as posições válidas no mapa
	restaurarPosicoesValidas();

	// Reinicia os fantasmas
	if (fantasmas != NULL) {
		free(fantasmas);
		fantasmas = NULL;
	}
	int_ghost(); // Recria os fantasmas

	// Reposiciona o jogador e fantasmas no mapa
	posicionarEntidades();

	// Atualiza a barra de status com pontuação inicial
	sprintf(buffer, "PONTUAÇÃO -> %i", point);
	statusBarPosition(&statusBar,-1, 1.5);
	updateStatusBarText(&statusBar,buffer);

	// Restaura a câmera para o estado inicial
	distCamara = 3.0;
	alfaLongitude = 0.0;
	alfaLatitude = 135.0;

	// Volta para a tela inicial
	tela = 1; // Mostra a tela inicial novamente
	glutPostRedisplay(); // Atualiza o display
}

//void display(void)
//{
//	int maxSize;
//
//	if (xTabSize() > yTabSize()) {
//		maxSize = xTabSize();
//	}
//	else {
//		maxSize = yTabSize();
//	}
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	x = distCamara * sin(alfaLongitude * M_PI / 180) * cos(alfaLatitude * M_PI / 180);
//	z = distCamara * cos(alfaLongitude * M_PI / 180) * cos(alfaLatitude * M_PI / 180);
//	y = distCamara * sin(alfaLatitude * M_PI / 180);
//	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
//
//	 Mostrar mensagem de vitória ou derrota durante 5 segundos
//	if (fimDoJogo == 1 || fimDoJogo == 2) {
//		glPushMatrix();
//		if (fimDoJogo == 1) {
//			glColor3f(1.0, 1.0, 0.0);
//		}
//		else {
//			glColor3f(1.0, 0.0, 0.0);
//		}
//
//		glRasterPos2f(-0.5, 0.0); // Centralizar na tela
//		char* message;
//		if (fimDoJogo == 1) {
//			message = "YOU WIN!";
//		}
//		else {
//			message = "PERDEU!";
//		}
//		for (int i = 0; message[i] != '\0'; i++) {
//			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
//		}
//		glPopMatrix();
//		glutSwapBuffers();
//		return; // Não desenhar mais nada até que o jogo reinicie
//	}
//
//	glPushMatrix();
//	glScalef(1.0 / maxSize, 1.0 / maxSize, 1.0 / maxSize);
//	glTranslatef(-(xTabSize() - 1), -(yTabSize() - 8), 7.0); // Centraliza o tabuleiro
//
//	glRotatef(-90, 1.0, 0.0, 0.0);
//
//	drawRawMap(map); // Desenhar o tabuleiro
//	desenharCubo(0); // Desenhar o jogador
//
//	for (int i = 0; i < totalFantasmas; i++) {
//		desenharFantasma(i);
//	}
//
//	glPopMatrix();
//	eixos();
//
//	statusBarRender();
//	glFlush();
//
//	glutSwapBuffers();
//}


void mouse(GLint button, GLint state, GLint x, GLint y)
{
	
		xRato = x;
		yRato = y;
	
}
void mousePress(int X, int Y)
{
	// Calcula a diferença do movimento do rato
	float deltaX = X - xRato;
	float deltaY = Y - yRato;

	// Atualiza longitude e latitude baseando-se no movimento do rato
	alfaLongitude += deltaX * angRato;
	alfaLatitude -= deltaY * angRato;

	// Atualiza posição anterior do rato
	xRato = X;
	yRato = Y;

	// Limita a latitude para evitar rotações extremas ao redor do eixo X
	if (alfaLongitude >= 360) alfaLongitude = 360;
	if (alfaLongitude <= -360) alfaLongitude = -360;

	if (alfaLatitude >= 45) alfaLatitude = 45;
	if (alfaLatitude <= 0) alfaLatitude = 0;

	printf("Longitude,Latitude =(%.2f,%.2f)\n", alfaLongitude, alfaLatitude);

	glutPostRedisplay();
}

void displayAbortMessage() {
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0); // Cor vermelha
	glRasterPos2f(-0.5, 0.0); // Posição centralizada
	char* message = "Jogo abortado!";
	for (int i = 0; message[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	}
	glPopMatrix();
	glFlush();
}
void displayInitialMessage() {
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-0.5, 0.0);
	char* message = "Pressione qualquer tecla para comecar";
	for (int i = 0; message[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	}
	glPopMatrix();
	glFlush();
}
void displayPauseMessage() {
	glPushMatrix();
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2f(-0.5, 0.0);
	char* message = "Jogo pausado. Pressione R para continuar.";
	for (int i = 0; message[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	}
	glPopMatrix();
	glFlush();
}
void displayEndMessage() {
	glPushMatrix();
	char* message;
	if (fimDoJogo == 1) {
		glColor3f(1.0, 1.0, 0.0); // Cor de vitória
		message = "Parabens! Voce venceu!";
		glRasterPos2f(-0.5, -0.2);
		char scoreMessage[50];
		sprintf(scoreMessage, "Pontuacao final: %d", point);
		for (int i = 0; scoreMessage[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreMessage[i]);
		}
	}
	else {
		glColor3f(1.0, 0.0, 0.0); // Cor de derrota
		message = "Voce perdeu! Tente novamente.";
		glRasterPos2f(-0.5, -0.2);
		char scoreMessage[50];
		sprintf(scoreMessage, "Pontuacao final: %d", point);
		for (int i = 0; scoreMessage[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreMessage[i]);
		}
	}
	glRasterPos2f(-0.5, 0.0); // Posição centralizada
	for (int i = 0; message[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	}

	
	glPopMatrix();
	glFlush();
}

// Função para verificar se uma posição é válida
int EPosicaoValida(int x, int y) {
	for (int i = 0; i < total_pos_livre; i++) {
		if (posicoesValidas[i].x == x && posicoesValidas[i].y == y) {
			if (posicoesValidas[i].livre == LIVRE) {
				posicoesValidas[i].livre = OCUPADA;  // Marca como ocupada
				return TRUE;  // Retorna 1 indicando que a posição foi validada e ocupada
			}
			else {
				return FALSE;  // Já está ocupada
			}
		}
	}
	return  FALSE;  // A posição não está na lista de válidas
}

void teclasEspeciais(int key, int x, int y) {
	if (fimDoJogo != 0) {
		return;
	}

	float xA = players[0].xCubo;
	float yA = players[0].yCubo;

	if (pausa == 0) {
		float moveX = 0.0f;
		float moveY = 0.0f;

		switch (key) {
		case GLUT_KEY_UP:
			if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
				moveY = 1.0f; // Frente no eixo Y
				ladoVirar = 0; // Frente
			}
			else if (alfaLongitude > 60 && alfaLongitude <= 150) {
				moveX = -1.0f; // Esquerda no eixo X
				ladoVirar = 2; // Esquerda
			}
			else if (alfaLongitude > 150 && alfaLongitude <= 250) {
				moveY = -1.0f; // Trás no eixo Y
				ladoVirar = 1; // Trás
			}
			else if (alfaLongitude < -60 && alfaLongitude >= -150) {
				moveX = 1.0f; // Direita no eixo X
				ladoVirar = 3; // Direita
			}
			else if (alfaLongitude < -150 && alfaLongitude >= -240) {
				moveY = -1.0f; // Trás no eixo Y
				ladoVirar = 1; // Trás
			}
			else if (alfaLongitude < -240 && alfaLongitude >= -330) {
				moveX = -1.0f; // Esquerda no eixo X
				ladoVirar = 2; // Esquerda
			}
			break;
		case GLUT_KEY_DOWN:
			if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
				moveY = -1.0f; // Trás no eixo Y
				ladoVirar = 1; // Trás
			}
			else if (alfaLongitude > 60 && alfaLongitude <= 150) {
				moveX = 1.0f; // Direita no eixo X
				ladoVirar = 3; // Direita
			}
			else if (alfaLongitude > 150 && alfaLongitude <= 250) {
				moveY = 1.0f; // Frente no eixo Y
				ladoVirar = 0; // Frente
			}
			else if (alfaLongitude < -60 && alfaLongitude >= -150) {
				moveX = -1.0f; // Esquerda no eixo X
				ladoVirar = 2; // Esquerda
			}
			else if (alfaLongitude < -150 && alfaLongitude >= -240) {
				moveY = 1.0f; // Frente no eixo Y
				ladoVirar = 0; // Frente
			}
			else if (alfaLongitude < -240 && alfaLongitude >= -330) {
				moveX = 1.0f; // Direita no eixo X
				ladoVirar = 3; // Direita
			}
			break;
		case GLUT_KEY_LEFT:
			if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
				moveX = -1.0f; // Esquerda no eixo X
				ladoVirar = 2; // Esquerda
			}
			else if (alfaLongitude > 60 && alfaLongitude <= 150) {
				moveY = -1.0f; // Trás no eixo Y
				ladoVirar = 1; // Trás
			}
			else if (alfaLongitude > 150 && alfaLongitude <= 250) {
				moveX = 1.0f; // Direita no eixo X
				ladoVirar = 3; // Direita
			}
			else if (alfaLongitude < -60 && alfaLongitude >= -150) {
				moveY = 1.0f; // Frente no eixo Y
				ladoVirar = 0; // Frente
			}
			else if (alfaLongitude < -150 && alfaLongitude >= -240) {
				moveX = 1.0f; // Direita no eixo X
				ladoVirar = 3; // Direita
			}
			else if (alfaLongitude < -240 && alfaLongitude >= -330) {
				moveY = -1.0f; // Trás no eixo Y
				ladoVirar = 1; // Trás
			}
			break;
		case GLUT_KEY_RIGHT:
			if ((alfaLongitude >= -60 && alfaLongitude <= 60) || (alfaLongitude > 250 && alfaLongitude <= 360) || (alfaLongitude < -330 && alfaLongitude >= -360)) {
				moveX = 1.0f; // Direita no eixo X
				ladoVirar = 3; // Direita
			}
			else if (alfaLongitude > 60 && alfaLongitude <= 150) {
				moveY = 1.0f; // Frente no eixo Y
				ladoVirar = 0; // Frente
			}
			else if (alfaLongitude > 150 && alfaLongitude <= 250) {
				moveX = -1.0f; // Esquerda no eixo X
				ladoVirar = 2; // Esquerda
			}
			else if (alfaLongitude < -60 && alfaLongitude >= -150) {
				moveY = -1.0f; // Trás no eixo Y
				ladoVirar = 1; // Trás
			}
			else if (alfaLongitude < -150 && alfaLongitude >= -240) {
				moveX = -1.0f; // Esquerda no eixo X
				ladoVirar = 2; // Esquerda
			}
			else if (alfaLongitude < -240 && alfaLongitude >= -330) {
				moveY = 1.0f; // Frente no eixo Y
				ladoVirar = 0; // Frente
			}
			break;
		}

		// Atualiza posição do jogador
		players[0].xCubo += moveX;
		players[0].yCubo += moveY;

		if (EPosicaoValida(players[0].xCubo, players[0].yCubo)) {
			if (map[(int)players[0].xCubo][(int)players[0].yCubo] == 1) {
				point += 5; // Incrementa a pontuação para casas não visitadas
				map[(int)players[0].xCubo][(int)players[0].yCubo] = 2; // Marca como visitada
			}
			PosicaoLivre(xA, yA); // Libera a posição anterior
		}
		else {
			// Restaura a posição anterior se for inválida
			players[0].xCubo = xA;
			players[0].yCubo = yA;
		}
	}

	sprintf(buffer, "PONTUACAO -> %i\n", point);

	// Atualiza a barra de status
	statusBarPosition(&statusBar, -1, 1.5);
	updateStatusBarText(&statusBar, buffer);

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if (tela) {
		tela = 0;
		point = 0; // Resetar pontuação
		glutPostRedisplay();
		return;
	}

	switch (key) {
	case '+':
		distCamara -= 0.1;
		glutPostRedisplay();
		break;
	case '-':
		distCamara += 0.1;
		glutPostRedisplay();
		break;
	case 'A':
	case 'a':
		restaurarPosicoesValidas();
		posicionarEntidades();
		glutPostRedisplay();
		break;
	case 'P':
	case 'p':
		pausa = 1;
		break;

	case 'o':
	case 'O':
		rotacaoCabecaAtiva = 1;
		glutPostRedisplay();
		break;
	case 'i':
	case 'I':
		rotacaoCabecaAtiva = 0;
		glutPostRedisplay();
		break;
	case 'R':
	case 'r':
		pausa = 0;
		break;

		glutPostRedisplay();
	case 'f':
	case 'F':
		printf("Jogo abortado pelo usuário.\n");
		jogoAbortado = 1; // Marca o jogo como abortado
		glutTimerFunc(2000, reiniciarJogo, 0); // Após 2 segundos, reinicia o jogo
		break;
	}
	
}
int verificarColisaoFantasma(float x, float y, int idAtual) {
	for (int i = 0; i < fantamas_total; i++) {
		if (i != idAtual && fantasmas[i].x == x && fantasmas[i].y == y) {
			return 1; // Posição já ocupada por outro fantasma
		}
	}
	return 0; // Posição livre
}


void atualizarMovimentoFantasma(int i) {
	float jogadorX = players[0].xCubo;
	float jogadorY = players[0].yCubo;

	float fantasmaX = fantasmas[i].x;
	float fantasmaY = fantasmas[i].y;

	int tentativa = 0;

	while (tentativa < total_pos_livre) {
		float novoX = fantasmaX;
		float novoY = fantasmaY;

		if (fantasmas[i].tipo == 1) { // Fantasma inteligente
			float deltaX = jogadorX - fantasmaX;
			float deltaY = jogadorY - fantasmaY;

			if (fabs(deltaX) > fabs(deltaY)) {
				novoX += (deltaX > 0) ? 1.0 : -1.0;
			}
			else {
				novoY += (deltaY > 0) ? 1.0 : -1.0;
			}
		}
		else { // Fantasma normal
			int direcao = rand() % 4;
			switch (direcao) {
			case 0: novoY += 1.0; break; // Cima
			case 1: novoY -= 1.0; break; // Baixo
			case 2: novoX -= 1.0; break; // Esquerda
			case 3: novoX += 1.0; break; // Direita
			}
		}

		// Verifica se a nova posição é válida e sem colisão
		if (EPosicaoValida(novoX, novoY) && !verificarColisaoFantasma(novoX, novoY, i)) {
			PosicaoLivre(fantasmaX, fantasmaY);
			fantasmas[i].x = novoX;
			fantasmas[i].y = novoY;

			// Verifica se o fantasma colidiu com o jogador
			if (fabs(novoX - jogadorX) < 1.0 && fabs(novoY - jogadorY) < 1.0) {
				fimDoJogo = 2; // Jogador perdeu
				printf("Colisão com jogador! Jogo terminado.\n");
			}

			break;
		}
		else {
			tentativa++;
		}
	}

	if (tentativa >= total_pos_livre) {
		printf("Fantasma %d não conseguiu se mover.\n", i);
	}
}



void update(int value) {
	if (fimDoJogo == 0 && pausa == 0) { // Apenas atualiza se o jogo estiver em andamento
		for (int i = 0; i < fantamas_total; i++) {
			atualizarMovimentoFantasma(i); // Atualiza a posição dos fantasmas
		}

		// Atualiza o ângulo da cabeça
		anguloCabeca += velocidadeRotacaoCabeca;
		if (anguloCabeca >= 360.0) {
			anguloCabeca -= 360.0; // Mantém o ângulo dentro de 0 a 360
		}

		// Verificar colisão entre jogador e fantasmas
		if (verificarColisao(players[0].xCubo, players[0].yCubo)) {
			fimDoJogo = 2; // Estado de derrota
			glutTimerFunc(tempoExibicaoFim, reiniciarJogo, 0); // Configura reinício após 5 segundos
			return;
		}

		// Verificar vitória
		int ganhou = 1;
		for (int i = 0; i < xTabSize(); i++) {
			for (int j = 0; j < yTabSize(); j++) {
				if (map[i][j] == 1) { // Ainda existem casas não visitadas
					ganhou = 0;
					break;
				}
			}
			if (!ganhou) break;
		}

		if (ganhou) {
			fimDoJogo = 1; // Estado de vitória
			glutTimerFunc(tempoExibicaoFim, reiniciarJogo, 0); // Configura reinício após 5 segundos
			return;
		}
	}

	glutPostRedisplay(); // Atualiza o display
	glutTimerFunc(TIME, update, TIME); // Continua o loop de atualização
}

void atualizarCabeca(int value) {
	if (rotacaoCabecaAtiva) { // Só atualiza se estiver ativo
		anguloCabeca += velocidadeRotacaoCabeca;
		if (anguloCabeca >= 360.0) {
			anguloCabeca -= 360.0; // Mantém o ângulo dentro de 0 a 360
		}

		// Redesenhar a tela
		glutPostRedisplay();
	}

	// Reconfigurar o timer para continuar chamando a função
	glutTimerFunc(TIME_CABECA, atualizarCabeca, 0); // Atualiza a cada 100ms
}


void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glFrustum(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w, 2.0 * (GLfloat)h / (GLfloat)w, 2.0, 20.0);
	else
		glFrustum(-2.0 * (GLfloat)w / (GLfloat)h, 2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, 2.0, 20.0);
	/* gluPerspective(45.0, w / h, -10.0, 10.0); */

	glMatrixMode(GL_MODELVIEW);
}

void iniciarJogo() {
	PosicoesValidas();      // Inicializa as posições válidas no mapa
	int_ghost(); // Inicializa os fantasmas
	posicionarEntidades();  // Posiciona o jogador e os fantasmas
}
void display(void)
{
	int maxSize;

	if (xTabSize() > yTabSize()) {
		maxSize = xTabSize();
	}
	else {
		maxSize = yTabSize();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	x = distCamara * sin(alfaLongitude * M_PI / 180) * cos(alfaLatitude * M_PI / 180);
	z = distCamara * cos(alfaLongitude * M_PI / 180) * cos(alfaLatitude * M_PI / 180);
	y = distCamara * sin(alfaLatitude * M_PI / 180);
	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	if (tela) {
		displayInitialMessage();
	}
	else if (pausa) {
		displayPauseMessage();
	}
	else if (jogoAbortado) {
		displayAbortMessage();
	}
	else if (fimDoJogo !=0) {
		displayEndMessage();
	}
	else {

		glPushMatrix();
		glScalef(1.0 / maxSize, 1.0 / maxSize, 1.0 / maxSize);
		glTranslatef(-(xTabSize() - 1), -(yTabSize() - 8), 7.0); // Centraliza o tabuleiro

		glRotatef(-90, 1.0, 0.0, 0.0);

		drawRawMap(map); // Desenhar o tabuleiro
		desenhar_boneco_player(&player,) // Desenhar o jogador

		for (int i = 0; i < fantamas_total; i++) {
			desenharFantasma(i);
		}

		glPopMatrix();
		eixos();

		drawStatusBar(&statusBar);
		glFlush();
		
	}
	
	glutSwapBuffers();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);
	glutCreateWindow("Pac-Man");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);

	glutSpecialFunc(teclasEspeciais);
	glutKeyboardFunc(keyboard);

	glutMouseFunc(mouse);
	glutMotionFunc(mousePress);

	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	srand(time(NULL));

	init_player();

	// PACMAN - MAP
	if (argc <= 1) {        // checking if the user give the map file
		printf("Please run as:\n");
		printf("%s <map file>", argv[0]);
		exit(0);
	}
	printf("Reading map...\n");
	openMapsFile(argv[1]);       // Opem map file 

	map = nextMap();             // read the first map from 
	map = nextMap();
	iniciarJogo();
	glutPostRedisplay();
	printMap();


	glutTimerFunc(TIME, update, TIME);

	glutTimerFunc(100, atualizarCabeca, 0); 

	glutMainLoop();
}