================================================================================
IL PICCOLO PRINCIPE - PROGETTO MIDTERM DI COMPUTER GRAPHICS
Samuele Rocca | A.A. 2026/2027
================================================================================

Platform cooperativo 2D per due giocatori sulla stessa tastiera, sviluppato
in C++17 con OpenGL 3.3 Core e shader GLSL. Il Piccolo Principe e la Volpe
collaborano per raccogliere le stelle e superare le tre stanze della Storia.


--------------------------------------------------------------------------------
1. REQUISITI E DIPENDENZE
--------------------------------------------------------------------------------
- Windows con Visual Studio 2022 e il carico di lavoro "Sviluppo di
  applicazioni desktop con C++", il toolset MSVC v143 e il Windows SDK 10.
- Scheda grafica e driver compatibili con OpenGL 3.3 Core Profile.
- GLFW per la finestra, il contesto OpenGL e l'input da tastiera.
- GLAD per il caricamento delle funzioni OpenGL.
- GLM 1.0.3 per il calcolo con vettori e matrici.
- Dear ImGui per il menu, i dialoghi e gli indicatori di gioco.


--------------------------------------------------------------------------------
2. COMPILAZIONE E AVVIO (VISUAL STUDIO)
--------------------------------------------------------------------------------
1. Se il progetto è in un archivio ZIP, estrailo in una cartella locale
   mantenendo la struttura delle cartelle.
2. Apri IlPiccoloPrincipe.sln in Visual Studio 2022. Se GLM non è presente,
   seleziona "Ripristina pacchetti NuGet" sulla soluzione.
3. Seleziona Release o Debug e la piattaforma x64. Compila la soluzione
   con Ctrl+Shift+B.
4. Premi Ctrl+F5 per avviare il gioco. Dal menu scegli "Tutorial" o
   "Avvia storia".

La cartella di lavoro contiene entrambe le cartelle shaders/ e assets/.


--------------------------------------------------------------------------------
3. CONTROLLI
--------------------------------------------------------------------------------
Piccolo Principe    A / D per muoversi, W per saltare, S per abbassarsi.
Volpe               Frecce sinistra / destra per muoversi, su per saltare,
                    giù per abbassarsi.
Menu                Clic su una voce oppure frecce su / giù e Invio.
Dialoghi            Clic sul pulsante di avanzamento oppure Invio.
R                   Ricomincia la stanza corrente.
Esc                 Nel Tutorial torna al menu. Nelle altre schermate
                    chiude il gioco.


--------------------------------------------------------------------------------
4. REGOLE DI GIOCO
--------------------------------------------------------------------------------
- Storia: in ogni stanza raccogli tutte le stelle e porta entrambi i
  personaggi alla porta prima che scada il tempo. Sullo schermo puoi
  controllare quante stelle mancano e il tempo residuo.
- Ostacoli: spingi le casse camminandoci contro. Puoi tenere premuta una
  pedana con un personaggio o una cassa per azionare un muro mobile.
- Ripartenza: la stanza ricomincia se scade il tempo o se uno dei due
  personaggi viene colpito da un nemico o da un muro mobile.
- Tutorial: puoi provare i controlli e i meccanismi senza limite di tempo.
