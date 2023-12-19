Code compiled with VisualStudio 2022 (Free version)
Code compiled in Release with optimization set for speed

During presentation, the following commands were executed:

simulWriters --mask 0x02 --off 1
simulWriters --mask 0x02 --off 1 --mask 0x08 --off 2
simulWriters --mask 0x02 --off 1 --mask 0x08 --off 2 --mask 0x20 --off 3
simulWriters --mask 0x02 --off 1 --mask 0x08 --off 2 --mask 0x20 --off 3 --mask 0x80 --off 4
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32 --mask 0x04 --off 17
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32 --mask 0x04 --off 17 --mask 0x10 --off 25
simulWriters --mask 0x02 --off 8 --mask 0x08 --off 16 --mask 0x20 --off 24 --mask 0x80 --off 32 --mask 0x04 --off 17 --mask 0x10 --off 25 --mask 0x40 --off 33

matrixWalk --byCol
matrixWalk
matrixWalk --byCol --wr
matrixWalk --wr

