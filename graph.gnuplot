set title "Histogramme de la charge displonible pour les 20 lv les plus énergivores et moins energivores"
set xlabel "id"
set ylabel "capacité restante"
set style data histograms
set style fill solid 0.5
set datafile separator ":"
set grid
unset key
set terminal pngcairo size 2000,1500
set output "graphs/graph_actuel.png"
first_value = system("sed -n 1p tmp/donnees_graph.csv | cut -d':' -f4")
last_value = system("sed -n '$p' tmp/donnees_graph.csv | cut -d':' -f4")
set cbrange [-203929:986708]
set palette defined (-203929 "red", 0 "red", 0 "green", 986708 "green")
plot "tmp/donnees_graph.csv" using ($4):xtic(1) palette