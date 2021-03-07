/*
  Projeto pratico de Estrutura de dados 
     (Estrutura de Dados)
  Sequence Set com Arvore B+ de indices
     para armazenamentos de pacotes de rede
  Copyright 2020 by Ani,Frederico,
     Guilherme
 
  Este e o arquivo de implementacao da classe Bmais, estrutura principal do projeto que contem
     um ponteiro para o noh raiz da arvore de indices
     e um ponteiro para o sequenceSet (estrutura que armazena os dados em disco)
*/

#include <iostream>
#include <fstream>
#include <string.h>
#include "Bmais.h"
#include "Dado.h"
#include "Pagina.h"
#include "Key.h"
#include "Noh.h"
#include "sequenceSet.h"
using namespace std;

//se o sequenceSet existe com mais de uma pagina, faz leitura da raiz e de seus respectivos filhos
//caso contrario, apenas deixa a raiz como nula
Bmais::Bmais(string nome)
{

    nomeArqBmais = nome + "INDEX.txt";
    nome += ".txt";
    sequenceSet *aux;
    aux = new sequenceSet(nome);
    set = aux;

    fstream arq(nomeArqBmais.c_str(), ios::in | ios::out | ios::binary);
    raiz = NULL;
    if (!arq)
    { //se nao houver arquivo, cria um
        ofstream arq(nomeArqBmais.c_str(), ios::binary);
    }
    else
    {
        if (set->cab.nSequencia > 1)
        {
            raiz = new noh(); //cria a raiz como folha
            arq.read((char *)&(raiz->folha), sizeof(int));
            arq.read((char *)&(raiz->quantidade), sizeof(int));
            arq.read((char *)(raiz->keys), sizeof(key) * ORDEM);
            arq.read((char *)(raiz->posFilho), sizeof(int) * (ORDEM + 1));
        }
    }
    arq.close();

    if (raiz != NULL)
        lerFilhosArq(raiz);
}

//faz leitura de todos os noh recursivamente do arquivo para memoria
void Bmais::lerFilhosArq(noh *umNoh)
{
    if (umNoh->folha == 0)
    {

        int tamNoh = sizeof(int) + sizeof(int) + sizeof(key) * (ORDEM) + sizeof(int) * (ORDEM + 1);
        //folha + quantidade + chaves + posFilho
        fstream arq(nomeArqBmais.c_str(), ios::in | ios::out | ios::binary);
        noh *novo;
        for (int i = 0; i < umNoh->quantidade + 1; i++)
        {

            novo = new noh();
            arq.seekg(tamNoh * umNoh->posFilho[i]);

            arq.read((char *)&(novo->folha), sizeof(int));
            arq.read((char *)&(novo->quantidade), sizeof(int));
            arq.read((char *)(novo->keys), sizeof(key) * ORDEM);
            arq.read((char *)(novo->posFilho), sizeof(int) * (ORDEM + 1));
            novo->pai = umNoh;
            umNoh->filhos[i] = novo;
        }
        arq.close();
        for (int i = 0; i < umNoh->quantidade + 1; i++)
        {
            lerFilhosArq(umNoh->filhos[i]);
        }
    }
}

//arquiva a arvore num arquivo
//destroi todos os noh's da arvore
Bmais::~Bmais()
{
    if (raiz != NULL)
    {

        int num = 1;
        if (!raiz->folha)
        { //se nao for folha atualiza a posicao dos filhos
            for (int i = 0; i < raiz->quantidade + 1; i++)
            {
                raiz->posFilho[i] = posFilhosArq(raiz->filhos[i], &num);
                //atualiza a posicao dos filhos na raiz
            }
        }
        fstream arq(nomeArqBmais.c_str(), ios::in | ios::out | ios::binary);
        arq.write((char *)&(raiz->folha), sizeof(int));
        arq.write((char *)&(raiz->quantidade), sizeof(int));
        arq.write((char *)(raiz->keys), sizeof(key) * ORDEM);
        arq.write((char *)(raiz->posFilho), sizeof(int) * (ORDEM + 1));
        //escreve raiz no arquivo
        arq.close();

        if (!raiz->folha)
            for (int i = 0; i < raiz->quantidade + 1; i++)
                escreveFilhosArq(raiz->filhos[i], raiz->posFilho[i]);
    }
    if (raiz != NULL)
    {
        destruirNoh(raiz);
    }
}

//a partir de um noh, destroi todos filhos
void Bmais::destruirNoh(noh *umNoh)
{
    for (int i = 0; i < umNoh->quantidade + 1; i++)
    {
        if (umNoh->filhos[i] != NULL)
        {
            destruirNoh(umNoh->filhos[i]);
        }
    }
    delete umNoh;
}

//atualiza a posicao de cada filho em um noh
int Bmais::posFilhosArq(noh *umNoh, int *num)
{
    int posAtual;
    posAtual = *num;
    *num += 1;
    if (!umNoh->folha)
    {

        for (int i = 0; i < umNoh->quantidade + 1; i++)
        {
            umNoh->posFilho[i] = posFilhosArq(umNoh->filhos[i], num);
        }
    }

    return posAtual;
}

