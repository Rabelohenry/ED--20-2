#include <iostream>
#include <string>
using namespace std;

/*
 *  Exemplo de código desenvolvido para atividade avaliativa sobre TADs,
 * parte prática.
 * Decisões de projeto:
 * 
 *  1 - os valores utilzados para definição de arranjos por exemplo, foram
 * definidos de forma que na hora do uso e teste do programa o número de 
 * inserções possa ser feito tranquilamente sem muitos problemas. Exemplo disso:
 * o vetor de structs que representa os motoristas foi alocado dinamicamente com
 * 100 posições.
 *  
 *  2 - embora no planejamento tenham sido definidas classes para as entidades 
 * "motociclistas", "estabelecimentos" e "parcerias", como elas não tiveram seu 
 * comportamento explicitamente descrito foram implementadas com registros.
 * 
 *  3 - foi criada somente uma estrutura para armazenar as loja e oficinas 
 * chamada "estabelecimentos" por conta da semelhança de atributos, dessa forma 
 * as duas são diferenciadas por um campo que indica o tipo do estabelecimento: 
 * 1 - Loja ou 2 - Oficina.
 * 
 *  4 - foram implementados aleḿ dos métodos descritos no texto as funções de 
 * listar os motoristas cadastrados na associação e listar todas as parcerias
 * cadastradas.
 * 
 * 5 - como toda parceria envolve um estabelecimento, e nem todo estabelecimento
 * precisa ser necessariamente uma parceria, na hora do cadastro é informado o
 * nome do estabelecimento, o tipo , a marca e logo depois o usuário escolhe
 * se quer adicionar como uma parceria ou não. Caso a escolha seja sim o cadastro
 * daquela parceria continua até ser finalizado.
 *  
 *  
 *  
 **/
struct motociclista
{
  string nomeMotociclista;
  string marcaMoto;
  string modeloMoto;
  string telefone;
  int anoMoto;
};

struct estabelecimento
{
  string nomeEstabelecimento;
  string tipoEstabelecimento;
  string marcaMoto;
};

struct parceria
{
  string nomeEstabelecimento;
  string tipoEstabelecimento;
  string marcaMoto;
  string servico;
  float preco;
  string fidelizacao;
  string beneficio;
};

class associacao
{
private:
  motociclista *listaMotociclistas;
  estabelecimento *listaEstabelecimentos;
  parceria *listaParcerias;
  int parceriasAtual;

public:
  associacao(int totalMotociclistas, int totalParcerias, int totalEstabelecimentos);
  ~associacao();
  void cadastrarMotociclistas(int motociclistasAtual);
  void cadastrarEstabelecimento(int estabelecimentosAtual);
  void cadastrarParceria(string nomeEstabelecimento, string tipoEstabelecimento, string marcaMoto);
  void consultarParceria(string marca);
  void listarMotociclistas(int motociclistasAtual);
  void listarParcerias();
};

associacao::associacao(int totalMotociclista, int totalEstabelecimentos, int totalParceirias)
{
  parceriasAtual = 0;
  listaMotociclistas = new motociclista[totalMotociclista];
  listaEstabelecimentos = new estabelecimento[totalEstabelecimentos];
  listaParcerias = new parceria[totalParceirias];
};

associacao::~associacao()
{
  delete[] listaMotociclistas;
  delete[] listaParcerias;
}

void associacao::cadastrarMotociclistas(int motociclistasAtual)
{
  cin.ignore();
  cout << "Cadastrar novo motorista:" << endl
       << "Nome: ";
  getline(cin, listaMotociclistas[motociclistasAtual].nomeMotociclista);
  cout << "Marca da moto: ";
  getline(cin, listaMotociclistas[motociclistasAtual].marcaMoto);
  cout << "Modelo da moto: ";
  getline(cin, listaMotociclistas[motociclistasAtual].modeloMoto);
  cout << "Ano da moto: ";
  cin >> listaMotociclistas[motociclistasAtual].anoMoto;
  cin.ignore();
  cout << "Telefone para contato: ";
  getline(cin, listaMotociclistas[motociclistasAtual].telefone);

  cout << "Motorista cadastrado com sucesso!" << endl
       << endl;
  cout << "Nome: " << listaMotociclistas[motociclistasAtual].nomeMotociclista << endl
       << "Marca da moto: " << listaMotociclistas[motociclistasAtual].marcaMoto << endl
       << "Modelo da moto: " << listaMotociclistas[motociclistasAtual].modeloMoto << endl
       << "Ano da moto: " << listaMotociclistas[motociclistasAtual].anoMoto << endl
       << "Telefone: " << listaMotociclistas[motociclistasAtual].telefone << endl
       << endl;
  cout << "Total de motoristas cadastrados: " << motociclistasAtual + 1 << endl
       << endl;
}

