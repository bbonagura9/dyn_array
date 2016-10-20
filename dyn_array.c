#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
  Macro para valor nulo de nó do array 
*/

#define DYN_ARRAY_NULL_DATA (union dyn_array_data) ((char *) NULL)

/*
  Union que armazena cada valor do array dinâmico
*/
union dyn_array_data {
	int i;
	double d;
	char *s;
};

/*
  Nós da lista encadeada que armazena o array dinâmico. O uso da
  lista encadeada se deve ao fato do array ter tamanho variável,
  podendo crescer indefinidamente.
*/
typedef struct dyn_array_nodes {
	union dyn_array_data data;
	struct dyn_array_nodes *next;
} dyn_array_node;

/*
  Estrutura que representa o array dinâmico com ponteiros para o 
  primeiro e último nó, além de um contador de nós. A função do
  ponteiro para o último nó é otimizar o processo de inserção e
  busca de nós, para grandes números de nós. Sem este ponteiro
  toda inserção precisaria percorrer toda a lista para encontrar
  seu final, sendo então O(n), com ela a complexidade é O(1). O 
  contador de nós tem funcionalidade similar, mas serve para a 
  verficação de índices no get e set.

  Obs: a lista encadeada sempre tem um "nó âncora" vazio como
       nó inicial para facilitar as implementações
*/
typedef struct dyn_arrays {
	dyn_array_node *first;
	dyn_array_node *last;
	int node_count;
} dyn_array;

// Cabeçalhos das funções internas 
dyn_array_node *dyn_array_new_node();
dyn_array_node *dyn_array_get_node(int index, dyn_array *array);

// Cabeçalhos das funções externas
dyn_array *dyn_array_init();
void dyn_array_add(union dyn_array_data value, dyn_array *array);
union dyn_array_data dyn_array_get(int index, dyn_array *array);
void dyn_array_set(union dyn_array_data value, int index, dyn_array *array);

/*
  Alloca um novo nó na memória, com lixo nos dados
  e ponteiro para o próximo como nulo
  Retorna: ponteiro para o novo nó
*/
dyn_array_node *dyn_array_new_node() {
	dyn_array_node *p = (dyn_array_node*) malloc(sizeof(dyn_array_node));
	p->next = NULL;
	return p;
}

/*
  Busca nó na posição index do array
  Parâmetros: index = índice do nó, começando do zero
              array = ponteiro para o array dinâmico
  Retorno: ponteiro para o nó, nulo em caso de erro
*/
dyn_array_node *dyn_array_get_node(int index, dyn_array *array) {
	int i;
	dyn_array_node *p; 

	if(index < 0 || index > array->node_count || array == NULL) {
		return NULL;
	}

	// Desloca ponteiro p até o nó na posição index
	p = array->first;
	for(i = 0; i <= index; i++) {
		p = p->next;
	}

	return p;
}

/*
  Inicializa um novo array dinâmico vazio, alocando na memória
  e definindo os valores iniciais e nó âncora
  Retorno: ponteiro o novo array dinâmico
*/
dyn_array *dyn_array_init() {
	dyn_array *a = (dyn_array*) malloc(sizeof(dyn_array));
	a->node_count = 0;
	// nó âncora
	a->first = dyn_array_new_node();
	a->last = a->first;
	return a;
}

/*
  Adiciona novo valor ao final do array dinâmico
  Parâmetros: value = valor a ser adicionado
              array = ponteiro para o array dinâmico
*/
void dyn_array_add(union dyn_array_data value, dyn_array *array) {
	dyn_array_node *p = array->last;
	p->next = dyn_array_new_node();
	p = p->next;
	p->data = value;
	array->last = p;
	array->node_count++;
}

/*
  Busca valor na posição index do array
  Parâmetros: index = índice do valor, começando do zero
              array = ponteiro para o array dinâmico
  Retorno: valor buscado, ou nulo em caso de erro
*/
union dyn_array_data dyn_array_get(int index, dyn_array *array) {
	dyn_array_node *p = dyn_array_get_node(index, array);
	if(p == NULL) {
		return DYN_ARRAY_NULL_DATA;
	} else {
		return p->data;
	}
}

/*
  Atribui valor à posição index do array dinâmico
  Parâmetros: value = valor a ser atribuido
              index = índice do valor, começando do zero
              array = ponteiro para o array dinâmico
*/
void dyn_array_set(union dyn_array_data value, int index, dyn_array *array) {
	dyn_array_node *p = dyn_array_get_node(index, array);

	if(p == NULL) {
		return;
	} else {
		p->data = value;
	}
}

/*
	Entry point. Executa testes descritos.
*/
int main() {
	printf("Testando criação de array, inserção e busca de valores inteiros\n");
	dyn_array *a = dyn_array_init();
	int i; 
	union dyn_array_data v;
	for(i = 0; i < 10; i++) {
		v.i  = i*i;
		dyn_array_add(v, a);
	}

	int has_error = 0;
	for(i = 0; i < 10; i++) {
		v = dyn_array_get(i, a);
		if(v.i != i*i) {
			printf("Erro no indice %d, valor %d deveria ser %d", i, v.i, i*i);
			has_error = 1;
		}
	}
	if(has_error == 0) printf("OK!\n");
	printf("-----\n");

	printf("Testando buscas e atribuições com índices invalidos\n");

	if(dyn_array_get(-1, a).s == NULL) 
		printf("Get negative index OK!\n");

	if(dyn_array_get(100, a).s == NULL)
		printf("Get out of bounds index OK!\n");

	v.i = 0;
	dyn_array_set(v, -1, a);
	dyn_array_set(v, 1000, a);

	printf("-----\n");
	printf("Testando atribuição e busca de valores double\n");

	for(i = 0; i < 10; i++) {
		v.d  = 3.141593 * i * i;
		dyn_array_set(v, i, a);
	}

	has_error = 0;
	for(i = 0; i < 10; i++) {
		v = dyn_array_get(i, a);
		if(v.d - 3.141593 * i * i > 0.0001) {
			printf("Erro no indice %d, valor %.3f deveria ser %.3f", i, v.d, 3.141593 * i * i);
			has_error = 1;
		}
	}
	if(has_error == 0) printf("OK!\n");
	printf("-----\n");

	printf("Testando atribuição e busca de valores string\n");

	char strings[][20] = {
		"Valor 1",
		"Segundo valor",
		"Terceiro",
		"quarto",
		"Quinto valor",
		"Sexto",
		"7o",
		"Oitavo",
		"nono",
		"ultimo, decimo"
	};

	for(i = 0; i < 10; i++) {
		v.s = strings[i];
		dyn_array_set(v, i, a);
	}

	has_error = 0;
	for(i = 0; i < 10; i++) {
		v = dyn_array_get(i, a);
		if(strcmp(v.s, strings[i])) {
			printf("Erro no indice %d, valor '%s' deveria ser '%s'", i, v.s, strings[i]);
			has_error = 1;
		}
	}
	if(has_error == 0) printf("OK!\n");
	printf("-----\n");

	printf("Testando recriação de array, inserção e busca de valores string\n");

	free(a);
	a = dyn_array_init();
	for(i = 0; i < 10; i++) {
		v.s = strings[i];
		dyn_array_add(v, a);
	}

	has_error = 0;
	for(i = 0; i < 10; i++) {
		v = dyn_array_get(i, a);
		if(strcmp(v.s, strings[i])) {
			printf("Erro no indice %d, valor '%s' deveria ser '%s'", i, v.s, strings[i]);
			has_error = 1;
		}
	}
	if(has_error == 0) printf("OK!\n");
	printf("-----\n");

	return 0;
}