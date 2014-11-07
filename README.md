egg-player
==========

Un reproductor de video para sus aplicaciones GTK+

EggPlayer es un objeto que reproduce videos. Implementa el mecanismo de objetos de GObject y está escrito en C (ANSI). Se enlaza con GTK+ y GStreamer para generar una biblioteca dinámicamente enlazable .dll u objeto compartido .so. Esta biblioteca libegg.(dll|so) se puede cargar en el diseñador de interfaces gráficas de usuario Glade. Es un intento de comprender la técnica de incorporación de nuevos widgets a la paleta de Glade. Aporta un nuevo ejemplo al objeto EggClockFace de Davyd Madeley que incluye una propiedad que se puede manejar desde Glade.

![pantallazo](https://dl.dropboxusercontent.com/u/36581494/captura-8.png "Pantallazo")

Antes de ejecutar Glade, recuerde exportar al shell las siguientes variables de entorno:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
export GLADE_CATALOG_PATH=$GLADE_CATALOG_PATH:.
export GLADE_MODULE_PATH=$GLADE_MODULE_PATH:.
```

Contenido del paquete:

- **egg.h** Encabezado general de la biblioteca.
- **egg-player.c** El widget reproductor.
- **egg-player.h** El encabezado del widget reproductor.
- **egg-player-marshallers.list** Lista de argumentos y valores de retorno para las retrollamadas.
- **egg.xml** El archivo de catálogo para Glade.
- **Makefile** El archivo de construcción.
- **player.c** Un ejemplo que usa el widget reproductor sin Glade.
- **README** Este archivo.
- **test.c** Un ejemplo que usa el widget reproductor con Glade.
- **test.ui** El archivo de interfaz Glade.
