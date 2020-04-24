#define _CRT_SECURE_NO_WARNINGS
#define _SEMENTE_ 20200502 
 
#include    "limits.h"
#include    "locale.h"
#include    "malloc.h"
#include	"memory.h"
#include	"stdio.h"
#include	"stdlib.h"
#include	"string.h"
#include	"time.h"
#include    "windows.h"

struct fpa_info
{
	char	evento[30];  // nome do evento
	char	grupo;       // a, b ou c
	char	momento[50]; // dd/mm/aa hh:mm:ss.nnnnn
	char	prioridade;  // 0: alta, 1: media, 2: baixa
};
typedef struct fpa_info Evento;

int     compara_eventos(const void*, const void*); // pra por em ordem de ex.
Evento* cria_um_evento();
char*   get_time(char*);
void    trata_um_ciclo(Evento**, int);
int     cls();

int	main(int argc, char** argv)
{
	setlocale(LC_ALL, "Portuguese");
	srand(20200422);
	int N = 0; // total de amostras
	int E = 0; // total de eventos
	Evento* eventos = NULL;

	if (argc > 1)	// veio algo na linha de comando
	{
		N = atoi(argv[1]);
		if (N > 60) N = 60;
	};	// end if
	if (N == 0) N = 3; // padrao de 3 ciclos apenas. use a linha de comando
	printf("\n\n***** Nessa simulacao: %d [Max 60] ciclos com ate 20 eventos cada\n\n\n", N);
	srand(_SEMENTE_);
	int total = 0;
	int etapas = 0;
	for (int i = 0; i < N; i = i + 1)
	{
		etapas = (rand() % 20) + 1;
		printf("\n***** Ciclo %2d de %2d vai ter %2d eventos\n", i + 1, N, etapas);
		Evento* e = (Evento*)malloc(sizeof(Evento*) * etapas);
		Evento** p = (Evento**)e;
		for (int i = 0; i < etapas; i += 1)
			*(i + p) = cria_um_evento();
		trata_um_ciclo(p, etapas);
		// agora apaga os ponteiros
		free(e);
		total += etapas;
		printf("\nTecle ENTER para executar o proximo ciclo ");
		getchar();
		cls();
		// em p está a lista de eventos para processar
	};	// for()
	printf("Final: processados %d eventos em %d ciclos\n", total, N);
	return 0;
}	// end main()

int		    cls()
{	// limpa a tela no windows, do jeito oficial
	CONSOLE_SCREEN_BUFFER_INFO		info;
	HANDLE		H = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD		origem = { 0,0 };
	int			total;
	if (H == INVALID_HANDLE_VALUE) return -1;
	GetConsoleScreenBufferInfo(H, &info);
	int r = FillConsoleOutputCharacter(H, (TCHAR)' ',
		info.dwSize.X * info.dwSize.Y,
		origem, &total);
	int s = FillConsoleOutputAttribute(
		H, info.wAttributes,
		info.dwSize.X * info.dwSize.Y,
		origem, &total);
	SetConsoleCursorPosition(H, origem);
	return 0;
};	// end cls()

int         compara_eventos(const void* evento1, const void* evento2)
{
	// comparar os eventos eh muito simples
	// a gente quer classificar por prioridade DECRESCENTE e 
	// quando a prioridade for igual por hora de entrada
	Evento** p1 = (Evento**)evento1;
	Evento* e1 = *p1;
	Evento** p2 = (Evento**)evento2;
	Evento* e2 = *p2;
	if (e2->prioridade > e1->prioridade) return 1;  // inverte esse 
	if (e2->prioridade < e1->prioridade) return -1; // ok o segundo e menor
	// prioridades iguais: classifica por horario comparando as datas
	return strcmp(e1->momento, e2->momento); // ordem crescente aqui
};  // compara_eventos()

Evento*     cria_um_evento()
{
	// a cada chamada essa rotina devolve um evento
	// pronto para uso
	// para tornar a vida mais simples a prioridade
	// esta embutida no tipo
	const char tipo[9][30] =
	{
		"6Colisao",
		"5Atualizacao de Tela",
		"4Particulas",
		"3Logica do Jogo",
		"3Teclado",
		"2Inteligencia Artificial",
		"2Mouse",
		"1Simulacao Fisica",
		"1Internet"
	}; // 9 tipos de evento
	const char categorias[3] = { 'a', 'b', 'c' };
	Evento* E = (Evento*)malloc(sizeof(Evento));
	// criou o evento 'E'. Agora preenche os campos
	get_time(E->momento); // marca a hora
	int i = rand() % 9; // o tipo de 0 a 8
	strncpy(E->evento, &tipo[i][1], strlen(tipo[i]));
	E->prioridade = tipo[i][0];
	E->grupo = categorias[i / 3];
	return E;
};	// cria_evento()

char*       get_time(char* timestamp)
{
	struct timespec time;
	timespec_get(&time, TIME_UTC);
	char dmahms[50];
	strftime(dmahms, sizeof(dmahms), "%y/%m/%d %H:%M:%S", localtime(&time.tv_sec));
	sprintf(timestamp, "%s.%d", dmahms, time.tv_nsec / 10000);
	return timestamp;
};	// get_time()

void        trata_um_ciclo(Evento** Ev, int N)
{
	printf("\ntrata_um_ciclo(): %d eventos:\n\n", N);
	for (int i = 0; i < N; i = i + 1)
		printf("[%s] %2d: %25s, prioridade %c grupo %c\n",
			Ev[i]->momento,
			i+1,
			Ev[i]->evento, 
			Ev[i]->prioridade,
			Ev[i]->grupo);
	// para ficar mais legivel:
	// os caras estao em Ev[]
	// o total deles: N
	// o tamanho de cada um sizeof(Evento*), tanto faz. E um ponteiro
	Evento* eventos = (Evento*) &Ev[0];
	qsort(eventos, N, sizeof(Evento*), compara_eventos);
	printf("\nNa ordem de execucao:\n\n");
	for (int i = 0; i < N; i = i + 1)
	{
		printf("[%s] %2d: %25s, prioridade %c grupo %c\n",
			Ev[i]->momento,
			i + 1,
			Ev[i]->evento,
			Ev[i]->prioridade,
			Ev[i]->grupo);
		free(Ev[i]);  // ja pode apagar o evento
	};  // for() 
	return;
};

// fim do texto