//com as posicoes dos filhos atualizadas, escreve os noh's no arquivo
void Bmais::escreveFilhosArq(noh *umNoh, int num)
{
    fstream arquivo(nomeArqBmais.c_str(), ios::in | ios::out | ios::binary);
    int tamNoh = sizeof(int) + sizeof(int) + sizeof(key) * (ORDEM) + sizeof(int) * (ORDEM + 1);
    //folha + quantidade + chaves + posFilho
    arquivo.seekg(num * tamNoh);
    arquivo.write((char *)&(umNoh->folha), sizeof(int));
    arquivo.write((char *)&(umNoh->quantidade), sizeof(int));
    arquivo.write((char *)(umNoh->keys), sizeof(key) * ORDEM);
    arquivo.write((char *)(umNoh->posFilho), sizeof(int) * ORDEM + 1);

    if (!umNoh->folha)
    {
        for (int i = 0; i < umNoh->quantidade + 1; i++)
            escreveFilhosArq(umNoh->filhos[i], umNoh->posFilho[i]);
    }
    arquivo.close();
}

//insere um dado(se nao ja nao estiver arquivado) no sequenceSet
//faz as operacoes necessarias na arvore
void Bmais::inserir(Dado &d)
{

    Pagina p, nova, antP, nextP;
    int posP = set->cab.pSequencia;
    int posNova = set->cab.proxPaginaDisponivel;

    if (set->vazio())
    {
        //nao ha nenhum dado cadastrado

        set->cab.nSequencia = 1;
        set->cab.pSequencia = 0;
        //atualiza cabecalho da pagina

        p.insereDado(d);
        //insere o dado na pagina
        //sobreescreve o arquivo no caso de ter todos os dados apagados
        set->cab.proxPaginaDisponivel = 1;
        ofstream arq(set->nomeArq.c_str(), ios::binary);

        arq.write((char *)&(set->cab), sizeof(Cabecalho));
        arq.write((char *)&(p), sizeof(Pagina));
        //insere o dado com cabecalho atualizado

        Pagina vazia;
        arq.write((char *)&(vazia), sizeof(Pagina));
        //insere uma pagina vazia para nao ler partes vazias do arquivo

        arq.close();
    }
    else
    {
        posP = buscaAux(raiz, d);
        set->lerPaginaArquivo(&p, posP);
        if (p.buscaDado(0, p.quantidade - 1, d) == -1)
        { //dado ainda nao existe

            if (p.quantidade < ORDEM)
            {
                //pagina nao esta cheia
                p.insereDado(d);
                set->inserirPaginaArq(p, posP);
            }
            else
            {
                //pagina cheia
                bool emprestou = false;
                if (set->cab.nSequencia > 1)
                {
                    key dataKey, antK;
                    dataKey = d;
                    int irmaoE;
                    int irmaoD;
                    noh *pai;

                    pai = pai->buscaNohFolha(raiz, dataKey);
                    irmaoE = irmaoEsq(pai, posP); // retorna a posicao no vetor de filhos
                    irmaoD = irmaoDir(pai, posP); // retorna a posicao no vetor de filhos

                    int posAntP = -1, posNextP = -1;

                    if (irmaoE >= 0)
                    {
                        posAntP = pai->posFilho[irmaoE];
                        set->lerPaginaArquivo(&antP, posAntP);
                    }
                    if (irmaoD < (pai->quantidade + 1))
                    {
                        posNextP = pai->posFilho[irmaoD];
                        set->lerPaginaArquivo(&nextP, posNextP);
                    }

                    if ((posAntP != -1) && (antP.quantidade < ORDEM))
                    {
                        //Empresta pro Anterior
                        antK = p.dado[0];

                        antP.insereDado(p.dado[0]);
                        p.removeDado(0);
                        p.insereDado(d);

                        dataKey = p.dado[0];

                        raiz->replaceChave(antK, dataKey);

                        //salva mudancas no disco
                        set->inserirPaginaArq(p, posP);
                        set->inserirPaginaArq(antP, posAntP);
                        emprestou = true;
                    }
                    else if ((posNextP != -1) && (nextP.quantidade < ORDEM))
                    {
                        //Empresta pro proximo
                        antK = nextP.dado[0];

                        nextP.insereDado(p.dado[p.quantidade - 1]);
                        p.removeDado(p.quantidade - 1);
                        p.insereDado(d);

                        dataKey = nextP.dado[0];

                        raiz->replaceChave(antK, dataKey);

                        //salva mudancas no disco
                        set->inserirPaginaArq(p, posP);
                        set->inserirPaginaArq(nextP, posNextP);
                        emprestou = true;
                    }
                }

                if (!emprestou)
                { //Se nao foi possivel emprestar
                    //divide
                    //pagina atual cheia
                    //divide a pagina
                    set->lerPaginaArquivo(&nova, posNova);
                    p.insereDado(d);
                    p.copiarSegundaMetade(&nova);
                    //copia a metade de p em nova

                    if (nova.proxPagina == -1)
                    {
                        //ultima pagina no seqeunceSet
                        //necessario criar mais uma
                        set->cab.nSequencia++;
                        set->cab.proxPaginaDisponivel = set->cab.nSequencia;
                        Pagina vazia;
                        set->inserirPaginaArq(vazia, set->cab.proxPaginaDisponivel);
                        //pagina é usada para nao ler partes invalidas do sequenceSet
                    }
                    else
                    {
                        set->cab.proxPaginaDisponivel = nova.proxPagina;
                    }
                    set->atualizaCabecalhoArquivo();

                    nova.proxPagina = p.proxPagina;
                    p.proxPagina = posNova;
                    //atualiza proximo das paginas

                    nova.antPagina = posP;
                    if (nova.proxPagina != -1)
                    {
                        //pagina nao é a ultima do sequenceSet
                        //necessario atualizar 'anterior' da proxima pagina
                        Pagina nextP;
                        set->lerPaginaArquivo(&nextP, nova.proxPagina);
                        nextP.antPagina = posNova;
                        set->inserirPaginaArq(nextP, nova.proxPagina);
                    }
                    // atualiza anterior das paginas

                    set->inserirPaginaArq(p, posP);
                    set->inserirPaginaArq(nova, p.proxPagina);
                    //insere o pagina na devida posicao

                    //arruma a arvore
                    if (raiz == NULL)
                        raiz = new noh();
                    key k;
                    k = nova.dado[0];

                    raiz = raiz->insereChave(k, posP, posNova);
                }
            }
        }
        else
        {
            cout << "ERRO AO INSERIR! Dado ja existe!" << endl
                 << endl;
        }
    }
}

