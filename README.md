# Simulador de Fábrica Multi-hilo (Productor-Consumidor)

Simulación en **C** del funcionamiento de una fábrica con múltiples líneas de producción concurrentes, desarrollada para la asignatura de Sistemas Operativos (UC3M). Implementa el patrón clásico **productor-consumidor** usando **POSIX Threads**, semáforos y variables de condición.

## Descripción

El sistema simula una fábrica jerárquica de tres niveles:

```
factory_manager                 # Lanza y coordina los process_manager
   ├── process_manager (cinta 1)
   │      ├── producer  ──▶ cinta de transporte (buffer circular) ──▶  consumer
   ├── process_manager (cinta 2)
   │      └── ...
   └── ...
```

- **`factory_manager`**: lee el fichero de configuración de la fábrica, valida su contenido y lanza los hilos `process_manager` necesarios, sincronizando su ejecución mediante **semáforos** para respetar el orden y el número máximo de procesos indicado.
- **`process_manager`**: gestiona una cinta de transporte (cola circular) y lanza un hilo **productor** y un hilo **consumidor** que trabajan sobre ella.
- **`queue`**: cola circular *thread-safe*, implementada con **mutex y variables de condición**, que soporta inserción y extracción bloqueante cuando está llena/vacía respectivamente.

## Estructura del código

```
factory_manager.c   # Lectura/validación del fichero de configuración y lanzamiento de hilos
process_manager.c   # Gestión de la cinta y de los hilos productor/consumidor
queue.c / queue.h   # Cola circular thread-safe (buffer compartido)
Makefile
```

### Funciones principales de la cola (`queue.c`)

| Función | Descripción |
|---|---|
| `queue_init(int num_elements)` | Reserva e inicializa la cola circular |
| `queue_put(struct element *ele)` | Inserta un elemento (bloquea si está llena) |
| `queue_get(void)` | Extrae un elemento (bloquea si está vacía) |
| `queue_empty(void)` / `queue_full(void)` | Consultan el estado de la cola |
| `queue_destroy(void)` | Libera todos los recursos asociados |

## Formato del fichero de entrada

```
<nº máx. cintas> [<id cinta> <tamaño cinta> <nº elementos>]+
```

Ejemplo:

```
4 5 5 2 1 2 3 3 5 2
```

Define un máximo de 4 líneas de producción; la primera (id 5) con cinta de tamaño 5 y 2 elementos a producir, y así sucesivamente. Los IDs no necesitan ser consecutivos, pero el orden de ejecución especificado en el fichero se preserva mediante los semáforos.

## Compilación y ejecución

```bash
make
./factory_manager fichero_configuracion.txt
```

Ejemplo de salida:

```
[OK][factory_manager] Process_manager with id 5 has been created.
[OK][process_manager] Process_manager with id 5 waiting to produce 2 elements.
[OK][process_manager] Belt with id 5 has been created with a maximum of 5 elements.
[OK][queue] Introduced element with id 0 in belt 5.
[OK][queue] Obtained element with id 0 in belt 5.
...
[OK][factory_manager] Finishing.
```

## Manejo de errores

- Fichero inválido, argumentos incorrectos, tamaños o valores negativos → mensaje de error por `stderr` y código de retorno `-1`.
- Si un `process_manager` concreto falla, el `factory_manager` lo detecta y **continúa** con el resto sin detener la ejecución completa.

## Pruebas realizadas

Se han validado tanto casos funcionales (una cinta, múltiples cintas, cinta de tamaño 1, número máximo de procesos) como casos de error (fichero inexistente, valores negativos, más cintas que el máximo declarado, argumentos faltantes), verificando en todos los casos la correcta liberación de memoria y ausencia de condiciones de carrera.

## Tecnologías

`C` · `POSIX Threads (pthread)` · Semáforos · Mutex y variables de condición · Linux

## Autores

Proyecto desarrollado en pareja para la asignatura de Sistemas Operativos, Grado en Ingeniería Informática, Universidad Carlos III de Madrid (2024/2025).
