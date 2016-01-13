#!/bin/bash

if [ $# -ne 8 ]
then
echo "parameters missing"
exit 1
fi

trial=$1
seed=$2
NumSta=$3
UdpInterval=$4
NGroup=$5

SlotFormat=$6
NRawSlotCount=$7
NRawSlotNum=$8

simTime=60
BeaconInterval=100000
DataMode="OfdmRate650KbpsBW2MHz"
datarate=0.65
bandWidth=2
rho="50"






echo $seed

if [ ! -d TestMacresult-s1g/ ]
then
mkdir TestMacresult-s1g/
fi

if [ ! -d TestMacresult-s1g/mode0/ ]
then
mkdir TestMacresult-s1g/mode0/
fi

if [ ! -d TestMacresult-s1g/mode0/test$trial/ ]
then
mkdir TestMacresult-s1g/mode0/test$trial/
fi

if [ ! -d TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/ ]
then
mkdir TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/
fi


totalSta=$NumSta
totalRawstas=$NumSta

if [ ! -d TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/pcap-sta-$NumSta/ ]
then
mkdir TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/pcap-sta-$NumSta/
fi

touch TestMacresult-s1g/mode0/test$trial/time-record.txt

echo "&&-$NGroup $Numsta"
if [ $NGroup -gt $NumSta ]; then
   echo "$NGroup $Numsta"
   break
fi

if [ ! -d TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/pcap-sta-$NumSta/$NGroup-groups-$NRawSlotNum-slots/ ]
then
mkdir TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/pcap-sta-$NumSta/$NGroup-groups-$NRawSlotNum-slots/
fi

pcapf="./TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/pcap-sta-$NumSta/$NGroup-groups-$NRawSlotNum-slots/mac-s1g-$NGroup-groups-$NRawSlotNum-slots"

totalSta=$NumSta
totalRawstas=$NumSta
folder="./TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/"
touch ./TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/mac-sta-$NumSta.txt
file="./TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/mac-sta-$NumSta.txt"

echo "Time: $(date) $NGroup $UdpInterval $NumSta" >> TestMacresult-s1g/mode0/test$trial/time-record.txt

./waf --run "s1g-mac-test --seed=$seed --simulationTime=$simTime --payloadSize=256 --Nsta=$totalSta --NRawSta=$totalRawstas --NGroup=$NGroup --SlotFormat=$SlotFormat --NRawSlotCount=$NRawSlotCount --NRawSlotNum=$NRawSlotNum --BeaconInterval=$BeaconInterval --DataMode=$DataMode --datarate=$datarate  --bandWidth=$bandWidth  --UdpInterval=$UdpInterval --rho=$rho --folder=$folder --file=$file --pcapfile=$pcapf" > ./TestMacresult-s1g/mode0/test$trial/traffic-$UdpInterval/pcap-sta-$NumSta/$NGroup-groups-$NRawSlotNum-slots/record-$NumSta-$NGroup.txt 2>&1