//com auxilio de buscaAux, busca um dado na pagina do sequenceSet
//se este dado existr na pagina, faz impressao do mesmo
//caso contrario exibe mensagem de erro
void Bmais::buscar(key &k)
{
    Pagina p;
    int posP;
    Dado d;

    d.indice = k.indice;
    strcpy(d.destino, k.destino);

    posP = buscaAux(raiz, d);

    set->lerPaginaArquivo(&p, posP);

    int r = p.buscaDado(0, p.quantidade - 1, d);

    if (r != -1)
    {
        d = p.dado[r];
        cout << endl
             << "Item na pagina: " << posP << endl;
        cout << "Posicao: " << r << endl;
        cout << "-------------------------------------------------------------------------" << endl;
        cout << d << endl
             << endl;
    }
    else
    {
        cout << endl
             << "Dado nao Encontrado!" << endl;
    }
}

//se o dado existir no arquivo, faz a edicao(menos das chaves)
void Bmais::editar(Dado &d)
{
    Pagina p;
    int posP;

    posP = buscaAux(raiz, d);

    set->lerPaginaArquivo(&p, posP);

    int r = p.buscaDado(0, p.quantidade - 1, d);

    if (r != -1)
    {
        p.dado[r] = d;
        set->inserirPaginaArq(p, posP);
    }
    else
    {
        cout << endl
             << "Erro ao editar: DADO NAO ENCONTRADO!" << endl;
    }
}

