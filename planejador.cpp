#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>

#include "planejador.h"

using namespace std;

/* *************************
 * CLASSE IDPONTO        *
 ************************* */

/// Atribuicao de string
void IDPonto::set(string &&S) {
  t = std::move(S);
  if (!valid())
    t.clear();
}

/* *************************
 * CLASSE IDROTA         *
 ************************* */

/// Atribuicao de string
void IDRota::set(string &&S) {
  t = std::move(S);
  if (!valid())
    t.clear();
}

/* *************************
 * CLASSE PONTO          *
 ************************* */

/// Distancia entre 2 pontos (formula de haversine)
double haversine(const Ponto &P1, const Ponto &P2) {
  // Tratar logo pontos identicos
  if (P1.id == P2.id)
    return 0.0;

  static const double MY_PI = 3.14159265358979323846;
  static const double R_EARTH = 6371.0;
  // Conversao para radianos
  double lat1 = MY_PI * P1.latitude / 180.0;
  double lat2 = MY_PI * P2.latitude / 180.0;
  double lon1 = MY_PI * P1.longitude / 180.0;
  double lon2 = MY_PI * P2.longitude / 180.0;

  double cosseno =
      sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon1 - lon2);
  // Para evitar eventuais erros na funcao acos por imprecisao numerica
  // nas operacoes com double: acos(1.0000001) eh NAN
  if (cosseno > 1.0)
    cosseno = 1.0;
  if (cosseno < -1.0)
    cosseno = -1.0;
  // Distancia entre os pontos
  return R_EARTH * acos(cosseno);
}

/* *************************
 * CLASSE PLANEJADOR     *
 ************************* */

/// Torna o mapa vazio
void Planejador::clear() {
  pontos.clear();
  rotas.clear();
}

/// Retorna um Ponto do mapa, passando a id como parametro.
/// Se a id for inexistente, retorna um Ponto vazio.
Ponto Planejador::getPonto(const IDPonto &Id) const {
  auto it = find(pontos.begin(), pontos.end(), Id);
  return (it != pontos.end()) ? *it : Ponto();
}

/// Retorna um Rota do mapa, passando a id como parametro.
/// Se a id for inexistente, retorna um Rota vazio.
Rota Planejador::getRota(const IDRota &Id) const {
  auto it = find(rotas.begin(), rotas.end(), Id);
  return (it != rotas.end()) ? *it : Rota();
}

/// Imprime os pontos do mapa no console
void Planejador::imprimirPontos() const {
  for (const auto &P : pontos) {
    cout << P.id << '\t' << P.nome << " (" << P.latitude << ',' << P.longitude
         << ")\n";
  }
}

/// Imprime as rotas do mapa no console
void Planejador::imprimirRotas() const {
  for (const auto &R : rotas) {
    cout << R.id << '\t' << R.nome << '\t' << R.comprimento << "km"
         << " [" << R.extremidade[0] << ',' << R.extremidade[1] << "]\n";
  }
}

