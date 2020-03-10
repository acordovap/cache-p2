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
echo "++++++++++++++++++++++++++++++++ 2.4 "
