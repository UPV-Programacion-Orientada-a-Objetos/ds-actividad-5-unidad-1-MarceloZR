#include <iostream>
#include <iomanip> 

template <typename t>
class matriz_base {
protected:
    int f; // filas
    int c; // columnas

public:
    matriz_base(int filas, int columnas) : f(filas), c(columnas) {}

    // Destructor virtual (Obligatorio para polimorfismo)
    virtual ~matriz_base() {}

    // Getters
    int get_f() const { return f; }
    int get_c() const { return c; }

    // Métodos Virtuales Puros (Interfaz)
    virtual void cargar_valores() = 0;
    virtual matriz_base<t>* sumar(const matriz_base<t>& otra) const = 0;
    virtual void imprimir() const = 0;

    // Sobrecarga del Operador +
    matriz_base<t>* operator+(const matriz_base<t>& otra) const {
        return this->sumar(otra);
    }
};


template <typename t>
class matriz_dinamica : public matriz_base<t> {
private:
    t **d; // datos

    void _asignar_memoria() {
        if (this->f > 0 && this->c > 0) {
            d = new t*[this->f];
            for (int i = 0; i < this->f; ++i) {
                d[i] = new t[this->c];
            }
        } else {
            d = nullptr;
        }
    }

    void _liberar_memoria() {
        if (d) {
            std::cout << "Liberando memoria de " << this << " (Matriz Dinámica)..." << std::endl;
            for (int i = 0; i < this->f; ++i) {
                delete[] d[i];
            }
            delete[] d;
            d = nullptr;
        }
    }

    void _copia_profunda(const matriz_dinamica<t>& otra) {
        this->f = otra.f;
        this->c = otra.c;
        _asignar_memoria();

        for (int i = 0; i < this->f; ++i) {
            for (int j = 0; j < this->c; ++j) {
                d[i][j] = otra.d[i][j];
            }
        }
    }

public:
    matriz_dinamica(int filas, int columnas) : matriz_base<t>(filas, columnas) {
        _asignar_memoria();
    }

    // Destructor (Regla de los Cinco)
    virtual ~matriz_dinamica() override {
        std::cout << "Llamando al destructor de " << this << "..." << std::endl;
        _liberar_memoria();
    }

    // Constructor de Copia (Regla de los Cinco)
    matriz_dinamica(const matriz_dinamica<t>& otra) 
        : matriz_base<t>(otra.f, otra.c) {
        _copia_profunda(otra);
    }

    // Operador de Asignación (Regla de los Cinco)
    matriz_dinamica<t>& operator=(const matriz_dinamica<t>& otra) {
        if (this != &otra) {
            _liberar_memoria(); // Liberar recursos viejos
            _copia_profunda(otra); // Copiar
        }
        return *this;
    }
    
    virtual void cargar_valores() override {
        if (this->f == 3 && this->c == 2) { // Valores del ejemplo
            d[0][0] = static_cast<t>(1.5); d[0][1] = static_cast<t>(2.0);
            d[1][0] = static_cast<t>(0.0); d[1][1] = static_cast<t>(1.0);
            d[2][0] = static_cast<t>(4.5); d[2][1] = static_cast<t>(3.0);
        }
    }

    // Implementación de sumar (Devolver NEW matriz_dinamica)
    virtual matriz_base<t>* sumar(const matriz_base<t>& otra) const override {
        if (this->f != otra.get_f() || this->c != otra.get_c()) {
            std::cout << "Error: Dimensiones incompatibles para la suma." << std::endl;
            return nullptr;
        }

        matriz_dinamica<t>* res = new matriz_dinamica<t>(this->f, this->c);
        
        // Downcast a matriz_dinamica para acceder a sus datos.
        const matriz_dinamica<t>* otra_d = dynamic_cast<const matriz_dinamica<t>*>(&otra);
        if (otra_d) {
            for (int i = 0; i < this->f; ++i) {
                for (int j = 0; j < this->c; ++j) {
                    res->d[i][j] = d[i][j] + otra_d->d[i][j];
                }
            }
        } else {
             delete res;
             return nullptr;
        }

        return res;
    }