/// Leh um mapa dos arquivos arq_pontos e arq_rotas.
/// Caso nao consiga ler dos arquivos, deixa o mapa inalterado e retorna false.
/// Retorna true em caso de leitura bem sucedida
bool Planejador::ler(const std::string &arq_pontos,
                     const std::string &arq_rotas) {
  // Listas temporarias para armazenamento dos dados lidos
  list<Ponto> listP;
  list<Rota> listR;
  // Variaveis auxiliares para buscas nas listas
  list<Ponto>::iterator itr_ponto;
  list<Rota>::iterator itr_rota;
  // Variaveis auxiliares para leitura de dados
  Ponto P;
  Rota R;
  string prov;

  // Leh os pontos do arquivo
  try {
    // Abre o arquivo de pontos
    ifstream arq(arq_pontos);
    if (!arq.is_open())
      throw 1;

    // Leh o cabecalho
    getline(arq, prov);
    if (arq.fail() || prov != "ID;Nome;Latitude;Longitude")
      throw 2;

    // Leh os pontos
    do {
      // Leh a ID
      getline(arq, prov, ';');
      if (arq.fail())
        throw 3;
      P.id.set(std::move(prov));
      if (!P.valid())
        throw 4;

      // Leh o nome
      getline(arq, prov, ';');
      if (arq.fail() || prov.size() < 2)
        throw 5;
      P.nome = std::move(prov);

      // Leh a latitude
      arq >> P.latitude;
      if (arq.fail())
        throw 6;
      arq.ignore(1, ';');

      // Leh a longitude
      arq >> P.longitude;
      if (arq.fail())
        throw 7;
      arq >> ws;

      itr_ponto = find(listP.begin(), listP.end(), P);

      if (itr_ponto != listP.end())
        throw 8;

      // Verifica se jah existe ponto com a mesma ID no conteiner de pontos
      // lidos (listP) Caso exista, throw 8
      /* ***********  /
      /  FALTA FAZER  /
      /  *********** */

      // Inclui o ponto na lista de pontos
      listP.push_back(std::move(P));
    } while (!arq.eof());

    // Fecha o arquivo de pontos
    arq.close();
  } catch (int i) {
    cerr << "Erro " << i << " na leitura do arquivo de pontos " << arq_pontos
         << endl;
    return false;
  }

  // Leh as rotas do arquivo
  try {
    // Abre o arquivo de rotas
    ifstream arq(arq_rotas);
    if (!arq.is_open())
      throw 1;

    // Leh o cabecalho
    getline(arq, prov);
    if (arq.fail() ||
        prov != "ID;Nome;Extremidade 1;Extremidade 2;Comprimento") {
      throw 2;
    }

    // Leh as rotas
    do {
      // Leh a ID
      getline(arq, prov, ';');
      if (arq.fail())
        throw 3;
      R.id.set(std::move(prov));
      if (!R.valid())
        throw 4;

      // Leh o nome
      getline(arq, prov, ';');
      if (arq.fail() || prov.size() < 2)
        throw 4;
      R.nome = std::move(prov);

      // Leh a id da extremidade[0]
      getline(arq, prov, ';');
      if (arq.fail())
        throw 6;
      R.extremidade[0].set(std::move(prov));
      if (!R.extremidade[0].valid())
        throw 7;

      itr_ponto = find(listP.begin(), listP.end(), R.extremidade[0]);

      if (itr_ponto == listP.end())
        throw 8;
      // Verifica se a Id corresponde a um ponto no conteiner de pontos lidos
      // (listP) Caso ponto nao exista, throw 8
      /* ***********  /
      /  FALTA FAZER  /
      /  *********** */

      // Leh a id da extremidade[1]
      getline(arq, prov, ';');
      if (arq.fail())
        throw 9;
      R.extremidade[1].set(std::move(prov));
      if (!R.extremidade[1].valid())
        throw 10;

      itr_ponto = find(listP.begin(), listP.end(), R.extremidade[1]);

      if (itr_ponto == listP.end())
        throw 8;

      // Verifica se a Id corresponde a um ponto no conteiner de pontos
      // lidos (listP) Caso ponto nao exista, throw 11
      /* ***********  /
      /  FALTA FAZER  /
      /  *********** */

      // Leh o comprimento
      arq >> R.comprimento;
      if (arq.fail())
        throw 12;
      arq >> ws;

      itr_rota = find(listR.begin(), listR.end(), R);

      if (itr_rota != listR.end())
        throw 13;
      // Verifica se jah existe rota com a mesma ID no conteiner de rotas lidas
      // (listR) Caso exista, throw 13
      /* ***********  /
      /  FALTA FAZER  /
      /  *********** */

      // Inclui a rota na lista de rotas
      listR.push_back(std::move(R));
    } while (!arq.eof());

    // Fecha o arquivo de rotas
    arq.close();
  } catch (int i) {
    cerr << "Erro " << i << " na leitura do arquivo de rotas " << arq_rotas
         << endl;
    return false;
  }

  // Soh chega aqui se nao entrou no catch, jah que ele termina com return.
  // Move as listas de pontos e rotas para o planejador.
  pontos = std::move(listP);
  rotas = std::move(listR);

  return true;
}

/// *******************************************************************************
/// Calcula o caminho entre a origem e o destino do planejador usando o
/// algoritmo A*
/// *******************************************************************************

/// Noh: os elementos dos conjuntos de busca do algoritmo A*
/* ***********  /
/  FALTA FAZER  /
/  *********** */

