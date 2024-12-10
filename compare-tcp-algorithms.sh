ALGORITHMS=(TcpNewReno TcpHighSpeed TcpHybla TcpWestwood TcpWestwoodPlus TcpVegas TcpScalable TcpVeno TcpBic TcpYeah TcpIllinois TcpHtcp)

for item in ${ALGORITHMS[@]}; do
  echo "----- Simulating $item -----"
  ./ns3 run "my-tcp-variants-comparison --transport_prot=$item --prefix_name='data/$item' --tracing=True --duration=20"
done
