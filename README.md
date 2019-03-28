# Arduino
Ce répertoire contient tous mes projets avec des microcontrôleurs Arduino. Le dossier racine *arduino* de ce répertoire fait référence au dossier local *Projects* de l'environnement de développement [PlatformIO IDE](https://platformio.org/platformio-ide), utilisé sous *Visual Studio Code*. Pour téléverser les fichiers contenus dans le répertoire ici-présent vers un microcontrôleur, il faut installer cet environnement de développement, le configurer et télécharger les librairies appropriées. Sinon, il est également possible de simplement copier/coller le contenu des sketchs et de les utiliser via l'environnement de développement officiel [Arduino IDE](https://www.arduino.cc/en/main/software).

## Répartition des fichiers

Dans chaque projet, le dossier *src* contient le sketch *main.cpp*. C'est ce fichier qui est téléversé vers le microcontrôleur :

````
dossier-racine-du-projet/src/main.cpp
````

Dans quelques projets, des sketchs alternatifs sont également contenus dans le dossier *lib* :

````
dossier-racine-du-projet/lib/sketch-secondaire.cpp
````

Pour utiliser ces sketchs, il faut copier-coller le contenu dans le fichier *main.cpp*, puis téléverser ce dernier.
