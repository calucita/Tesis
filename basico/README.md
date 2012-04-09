El script en Perl para la visualización en tiempo real viene originalmente de [Visualize real-time data streams with Gnuplot][orig] por [Thanassis Tsiodras][ttsiod].  La versión del script incluida es (casi igual a) la modificación realizada por [Andreas Bernauer][andreas] en [Plotting data with gnuplot in real-time][mod].

Un ejemplo de corrida para visualizar datos en vivo es así:

    stdbuf -o0 ./basico -x 35 -y 20 -d 0.001 -i -1 --show-coop-count | stdbuf -i0 -o0 sed -n '/^coop count [0-9]\+: /s//0:/p' | ./driveGnuPlotStreams 1 1 100 0 "$((35*20))" 640x480+0+0 Fitness 0

Son tres procesos en un *pipeline*:

*   El primero corre la simulación y genera en la salida el número de cooperadores en cada iteración.

*   El segundo procesa esa salida para convertirla al formato de datos que requiere el script en Perl para la visualización de datos en tiempo real.  Es básicamente *duct tape* para adaptar la salida de un programa a la entrada de otro programa.

*   El tercero es el script en Perl que llama a Gnuplot para hacer la visualización.

Las opciones del primer proceso, después de `./basico` y antes del primer `|`, definen cómo va a ser la simulación.

Las opciones del tercer proceso definen cómo es la visualización.  Lo más importante es que el argumento `"$((35*20))"` debe corresponder a los argumentos del primer proceso que definen el tamaño del *grid*: `-x 35 -y 20`.

Para detener la visualización en vivo hay que hacer `Ctrl-C` en el terminal.  Cerrar la ventana de visualización no hace nada.

[orig]:    <http://users.softlab.ece.ntua.gr/~ttsiod/gnuplotStreaming.html>
[mod]:     <http://www.lysium.de/blog/index.php?/archives/234-Plotting-data-with-gnuplot-in-real-time.html>
[andreas]: <http://www.lysium.de/blog/index.php?/authors/1-Andreas-Bernauer>
[ttsiod]:  <http://users.softlab.ece.ntua.gr/~ttsiod/>
