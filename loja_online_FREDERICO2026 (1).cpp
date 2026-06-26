#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <limits>
#include <iomanip>

using namespace std;

//=================== PESSOA ===================

class Pessoa {
protected:
    string nome;
    string email;
    string senha;

public:
    Pessoa() {}
    Pessoa(string n, string e, string s) : nome(n), email(e), senha(s) {}

    string getNome()  { return nome;  }
    string getEmail() { return email; }
    string getSenha() { return senha; }
};

//=================== CLIENTE ===================

class Cliente : public Pessoa {
public:
    Cliente() {}
    Cliente(string n, string e, string s) : Pessoa(n, e, s) {}
};

//=================== DONO ===================

class DonoLoja : public Pessoa {
private:
    bool cadastrado;

public:
    DonoLoja() : cadastrado(false) {}
    DonoLoja(string n, string e, string s)
        : Pessoa(n, e, s), cadastrado(true) {}

    bool estaCadastrado() { return cadastrado; }
};

//=================== PRODUTO ===================

class Produto {
private:
    int    id;
    string nome;
    double preco;
    int    estoque;

public:
    Produto() : id(0), preco(0.0), estoque(0) {}
    Produto(int i, string n, double p, int e)
        : id(i), nome(n), preco(p), estoque(e) {}

    int    getId()      { return id;      }
    string getNome()    { return nome;    }
    double getPreco()   { return preco;   }
    int    getEstoque() { return estoque; }

    void diminuirEstoque() { if (estoque > 0) estoque--; }
    bool temEstoque()      { return estoque > 0; }

    void mostrar() {
        cout << "\n  ID: "      << id;
        cout << "\n  Nome: "    << nome;
        cout << "\n  Preco: "   << fixed << setprecision(2) << preco << " MZN";
        cout << "\n  Estoque: " << estoque;
        cout << "\n  ----------------------";
    }
};

//=================== LEITURA SEGURA ===================

string lerDado(const string& prompt) {
    string valor;
    cout << prompt;
    getline(cin, valor);
    return valor;
}

//=================== VERIFICAR ID DUPLICADO ===================

bool idExiste(vector<Produto>& produtos, int id) {
    for (unsigned int i = 0; i < produtos.size(); i++)
        if (produtos[i].getId() == id) return true;
    return false;
}

//=================== NUMERO DO RECIBO ===================

int obterProximoNumeroRecibo() {
    ifstream arq("contador_recibo.txt");
    int numero = 0;
    if (arq.is_open()) { arq >> numero; arq.close(); }
    numero++;
    ofstream arqOut("contador_recibo.txt");
    arqOut << numero;
    arqOut.close();
    return numero;
}

//=================== GERAR RECIBO ===================

