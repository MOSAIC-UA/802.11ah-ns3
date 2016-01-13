This file explains the parameters of script file "s1g-mac-test.cc" and how to use it to do simulation of 802.11ah.

1) Breif introduction of file "s1g-mac-test.cc".

Users can use file "s1g-mac-test.cc" to simulate RAW feature of 802.11ah with different wifi mode. A topology in which stations are distributed randomly with distance to accsee point (AP) less than "rho" meters is bulit, after get associated with AP using fast association mechanism, all stations send udp packets to AP in their RAW slots as defined by RAW related parameters which will be explained in following section. 

2) Related Paramters

The parameters include:
  RAW related parameters:
  --NRawSta:            Number of stations supporting RAW.
  --SlotFormat:         Format of RAW slot count, details is on file ...
  --NRawSlotCount:      Used to calculate number of RAW slot duration, details of calculating RAW slot duration is on file ....
  --NRawSlotNum:        Number of slots per RAW group.
  --NGruop:             Number of RAW groups.
  Wifi mode parameters:
  --DataMode:           Data mode. 
  --datarate:           Data rate of data mode.
  --bandWidth:          BandWidth of data mode.
  Topology paramters:
  --rho:                Maximal distance between AP and stations.
  --APpos:              AP node location.
  --APpositon:          Ap node location.
  Other parameters:
  --SimulationTime:     Simulatiom time in seconds after all stations get associated with AP.
  --Nsts:               Number of total stations.
  --BeaconInterval:     Beacon interval time in us.
  --UdpInterval:        Traffic mode, station send one packet every UdpInterval seconds. 
  

  


