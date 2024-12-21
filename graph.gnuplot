set title "[nom]"
set xlabel "id"
set ylabel "capacité restante"
set style data histograms
set style fill solid 0.5
set datafile separator ":"
set grid
unset key
set terminal pngcairo size 2000,1500
set output "graphs/graph.png"

first_value = system("sed -n 1p tmp/donnees_graph.csv | cut -d':' -f4")
last_value = system("sed -n '$p' tmp/donnees_graph.csv | cut -d':' -f4")

set cbrange [first_value:last_value]


set palette defined (first_value "red", 0 "red", 0 "green", last_value "green")

plot "tmp/donnees_graph.csv" using ($4):xtic(1) lc palette