int gerarRecibo(string cliente, vector<Produto> carrinho, double total) {
    int numRecibo = obterProximoNumeroRecibo();

    stringstream nomeArq;
    nomeArq << "recibo_" << setw(3) << setfill('0') << numRecibo << ".txt";

    ofstream arquivo(nomeArq.str().c_str());
    time_t agora = time(0);
    struct tm* t = localtime(&agora);

    string meses[] = {
        "Janeiro","Fevereiro","Marco","Abril",
        "Maio","Junho","Julho","Agosto",
        "Setembro","Outubro","Novembro","Dezembro"
    };

    arquivo << "=========================================\n";
    arquivo << "             LOJA ONLINE\n";
    arquivo << "=========================================\n";
    arquivo << "Recibo N.: " << setfill('0') << setw(3) << numRecibo << setfill(' ') << "\n";
    arquivo << "Cliente  : " << cliente << "\n";
    arquivo << "Data     : "
            << setfill('0') << setw(2) << t->tm_mday << " de "
            << meses[t->tm_mon] << " de "
            << (t->tm_year + 1900) << "  "
            << setw(2) << t->tm_hour << ":"
            << setw(2) << t->tm_min
            << setfill(' ') << "\n";
    arquivo << "-----------------------------------------\n";
    arquivo << "\n  PRODUTOS\n\n";
    arquivo << left
            << "  " << setw(18) << "Nome"
            << setw(6)          << "Qtd"
            << setw(14)         << "Preco Unit."
            << "Subtotal" << "\n";
    arquivo << "  " << string(50, '-') << "\n";

    vector<int> vistos;
    int totalItens = 0;

    for (unsigned int i = 0; i < carrinho.size(); i++) {
        bool jaVisto = false;
        for (unsigned int v = 0; v < vistos.size(); v++)
            if (vistos[v] == carrinho[i].getId()) { jaVisto = true; break; }
        if (jaVisto) continue;

        int quantidade = 0;
        for (unsigned int j = 0; j < carrinho.size(); j++)
            if (carrinho[j].getId() == carrinho[i].getId()) quantidade++;

        double precoUnit = carrinho[i].getPreco();
        double subtotal  = precoUnit * quantidade;
        totalItens      += quantidade;

        arquivo << fixed << setprecision(2) << left << setfill(' ');
        arquivo << "  " << setw(18) << carrinho[i].getNome()
                << setw(6)  << quantidade
                << setw(10) << precoUnit << "MZN   "
                << subtotal << " MZN\n";

        vistos.push_back(carrinho[i].getId());
    }

    arquivo << "  " << string(50, '-') << "\n";
    arquivo << "\n  Itens comprados : " << totalItens << "\n";
    arquivo << fixed << setprecision(2);
    arquivo << "  TOTAL A PAGAR  : " << total << " MZN\n";
    arquivo << "\n=========================================\n";
    arquivo << "      Obrigado pela sua compra!\n";
    arquivo << "=========================================\n";
    arquivo.close();
    return numRecibo;
}

//=================== GUARDAR VENDA ===================

void guardarVenda(string cliente, double total) {
    ofstream venda("vendas.txt", ios::app);
    venda << cliente << " | " << fixed << setprecision(2) << total << " MZN\n";
    venda.close();
}

//=================== SALVAR DADOS ===================

void salvarDono(DonoLoja& dono) {
    if (!dono.estaCadastrado()) return;
    ofstream arq("dono.txt");
    arq << dono.getNome()  << ";"
        << dono.getEmail() << ";"
        << dono.getSenha() << "\n";
    arq.close();
}

void salvarClientes(vector<Cliente>& clientes) {
    ofstream arq("clientes.txt");
    for (unsigned int i = 0; i < clientes.size(); i++)
        arq << clientes[i].getNome()  << ";"
            << clientes[i].getEmail() << ";"
            << clientes[i].getSenha() << "\n";
    arq.close();
}

void salvarProdutos(vector<Produto>& produtos) {
    ofstream arq("produtos.txt");
    for (unsigned int i = 0; i < produtos.size(); i++)
        arq << produtos[i].getId()      << ";"
            << produtos[i].getNome()    << ";"
            << produtos[i].getPreco()   << ";"
            << produtos[i].getEstoque() << "\n";
    arq.close();
}

void salvarTodosDados(DonoLoja& dono, vector<Cliente>& clientes, vector<Produto>& produtos) {
    salvarDono(dono);
    salvarClientes(clientes);
    salvarProdutos(produtos);
    cout << "\n>>> Todos os dados foram salvos com sucesso nos arquivos .txt! <<<\n";
}

//=================== LER DADOS ===================

void carregarDono(DonoLoja& dono) {
    ifstream arq("dono.txt");
    if (!arq.is_open()) return;
    string linha;
    if (getline(arq, linha)) {
        stringstream ss(linha);
        string nome, email, senha;
        getline(ss, nome,  ';');
        getline(ss, email, ';');
        getline(ss, senha, ';');
        if (!nome.empty())
            dono = DonoLoja(nome, email, senha);
    }
    arq.close();
}

