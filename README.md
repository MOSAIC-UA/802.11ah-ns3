# IEEE 802.11ah simulation module for NS-3

This repository hosts an implementation of the IEEE802.11ah (Wi-Fi HaLow) protocol for the NS-3 network simulator. It comes pre-packaged with NS-3 version 3.23 and an example showing how to use and configure it. For more details about our implementation and supported features, please visit [our webpage](https://www.uantwerpen.be/en/rg/mosaic/projects/ieee-802-11ah/). If you have questions or want to contribute, plese send us an [email](mailto:80211ah@mosaic.uantwerpen.be).

If you use our work, please use the following citation to refer to it:

> Le Tian, Steven Latre, Jeroen Famaey. An IEEE 802.11ah simulation module for NS-3. University of Antwerp technical report, 2016, Retrieved from https://www.uantwerpen.be/en/rg/mosaic/projects/ieee-802-11ah/.


# Installation and usage instructions

1. Download source code from our github repository.  
2. Change into /802.11ah-ns3/ns-3 directory.  
3. Build and run:
```bash
./waf configure  
./waf  
./waf --run "scratch/s1g-mac-test --NRawSta=30 --NGroup=1 --SlotFormat=0 --NRawSlotCount=162 --NRawSlotNum=5 --DataMode="OfdmRate650KbpsBW2MHz" --datarate=0.65 --bandWidth=2 --rho="50" --simulationTime=60 --payloadSize=256 --BeaconInterval=100000 --UdpInterval=0.1 --Nsta=30 --file="./TestMac/mac-sta.txt"  --pcapfile="./TestMac/mac-sta" --seed=1"
```
Note: if error related to nullptr arise when compilies, CXXFLAGS should be included into the "./waf configure" commnand, showns as follows:   
CXXFLAGS="-std=c++0x" ./waf configure --disable-examples --disable-tests

Moreover, there're are some parameter which can be changed by users inside script file "s1g-mac-test.cc", some of which are described below.

## PropagationLoss model:

The current configuration is used in outdoor macro scenarios, users can change these parameters to test other scenarios as well. More information can be found at [our website](https://www.uantwerpen.be/en/rg/mosaic/projects/ieee-802-11ah/).  

```cpp
channel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel","Exponent", DoubleValue(3.76) ,"ReferenceLoss", DoubleValue(8.0), "ReferenceDistance", DoubleValue(1.0));
```

## WifiMacQueue length

Users can change vaule of "MaxPacketNumber" and "MaxDelaysend" attributes according to their own requirements.

```cpp
Config::Set ("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_EdcaTxopN/Queue/MaxPacketNumber", UintegerValue(60000));
Config::Set ("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_EdcaTxopN/Queue/MaxDelay", TimeValue (NanoSeconds (6000000000000)));
```

# Example scenario

An example is included in the script file "./ns-3/scratch/s1g-mac-test.cc". Users can use the file "s1g-mac-test.cc" to simulate the RAW feature of 802.11ah with different Wi-Fi modes. A topology in which stations are distributed randomly within a "rho" meters distance of the access point (AP) is created. Subsequently, the stations are associated with the AP using the new fast association mechanism. Then, all stations generate packets and send them to the AP in their assigned RAW slots. Finally, after "SimulationTime" seconds, all stations stop generating new packets and finish after sending all packets still in their transmit buffer.

## RAW related parameters:     

* NRawSta:            Number of stations supporting RAW.                 
* NGroup:             Number of RAW groups. 
* NRawSlotNum:        Number of slots per RAW group.                     
* SlotFormat:         Format of RAW slot count.                 
* NRawSlotCount:      Used to calculate number of RAW slot duration.  

Notes:                                              
* NGroup, stations are divided evenly among each RAW groups
* RAW slot duration = 500 us + NRawSlotCount * 120 us, NRawSlotCount is y = 11(8) bits length when SlotFormat is set to
1(0), and NRawSlotNum is (14 - y) bits length.
  
## Wi-Fi mode parameters:  
* DataMode:           Data mode.  
* datarate:           Data rate of data mode.  
* bandWidth:          BandWidth of data mode.  

Note: Relation between the above 3 parameters and MCS is described in file "MCStoWifiMode".       
    
## Other parameters:  
* SimulationTime:     Simulatiom time in seconds after all stations get associated with AP.  
* payloadSize:        Size of payload.                   
* BeaconInterval:     Beacon interval time in us.    
* UdpInterval:        Traffic mode, station send one packet every UdpInterval seconds.  
* Nsta:               Number of total stations.  
* rho:                Maximal distance between AP and stations.   
* file:               Path of files containing result information.        
* pcapfile:           Pcap file path.   
* seed:               Seed of RandomVariableStream. 
