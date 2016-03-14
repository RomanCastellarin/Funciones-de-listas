/**
    INT�RPRETE DE FUNCIONES DE LISTAS
    Rom�n Castellarin
*/
#include <iostream>     // entrada y salida por teclado y pantalla
#include <fstream>      // entrada y salida por archivo
#include <list>         // listas
#include <map>          // mapeos
#include <string>       // cadenas de caracteres
#include <algorithm>    // ordenamiento
#define forn(a, b) for(int a = 0; a < int(b); ++a)      // atajos
#define rforn(a, b) for(int a = int(b-1); a >= 0; --a)
#ifdef __unix__
    #define limpiarPantalla system("clear")
#else
    #define limpiarPantalla system("cls")
#endif
using namespace std;

/// nuestros tipos de dato
typedef list<unsigned int> lista;   // lista de enteros positivos
typedef list<struct fun> comp;      // lista de m�dulos de funciones

/// funciones elementales
enum{ REPETIR = 0, BORRA_DER, BORRA_IZQ, CERO_DER, CERO_IZQ, SIG_DER, SIG_IZQ };
string Etiqueta[] = { "", "Bd", "Bi", "Od", "Oi", "Sd", "Si" };

/// expresion global y lista de funciones
int Idx; string S;
map<string, comp> M;

/// m�dulo de funcion: funci�n elemental o repeticion
struct fun{
    fun(int v = REPETIR) : valor(v) {}  // por defecto, un m�dulo es repetici�n
    int valor;                          // alguno de los enumerados de arriba
    comp pasos;                     // si es repetici�n, composici�n a repetir
};

/// prototipos
void evaluar(fun &, lista &, bool);
void evaluar(comp &, lista &, bool);
inline void limpiarNombre(string &);
string leerNombre();
bool parsear(comp &);
void mostrar(comp &, ostream &, bool);
void mostrar(lista &);

void definirFuncion(){
    /// define una funci�n nueva
    string nombre; comp f;
    // ingreso nombre y ley
    cout<<"Ingrese el nombre de su funcion:"<<endl; cin>>nombre; cin.ignore();
    cout<<"Ingrese la ley de su funcion:"<<endl;    getline(cin, S);
    // si no se pudo interpretar la funci�n, salir tempranamente
    Idx = 0; if( !parsear(f) ) return;
    // en caso contrario, normalizar el nombre y asociarlo a la ley
    limpiarNombre(nombre); M[ nombre ] = f;
}

void evaluarFuncion(bool apasos){
    /// toma una funci�n f y una lista L, y aplica f a L, modific�ndola
    /// apasos: true/false, indica si se deben mostrar los pasos intermedios
    string nombre; lista l; int n, v;
    // ingreso nombre de funci�n, normalizaci�n
    cout<<"Ingrese el nombre de su funcion:"<<endl; cin>>nombre;
    limpiarNombre(nombre);
    // si la funci�n no est� definida, volver tempranamente
    if( !M.count(nombre) ){
        cout<<"No se encontro la funcion especificada."<<endl;
        return;
    }
    // en caso contrario, ingresar lista concreta a evaluar
    cout<<"Ingrese la longitud de la lista:"<<endl; cin>>n;
    cout<<"Ingrese los valores de la lista separados por blancos:"<<endl;
    forn(i, n){
        // leo entero y lo adjunto al final de la lista que voy construyendo
        cin>>v; l.push_back(v);
    }
    // obtener ley de la funci�n a partir de su nombre
    comp f = M[ nombre ];
    cout<<"Resultado:"<<endl;
    evaluar(f, l, apasos);  // llamar a la funci�n evaluadora
    mostrar(l);             // llamar a la funci�n que muestra listas
    cout<<endl;
}

void verFunciones(){
    /// muestra una lista de funciones no elementales y c�mo est�n compuestas
    for(auto &p : M)
        // por cada funci�n p = ( nombre, ley ), si no es elemental
        if( !binary_search(Etiqueta+1, Etiqueta+7, p.first ) ){
            // mostrar nombre, luego dos puntos y luego mostrar ley
            cout<<p.first<<": "; mostrar(p.second, cout, false);
            cout<<endl<<endl;
    }
}