/// Calcula o caminho entre a origem e o destino do planejador usando o
/// algoritmo A* Retorna o comprimento do caminho encontrado.
/// (<0 se  parametros invalidos ou nao existe caminho).
/// O parametro C retorna o caminho encontrado
/// (vazio se  parametros invalidos ou nao existe caminho).
/// O parametro NA retorna o numero de nos em aberto ao termino do algoritmo A*
/// (<0 se parametros invalidos, retorna >0 mesmo quando nao existe caminho).
/// O parametro NF retorna o numero de nos em fechado ao termino do algoritmo A*
/// (<0 se parametros invalidos, retorna >0 mesmo quando nao existe caminho).
double Planejador::calculaCaminho(const IDPonto &id_origem,
                                  const IDPonto &id_destino, Caminho &C,
                                  int &NA, int &NF) {
  C.clear(); // Inicializa o caminho como vazio

  try {
    if (empty())
      throw 1; // Verifica se o mapa está vazio

    Ponto pt_orig = getPonto(id_origem); // Obtém o ponto de origem
    if (!pt_orig.valid())
      throw 4; // Verifica a validade do ponto

    Ponto pt_dest = getPonto(id_destino); // Obtém o ponto de destino
    if (!pt_dest.valid())
      throw 5; // Verifica a validade do ponto

    // Conjuntos para os nós abertos e fechados
    std::list<Noh> Aberto, Fechado;

    // Inicializa o nó inicial
    Noh atual{id_origem, IDRota(), 0.0, haversine(pt_orig, pt_dest)};
    Aberto.push_back(atual);

    while (!Aberto.empty()) {
      // Ordena e seleciona o nó de menor custo
      Aberto.sort();
      atual = Aberto.front();
      Aberto.pop_front();

      // Adiciona o nó em Fechado
      Fechado.push_back(atual);

      // Verifica se chegou ao destino
      if (atual.id_pt == id_destino) {
        C.clear();
        while (!atual.id_rt.valid()) {
          // Reconstrói o caminho
          C.emplace_front(atual.id_rt, atual.id_pt);

          // Recupera o antecessor
          auto rota_ant = getRota(atual.id_rt);
          auto id_pt_ant = (rota_ant.extremidade[0] == atual.id_pt)
                               ? rota_ant.extremidade[1]
                               : rota_ant.extremidade[0];
          auto it_ant = std::find_if(
              Fechado.begin(), Fechado.end(),
              [&id_pt_ant](const Noh &noh) { return noh.id_pt == id_pt_ant; });
          if (it_ant == Fechado.end())
            break; // Caso não encontre, termina
          atual = *it_ant;
        }

        C.emplace_front(IDRota(), id_origem); // Adiciona a origem ao caminho
        NA = Aberto.size();
        NF = Fechado.size();
        return atual.g; // Retorna o comprimento do caminho
      }

      // Gera os sucessores do nó atual
      for (const auto &rota : rotas) {
        if (rota.extremidade[0] == atual.id_pt ||
            rota.extremidade[1] == atual.id_pt) {
          IDPonto id_suc = (rota.extremidade[0] == atual.id_pt)
                               ? rota.extremidade[1]
                               : rota.extremidade[0];
          Ponto pt_suc = getPonto(id_suc);

          double g_suc = atual.g + rota.comprimento;
          double h_suc = haversine(pt_suc, pt_dest);

          // Verifica se o sucessor já está em Fechado
          auto it_fechado = std::find_if(
              Fechado.begin(), Fechado.end(),
              [&id_suc](const Noh &noh) { return noh.id_pt == id_suc; });
          if (it_fechado != Fechado.end())
            continue;

          // Verifica se o sucessor já está em Aberto
          auto it_aberto = std::find_if(
              Aberto.begin(), Aberto.end(),
              [&id_suc](const Noh &noh) { return noh.id_pt == id_suc; });

          if (it_aberto != Aberto.end()) {
            if (g_suc + h_suc < it_aberto->f()) {
              Aberto.erase(it_aberto);
              Aberto.push_back({id_suc, rota.id, g_suc, h_suc});
            }
          } else {
            Aberto.push_back({id_suc, rota.id, g_suc, h_suc});
          }
        }
      }
    }

    throw 2; // Caso nenhuma solução seja encontrada
  } catch (int i) {
    std::cerr << "Erro " << i << " no cálculo do caminho\n";
  }

  NA = NF = -1;
  return -1.0; // Retorna indicando que o caminho não foi encontrado
}

void Planejador::reconstruir_caminho(const Noh &atual,
                                     const std::list<Noh> &fechado,
                                     Caminho &C) {

  C.clear();
  Noh noh = atual;

  while (noh.id_rt.valid()) {
    C.push_front({noh.id_rt, noh.id_pt});
    auto it = find(fechado.begin(), fechado.end(), noh);
    if (it == fechado.end())
      break;
    noh = *it;
  }

  C.push_front({IDRota(), noh.id_pt});
}
