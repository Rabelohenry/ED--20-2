#include <iostream>
#include <string>

using namespace std;

struct motociclista
{
  string nomeMotociclista;
  string marcaMoto;
  string modeloMoto;
  string telefone;
  int anoMoto;
};

struct oficina
{
  string nomeOficina;
  string marcaMoto;
};

struct parcerias
{
  string nomeOficina;
  string marcaMoto;
  string beneficioParceria;
};

class associacao
{
private:
  motociclista *ListaMotociclistas;
  parcerias *listaParcerias;

public:
  associacao();
  ~associacao();
  void cadastrarMotorista();
  void cadastrarParceria();
  void consultarParceria();
};

int main()
{
  cout << "avalana";
  return 0;
}