//busca pela chave, se houver o dado, remove o do sequenceSet
//apos remocao, arruma a arvore se necessario
void Bmais::remover(key &k)
{
    Pagina p, nextP, antP;
    int posP;
    Dado d;
    key antK, novaK;
    d.indice = k.indice;
    strcpy(d.destino, k.destino);

    posP = buscaAux(raiz, d);

    set->lerPaginaArquivo(&p, posP);

    int posD = p.buscaDado(0, p.quantidade - 1, d);

    if (posD != -1)
    { // elemento encontrado
        //TODO: Implementar remocao

        if (p.quantidade <= (ORDEM / 2))
        {
            //TODO: Implementar empresta (esquerdo -> direitp) e UNIR

            if (raiz == NULL)
            { // só uma pagina no sequence set -> pode remover todos os dados

                if (p.quantidade == 1)
                {
                    set->cab.nSequencia = 0;
                    set->cab.pSequencia = -1;
                    set->cab.proxPaginaDisponivel = -1; //TODO: MARCACAO PARA SOBREESCREVER ARQUIVO NA INSERCAO
                    set->atualizaCabecalhoArquivo();
                }

                p.removeDado(posD);
                set->inserirPaginaArq(p, posP);
            }
            else
            {
                //mais de uma pagina no sequence set -> deve verificar realizar impresta ou uniao
                noh *pai = pai->buscaNohFolha(raiz, k);
                int irmaoE = irmaoEsq(pai, posP); // retorna a posicao no vetor de filhos
                int irmaoD = irmaoDir(pai, posP); // retorna a posicao no vetor de filhos
                int posAntP = -1, posNextP = -1;

                if (irmaoE >= 0)
                {
                    posAntP = pai->posFilho[irmaoE];
                    set->lerPaginaArquivo(&antP, posAntP);
                }
                if (irmaoD < (pai->quantidade + 1))
                {
                    posNextP = pai->posFilho[irmaoD];
                    set->lerPaginaArquivo(&nextP, posNextP);
                }

                if (posAntP != -1 && (antP.quantidade > ORDEM / 2))
                {
                    //pagina anterior é do mesmo pai e pode emprestar
                    antK = p.dado[0];
                    p.removeDado(posD);
                    p.insereDado(antP.dado[antP.quantidade - 1]);
                    antP.removeDado(antP.quantidade - 1);
                    //removeu o dado, inseriu o ultimo do irmao a esquerda em p

                    //atualiza o pai
                    novaK = p.dado[0];
                    raiz->replaceChave(antK, novaK);

                    //atualiza as paginas no arquivo
                    set->inserirPaginaArq(p, posP);
                    set->inserirPaginaArq(antP, posAntP);
                }
                else if (posNextP != -1 && (nextP.quantidade > ORDEM / 2))
                {
                    //proxima pagina é do mesmo pai e pode emprestar
                    antK = nextP.dado[0];
                    p.removeDado(posD);
                    p.insereDado(nextP.dado[0]);
                    nextP.removeDado(0);
                    //removeu o dado, inseriu o primeiro do irmao a direita em p

                    //atualiza o pai
                    novaK = nextP.dado[0];
                    raiz->replaceChave(antK, novaK);

                    if ((posP != set->cab.pSequencia) && (posD == 0))
                    {
                        antK = k;
                        novaK = p.dado[0];
                        raiz->replaceChave(antK, novaK);
                    }

                    //atualiza as paginas no arquivo
                    set->inserirPaginaArq(p, posP);
                    set->inserirPaginaArq(nextP, posNextP);
                }
                else
                {
                    //proxima e anterior tem o minimo de elementos ou sao de pais diferentes
                    //necessario unir pagina e verificar pai ate a raiz
                    if (posAntP == -1)
                    {
                        //primeira pagina de um mesmo pai(uniao com proximo)

                        //Substitui Chave na Arvore
                        if ((posP != set->cab.pSequencia) && (posD == 0))
                        {
                            antK = k;
                            novaK = p.dado[1];
                            raiz->replaceChave(antK, novaK);
                        }

                        //Chave a ser deletada do noh pai
                        antK = nextP.dado[0];

                        //Procedimento com Paginas
                        //Copia nextP para p
                        p.copiarPrimeiraMetade(&nextP);
                        p.proxPagina = nextP.proxPagina;

                        // atualiza anterior da pagina seguinte e salva ela no arquivo

                        if (nextP.proxPagina != -1)
                        {
                            Pagina nextProx;
                            set->lerPaginaArquivo(&nextProx, nextP.proxPagina);
                            nextProx.antPagina = posP;
                            set->inserirPaginaArq(nextProx, nextP.proxPagina);
                        }

                        //Organiza sequencia
                        nextP.proxPagina = set->cab.proxPaginaDisponivel;
                        set->cab.proxPaginaDisponivel = posNextP;
                        set->cab.nSequencia--;
                        set->atualizaCabecalhoArquivo();
                        //remove dado de p
                        p.removeDado(posD);
                        //insere paginas modificadas no arquivo
                        set->inserirPaginaArq(p, posP);
                        set->inserirPaginaArq(nextP, posNextP);

                        //TODO: REMOCAO INTERNA
                        removerInterno(antK, pai, posNextP); // remove a chave pai no noh pai e o irmao da direita
                    }
                    else
                    {
                        //(uniao com anterior)

                        //Chave a ser deletada do noh pai
                        antK = p.dado[0];

                        //Procedimento com Paginas
                        //Copia nextP para p
                        antP.copiarPrimeiraMetade(&p);
                        antP.proxPagina = p.proxPagina;

                        // atualiza anterior da pagina seguinte e salva ela no arquivo
                        if (p.proxPagina != -1)
                        {
                            set->lerPaginaArquivo(&nextP, p.proxPagina);
                            nextP.antPagina = posAntP;
                            set->inserirPaginaArq(nextP, p.proxPagina);
                        }

                        //Organiza sequencia
                        p.proxPagina = set->cab.proxPaginaDisponivel;
                        set->cab.proxPaginaDisponivel = posP;
                        set->cab.nSequencia--;
                        set->atualizaCabecalhoArquivo();
                        //remove dado de antP
                        antP.removeDado(((ORDEM / 2) + posD));
                        //insere paginas modificadas no arquivo
                        set->inserirPaginaArq(p, posP);
                        set->inserirPaginaArq(antP, posAntP);

                        //TODO: REMOCAO INTERNA
                        removerInterno(antK, pai, posP); // remove a chave pai no noh pai e a pagina atual :D
                    }
                }
            }
        }
        else if (p.quantidade > (ORDEM / 2))
        {

            if ((posP == set->cab.pSequencia) || (posD != 0))
            { // sem chave na arvore -> remove só na pagina mesmo
                p.removeDado(posD);
                set->inserirPaginaArq(p, posP);
            }
            else
            { // Substituir a chave na arvore.
                p.removeDado(posD);
                set->inserirPaginaArq(p, posP);
                key novaK;
                novaK = p.dado[0];
                if (raiz != NULL)
                    raiz->replaceChave(k, novaK);
            }
        }
    }
    else
    {
        cout << endl
             << "Erro ao Remover: DADO NAO Existe!" << endl;
    }
}

