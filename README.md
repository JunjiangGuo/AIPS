# AIPS
## TPs AIPS - 3 MIC C   DUAN Yu & GUO Junjiang

commandes:

Partie UDP:
sources: tsock -s -u insa-20234 9000
puits:   tsock -p -u 9000

Partie TCP:
sources: tsock -s insa-20234 9000
puits:   tsock -p 9000

Partie 3:
sources: tsock -s -n 3 -l 100 insa-20234 9000
puits:   tsock -p -n 3 -l 100 9000