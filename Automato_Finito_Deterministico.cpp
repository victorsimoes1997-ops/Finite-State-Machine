#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdio.h>

struct Estado
{
    int id;
    bool aceitacao;
};

struct Grafo
{
    int nro_vert;
    int Gmax;
    int* grau;
    int* aresta;
    char* peso;
    Estado* estado;
};

Grafo* criarGrafo(int nro_vert, int Gmax, int peso, Estado estado)
{
    Grafo* gr = new Grafo;
    
    gr->nro_vert = nro_vert;
    gr->Gmax = Gmax;
    
    gr->grau = new int[nro_vert];
    for(int i = 0; i < nro_vert; i++)
    {
        gr->grau[i] = 0;
    }
    
    gr->aresta = new int[nro_vert * Gmax];
    
    gr->peso = new char[nro_vert * Gmax];
    for(int i = 0; i < nro_vert; i++)
    {
        gr->peso[i] = 0;
    }
    
    gr->estado = new Estado[nro_vert];
    for(int i = 0; i < nro_vert; i++)
    {
        gr->estado[i].id = 0;
        gr->estado[i].aceitacao = false;
    }
    
    return gr;
}

void destruirGrafo(Grafo* gr)
{
    if(!gr) return;
    
    delete[] gr->grau;
    delete[] gr->aresta;
    delete[] gr->estado;
    delete[] gr->peso;
    
    delete gr;
    
    gr = nullptr;
}

void inserirAresta(Grafo* gr, int origem, int destino, char peso, bool digrafo)
{
    if(!gr) return;
    if(origem < 0 || origem >= gr->nro_vert) return;
    if(destino < 0 || destino >= gr->nro_vert) return;
    if(gr->grau[origem] >= gr->Gmax) return;
    
    int indiceIda = (gr->Gmax * origem) + gr->grau[origem];
    gr->aresta[indiceIda] = destino;
    gr->grau[origem]++;
    gr->peso[indiceIda] = peso;
    
    if(!digrafo) inserirAresta(gr, destino, origem, peso, true);
}

void removerAresta(Grafo* gr, int origem, int destino, char peso, bool digrafo)
{
    if(!gr) return;
    if(origem < 0 || origem >= gr->nro_vert) return;
    if(destino < 0 || destino >= gr->nro_vert) return;
    if(gr->grau[origem] >= gr->Gmax) return;
    
    int baseIda = origem * gr->Gmax;
    int i = 0;
    
    while(i < gr->grau[origem] && gr->aresta[baseIda + i] != destino) i++;
    
    if(i == gr->grau[origem])
    {
        if(!digrafo) removerAresta(gr, destino, origem, peso, true);
        return;
    }
    
    for(int j = i; j < gr->grau[origem] - 1; j++)
    {
        gr->aresta[baseIda] = gr->aresta[baseIda + 1];
    }
    gr->grau[origem]--;
    
    for(int j = i; j < gr->grau[origem] - 1; j++)
    {
        gr->peso[baseIda] = gr->peso[baseIda + 1];
    }
    
    if(!digrafo) removerAresta(gr, destino, origem, peso, true);
}

void adicionarTransicao(Grafo* gr, int origem, int destino, char simbolo)
{
    inserirAresta(gr, origem, destino, simbolo, true);
}

void imprimirGrafo(Grafo* gr)
{
    if(!gr) return;
    
    for(int v = 0; v < gr->nro_vert; v++)
    {
        std::cout << "Vértice: " << v << " (Aceitação: " << (gr->estado[v].aceitacao ? "Sim" : "Não") << ")\n";
        
        for(int a = 0; a < gr->grau[v]; a++)
        {
            int indice = (v * gr->Gmax) + a; 
            int destino = gr->aresta[indice];
            char p = gr->peso[indice];
            
            std::cout << "  -> Aresta para: " << destino << " [com simbolo: '" << p << "']\n";
        }
    }
}

char nextChar(const std::string& cadeia, int &indice)
{
    if(indice < cadeia.length())
    {
        char atual = cadeia[indice];
        indice++;
        return atual;
    }
    
    return '\0';
}


bool funcaoTransicao(Grafo* gr, const std::string& cadeia)
{
    Grafo* atual = gr;
    int indice = 0;
    int estadoAtual = 0;
    char c;
    
    while((c = nextChar(cadeia, indice)) != '\0')
    {
        bool encontrou = false;
        for(int i  = 0; i < gr->grau[estadoAtual]; i++)
        {
            int deslocamento = (estadoAtual * gr->Gmax) + i;
            if(gr->peso[deslocamento] == c)
            {
                estadoAtual = gr->aresta[deslocamento];
                encontrou = true;
                break;
            }
        }
        
        if(!encontrou) return false;
    }
    
    return gr->estado[estadoAtual].aceitacao;
}

int main() 
{
    Grafo* gr = new Grafo;
    gr->nro_vert = 4; // 0, 1, 2, 3
    gr->Gmax = 2;     // 2 transições possíveis por estado ('0' e '1')

    // Alocações na Heap
    gr->grau = new int[4]{0, 0, 0, 0};
    gr->aresta = new int[8]; 
    gr->peso = new char[8];   
    
    gr->estado = new Estado[4];
    for(int i = 0; i < 4; i++) 
    {
        gr->estado[i].id = i;
        gr->estado[i].aceitacao = (i == 3); // Apenas o Estado 3 é aceitação
    }

    // --- Cadastrando as Transições com a API Limpa ---

    // Estado 0 (Início)
    adicionarTransicao(gr, 0, 1, '0');
    adicionarTransicao(gr, 0, 0, '1');

    // Estado 1 (Viu "0")
    adicionarTransicao(gr, 1, 2, '1');
    adicionarTransicao(gr, 1, 0, '0');

    // Estado 2 (Viu "00")
    adicionarTransicao(gr, 2, 0, '1');
    adicionarTransicao(gr, 2, 3, '0');

    // Estado 3 (Final - Viu "010")
    adicionarTransicao(gr, 3, 1, '0');
    adicionarTransicao(gr, 3, 2, '1');

    imprimirGrafo(gr);

    // Bateria de Testes
    std::string testes[] = {"00110010001110011111101011110101100010000111", "00101"};

    std::cout << "--- RESULTADOS DO AUTÔMATO (Termina em '010') ---\n";
    for (const std::string& cadeia : testes) 
    {
        bool resultado = funcaoTransicao(gr, cadeia);
        std::cout << "Cadeia: \"" << cadeia << "\" -> " 
                  << (resultado ? "ACEITA" : "REJEITADA") << "\n";
    }
    
    destruirGrafo(gr);
    return 0;
}
