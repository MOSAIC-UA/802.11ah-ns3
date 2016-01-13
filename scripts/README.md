This file explains the parameters of script file "s1g-mac-test.cc" and 
how to use it to do simulation of 802.11ah.

1) Brief introduction of file "s1g-mac-test.cc".

Users can use file "s1g-mac-test.cc" to simulate RAW feature of 802.11ah with different wifi mode. A topology in which stations are distributed randomly with distance to accsee point (AP) less than "rho" meters is bulit, after get associated with AP using fast association mechanism, with random staring time, all stations generate packets and try to send to AP as defined by RAW related parameters,  after "SimulationTime" seconds, all stations stop generating new packets but still continue sending packets to AP until the send queue of all stations are empty. 

2) Related Paramters

The parameters include: 
  RAW related parameters:                                                    
    --NRawSta:            Number of stations supporting RAW.                 
    --NGroup:             Number of RAW groups. 
    --NRawSlotNum:        Number of slots per RAW group.                     
    --SlotFormat:         Format of RAW slot count.                 
    --NRawSlotCount:      Used to calculate number of RAW slot duration.  
    Note:                                               
      1. NGroup, stations are dividen evenly among each RAW groups          
      2. RAW slot duration = 500 us + NRawSlotCount * 120 us, NRawSlotCount is y = 11(8) bits length when SlotFormat is set to
1(0), and NRawSlotNum is (14 - y) bits length.
  
  Wifi mode parameters:  
  --DataMode:           Data mode.  
  --datarate:           Data rate of data mode.  
  --bandWidth:          BandWidth of data mode.  
    Note: Relation between above 3 parameters and MCS is described in file "MCStoWifiMode".       
    
  Other parameters:  
  --SimulationTime:     Simulatiom time in seconds after all stations get associated with AP.  
  --payloadSize:        Size of payload.                   
  --BeaconInterval:     Beacon interval time in us.    
  --UdpInterval:        Traffic mode, station send one packet every UdpInterval seconds.  
  --Nsta:               Number of total stations.  
  --rho:                Maximal distance between AP and stations.   
  --file:               Path of files containing reslut information.        
  --pcapfile:           Pcap file path.     

  
3) Usage of script file

Example of using script file is as follows:
./waf --run "scratch/s1g-mac-test --NRawSta=60 --NGroup=1 --SlotFormat=1 --NRawSlotCount=412 --NRawSlotNum=2 --DataMode="OfdmRate650KbpsBW2MHz" --datarate=0.65 --bandWidth=2 --rho="100" --simulationTime=60 --payloadSize=256 --BeaconInterval=100000 --UdpInterval=0.1 --Nsta=60 --rho="50" --file="./TestMac/mac-sta.txt"  --pcapfile="./TestMac/mac-sta" "

Moreover, there're are some parameter which can be changed by users inside script file "s1g-mac-test.cc", some of which are described as follows:
  1.  propagarionsLoss model:
      channel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel","Exponent", DoubleValue(3.76) ,"ReferenceLoss", DoubleValue(8.0), "ReferenceDistance", DoubleValue(1.0));
      The current configuration are used in outdoor macro sceniros, users can change these parameters to test other sceniros as well, much information can be found at https://www.uantwerpen.be/en/rg/mosaic/projects/ieee-802-11ah/.

  2.  WifiMacQueue length:
      Config::Set ("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_EdcaTxopN/Queue/MaxPacketNumber", UintegerValue(60000));
    Config::Set ("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_EdcaTxopN/Queue/MaxDelay", TimeValue (NanoSeconds (6000000000000)));
     Users can change vaule of "MaxPacketNumber" and "MaxDelaysend" attributes according to their own requirement.
  