void associacao::cadastrarEstabelecimento(int estabelecimentosAtual)
{
  int tipoEstabelecimento = 0;
  int respostaParceria = 0;

  cout << "Cadastrar novo estabelecimento" << endl
       << "Nome: ";
  cin.ignore();
  getline(cin, listaEstabelecimentos[estabelecimentosAtual].nomeEstabelecimento);
  cout << "Qual o tipo do estabelecimento: 1 - LOJA / 2 - OFICINA" << endl
       << "Tipo: ";
  cin >> tipoEstabelecimento;
  while (tipoEstabelecimento != 1 and tipoEstabelecimento != 2)
  {
    cout << "Favor inserir entrada de dados indicada: 1 - LOJA / 2 - OFICINA";
    cin >> tipoEstabelecimento;
  }
  if (tipoEstabelecimento == 1)
  {
    listaEstabelecimentos[estabelecimentosAtual].tipoEstabelecimento = "LOJA";
  }
  else if (tipoEstabelecimento == 2)
  {
    listaEstabelecimentos[estabelecimentosAtual].tipoEstabelecimento = "OFICINA";
  }

  cout << "Com qual marca esse estabelecimento trabalha?" << endl
       << "Marca: ";
  cin.ignore();
  getline(cin, listaEstabelecimentos[estabelecimentosAtual].marcaMoto);

  cout << "Deseja tornar esse estabelecimento uma parceria: 1 - SIM / 2 - NAO" << endl
       << "Escolha : ";
  cin >> respostaParceria;
  while (respostaParceria != 1 and respostaParceria != 2)
  {
    cout << "Favor inserir entrada de dados indicada: 1 - SIM / 2 - NAO" << endl
         << "Escolha : ";
    cin >> respostaParceria;
  }
  if (respostaParceria == 1)
  {
    parceriasAtual++;
    associacao::cadastrarParceria(listaEstabelecimentos[estabelecimentosAtual].nomeEstabelecimento, listaEstabelecimentos[estabelecimentosAtual].tipoEstabelecimento, listaEstabelecimentos[estabelecimentosAtual].marcaMoto);
  }
  else if (respostaParceria == 2)
  {
    cout << "Estabelecimento cadastrado" << endl;
  }
};

void associacao::cadastrarParceria(string nomeEstabelecimento, string tipoEstabelecimento, string marcaMoto)
{
  cout << "Estabelecimento cadastrado como parceira" << endl
       << nomeEstabelecimento << endl
       << tipoEstabelecimento << endl
       << marcaMoto << endl;
  listaParcerias[parceriasAtual - 1].nomeEstabelecimento = nomeEstabelecimento;
  listaParcerias[parceriasAtual - 1].tipoEstabelecimento = tipoEstabelecimento;
  listaParcerias[parceriasAtual - 1].marcaMoto = marcaMoto;

  cout << "Para completar o cadastro insira mais algumas informações" << endl
       << "Servico prestado: ";
  cin.ignore();
  getline(cin, listaParcerias[parceriasAtual - 1].servico);
  cout << "Preço pelo serviço: R$ ";
  cin >> listaParcerias[parceriasAtual - 1].preco;
  cin.ignore();
  cout << "Forma de fidelizacao: ";
  getline(cin, listaParcerias[parceriasAtual - 1].fidelizacao);
  cout << "Beneficio: ";
  getline(cin, listaParcerias[parceriasAtual - 1].beneficio);
  cout << "Parceria cadastrada" << endl
       << endl;
};

