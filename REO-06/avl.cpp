#include <iostream>
#include <stdexcept>
using namespace std;

typedef int tipoChave;

struct dado
{
  unsigned chave;
  string nomeProduto;
  string marca;
  int quantidadeDisponivel;
};

ostream &operator<<(ostream &saida, const dado &e)
{
  saida << "(" << e.chave << "," << e.nomeProduto << "," << e.marca << "," << e.quantidadeDisponivel << ")";
  return saida;
}

istream &operator>>(istream &entrada, dado &e)
{
  entrada >> e.chave >> e.nomeProduto >> e.marca >> e.quantidadeDisponivel;
  return entrada;
}

class noh
{
  friend class avl;

private:
  dado elemento;
  noh *esq;
  noh *dir;
  noh *pai;
  unsigned altura;
  int calculaBalanceamento(noh *noh);
  unsigned int calculaAltura(noh *noh);

public:
  noh(const dado &umDado) : elemento(umDado), esq(NULL), dir(NULL), altura(1) {}
  ~noh() {}
  int fatorBalanceamento();
};

unsigned int noh::calculaAltura(noh *noh)
{
  if (noh == NULL)
  {
    return 0;
  }
  return noh->altura;
}

int noh::calculaBalanceamento(noh *noh)
{
  if (noh == NULL)
  {
    return 0;
  }
  return noh->fatorBalanceamento();
}

int noh::fatorBalanceamento()
{
  return (calculaAltura(esq) - calculaAltura(dir));
}

class avl
{
  friend ostream &operator<<(ostream &output, avl &arvore);

private:
  noh *raiz;
  // percorrimento em ordem da árvore
  void percorreEmOrdemAux(noh *atual, int nivel);
  // funções auxiliares para remoção
  noh *encontraMenor(noh *raizSub);
  noh *removeMenor(noh *raizSub);
  // funções auxiliares para inserção e remoção usando método recursivo
  // retorna o nó para ajustar o pai ou o raiz
  noh *insereAux(noh *umNoh, const dado &umDado);
  noh *removeAux(noh *umNoh, tipoChave chave);
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
  int levantamentoAux(noh *umNoh, string marca);

public:
  avl() { raiz = NULL; }
  ~avl();
  void imprimir();
  // inserção e remoção são recursivos
  void insere(const dado &umDado);
  void remove(tipoChave chave);
  // inserção e remoção, métodos recursivos
  // busca retorna uma cópia do objeto armazenado
  dado busca(tipoChave chave);
  // efetua levantamento de quantos produtos existem de uma dada marca
  int levantamento(string marca);
};

// destrutor
avl::~avl()
{
  destruirRecursivamente(raiz);
}

// destrutor é recursivo, fazendo percorrimento pós-ordem
void avl::destruirRecursivamente(noh *umNoh)
{
#WARNING implemente
}

void avl::insere(const dado &umDado)
{
  raiz = insereAux(raiz, umDado);
}

// inserção recursiva, devolve nó para atribuição de pai ou raiz
noh *avl::insereAux(noh *umNoh, const dado &umDado)
{

  if (umNoh == NULL)
  {
    noh *novo = new noh(umDado);

    return novo;
  }

  if (umDado.chave < umNoh->elemento.chave)
  {
    umNoh->esq = insereAux(umNoh->esq, umDado);
    umNoh->esq->pai = umNoh;
  }
  else if (umDado.chave > umNoh->elemento.chave)
  {
    umNoh->dir = insereAux(umNoh->dir, umDado);
    umNoh->dir->pai = umNoh;
  }
  else
  {
    return umNoh;
  }

  return arrumaBalanceamento(umNoh);
}

// checa e arruma, se necessário, o balanceamento em umNoh,
// fazendo as rotações e ajustes necessários
noh *avl::arrumaBalanceamento(noh *umNoh)
{
  unsigned int alturaEsq = umNoh->calculaAltura(umNoh->esq);
  unsigned int alturaDir = umNoh->calculaAltura(umNoh->dir);
  umNoh->altura = 1 + max(alturaEsq, alturaDir);
  int bal = umNoh->fatorBalanceamento();

  if ((bal >= -1) and (bal <= 1))
  {
    return umNoh;
  }
  if ((bal > 1) and (umNoh->calculaBalanceamento(umNoh->esq) >= 0))
  {
    return rotacaoDireita(umNoh);
  }
  if ((bal > 1) and (umNoh->calculaBalanceamento(umNoh->esq) < 0))
  {
    umNoh->esq = rotacaoEsquerda(umNoh->esq);
    umNoh->esq->pai = umNoh;
    return rotacaoDireita(umNoh);
  }
  if (bal < -1 and (umNoh->calculaBalanceamento(umNoh->dir) <= 0))
  {
    return rotacaoEsquerda(umNoh);
  }
  if ((bal < -1) and (umNoh->calculaBalanceamento(umNoh->dir) > 0))
  {
    umNoh->dir = rotacaoDireita(umNoh->dir);
    umNoh->dir->pai = umNoh;
    return rotacaoEsquerda(umNoh);
  }
  return NULL;
}

