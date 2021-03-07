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
  saida << "(" << e.chave << ")";
  return saida;
}

istream &operator>>(istream &entrada, dado &e)
{
  entrada >> e.chave;
  return entrada;
}

class noh
{
  friend class avl;

private:
  dado elemento;
  noh *esq;
  noh *dir;
  unsigned altura;

public:
  noh(const dado &umDado) : elemento(umDado), esq(NULL), dir(NULL), altura(1) {}
  ~noh() {}
};

class avl
{
  friend ostream &operator<<(ostream &output, avl &arvore);

private:
  noh *raiz;
  // funções auxiliares para inserção e remoção usando método recursivo
  // retorna o nó para ajustar o pai ou o raiz
  noh *insereAux(noh *umNoh, const dado &umDado);
  // métodos para manutenção do balanceamento
  noh *rotacaoEsquerda(noh *umNoh);
  noh *rotacaoDireita(noh *umNoh);
  noh *arrumaBalanceamento(noh *umNoh);
  // busca, método iterativo
  noh *buscaAux(tipoChave chave);
  // função auxiliar do destrutor, usa percorrimento pós-ordem
  void destruirRecursivamente(noh *umNoh);
  void imprimirDir(const string &prefixo, const noh *meuNoh);
  void imprimirEsq(const string &prefixo, const noh *meuNoh, bool temIrmao);
  int informarAltura(noh *umNoh);
  int fatorBalanceamento(noh *umNoh);
  void atualizaAltura(noh *umNoh);

public:
  avl() { raiz = NULL; }
  ~avl();
  void imprimir();
  // inserção e remoção são recursivos
  void insere(const dado &umDado);
  dado busca(tipoChave chave);
};

// destrutor
avl::~avl()
{
  destruirRecursivamente(raiz);
}

// destrutor é recursivo, fazendo percorrimento pós-ordem
void avl::destruirRecursivamente(noh *umNoh)
{
  if (umNoh != NULL)
  {
    destruirRecursivamente(umNoh->esq);
    destruirRecursivamente(umNoh->dir);
    delete umNoh;
  }
}

int avl::informarAltura(noh *umNoh)
{
  acessosTotal++;
  if (umNoh == NULL)
  {
    return 0;
  }
  return umNoh->altura;
}

void avl::atualizaAltura(noh *umNoh)
{
  acessosTotal++;
  int altArvEsq = informarAltura(umNoh->esq);
  int altArvDir = informarAltura(umNoh->dir);
  umNoh->altura = 1 + max(altArvEsq, altArvDir);
}

int avl::fatorBalanceamento(noh *umNoh)
{
  acessosTotal++;
  int altArvEsq = informarAltura(umNoh->esq);
  int altArvDir = informarAltura(umNoh->dir);
  int fatorBal = altArvEsq - altArvDir;
  return fatorBal;
}

void avl::insere(const dado &umDado)
{
  raiz = insereAux(raiz, umDado);
}

// inserção recursiva, devolve nó para atribuição de pai ou raiz
noh *avl::insereAux(noh *umNoh, const dado &umDado)
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

  return arrumaBalanceamento(umNoh);
}

// checa e arruma, se necessário, o balanceamento em umNoh,
// fazendo as rotações e ajustes necessários
noh *avl::arrumaBalanceamento(noh *umNoh)
{
  acessosTotal++;
  atualizaAltura(umNoh);
  int bal = fatorBalanceamento(umNoh);
  if ((bal >= -1) and (bal <= 1))
  {
    return umNoh;
  }
  if ((bal > 1) and (fatorBalanceamento(umNoh->esq) >= 0))
  {
    return rotacaoDireita(umNoh);
  }
  if ((bal > 1) and (fatorBalanceamento(umNoh->esq) < 0))
  {
    umNoh->esq = rotacaoEsquerda(umNoh->esq);
    return rotacaoDireita(umNoh);
  }
  if (bal < -1 and (fatorBalanceamento(umNoh->dir) <= 0))
  {
    return rotacaoEsquerda(umNoh);
  }
  if ((bal < -1) and (fatorBalanceamento(umNoh->dir) > 0))
  {
    umNoh->dir = rotacaoDireita(umNoh->dir);
    return rotacaoEsquerda(umNoh);
  }
  return NULL;
}

// rotação à esquerda na subárvore com raiz em umNoh
// retorna o novo pai da subárvore
noh *avl::rotacaoEsquerda(noh *umNoh)
{
  acessosTotal++;
  noh *aux = umNoh->dir;
  umNoh->dir = aux->esq;
  aux->esq = umNoh;
  atualizaAltura(umNoh);
  atualizaAltura(aux);
  return aux;
}

// rotação à direita na subárvore com raiz em umNoh
// retorna o novo pai da subárvore
noh *avl::rotacaoDireita(noh *umNoh)
{
  acessosTotal++;
  noh *aux = umNoh->esq;
  umNoh->esq = aux->dir;
  aux->dir = umNoh;
  atualizaAltura(umNoh);
  atualizaAltura(aux);
  return aux;
}

// método de busca auxiliar (retorna o nó), iterativo
noh *avl::buscaAux(tipoChave chave)
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
dado avl::busca(tipoChave chave)
{
  noh *resultado = buscaAux(chave);
  if (resultado != NULL)
    return resultado->elemento;
  else
    cout << "Erro na busca: elemento não encontrado!";
}

ostream &
operator<<(ostream &output, avl &arvore)
{
  arvore.imprimir();
  return output;
}

// imprime formatado seguindo o padrao tree as subarvores direitas de uma avl
void avl::imprimirDir(const std::string &prefixo, const noh *meuNoh)
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

// imprime formatado seguindo o padrao tree as subarvores direitas de uma avl
void avl::imprimirEsq(const std::string &prefixo, const noh *meuNoh, bool temIrmao)
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

// imprime formatado seguindo o padrao tree uma avl
void avl::imprimir()
{
  if (this->raiz != nullptr)
  {
    std::cout << "(" << this->raiz->elemento.chave
              << ")" << std::endl;
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
  avl arvore;
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
      case 'i':
        cin >> umDado;
        cout << acessosTotal << endl;
        arvore.insere(umDado);
        cout << acessosTotal << endl;
        acessosTotal = 0;
        break;
      case 'b':       // Buscar
        cin >> chave; // ler a chave
        cout << acessosTotal << endl;
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