    virtual void imprimir() const override {
        for (int i = 0; i < this->f; ++i) {
            std::cout << "|";
            for (int j = 0; j < this->c; ++j) {
                std::cout << std::fixed << std::setprecision(1) << std::setw(4) << d[i][j] << " |";
            }
            std::cout << std::endl;
        }
    }
};


template <typename t, int f_max, int c_max>
class matriz_estatica : public matriz_base<t> {
private:
    t d[f_max][c_max]; // Arreglo fijo

public:
    matriz_estatica() : matriz_base<t>(f_max, c_max) {}

    virtual ~matriz_estatica() override {}

    virtual void cargar_valores() override {
        if (f_max == 3 && c_max == 2) { // Valores del ejemplo
            d[0][0] = static_cast<t>(0.5); d[0][1] = static_cast<t>(1.0);
            d[1][0] = static_cast<t>(2.0); d[1][1] = static_cast<t>(3.0);
            d[2][0] = static_cast<t>(1.0); d[2][1] = static_cast<t>(1.0);
        }
    }

    // Implementación de sumar (Devolver NEW matriz_estatica)
    virtual matriz_base<t>* sumar(const matriz_base<t>& otra) const override {
        if (this->f != otra.get_f() || this->c != otra.get_c()) {
            return nullptr;
        }

        matriz_estatica<t, f_max, c_max>* res = new matriz_estatica<t, f_max, c_max>();
        
        // Downcast a matriz_estatica
        const matriz_estatica<t, f_max, c_max>* otra_e = dynamic_cast<const matriz_estatica<t, f_max, c_max>*>(&otra);
        if (otra_e) {
            for (int i = 0; i < f_max; ++i) {
                for (int j = 0; j < c_max; ++j) {
                    res->d[i][j] = d[i][j] + otra_e->d[i][j];
                }
            }
        } else {
            delete res;
            return nullptr;
        }

        return res;
    }

    virtual void imprimir() const override {
        for (int i = 0; i < f_max; ++i) {
            std::cout << "|";
            for (int j = 0; j < c_max; ++j) {
                std::cout << std::fixed << std::setprecision(1) << std::setw(4) << d[i][j] << " |";
            }
            std::cout << std::endl;
        }
    }
};


int main() {
    std::cout << std::fixed << std::setprecision(1);

    std::cout << "--- Sistema Genérico de Álgebra Lineal ---" << std::endl;
    std::cout << "\n>> Demostración de Genericidad (Tipo FLOAT) <<" << std::endl;
    
    // 1. Creación de Matriz Dinámica A (3x2)
    std::cout << "// 1. Creación de Matriz Dinámica A (a través del puntero base)" << std::endl;
    std::cout << "Creando Matriz Dinámica A (3x2)..." << std::endl;
    matriz_base<float>* a = new matriz_dinamica<float>(3, 2);
    a->cargar_valores();

    std::cout << "A =" << std::endl;
    a->imprimir();

    // 2. Creación de Matriz Estática B (3x2)
    std::cout << "\n// 2. Creación de Matriz Estática B (a través del puntero base)" << std::endl;
    std::cout << "Creando Matriz Estática B (3x2)..." << std::endl;
    matriz_base<float>* b = new matriz_estatica<float, 3, 2>();
    b->cargar_valores();

    std::cout << "B =" << std::endl;
    b->imprimir();

    // 3. Operación Polimórfica (Suma)
    std::cout << "\n// 3. Operación Polimórfica (Suma)" << std::endl;
    std::cout << "SUMANDO: Matriz C = A + B ..." << std::endl;
    // La suma es manejada por el método virtual de matriz_dinamica (tipo de 'a')
    matriz_base<float>* c = *a + *b;
    
    if (c) {
        std::cout << "Matriz Resultado C (3x2, Tipo FLOAT):" << std::endl;
        c->imprimir();
    }

    // Demostración de Limpieza de Memoria
    std::cout << "\n>> Demostración de Limpieza de Memoria <<" << std::endl;
    
    if (c) {
        std::cout << "Llamando al destructor de C..." << std::endl;
        delete c; 
    }
    
    std::cout << "Llamando al destructor de B..." << std::endl;
    delete b; 
    
    std::cout << "Llamando al destructor de A..." << std::endl;
    delete a; 

    std::cout << "Sistema cerrado." << std::endl;

    return 0;
}