void carregarClientes(vector<Cliente>& clientes) {
    clientes.clear();
    ifstream arq("clientes.txt");
    if (!arq.is_open()) return;
    string linha;
    while (getline(arq, linha)) {
        stringstream ss(linha);
        string nome, email, senha;
        getline(ss, nome,  ';');
        getline(ss, email, ';');
        getline(ss, senha, ';');
        if (!nome.empty())
            clientes.push_back(Cliente(nome, email, senha));
    }
    arq.close();
}

void carregarProdutos(vector<Produto>& produtos) {
    produtos.clear();
    ifstream arq("produtos.txt");
    if (!arq.is_open()) return;
    string linha;
    while (getline(arq, linha)) {
        stringstream ss(linha);
        string sid, nome, spreco, sestoque;
        getline(ss, sid,      ';');
        getline(ss, nome,     ';');
        getline(ss, spreco,   ';');
        getline(ss, sestoque, ';');
        if (!sid.empty())
            produtos.push_back(Produto(stoi(sid), nome, stod(spreco), stoi(sestoque)));
    }
    arq.close();
}

void lerTodosDados(DonoLoja& dono, vector<Cliente>& clientes, vector<Produto>& produtos) {
    carregarDono(dono);
    carregarClientes(clientes);
    carregarProdutos(produtos);
    cout << "\n>>> Dados recuperados com sucesso dos arquivos .txt! <<<\n";
}

//=================== MAIN ===================

