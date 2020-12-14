#include <iostream>
#include <string>

using namespace std;

class Noh
{
  friend class Hash;

private:
  string palavra;
  string significado;
  Noh *prox;

public:
  Noh(string palavraNova, string significadoNovo);
};

Noh::Noh(string palavraNova, string significadoNovo)
{
  palavra = palavraNova;
  significado = significadoNovo;
  prox = NULL;
}

class Hash
{
private:
  Noh **posicoes;

public:
  Hash();
  ~Hash();
  void inserirNova(string palavraNova, string significadoNovo);
  string verificaSignificado(string palavra);
};

Hash::Hash()
{
  posicoes = new Noh *[23];

  for (int i = 0; i < 23; i++)
  {
    posicoes[i] = NULL;
  }
}

Hash::~Hash()
{
  for (int i = 0; i < 23; i++)
  {
    Noh *atual = posicoes[i];

    while (atual != NULL)
    {
      Noh *aux = atual;
      atual = atual->prox;
      delete aux;
    }
  }
  delete[] posicoes;
}

int funcaoHash(string palavra)
{
  int hash = 0;

  hash = palavra.length() % 23;

  return hash;
}

void Hash::inserirNova(string palavraNova, string significadoNovo)
{
  Noh *novo = new Noh(palavraNova, significadoNovo);
  int n = funcaoHash(palavraNova);

  if (verificaSignificado(palavraNova) == "NULL")
  {
    if (posicoes[n] == NULL)
    {
      posicoes[n] = novo;
    }
    else
    {
      Noh *atual = posicoes[n];

      while (atual->prox != NULL)
      {
        atual = atual->prox;
      }

      atual->prox = novo;
    }
  }
}

string Hash::verificaSignificado(string palavra)
{
  int n = funcaoHash(palavra);

  if ((posicoes[n] != NULL) and (posicoes[n]->palavra == palavra))
  {
    return posicoes[n]->significado;
  }
  else
  {
    Noh *atual = posicoes[n];

    while ((atual != NULL) and (atual->palavra != palavra))
    {
      atual = atual->prox;
    }

    if ((atual != NULL) and (atual->palavra == palavra))
    {
      return atual->significado;
    }
    else
    {
      return "NULL";
    }
  }
}

int main()
{
  Hash tabela;

  int qtd;
  string palavra, significado;

  cin >> qtd;
  for (int i = 0; i < qtd; i++)
  {
    cin >> palavra;
    cin.ignore();
    getline(cin, significado);
    tabela.inserirNova(palavra, significado);
  }

  string busca;
  while (busca != "-1")
  {
    cin >> busca;
    if (busca != "-1")
    {
      cout << "[" << busca << "] => " << tabela.verificaSignificado(busca) << endl;
    }
  }

  return 0;
}