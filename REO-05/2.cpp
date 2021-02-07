#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>

typedef int TChave;
typedef int TValor;

class Arvore
{
    friend std::ostream &operator<<(std::ostream &saida, const Arvore &arv);

public:
    Arvore();
    Arvore(const Arvore &arv);
    ~Arvore(); // Destrutor recursivo!
    // Retorna o valor associado a uma chave.
    TValor &Buscar(const TChave &chave);
    // Escreve uma árvore nível a nível.
    void EscreverNivelANivel(std::ostream &saida);
    // Insere um chave/valor numa árvore.
    void Inserir(const TChave &chave, const TValor &valor);
    // Remove chave da árvore. Retorna falso se não foi possível remover.
    bool Remover(const TChave &chave);
    // Verifica se uma árvore é vazia.
    bool Vazia() { return mVazia; }

protected:
    // Retorna ponteiro para a subárvore do menor valor.
    Arvore *Menor();
    // Substitui a raiz pela raiz de uma subárvore. A raiz da subárvore é removida.
    // Usado na remoção de chaves/valores.
    void SubstituirPor(Arvore *ptArv);

    bool mVazia;
    TChave mChave;
    TValor mValor;
    Arvore *mPtEsq;
    Arvore *mPtDir;
};

using namespace std;

Arvore::Arvore()
{
    mVazia = true;
    mPtEsq = NULL;
    mPtDir = NULL;
}

Arvore::Arvore(const Arvore &arv)
{
    if (not arv.mVazia)
    {
        mVazia = false;
        mChave = arv.mChave;
        mValor = arv.mValor;
        mPtEsq = new Arvore(*arv.mPtEsq);
        mPtDir = new Arvore(*arv.mPtDir);
    }
}

Arvore::~Arvore()
{
    delete mPtEsq;
    delete mPtDir;
}

void Arvore::EscreverNivelANivel(ostream &saida)
{
    queue<Arvore *> filhos;
    Arvore aux;
    Arvore *fimDeNivel = &aux;
    filhos.push(this);
    filhos.push(fimDeNivel);
    while (not filhos.empty())
    {
        Arvore *ptArv = filhos.front();
        filhos.pop();
        if (ptArv == fimDeNivel)
        {
            saida << "\n";
            if (not filhos.empty())
                filhos.push(fimDeNivel);
        }
        else
        {
            saida << '[';
            if (not ptArv->mVazia)
            {
                saida << ptArv->mChave << '/' << ptArv->mValor;
                filhos.push(ptArv->mPtEsq);
                filhos.push(ptArv->mPtDir);
            }
            saida << ']';
        }
    }
}

Arvore *Arvore::Menor()
{
    Arvore *atual = this;
    while (!atual->mPtEsq->mVazia)
    {
        atual = atual->mPtEsq;
    }
    return atual;
}

void Arvore::SubstituirPor(Arvore *ptArv)
{

    if (ptArv->mVazia)
    {
        throw invalid_argument("Substitutição de nós falhou.");
    }

    Arvore *antiga = this;
    Arvore *nova = new Arvore(*ptArv);
    nova->mPtEsq = antiga->mPtEsq;
    nova->mPtDir = antiga->mPtDir;
    antiga->mPtEsq = NULL;
    antiga->mPtDir = NULL;
    delete ptArv;
    ptArv = new Arvore;
    *this = *nova;
}

TValor &Arvore::Buscar(const TChave &chave)
{
    if (mVazia)
        throw runtime_error("Chave não existe na árvore.");
    if (chave == mChave)
        return mValor;
    if (chave < mChave)
        return mPtEsq->Buscar(chave);
    else
        return mPtDir->Buscar(chave);
}

void Arvore::Inserir(const TChave &chave, const TValor &valor)
{
    if (mVazia)
    {
        mChave = chave;
        mValor = valor;
        mVazia = false;
        mPtEsq = new Arvore;
        mPtDir = new Arvore;
    }
    else
    {
        if (chave < mChave)
        {
            mPtEsq->Inserir(chave, valor);
        }
        else if (chave > mChave)
        {
            mPtDir->Inserir(chave, valor);
        }
    }
}

bool Arvore::Remover(const TChave &chave)
{
    if (mVazia)
    {
        return false;
    }
    if (mPtEsq->mVazia and mPtDir->mVazia)
    {
        this->mVazia = true;
        return true;
    }
    bool removeu = true;
    try
    {
        if (chave == mChave)
        {
            if (!mPtDir->mVazia)
            {
                SubstituirPor(mPtDir->Menor());
            }
            else
            {
                SubstituirPor(mPtEsq);
            }
        }
        else
        {
            if (chave < mChave)
            {
                removeu = mPtEsq->Remover(chave);
            }
            else if (chave > mChave)
            {
                removeu = mPtDir->Remover(chave);
            }
        }
    }
    catch (runtime_error &e)
    {
        return false;
    }
    return removeu;
}

ostream &operator<<(ostream &saida, const Arvore &arv)
{
    if (arv.mVazia)
        saida << "()";
    else
        saida << '(' << arv.mChave << '/' << arv.mValor << ' '
              << *arv.mPtEsq << ' ' << *arv.mPtDir << ')';
    return saida;
}

int main()
{
    Arvore arvore;
    TChave chave;
    TValor valor;
    int operacao;
    do
    {
        cin >> operacao;
        switch (operacao)
        {
        case 1: // Inserir
            cin >> chave >> valor;
            arvore.Inserir(chave, valor);
            break;
        case 2: // Remover
            cin >> chave;
            if (not arvore.Remover(chave))
                cerr << "Impossível remover. A chave não existe." << endl;
            break;
        case 3: // Buscar
            try
            {
                cin >> chave;
                valor = arvore.Buscar(chave);
                cout << valor << endl;
            }
            catch (runtime_error &e)
            {
                cerr << e.what() << endl;
            }
            break;
        case 4: // Escrever tudo
            cout << arvore << endl;
            break;
        case 5: // Escrever nivel a nivel
            arvore.EscreverNivelANivel(cout);
            break;
        }
    } while (operacao > 0);
    return 0;
}
