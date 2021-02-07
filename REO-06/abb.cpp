#include <iostream>
#include <stdexcept>
using namespace std;

typedef unsigned int tipoChave;

int acessos = 0;
int acessosTotal = 0;

struct dado
{
  unsigned chave;
};

ostream &operator<<(ostream &saida, const dado &e)
{
  saida << "(" << e.chave << ","
        << ")";
  return saida;
}

istream &operator>>(istream &entrada, dado &e)
{
  entrada >> e.chave;
  return entrada;
}

class noh
{
  friend class abb;

private:
  dado elemento;
  noh *esq;
  noh *dir;

public:
  noh(const dado &umDado) : elemento(umDado), esq(NULL), dir(NULL) {}
  ~noh() {}
};

class abb
{
  friend ostream &operator<<(ostream &output, abb &arvore);

private:
  noh *raiz;
  // funções auxiliares para inserção e remoção usando método recursivo
  // retorna o nó para ajustar o pai ou o raiz
  noh *insereAux(noh *umNoh, const dado &umDado);
  noh *buscaAux(tipoChave chave);
  // função auxiliar do destrutor, usa percorrimento pós-ordem
  void destruirRecursivamente(noh *umNoh);
  void imprimirDir(const string &prefixo, const noh *meuNoh);
  void imprimirEsq(const string &prefixo, const noh *meuNoh, bool temIrmao);

public:
  abb() { raiz = NULL; }
  ~abb();
  void imprimir();
  // inserção e remoção são recursivos
  void insere(const dado &umDado);
  dado busca(tipoChave chave);
};

// destrutor
abb::~abb()
{
  destruirRecursivamente(raiz);
}

// destrutor é recursivo, fazendo percorrimento pós-ordem
void abb::destruirRecursivamente(noh *umNoh)
{
  if (umNoh != NULL)
  {
    destruirRecursivamente(umNoh->esq);
    destruirRecursivamente(umNoh->dir);
    delete umNoh;
  }
}

void abb::insere(const dado &umDado)
{
  raiz = insereAux(raiz, umDado);
}

// inserção recursiva, devolve nó para atribuição de pai ou raiz
noh *abb::insereAux(noh *umNoh, const dado &umDado)
{
  acessosTotal++;
  if (umNoh == NULL)
  {
    noh *novo = new noh(umDado);

    return novo;
  }
  else
  {

    if (umDado.chave < umNoh->elemento.chave)
    {
      umNoh->esq = insereAux(umNoh->esq, umDado);
    }
    else if (umDado.chave == umNoh->elemento.chave)
    {
      throw runtime_error("Erro na inserção: chave já existente!");
    }
    else
    {
      umNoh->dir = insereAux(umNoh->dir, umDado);
    }
  }

  return umNoh;
}

// método de busca auxiliar (retorna o nó), iterativo
noh *abb::buscaAux(tipoChave chave)
{
  noh *atual = raiz;
  while (atual != NULL)
  {
    acessosTotal++;
    if (atual->elemento.chave == chave)
    {
      return atual;
    }
    else if (atual->elemento.chave > chave)
    {
      atual = atual->esq;
    }
    else
    {
      atual = atual->dir;
    }
  }
  acessosTotal++;
  return NULL;
}

// busca elemento com uma dada chave na árvore e retorna o registro completo
dado abb::busca(tipoChave chave)
{
  noh *resultado = buscaAux(chave);
  if (resultado != NULL)
    return resultado->elemento;
  else
    cout << "Erro na busca : elemento não encontrado !";
}

ostream &
operator<<(ostream &output, abb &arvore)
{
  arvore.imprimir();
  return output;
}

// imprime formatado seguindo o padrao tree as subarvores direitas de uma abb
void abb::imprimirDir(const std::string &prefixo, const noh *meuNoh)
{
  if (meuNoh != nullptr)
  {
    std::cout << prefixo
              << "└d─"
              << "(" << meuNoh->elemento.chave
              << ")"
              << std::endl;

    // Repassa o prefixo para manter o historico de como deve ser a formatacao e chama no filho direito e esquerdo
    imprimirEsq(prefixo + "    ", meuNoh->esq, meuNoh->dir == nullptr);
    imprimirDir(prefixo + "    ", meuNoh->dir);
  }
}

// imprime formatado seguindo o padrao tree as subarvores direitas de uma abb
void abb::imprimirEsq(const std::string &prefixo, const noh *meuNoh, bool temIrmao)
{
  if (meuNoh != nullptr)
  {
    std::cout << prefixo;

    // A impressao da arvore esquerda depende da indicacao se existe o irmao a direita
    if (temIrmao)
      std::cout << "└e─";
    else
      std::cout << "├e─";

    std::cout << "(" << meuNoh->elemento.chave
              << ")"
              << std::endl;

    // Repassa o prefixo para manter o historico de como deve ser a formatacao e chama no filho direito e esquerdo
    imprimirEsq(prefixo + "│   ", meuNoh->esq, meuNoh->dir == nullptr);
    imprimirDir(prefixo + "│   ", meuNoh->dir);
  }
}

// imprime formatado seguindo o padrao tree uma abb
void abb::imprimir()
{
  if (this->raiz != nullptr)
  {
    std::cout << "(" << this->raiz->elemento.chave << ")"
              << std::endl;
    // apos imprimir a raiz, chama os respectivos metodos de impressao nas subarvore esquerda e direita
    // a chamada para a impressao da subarvore esquerda depende da existencia da subarvore direita
    imprimirEsq(" ", this->raiz->esq, this->raiz->dir == nullptr);
    imprimirDir(" ", this->raiz->dir);
  }
  else
    std::cout << "*arvore vazia*" << std::endl;
}

int main()
{
  abb arvore;
  tipoChave chave;
  dado umDado;

  char operacao;

  do
  {
    try
    {
      cin >> operacao;
      switch (operacao)
      {
      case 'i': // Inserir recursivamente
        // objeto recebe chave, nome do produto, marca, quantidade disponível
        cin >> umDado;
        cout << acessosTotal << endl;
        arvore.insere(umDado);
        cout << acessosTotal << endl;
        acessosTotal = 0;
        break;
      case 'b': // Buscar
        cin >> chave;
        cout << acessosTotal << endl; // ler a chave
        umDado = arvore.busca(chave); // escrever dados do produto
        cout << "Elemento buscado: " << umDado << endl;
        cout << acessosTotal << endl;
        acessosTotal = 0;
        break;
      case 'e': // Escrever tudo (em ordem)
        cout << arvore;
        break;
      case 'f': // Finalizar execução
        break;
      default:
        cout << "Comando invalido!\n";
      }
    }
    catch (runtime_error &e)
    {
      cout << e.what() << endl;
    }
  } while (operacao != 'f');

  return 0;
}