//Esse metodo remove uma chave e o filho de um noh folha apos a uniao
//de duas paginas do sequence set -> em casos de Undeflow com necessidade
//de uniao entre nohs é chamado uma funcao recursiva para subir a arvore
//removendo as chaves e filhos.
void Bmais::removerInterno(key k, noh *umNoh, int filho)
{

    if (umNoh == raiz)
    { // Se o noh folha pai é a raiz
        if (umNoh->quantidade == 1)
        { // e tem só uma chave

            raiz = NULL; // Raiz é nula
        }
        else
        {
            //Deleta Chave k no noh umNoh
            int i = 0;
            while ((i < umNoh->quantidade) && (umNoh->keys[i] != k))
            {
                i++;
            }

            for (int j = i; j < umNoh->quantidade - 1; j++)
            {
                umNoh->keys[j] = umNoh->keys[j + 1];
            }

            //Delete filho de umNoh
            i = 0;
            while ((i < umNoh->quantidade + 1) && (umNoh->posFilho[i] != filho))
            {
                i++;
            }

            for (int j = i; j < umNoh->quantidade; j++)
            {
                umNoh->posFilho[j] = umNoh->posFilho[j + 1];
            }

            //Diminui a quantidade
            umNoh->quantidade--;
        }
    }
    else
    { // Nao é raiz

        //Deleta Chave k no noh umNoh
        int i = 0;
        while ((i < umNoh->quantidade) && (umNoh->keys[i] != k))
        {
            i++;
        }

        for (int j = i; j < umNoh->quantidade - 1; j++)
        {
            umNoh->keys[j] = umNoh->keys[j + 1];
        }

        //Deleta filho de umNoh
        i = 0;
        while ((i < umNoh->quantidade + 1) && (umNoh->posFilho[i] != filho))
        {
            i++;
        }

        for (int j = i; j < umNoh->quantidade; j++)
        {
            umNoh->posFilho[j] = umNoh->posFilho[j + 1];
        }

        //Diminui a quantidade
        umNoh->quantidade--;

        //caso tenha Underflow
        if (umNoh->quantidade < ORDEM / 2)
        {
            int irmaoE, irmaoD;
            noh *nohEsquerdo;
            noh *nohDireita;

            irmaoE = irmaoEsq(umNoh->pai, umNoh);
            irmaoD = irmaoDir(umNoh->pai, umNoh);

            if (irmaoE >= 0)
            {
                nohEsquerdo = umNoh->pai->filhos[irmaoE];
            }
            if (irmaoD <= umNoh->pai->quantidade)
            {
                nohDireita = umNoh->pai->filhos[irmaoD];
            }

            if ((irmaoE >= 0) && (nohEsquerdo->quantidade > ORDEM / 2))
            {
                // Se tem um irmao pela esquerda
                // Noh Esquerdo pode emprestar

                int posUltimaK = nohEsquerdo->quantidade - 1; // ultima chave do noh esquerdo
                // insere chave pai na primeira posicao do umNoh
                umNoh->insereChavePos(0, umNoh->pai->keys[irmaoE]);
                umNoh->pai->keys[irmaoE] = nohEsquerdo->keys[posUltimaK]; // substitui chave pai pelo ultimo elemento do nohEsquerdo

                // insere ultimo filho do nohEsquerdo como primeiro filho
                int posUltimoFilho = nohEsquerdo->quantidade;
                umNoh->insereFilhoPos(0, nohEsquerdo->posFilho[posUltimoFilho]); //TODO: EM NOHS NAO FOLHA MUDAR PAI DOS FILHOS

                //Tira um da quantidade de chaves do nohEsquerdo
                nohEsquerdo->quantidade--;
            }
            else if ((irmaoD <= umNoh->pai->quantidade) && (nohDireita->quantidade > ORDEM / 2))
            {
                // Tem irmao pela direita
                //nohDireita pode emprestar

                // insere chave pai na ultima posicao do umNoh
                umNoh->insereChavePos(umNoh->quantidade, umNoh->pai->keys[irmaoD - 1]);
                umNoh->pai->keys[irmaoD - 1] = nohDireita->keys[0]; // substitui chave pai pelo ultimo elemento do nohEsquerdo

                // insere primeiro filho do nohEsquerdo como ultimo filho
                umNoh->insereFilhoPos(umNoh->quantidade, nohDireita->posFilho[0]); //TODO: EM NOHS NAO FOLHA MUDAR PAI DOS FILHOS

                nohDireita->apagaPrimeiraChaveEFilho();
            }
            else if (irmaoE >= 0)
            {
                //Preciso fazer a uniao e pode ser pela esquerda
                //nohEsquerdo com a chave pai e umNoh

                int posInincioFilho = nohEsquerdo->quantidade + 1;
                //passa a chave pai para nohEsquerdo
                nohEsquerdo->insereChavePos(nohEsquerdo->quantidade, umNoh->pai->keys[irmaoE]);
                //passa as chaves de umNoh para nohEsquerdo
                for (int i = 0; i < umNoh->quantidade; i++)
                {
                    nohEsquerdo->insereChavePos(nohEsquerdo->quantidade, umNoh->keys[i]);
                }

                //passa filhos de umNoh para nohEsquerdo
                for (int i = 0; i < (umNoh->quantidade + 1); i++)
                {
                    nohEsquerdo->insereFilhoPos((posInincioFilho + i), umNoh->posFilho[i]);
                    //desaponta vetor de filhos do umNoh?
                    //TODO: ARRUMAR PONTEIRO PAI DOS FILHOS PARA NOH ESQUERDO (OS QUE VEM DE umNoh TEM umNoh COMO PAI)!!!
                }

                umNoh->quantidade = 0;

                removerInterno(umNoh->pai->keys[irmaoE], umNoh->pai, umNoh);
            }
            else if (irmaoD <= umNoh->pai->quantidade)
            {
                //Preciso fazer a uniao e tem de ser pela direita
                //umNoh com a chave pai e nohDireita
                int posInincioFilho = umNoh->quantidade + 1;
                //passa a chave pai para umNoh
                umNoh->insereChavePos(umNoh->quantidade, umNoh->pai->keys[irmaoD - 1]);
                //passa as chaves de nohDireita para umNoh
                for (int i = 0; i < nohDireita->quantidade; i++)
                {
                    umNoh->insereChavePos(umNoh->quantidade, nohDireita->keys[i]);
                }

                //passa filhos de nohDireita para umNoh
                for (int i = 0; i < (nohDireita->quantidade + 1); i++)
                {
                    umNoh->insereFilhoPos((posInincioFilho + i), nohDireita->posFilho[i]);
                    //desaponta vetor de filhos do umNoh?
                    //TODO: ARRUMAR PONTEIRO PAI DOS FILHOS PARA NOH ESQUERDO (OS QUE VEM DE umNoh TEM umNoh COMO PAI)!!!
                }

                nohDireita->quantidade = 0;

                removerInterno(umNoh->pai->keys[irmaoD - 1], umNoh->pai, nohDireita);
            }
        }
    }
}

