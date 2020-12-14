#include <iostream>
using namespace std;

class Noh
{
private:
  friend class Lista;
  string comando;
  Noh *proximo;
  Noh *anterior;

public:
  Noh(string comandoNovo);
};

Noh::Noh(string comandoNovo)
{
  comando = comandoNovo;
  proximo = NULL;
  anterior = NULL;
}

class Lista
{
private:
  Noh *primeiro;
  Noh *ultimo;
  int tamanhoLista;

public:
  Lista();
  ~Lista();
  void insereFim(string comandoNovo);
  string removeInicio();
  string removeFim();
};

Lista::Lista()
{
  tamanhoLista = 0;
  primeiro = NULL;
  ultimo = NULL;
}

Lista::~Lista()
{
  Noh *aux = primeiro;
  Noh *proximo = NULL;
  while (aux != NULL)
  {
    proximo = aux->proximo;
    delete aux;
    aux = proximo;
  }
}

void Lista::insereFim(string comandoNovo)
{
  Noh *novo = new Noh(comandoNovo);
  Noh *aux = NULL;

  if (tamanhoLista == 0)
  {
    ultimo = novo;
    primeiro = novo;
    tamanhoLista++;
  }
  else if (tamanhoLista >= 1)
  {
    aux = ultimo;
    ultimo->proximo = novo;
    ultimo = novo;
    ultimo->anterior = aux;
    tamanhoLista++;
  }
}

string Lista::removeInicio()
{
  string comando;

  if (tamanhoLista == 0)
  {
    cout << "ERRO" << endl;
  }
  else
  {
    if (tamanhoLista == 1)
    {
      comando = primeiro->comando;
      primeiro = NULL;
      ultimo = NULL;
      tamanhoLista--;
    }
    else if (tamanhoLista > 1)
    {
      Noh *aux = NULL;
      aux = primeiro;
      comando = aux->comando;
      primeiro = aux->proximo;
      primeiro->anterior = NULL;
      delete aux;
      tamanhoLista--;
    }
  }
  return comando;
}

string Lista::removeFim()
{
  string comando;

  if (tamanhoLista == 0)
  {
    cout << "ERRO" << endl;
  }
  else
  {
    if (tamanhoLista == 1)
    {
      comando = primeiro->comando;
      ultimo = NULL;
      primeiro = NULL;
      tamanhoLista--;
    }
    else if (tamanhoLista > 1)
    {
      Noh *aux = NULL;
      aux = ultimo;
      comando = aux->comando;
      ultimo = aux->anterior;
      ultimo->proximo = NULL;
      delete aux;
      tamanhoLista--;
    }
  }
  return comando;
}

int main()
{
  Lista comandosLista;
  string comando;

  int tamanho, insercoes = 0;

  cin >> tamanho;
  cin.ignore();
  getline(cin, comando);

  while (comando != "sair")
  {
    if (comando == "undo")
    {
      if (insercoes == 0)
      {
        cout << "ERRO" << endl;
      }
      else if (insercoes > 0)
      {
        cout << "desfazer: " << comandosLista.removeFim() << endl;
        insercoes--;
      }
    }
    else
    {
      if (insercoes < tamanho)
      {
        comandosLista.insereFim(comando);
        insercoes++;
      }
      else if (insercoes == tamanho)
      {
        cout << "esqueci: " << comandosLista.removeInicio() << endl;
        comandosLista.insereFim(comando);
      }
    }
    getline(cin, comando);
  }
  return 0;
}
