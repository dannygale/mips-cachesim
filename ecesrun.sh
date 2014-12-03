#! /bin/bash
clear
echo "==================================================================="
echo ""
echo "                       MIPS CACHE SIMULATOR                        "
echo "                            run script"
echo ""
echo "                     Danny Gale, Crhis Messick"
echo "                         ECEN4593, F2010"
echo ""
echo "==================================================================="

##tracedir=traces
tracedir=/scratch/arp/ecen4593/traces-long
outputdir=output

##for tracefile in I1 I2 I3 I5 I10
for tracefile in gap.gz gcc.gz gzip.gz mcf.gz twolf.gz vpr.gz
do
   for i in {1..7}
   do
      outfile=$tracefile.$i.out
      configfile=configurations/$i.config
      echo ""
      echo "---------------------------------------------------------------------"
      echo "Running with configuration $configfile on trace $tracefile"
      echo "Output will be placed in $outputdir/$outfile"
      zcat $tracedir/$tracefile | ./cachesim -o $outputdir/$outfile -c $configfile
   done
done