void guardarFunciones(){
    /// guarda un archivo de definiciones
    // crea/sobreescribe un archivo funciones.txt en el directorio actual
    ofstream fout("funciones.txt");
    for(auto &p : M){
        // por cada funci�n p = ( nombre, ley ), mostrar nombre, espacio, ley
        fout<<p.first<<" "; mostrar(p.second, fout, true); fout<<endl;
    }
}

void leerFunciones(){
    /// carga un archivo de definiciones
    // intenta abrir un archivo funciones.txt en el directorio actual
    ifstream fin("funciones.txt");
    string nombre;
    // mientras haya algo qu� leer, leer nombre y ley
    while(fin>>nombre>>S){
        comp f;
        limpiarNombre(nombre);  // normalizar nombre
        Idx = 0; parsear(f);    // interpretar ley
        M[ nombre ] = f;        // asociar nombre con ley
    }
}

void acerca(){
    cout<<"Un interprete de funciones de listas.\n"
        <<"Las funciones de lista son 'Turing completo', es decir que pueden en\n"
        <<"teoria (con memoria virtualmente ilimitada y procesadores arbitrariamente\n"
        <<"veloces) computar cualquier cosa que una computadora pueda.\n"
        <<"Todo a partir de 3 operaciones elementales sobre listas y la repeticion.\n"
        <<"Roman Castellarin."<<endl;
}

void inicializar(){
    /// tareas de arranque de programa
    // cargar funciones elementales
    for(int i = 1; i < 7; ++i)     M[ Etiqueta[ i ] ] = { i };
    // cargar funciones �tiles no elementales
    comp f;
    S = "(Si)Oi";                 Idx = 0; f.clear(); parsear(f); M[ "Cd" ] = f;
    S = "(Sd)Od";                 Idx = 0; f.clear(); parsear(f); M[ "Ci" ] = f;
    S = "BdCd";                   Idx = 0; f.clear(); parsear(f); M[ "<" ] = f;
    S = "BiCi";                   Idx = 0; f.clear(); parsear(f); M[ ">" ] = f;
    S = "<BiBd(>>Sd<<Sd)Od>Od<";  Idx = 0; f.clear(); parsear(f); M[ "=" ] = f;
    S = "<BiBd(>Sd<Sd)Od>";       Idx = 0; f.clear(); parsear(f); M[ "+" ] = f;
    leerFunciones();    // si hubiera archivo de definiciones presente, cargarlo
}

/// MAIN
int main(){
    inicializar();
    int opcion;
    do{
        cout<<"Ingrese una opcion:\n"
            <<"1. Definir funcion\n"
            <<"2. Evaluar funcion\n"
            <<"3. Evaluar funcion paso a paso\n"
            <<"4. Ver funciones definidas\n"
            <<"5. Guardar archivo de definiciones\n"
            <<"6. Leer archivo de definiciones\n"
            <<"7. Acerca de...\n"
            <<"8. Salir"<<endl;
        cin>>opcion;
        limpiarPantalla;
        switch( opcion ){
            case 1: definirFuncion();       break;
            case 2: evaluarFuncion(false);  break;
            case 3: evaluarFuncion(true);   break;
            case 4: verFunciones();         break;
            case 5: guardarFunciones();     break;
            case 6: leerFunciones();        break;
            case 7: acerca();               break;
            case 8: /*exit*/                break;
            default: cout<<"Orden desconocida"<<endl;
        }
        cout<<"----------"<<endl;
    }while( opcion != 8 );  // mientras no se quiera salir, mostrar men�
    return 0;
}

void evaluar(comp &c, lista &l, bool apasos){
    /// toma una composici�n y una lista y aplica cada m�dulo de fun. a la lista
    for(auto i = c.rbegin(); i != c.rend(); ++i)
        // comenzando desde la funci�n "*i" "de m�s adentro" hacia afuera
        evaluar(*i, l, apasos); // aplicar "*i" a la lista
}