int main() {

    DonoLoja        dono;
    vector<Cliente> clientes;
    vector<Produto> produtos;

    // Carrega dados automaticamente ao iniciar
    lerTodosDados(dono, clientes, produtos);

    int opcao;

    do {
        cout << "\n\n========================================\n";
        cout << "        SISTEMA - LOJA ONLINE\n";
        cout << "========================================\n";
        cout << "  1. Cadastrar Dono\n";
        cout << "  2. Cadastrar Cliente\n";
        cout << "  3. Login Dono\n";
        cout << "  4. Login Cliente\n";
        cout << "  5. Salvar dados em arquivo (.txt)\n";
        cout << "  6. Ler dados do arquivo (.txt)\n";
        cout << "  0. Sair\n";
        cout << "========================================\n";
        cout << "  Opcao: ";
        cin >> opcao;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // ---- CADASTRO DONO ----
        if (opcao == 1) {
            cout << "\n--- CADASTRO DO DONO ---\n";
            string nome  = lerDado("  Nome: ");
            string email = lerDado("  Email: ");
            string senha = lerDado("  Senha: ");
            dono = DonoLoja(nome, email, senha);
            salvarDono(dono);
            cout << "\n  Dono cadastrado e salvo com sucesso!\n";
        }

        // ---- CADASTRO CLIENTE ----
        else if (opcao == 2) {
            cout << "\n--- CADASTRO DO CLIENTE ---\n";
            string nome  = lerDado("  Nome: ");
            string email = lerDado("  Email: ");
            string senha = lerDado("  Senha: ");
            clientes.push_back(Cliente(nome, email, senha));
            salvarClientes(clientes);
            cout << "\n  Cliente cadastrado e salvo com sucesso!\n";
        }

        // ---- LOGIN DONO ----
        else if (opcao == 3) {
            if (!dono.estaCadastrado()) {
                cout << "\n  Nenhum dono cadastrado ainda!\n";
            } else {
                cout << "\n--- LOGIN DO DONO ---\n";
                string email = lerDado("  Email: ");
                string senha = lerDado("  Senha: ");

                if (email == dono.getEmail() && senha == dono.getSenha()) {

                    int menuDono;
                    do {
                        cout << "\n========================================\n";
                        cout << "           AREA DO DONO\n";
                        cout << "========================================\n";
                        cout << "  1. Adicionar Produto\n";
                        cout << "  2. Listar Produtos\n";
                        cout << "  3. Ver Clientes\n";
                        cout << "  4. Ver Vendas\n";
                        cout << "  0. Voltar ao menu principal\n";
                        cout << "========================================\n";
                        cout << "  Opcao: ";
                        cin >> menuDono;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        if (menuDono == 1) {
                            int id;
                            cout << "\n  ID do produto: ";
                            cin >> id;
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');

                            if (idExiste(produtos, id)) {
                                cout << "\n  Erro: ja existe um produto com este ID!\n";
                            } else {
                                string nome = lerDado("  Nome: ");
                                double preco;
                                int estoque;
                                cout << "  Preco: ";   cin >> preco;
                                cout << "  Estoque: "; cin >> estoque;
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                produtos.push_back(Produto(id, nome, preco, estoque));
                                salvarProdutos(produtos);
                                cout << "\n  Produto cadastrado com sucesso!\n";
                            }
                        }
                        else if (menuDono == 2) {
                            if (produtos.empty())
                                cout << "\n  Nenhum produto cadastrado.\n";
                            else {
                                cout << "\n--- LISTA DE PRODUTOS ---";
                                for (unsigned int i = 0; i < produtos.size(); i++)
                                    produtos[i].mostrar();
                            }
                        }
                        else if (menuDono == 3) {
                            if (clientes.empty()) {
                                cout << "\n  Nenhum cliente cadastrado.\n";
                            } else {
                                cout << "\n--- CLIENTES ---\n";
                                for (unsigned int i = 0; i < clientes.size(); i++)
                                    cout << "  " << (i+1) << ". "
                                         << clientes[i].getNome()
                                         << " | " << clientes[i].getEmail() << "\n";
                            }
                        }
                        else if (menuDono == 4) {
                            ifstream v("vendas.txt");
                            string linha;
                            bool vazio = true;
                            cout << "\n--- VENDAS ---\n";
                            while (getline(v, linha)) {
                                cout << "  " << linha << "\n";
                                vazio = false;
                            }
                            if (vazio) cout << "  Nenhuma venda registada.\n";
                            v.close();
                        }
                        else if (menuDono != 0) {
                            cout << "\n  Opcao invalida!\n";
                        }

                    } while (menuDono != 0);

                } else {
                    cout << "\n  Login invalido! Verifique email e senha.\n";
                }
            }
        }

        // ---- LOGIN CLIENTE ----
        else if (opcao == 4) {
            cout << "\n--- LOGIN DO CLIENTE ---\n";
            string email = lerDado("  Email: ");
            string senha = lerDado("  Senha: ");

            bool   encontrado  = false;
            string clienteAtual;

            for (unsigned int i = 0; i < clientes.size(); i++) {
                if (clientes[i].getEmail() == email &&
                    clientes[i].getSenha() == senha) {
                    encontrado   = true;
                    clienteAtual = clientes[i].getNome();
                    break;
                }
            }

            if (encontrado) {
                vector<Produto> carrinho;
                int menuCliente;

                do {
                    cout << "\n========================================\n";
                    cout << "      BEM-VINDO, " << clienteAtual << "!\n";
                    cout << "========================================\n";
                    cout << "  1. Ver Produtos\n";
                    cout << "  2. Adicionar ao Carrinho\n";
                    cout << "  3. Ver Carrinho\n";
                    cout << "  4. Finalizar Compra\n";
                    cout << "  0. Sair\n";
                    cout << "========================================\n";
                    cout << "  Opcao: ";
                    cin >> menuCliente;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (menuCliente == 1) {
                        if (produtos.empty())
                            cout << "\n  Nenhum produto disponivel.\n";
                        else {
                            cout << "\n--- PRODUTOS DISPONIVEIS ---";
                            for (unsigned int i = 0; i < produtos.size(); i++)
                                produtos[i].mostrar();
                        }
                    }
                    else if (menuCliente == 2) {
                        int id;
                        cout << "\n  ID do produto: ";
                        cin >> id;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        bool achou = false;
                        for (unsigned int i = 0; i < produtos.size(); i++) {
                            if (produtos[i].getId() == id) {
                                achou = true;
                                if (!produtos[i].temEstoque()) {
                                    cout << "\n  Produto sem estoque!\n";
                                } else {
                                    int quantidade;
                                    cout << "  Quantidade (disponivel: " << produtos[i].getEstoque() << "): ";
                                    cin >> quantidade;
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                                    if (quantidade <= 0) {
                                        cout << "\n  Quantidade invalida!\n";
                                    } else if (quantidade > produtos[i].getEstoque()) {
                                        cout << "\n  Quantidade indisponivel! Estoque: " << produtos[i].getEstoque() << "\n";
                                    } else {
                                        for (int q = 0; q < quantidade; q++) {
                                            carrinho.push_back(produtos[i]);
                                            produtos[i].diminuirEstoque();
                                        }
                                        salvarProdutos(produtos);
                                        cout << "\n  " << quantidade << "x " << produtos[i].getNome() << " adicionado(s) ao carrinho!\n";
                                    }
                                }
                                break;
                            }
                        }
                        if (!achou) cout << "\n  Produto nao encontrado!\n";
                    }
                    else if (menuCliente == 3) {
                        if (carrinho.empty()) {
                            cout << "\n  Carrinho vazio!\n";
                        } else {
                            cout << "\n--- SEU CARRINHO ---\n";
                            double total = 0;
                            for (unsigned int i = 0; i < carrinho.size(); i++) {
                                cout << "  - " << carrinho[i].getNome()
                                     << " | " << fixed << setprecision(2)
                                     << carrinho[i].getPreco() << " MZN\n";
                                total += carrinho[i].getPreco();
                            }
                            cout << "  --------------------\n";
                            cout << "  Total: " << total << " MZN\n";
                        }
                    }
                    else if (menuCliente == 4) {
                        if (carrinho.empty()) {
                            cout << "\n  Carrinho vazio!\n";
                        } else {
                            double total = 0;
                            for (unsigned int i = 0; i < carrinho.size(); i++)
                                total += carrinho[i].getPreco();

                            int numRecibo = gerarRecibo(clienteAtual, carrinho, total);
                            guardarVenda(clienteAtual, total);

                            stringstream nomeArq;
                            nomeArq << "recibo_"
                                    << setw(3) << setfill('0')
                                    << numRecibo << ".txt";

                            // Mostrar recibo no ecra
                            cout << "\n";
                            ifstream reciboArq(nomeArq.str().c_str());
                            string linhaRecibo;
                            while (getline(reciboArq, linhaRecibo))
                                cout << "  " << linhaRecibo << "\n";
                            reciboArq.close();

                            cout << "\n  Compra finalizada com sucesso!\n";
                            cout << "  Recibo salvo em: " << nomeArq.str() << "\n";
                            carrinho.clear();
                        }
                    }
                    else if (menuCliente != 0) {
                        cout << "\n  Opcao invalida!\n";
                    }

                } while (menuCliente != 0);

            } else {
                cout << "\n  Email ou senha incorretos. Tente novamente.\n";
            }
        }

        // ---- SALVAR DADOS EM ARQUIVO ----
        else if (opcao == 5) {
            cout << "\n--- GRAVACAO EM ARQUIVO .TXT ---\n";
            salvarTodosDados(dono, clientes, produtos);
        }

        // ---- LER DADOS DO ARQUIVO ----
        else if (opcao == 6) {
            cout << "\n--- RECUPERACAO DE DADOS DO ARQUIVO .TXT ---\n";
            lerTodosDados(dono, clientes, produtos);
        }

        else if (opcao != 0) {
            cout << "\n  Opcao invalida! Tente novamente.\n";
        }

    } while (opcao != 0);

    // Salva automaticamente ao sair
    salvarTodosDados(dono, clientes, produtos);
    cout << "\n  Sistema encerrado. Ate logo!\n\n";
    return 0;
}
