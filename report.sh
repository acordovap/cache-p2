#!/bin/sh

#2.1
echo "++++++++++++++++++++++++++++++++ 2.1 "
max=131072
for f in trazas/cc.trace trazas/spice.trace trazas/tex.trace
do
  echo "-------------------------------- $f --------------------------------"
  for (( i=4; i <= $max; i*=2 ))
  do
      ./sim -is $i -ds $i -bs 4 -a $(($i / 4)) -wb -wa $f
  done
done

#2.2
echo "++++++++++++++++++++++++++++++++ 2.2 "
max=4096
for f in trazas/cc.trace trazas/spice.trace trazas/tex.trace
do
  echo "-------------------------------- $f --------------------------------"
  for (( i=4; i <= $max; i*=2 ))
  do
      ./sim -is 8192 -ds 8192 -bs $i -a 2 -wb -wa $f
  done
done

#2.3
echo "++++++++++++++++++++++++++++++++ 2.3 "
max=64
for f in trazas/cc.trace trazas/spice.trace trazas/tex.trace
do
  echo "-------------------------------- $f --------------------------------"
  for (( i=4; i <= $max; i*=2 ))
  do
      ./sim -is 8192 -ds 8192 -bs 128 -a $i -wb -wa $f
  done
done

#2.4
echo "++++++++++++++++++++++++++++++++ 2.4a "
max=64
for f in trazas/cc.trace trazas/spice.trace trazas/tex.trace
do
  echo "-------------------------------- $f --------------------------------"
  for p in wb wt # policy
  do
    for i in 8192 16384 32768 # cache size
    do
      for j in 64 128 # block size
      do
        for k in 2 4 # associativity
        do
          ./sim -is $i -ds $i -bs $j -a $k -$p -nw $f
        done
      done
    done
  done
done

echo "++++++++++++++++++++++++++++++++ 2.4b "
max=64
for f in trazas/cc.trace trazas/spice.trace trazas/tex.trace
do
  echo "-------------------------------- $f --------------------------------"
  for p in wa nw # policy
  do
    for i in 8192 16384 32768 # cache size
    do
      for j in 64 128 # block size
      do
        for k in 2 4 # associativity
        do
          ./sim -is $i -ds $i -bs $j -a $k -wb -$p $f
        done
      done
    done
  done
done