void evaluar(fun &f, lista &l, bool apasos){
    /// CORAZ�N 1ro DEL INT�RPRETE
    /// toma un m�dulo de funci�n y se lo aplica a la lista
    if( f.valor == REPETIR ){   // si es una repetici�n, repetir hasta que el
        while( l.front() != l.back() ) // primer elem. sea igual al ultimo elem.
            evaluar(f.pasos, l, apasos);  // recusivamente aplicar composici�n
    }else{
        // si no es una repetici�n...
        if( apasos ) mostrar(l);    // Mostrar paso intermedio si lo pidieron
        // ...entonces es alguno de las siguientes funciones elementales:
        switch( f.valor ){
            case CERO_IZQ: l.push_front(0); break;  // cero a la izquierda
            case CERO_DER: l.push_back(0); break;   // cero a la derecha
            case BORRA_IZQ: l.pop_front(); break;   // borrar izquierda
            case BORRA_DER: l.pop_back(); break;    // borrar derecha
            case SIG_IZQ: l.front()++; break;       // siguiente de la izquierda
            case SIG_DER: l.back()++; break;        // siguiente de la derecha
        }
    }
}

inline void limpiarNombre(string &s){
    /// normaliza nombres
    // primera letra en may�scula, el resto en min�sculas
    forn(i, s.size()) s[i] = i ? tolower( s[i] ) : toupper( s[i] );
}

string leerNombre(){
    /// lee la expresi�n global S y devuelve un nombre normalizado
    string f;
    f.push_back( S[Idx++] );   // el primer car�cter siempre es parte del nombre
    // mientras le sigan letras min�sculas, entonces es parte del nombre
    while( islower(S[Idx]) ) f.push_back( S[Idx++] );
    limpiarNombre(f);   // creo que no hace falta (?
    return f;
}

bool parsear(comp &C){
    /// CORAZ�N 2do DEL INT�RPRETE
    /// toma una cadena de caracteres global (S), representando una ley, y una
    /// composici�n C posiblemente vac�a, interpreta la ley y la anexa a C
    while( S[Idx] ){            // mientras haya funciones que ir componiendo
        if( S[Idx] == ' ' ){    // ignorar los blancos
            Idx++;
            continue;
        }else
        if( S[Idx] == '(' ){    // arranca una repetici�n
            Idx++;
            fun f( REPETIR );   // entonces creo un m�dulo de repetici�n
            // recursivamente parsear en la subcomposici�n (bajo de nivel)
            if( !parsear( f.pasos ) ) return false; // si hubo errores, abortar
            C.push_back( f );   // anexar el nuevo m�dulo a la repetici�n
        }else
        if( S[Idx] == ')' ){    // termina una repetici�n, ok.
            ++Idx;
            return true;        // volver al nivel superior
        }else{  // es una m�dulo com�n
            string nombre = leerNombre();   // leo el nombre de funci�n
            // si se lee el nombre de una funci�n no definida, abortar
            if( !M.count( nombre ) ){
                cout<<"No se encontro la funcion especificada"<<endl;
                return false;
            }
            // de otro modo, copiar la definici�n de la fun. en la comp. nueva
            for(auto &f : M[ nombre ]) C.push_back( f );
        }
    }
    return true;    // si no hubo errores, retornar verdadero
}

void mostrar(comp &c, ostream& out, bool archivo){
    /// imprime una composici�n a la salida "out" (archivo o teclado)
    /// archivo: true/false, suprime espacios al escribir leyes en archivos
    for(auto &n : c )
        if( n.valor == REPETIR ){   // si es repetici�n, encerrar en par�ntesis
                out<<(archivo ? "(" : "( ");
                mostrar(n.pasos, out, archivo); // recursivamente imprimir ley
                out<<(archivo ? ")" : ") ");
        }else   out<<Etiqueta[n.valor]<<(archivo ? "" : " "); // imprimir nombre
}

void mostrar(lista &l){
    /// muestra una lista
    cout<<'{';  // entre llaves
    for(auto i = l.begin(); i != l.end(); ++i){
        if( i != l.begin() ) cout<<", "; // separando por coma los elementos
        cout<<*i;
    }
    cout<<'}'<<endl;
}