// rotação à esquerda na subárvore com raiz em umNoh
// retorna o novo pai da subárvore
noh *avl::rotacaoEsquerda(noh *umNoh)
{
  noh *aux = umNoh->dir;

  umNoh->dir = aux->esq;

  if (aux->esq != NULL)
  {
    aux->esq->pai = umNoh;
  }

  aux->pai = umNoh->pai;

  if (umNoh->pai == NULL)
  {
    raiz = aux;
  }
  else if (umNoh == umNoh->pai->esq)
  {
    umNoh->pai->esq = aux;
  }
  else
  {
    umNoh->pai->dir = aux;
  }

  aux->esq = umNoh;
  umNoh->pai = aux;

  unsigned int alturaEsq = umNoh->calculaAltura(umNoh->esq);
  unsigned int alturaDir = umNoh->calculaAltura(umNoh->dir);

  umNoh->altura = 1 + max(alturaEsq, alturaDir);
  aux->altura = 1 + max(alturaEsq, alturaDir);

  return aux;
}

// rotação à direita na subárvore com raiz em umNoh
// retorna o novo pai da subárvore
noh *avl::rotacaoDireita(noh *umNoh)
{
  noh *aux = umNoh->esq;
  umNoh->esq = aux->dir;
  if (aux->dir != NULL)
  {
    aux->dir->pai = umNoh;
  }
  aux->pai = umNoh->pai;
  if (umNoh->pai == NULL)
  {
    raiz = aux;
  }
  else if (umNoh == umNoh->pai->esq)
  {
    umNoh->pai->esq = aux;
  }
  else
  {
    umNoh->pai->dir = aux;
  }
  aux->dir = umNoh;
  umNoh->pai = aux;
  unsigned int alturaEsq = umNoh->calculaAltura(umNoh->esq);
  unsigned int alturaDir = umNoh->calculaAltura(umNoh->dir);
  umNoh->altura = 1 + max(alturaEsq, alturaDir);
  aux->altura = 1 + max(alturaEsq, alturaDir);
  return aux;
}

// método de busca auxiliar (retorna o nó), iterativo
noh *avl::buscaAux(tipoChave chave){
#WARNING implemente
}

// busca elemento com uma dada chave na árvore e retorna o registro completo
dado avl::busca(tipoChave chave)
{
  noh *resultado = buscaAux(chave);
  if (resultado != NULL)
    return resultado->elemento;
  else
    throw runtime_error("Erro na busca: elemento não encontrado!");
}

// nó mínimo (sucessor) de subárvore com raiz em raizSub (folha mais à esquerda)
noh *avl::encontraMenor(noh *raizSub){
#WARNING implemente
}

// procedimento auxiliar para remover o sucessor substituíndo-o pelo
// seu filho à direita
noh *avl::removeMenor(noh *raizSub)
{
#WARNING implemente
}

// remoção recursiva
void avl::remove(tipoChave chave)
{
  raiz = removeAux(raiz, chave);
}

noh *avl::removeAux(noh *umNoh, tipoChave chave)
{
#WARNING implemente
}

// utiliza o nó atual e seu nível na árvore (para facilitar visualização)
void avl::percorreEmOrdemAux(noh *atual, int nivel){
#WARNING implemente
}

ostream &
operator<<(ostream &output, avl &arvore)
{
  arvore.imprimir();
  return output;
}

// imprime formatado seguindo o padrao tree as subarvores direitas de uma avl
void avl::imprimirDir(const string &prefixo, const noh *meuNoh)
{
  if (meuNoh != nullptr)
  {
    cout << prefixo
         << "└d─"
         << "(" << meuNoh->elemento.chave << "," << meuNoh->elemento.nomeProduto << ")"
         << endl;

    // Repassa o prefixo para manter o historico de como deve ser a formatacao e chama no filho direito e esquerdo
    imprimirEsq(prefixo + "    ", meuNoh->esq, meuNoh->dir == nullptr);
    imprimirDir(prefixo + "    ", meuNoh->dir);
  }
}

// imprime formatado seguindo o padrao tree as subarvores direitas de uma avl
void avl::imprimirEsq(const string &prefixo, const noh *meuNoh, bool temIrmao)
{
  if (meuNoh != nullptr)
  {
    cout << prefixo;

    // A impressao da arvore esquerda depende da indicacao se existe o irmao a direita
    if (temIrmao)
      cout << "└e─";
    else
      cout << "├e─";

    cout << "(" << meuNoh->elemento.chave << "," << meuNoh->elemento.nomeProduto << ")"
         << endl;

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
    cout << "(" << this->raiz->elemento.chave << "," << this->raiz->elemento.nomeProduto << ")" << endl;
    // apos imprimir a raiz, chama os respectivos metodos de impressao nas subarvore esquerda e direita
    // a chamada para a impressao da subarvore esquerda depende da existencia da subarvore direita
    imprimirEsq(" ", this->raiz->esq, this->raiz->dir == nullptr);
    imprimirDir(" ", this->raiz->dir);
  }
  else
    cout << "*arvore vazia*" << endl;
}

int main()
{
  avl arvore;
  tipoChave chave;
  dado umDado;
  string marca;
  int quantidade;

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
        arvore.insere(umDado);
        break;
      case 'r': // Remover recursivamente
        cin >> chave;
        arvore.remove(chave);
        break;
      case 'b':                       // Buscar
        cin >> chave;                 // ler a chave
        umDado = arvore.busca(chave); // escrever dados do produto
        cout << "Elemento buscado: " << umDado << endl;
        break;
      case 'l':       // Levantamento por marca
        cin >> marca; // ler a marca
        quantidade = arvore.levantamento(marca);
        cout << "Levantamento da marca " << marca << ": " << quantidade << endl;
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