//Sobrecarga do metodo removerInterno para nohs nao folha
//Esse metodo remove uma chave e o filho de um noh nao apos a uniao
//de dois nohs filhos do mesmo -> em casos de Undeflow com necessidade
//de uniao entre nohs é chamado recursivamente até a raiz
void Bmais::removerInterno(key k, noh *umNoh, noh *filho)
{

    if (umNoh == raiz)
    { // Se o umNoh é a raiz
        if (umNoh->quantidade == 1)
        { // e tem só uma chave

            if (umNoh->filhos[1] == filho)
            {

                //Nova Raiz é o filho a esquerda
                umNoh->filhos[0]->pai = NULL;
                raiz = umNoh->filhos[0];
            }
            else if (umNoh->filhos[0] == filho)
            {

                //Nova Raiz é o filho a direita
                umNoh->filhos[1]->pai = NULL;
                raiz = umNoh->filhos[1];
            }
        }
        else
        {
            //Deleta Chave k no noh umNoh
            int i = 0;
            while ((i < umNoh->quantidade) && (umNoh->keys[i] != k))
            {
                i++;
            }

            for (int j = i; j < umNoh->quantidade - 1; j++)
            {
                umNoh->keys[j] = umNoh->keys[j + 1];
            }

            //Delete filho de umNoh
            i = 0;
            while ((i < umNoh->quantidade + 1) && (umNoh->filhos[i] != filho))
            {
                i++;
            }

            for (int j = i; j < umNoh->quantidade; j++)
            {
                umNoh->filhos[j] = umNoh->filhos[j + 1];
            }

            //Diminui a quantidade
            umNoh->quantidade--;
        }
    }
    else
    { // Nao é raiz

        //Deleta Chave k no noh umNoh
        int i = 0;
        while ((i < umNoh->quantidade) && (umNoh->keys[i] != k))
        {
            i++;
        }

        for (int j = i; j < umNoh->quantidade - 1; j++)
        {
            umNoh->keys[j] = umNoh->keys[j + 1];
        }

        //Deleta filho de umNoh
        i = 0;
        while ((i < umNoh->quantidade + 1) && (umNoh->filhos[i] != filho))
        {
            i++;
        }

        for (int j = i; j < umNoh->quantidade; j++)
        {
            umNoh->filhos[j] = umNoh->filhos[j + 1];
        }

        //Diminui a quantidade
        umNoh->quantidade--;

        //caso tenha Underflow
        if (umNoh->quantidade < ORDEM / 2)
        {
            int irmaoE, irmaoD;
            noh *nohEsquerdo;
            noh *nohDireita;

            irmaoE = irmaoEsq(umNoh->pai, umNoh);
            irmaoD = irmaoDir(umNoh->pai, umNoh);

            if (irmaoE >= 0)
            {
                nohEsquerdo = umNoh->pai->filhos[irmaoE];
            }
            if (irmaoD <= umNoh->pai->quantidade)
            {
                nohDireita = umNoh->pai->filhos[irmaoD];
            }

            if ((irmaoE >= 0) && (nohEsquerdo->quantidade > ORDEM / 2))
            {
                // Se tem um irmao pela esquerda
                // Noh Esquerdo pode emprestar

                int posUltimaK = nohEsquerdo->quantidade - 1; // ultima chave do noh esquerdo
                // insere chave pai na primeira posicao do umNoh
                umNoh->insereChavePos(0, umNoh->pai->keys[irmaoE]);
                umNoh->pai->keys[irmaoE] = nohEsquerdo->keys[posUltimaK]; // substitui chave pai pelo ultimo elemento do nohEsquerdo

                // insere ultimo filho do nohEsquerdo como primeiro filho
                int posUltimoFilho = nohEsquerdo->quantidade;
                umNoh->insereFilhoPos(0, nohEsquerdo->filhos[posUltimoFilho]);

                //Tira um da quantidade de chaves do nohEsquerdo
                nohEsquerdo->quantidade--;
            }
            else if ((irmaoD <= umNoh->pai->quantidade) && (nohDireita->quantidade > ORDEM / 2))
            {
                // Tem irmao pela direita
                //nohDireita pode emprestar

                // insere chave pai na ultima posicao do umNoh
                umNoh->insereChavePos(umNoh->quantidade, umNoh->pai->keys[irmaoD - 1]);
                umNoh->pai->keys[irmaoD - 1] = nohDireita->keys[0]; // substitui chave pai pelo ultimo elemento do nohEsquerdo

                // insere primeiro filho do nohEsquerdo como ultimo filho
                umNoh->insereFilhoPos(umNoh->quantidade, nohDireita->filhos[0]);

                nohDireita->apagaPrimeiraChaveEFilho();
            }
            else if (irmaoE >= 0)
            {
                //Preciso fazer a uniao e pode ser pela esquerda
                //nohEsquerdo com a chave pai e umNoh

                int posInincioFilho = nohEsquerdo->quantidade + 1;
                //passa a chave pai para nohEsquerdo
                nohEsquerdo->insereChavePos(nohEsquerdo->quantidade, umNoh->pai->keys[irmaoE]);
                //passa as chaves de umNoh para nohEsquerdo
                for (int i = 0; i < umNoh->quantidade; i++)
                {
                    nohEsquerdo->insereChavePos(nohEsquerdo->quantidade, umNoh->keys[i]);
                }

                //passa filhos de umNoh para nohEsquerdo
                for (int i = 0; i < (umNoh->quantidade + 1); i++)
                {
                    nohEsquerdo->insereFilhoPos((posInincioFilho + i), umNoh->filhos[i]);
                    //desaponta vetor de filhos do umNoh?
                    umNoh->filhos[i] = NULL;
                }

                umNoh->quantidade = 0;

                removerInterno(umNoh->pai->keys[irmaoE], umNoh->pai, umNoh);
            }
            else if (irmaoD <= umNoh->pai->quantidade)
            {
                //Preciso fazer a uniao e tem de ser pela direita
                //umNoh com a chave pai e nohDireita
                int posInincioFilho = umNoh->quantidade + 1;
                //passa a chave pai para umNoh
                umNoh->insereChavePos(umNoh->quantidade, umNoh->pai->keys[irmaoD - 1]);
                //passa as chaves de nohDireita para umNoh
                for (int i = 0; i < nohDireita->quantidade; i++)
                {
                    umNoh->insereChavePos(umNoh->quantidade, nohDireita->keys[i]);
                }

                //passa filhos de nohDireita para umNoh
                for (int i = 0; i < (nohDireita->quantidade + 1); i++)
                {
                    umNoh->insereFilhoPos((posInincioFilho + i), nohDireita->filhos[i]);
                    //desaponta vetor de filhos do nohDireita?
                    nohDireita->filhos[i] = NULL;
                }

                nohDireita->quantidade = 0;

                removerInterno(umNoh->pai->keys[irmaoD - 1], umNoh->pai, nohDireita);
            }
        }
    }
}