void associacao::consultarParceria(string marca)
{
  int j = 0;
  cout << "ESTABELECIMENTOS QUE ATENDEM:" << marca << endl
       << endl;
  for (int i = 0; i < parceriasAtual; i++)
  {
    if (listaParcerias[i].marcaMoto == marca)
    {
      j++;
      cout << "Nome estabelecimento: " << listaParcerias[i].nomeEstabelecimento << endl;
      cout << "Tipo do estabelecimento: " << listaParcerias[i].tipoEstabelecimento << endl;
      cout << "Fidelizacao: " << listaParcerias[i].fidelizacao << endl;
      cout << "Beneficio: " << listaParcerias[i].beneficio << endl
           << endl;
    }
  }
  if (j == 0)
    cout << "Nenhum estabelecimento atende essa marca" << endl
         << endl;
}

void associacao::listarMotociclistas(int motociclistasAtual)
{
  cout << "Motorista cadastrados" << endl
       << endl;
  for (int i = 0; i < motociclistasAtual; i++)
  {
    cout << "Nome: " << listaMotociclistas[i].nomeMotociclista << endl
         << "Marca da moto: " << listaMotociclistas[i].marcaMoto << endl
         << "Modelo da moto: " << listaMotociclistas[i].modeloMoto << endl
         << "Ano da moto: " << listaMotociclistas[i].anoMoto << endl
         << "Telefone: " << listaMotociclistas[i].telefone << endl
         << endl
         << endl;
  }
  cout << "Total de motoristas cadastrados: " << motociclistasAtual << endl
       << endl;
}
void associacao::listarParcerias()
{
  cout << "Parcerias cadastradas: " << parceriasAtual << endl
       << endl;
  for (int i = 0; i < parceriasAtual; i++)
  {
    cout << "Nome do estabelecimento: " << listaParcerias[i].nomeEstabelecimento << endl
         << "Tipo: " << listaParcerias[i].tipoEstabelecimento << endl
         << "Marca: " << listaParcerias[i].marcaMoto << endl
         << "Servico prestado: " << listaParcerias[i].servico << endl
         << "Preço pelo serviço: R$ " << listaParcerias[i].preco << endl
         << "Fidelizacao: " << listaParcerias[i].fidelizacao << endl
         << "Beneficio: " << listaParcerias[i].beneficio << endl
         << endl;
  }
}

int main()
{
  int resposta = 0;
  int motociclistasAtual = 0;
  int estabelecimentosAtual = 0;
  string marca;

  associacao *associa = new associacao(99, 49, 49);

  cout << "SISTEMA DA ASSOCIACAO DE MOTOCICLISTAS" << endl;
  while (resposta != 6)
  {
    cout << "Qual operacao deseja fazer?" << endl
         << "1 - Cadastrar motociclista" << endl
         << "2 - Cadastrar estabelecimento" << endl
         << "3 - Consultar parceria" << endl
         << "4 - Listar motociclistas" << endl
         << "5 - Listar parcerias" << endl
         << "6 - Finalizar programa" << endl
         << "Escolha: ";

    cin >>
        resposta;
    switch (resposta)
    {
    case 1:
      motociclistasAtual++;
      associa->cadastrarMotociclistas(motociclistasAtual - 1);
      break;
    case 2:
      estabelecimentosAtual++;
      associa->cadastrarEstabelecimento(estabelecimentosAtual - 1);
      break;
    case 3:
      cout << "Qual marca deseja consultar?" << endl
           << "Marca: ";
      cin.ignore();
      getline(cin, marca);
      associa->consultarParceria(marca);
      break;
    case 4:
      associa->listarMotociclistas(motociclistasAtual);
      break;
    case 5:
      associa->listarParcerias();
      break;
    case 6:
      cout << "Programa finalizado" << endl;
      associa->~associacao();
      break;
    default:
      cout << "Digite uma opcao valida de numero por favor!!" << endl;
      break;
    }
  }

  return 0;
}