//retorna a posicao de uma pagina do sequence set(pagina é anterior de posP)
//metodo para nohs folha
int Bmais::irmaoEsq(noh *umNoh, int &posP)
{

    int i = 0;
    while ((i < umNoh->quantidade + 1) && (posP != umNoh->posFilho[i]))
    {
        i++;
    }
    return (i - 1);
}
//retorna a posicao do irmao esquerdo no vetor de filhos de um noh pai, dado o noh irmao
//metodo para nohs nao folha
int Bmais::irmaoEsq(noh *umNoh, noh *irmao)
{

    int i = 0;
    while ((i < umNoh->quantidade + 1) && (irmao != umNoh->filhos[i]))
    {
        i++;
    }

    return (i - 1);
}

//retorna a posicao de uma pagina do sequence set(pagina é proxima de posP)
//metodo para nohs folha
int Bmais::irmaoDir(noh *umNoh, int &posP)
{

    int i = 0;
    while ((i < umNoh->quantidade + 1) && (posP != umNoh->posFilho[i]))
    {
        i++;
    }

    return (i + 1);
}

//retorna a posicao do irmao direito no vetor de filhos de um noh pai, dado o noh irmao
//metodo para nohs nao folha
int Bmais::irmaoDir(noh *umNoh, noh *irmao)
{

    int i = 0;
    while ((i < umNoh->quantidade + 1) && (irmao != umNoh->filhos[i]))
    {
        i++;
    }

    return (i + 1);
}

//busca a pagina onde pode conter o dado ou deve ser inserido/removido
//metodo para quando estiver em um noh folha
int Bmais::buscaAux(noh *umNoh, Dado &d)
{
    int posPag;
    if (umNoh == NULL)
    {
        //raiz nula(sequenceSet tem no maximo uma pagina)
        return set->cab.pSequencia;
    }
    else
    {
        int i = 1;
        while ((i < umNoh->quantidade) && (umNoh->keys[i - 1] < d))
        {
            i++;
        }
        if (umNoh->keys[i - 1] > d)
        {
            if (umNoh->folha)
                posPag = umNoh->posFilho[i - 1];
            else
                posPag = buscaAux(umNoh->filhos[i - 1], d);
        }
        else
        {
            if (umNoh->folha)
                posPag = umNoh->posFilho[i];
            else
                posPag = buscaAux(umNoh->filhos[i], d);
        }
    }
    return posPag;
}

//percorre todos noh's da arvore imprimindo-os em arquivo
void Bmais::imprimirPelaArvoreAux(noh *umNoh, string nomeArq, int espaco)
{
    fstream arq(nomeArq.c_str());
    arq.seekg(0, arq.end);
    int i;
    for (int i = 0; i < (espaco - 1) * 5; i++)
        if (i % 5 == 0)
            arq << "│";
        else
            arq << " ";

    if (umNoh != raiz)
    {
        if (umNoh->filhos[0] == NULL)
            arq << "└────";
        else
            arq << "├────";
    }

    for (i = 0; i < umNoh->quantidade; i++)
    {
        if (umNoh->filhos[0] == NULL)
            arq << ".";
        arq << umNoh->posFilho[i] << " " << umNoh->keys[i] << " ";
    }
    if (umNoh->filhos[0] == NULL)
        arq << ".";
    arq << umNoh->posFilho[i] << "" << endl;
    arq.close();
    espaco++;
    for (int i = 0; i < umNoh->quantidade + 1; i++)
        if (!umNoh->folha)
            imprimirPelaArvoreAux(umNoh->filhos[i], nomeArq, espaco);
}

//imprime a arvore da raiz aos noh's folhas
void Bmais::imprimirPelaArvore()
{
    if (raiz == NULL)
    {
        cout << "Arvore B+ vazia!" << endl;
    }
    else
    {
        string nome;
        nome = "printINDEX" + set->nomeArq;

        ofstream arq(nome.c_str());
        arq.close();
        imprimirPelaArvoreAux(raiz, nome, 0);
        cout << "Arquivo 'printINDEX" << set->nomeArq << "' criado/atualizado!" << endl
             << endl;
    }
}

//imprime os dados e as informacoes de paginas
void Bmais::imprimirPeloSequenceSet()
{
    set->depurar();
}

//semelhante a imprirPeloSequenceSet(), faz o mesmo tipo de impressao, mas em arquivo
void Bmais::imprimirEmArquivo()
{
    set->depurarEmArq();
}

//a partir de 'inicio' e 'fim', insere um intevalo de dados do arquivo CSV disponibilizado
void Bmais::lecsv(unsigned inicio, unsigned fim)
{
    ifstream arquivo_csv("captura_pacotes.csv");

    if (arquivo_csv)
    {

        string linha;

        string campo;                 // variavel para obter um campo de cada linha lida
        string delimitador = "\",\""; // delimitador entre os campos
        unsigned posFimCampo;         // posicao final do campo

        Dado nDado;
        unsigned quant = fim - inicio;

        for (unsigned i = 0; i < inicio; i++)
            getline(arquivo_csv, linha);
        //DESCARTA AS LINHAS ANTES DO INICIO
        for (unsigned i = 0; i <= quant; i++)
        {
            getline(arquivo_csv, linha);

            campo = linha.erase(0, 1); // remove primeiro caracter da linha (")

            // obtendo primeiro campo, um inteiro - No.
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            nDado.indice = stoul(campo, NULL, 10);

            // obtendo segundo campo, um float - Time
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            nDado.tempo = stof(campo);

            // obtendo terceiro campo, um texto - Source
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            strcpy(nDado.origem, campo.c_str());

            // obtendo quarto campo, um texto - Destination
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            strcpy(nDado.destino, campo.c_str());

            // obtendo quinto campo, um texto - Protocol
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            strcpy(nDado.protocolo, campo.c_str());

            // obtendo sexto campo, um texto - Length
            posFimCampo = linha.find(delimitador);
            campo = linha.substr(0, posFimCampo);
            linha.erase(0, posFimCampo + delimitador.length());
            nDado.tamanho = stoul(campo);

            // obtendo �ltimo campo, um texto - Info
            // nesse caso, procuro o �ltimo aspas, fazendo busca reversa
            posFimCampo = linha.rfind("\"");
            campo = linha.substr(0, posFimCampo);
            strcpy(nDado.informacao, campo.c_str());
            // agora � com voc�, umPacote j� tem o conte�do de uma linha
            inserir(nDado);
        }
    }
}
