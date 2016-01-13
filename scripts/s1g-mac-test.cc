/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 MIRKO BANCHI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <sys/stat.h>

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("s1g-wifi-network-le");

uint32_t AssocNum = 0;
int64_t AssocTime=0;
uint32_t StaNum=0;
string TrafficInterval="10000";
uint32_t payloadLength;

class assoc_record
{
public:
    assoc_record ();
    bool GetAssoc ();
    void SetAssoc (std::string context, Mac48Address address);
    void UnsetAssoc (std::string context, Mac48Address address);
    void setstaid (uint16_t id);
 private:
    bool assoc;
    uint16_t staid;
};

assoc_record::assoc_record ()
{
    assoc = false;
    staid = 65535;
}

void
assoc_record::setstaid (uint16_t id)
{
    staid = id;
}

void
assoc_record::SetAssoc (std::string context, Mac48Address address)
{
   assoc = true;
}

void
assoc_record::UnsetAssoc (std::string context, Mac48Address address)
{
  assoc = false;
}

bool
assoc_record::GetAssoc ()
{
    return assoc;
}

typedef std::vector <assoc_record * > assoc_recordVector;
assoc_recordVector assoc_vector;


uint32_t
GetAssocNum ( )
{
  AssocNum = 0;
  for (assoc_recordVector::const_iterator index = assoc_vector.begin (); index != assoc_vector.end (); index++)
    {
      if ((*index)->GetAssoc ())
        {
            AssocNum++;
        }
    }
  return AssocNum;
}


void
PhyRxErrorTrace (std::string context, Ptr<const Packet> packet, double snr)
{
    std::cout << "PHYRXERROR snr=" << snr << " " << *packet << std::endl;
}

void
PhyRxOkTrace (std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, enum WifiPreamble preamble)
{
    std::cout << "PHYRXOK mode=" << mode << " snr=" << snr << " " << *packet << std::endl;
}


void
UdpTraffic ( string Interval, uint16_t num, uint32_t Size)
{
  TrafficInterval = Interval;
  StaNum = num;
  payloadLength = Size;
}

ApplicationContainer serverApp;
typedef std::vector <Ptr<WifiMacQueue> > QueueVector;
QueueVector m_queue;
QueueVector BEqueue;

uint32_t NPacketInQueue = 0;
uint32_t AppStartTime = 0;
uint32_t ApStopTime = 0;

void ReadQueue(QueueVector queuelist)
{
    BEqueue = queuelist;
}

uint16_t NEmptyQueue=0;
uint16_t kk=0;

void GetQueuelen (uint32_t Nsta)
{
   NEmptyQueue=0;
   kk=0;
   for (QueueVector::const_iterator i = BEqueue.begin (); i != BEqueue.end (); i++)
     {
      kk +=1;
      if ((*i)->GetSize () != 0)
        {
         std::cout << kk <<"th station, " << (*i)->GetSize () << " packets remains, " << Simulator::Now ().GetSeconds () << std::endl;
        }
   
      if ((*i)->GetSize () == 0)
         {
           NEmptyQueue +=1 ;
         }
      }
    
    if (NEmptyQueue == Nsta)
       {
         Simulator::Stop (Seconds (0));
         ApStopTime = Simulator::Now ().GetSeconds ();
         std::cout << "no packets in the queue, " << Simulator::Now ().GetSeconds () << std::endl;
         std::cout << "Simulator start at " << AppStartTime << " s, end at " << ApStopTime << " s"  << std::endl;
        }
    else
       {
         Simulator::Schedule(Seconds (1.0), &GetQueuelen, Nsta);
         std::cout << "there are still packets in the queue" << std::endl;
       }
 }


void CheckAssoc (uint32_t Nsta, double simulationTime, NodeContainer wifiApNode, NodeContainer  wifiStaNode, Ipv4InterfaceContainer apNodeInterface)
{
    if  (GetAssocNum () == Nsta)
      {
        std::cout << "Assoc Finished, AssocNum=" << AssocNum << ", time = " << Simulator::Now ().GetMicroSeconds () << std::endl;
        //Application start time
        double randomInterval = std::stod (TrafficInterval,nullptr);
        Ptr<UniformRandomVariable> m_rv = CreateObject<UniformRandomVariable> ();
        //UDP flow
        UdpServerHelper myServer (9);
        serverApp = myServer.Install (wifiApNode);
        serverApp.Start (Seconds (0));
        //serverApp.Stop (Seconds (simulationTime+1)); // serverApp stops when simulator stop
        //Set traffic start time for each station
        
        UdpClientHelper myClient (apNodeInterface.GetAddress (0), 9); //address of remote node
        myClient.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
        myClient.SetAttribute ("Interval", TimeValue (Time (TrafficInterval))); //packets/s
        myClient.SetAttribute ("PacketSize", UintegerValue (256));
        for (uint16_t i = 0; i < Nsta; i++)
          {
            double randomStart = m_rv->GetValue (0, randomInterval);
            std::cout << "Generate traffic in " << randomStart << "seconds, station " << double(i) << std::endl;
            
            ApplicationContainer clientApp = myClient.Install (wifiStaNode.Get(i));
            clientApp.Start (Seconds (1 + randomStart));
            clientApp.Stop (Seconds (simulationTime+1));
          }
        AppStartTime=Simulator::Now ().GetSeconds () + 1; //clientApp start time
        Simulator::Schedule(Seconds (simulationTime+1), &GetQueuelen, Nsta); //check before simulation stop
        //Simulator::Stop (Seconds (simulationTime+1)); //set stop time until no packet in queue
      }
    else
      {
        Simulator::Schedule(Seconds(1.0), &CheckAssoc, Nsta, simulationTime, wifiApNode, wifiStaNode, apNodeInterface);
      }
    
}


void
PopulateArpCache ()
{
    Ptr<ArpCache> arp = CreateObject<ArpCache> ();
    arp->SetAliveTimeout (Seconds(3600 * 24 * 365));
    for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
        NS_ASSERT(ip !=0);
        ObjectVectorValue interfaces;
        ip->GetAttribute("InterfaceList", interfaces);
        for(ObjectVectorValue::Iterator j = interfaces.Begin(); j !=
            interfaces.End (); j ++)
        {
            Ptr<Ipv4Interface> ipIface = (j->second)->GetObject<Ipv4Interface> ();
            NS_ASSERT(ipIface != 0);
            Ptr<NetDevice> device = ipIface->GetDevice();
            NS_ASSERT(device != 0);
            Mac48Address addr = Mac48Address::ConvertFrom(device->GetAddress ());
            for(uint32_t k = 0; k < ipIface->GetNAddresses (); k ++)
            {
                Ipv4Address ipAddr = ipIface->GetAddress (k).GetLocal();
                if(ipAddr == Ipv4Address::GetLoopback())
                    continue;
                ArpCache::Entry * entry = arp->Add(ipAddr);
                entry->MarkWaitReply(0);
                entry->MarkAlive(addr);
                std::cout << "Arp Cache: Adding the pair (" << addr << "," << ipAddr << ")" << std::endl;
            }
        }
    }
    for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
        NS_ASSERT(ip !=0);
        ObjectVectorValue interfaces;
        ip->GetAttribute("InterfaceList", interfaces);
        for(ObjectVectorValue::Iterator j = interfaces.Begin(); j !=interfaces.End (); j ++)
        {
            Ptr<Ipv4Interface> ipIface = (j->second)->GetObject<Ipv4Interface> ();
            ipIface->SetAttribute("ArpCache", PointerValue(arp));
        }
    }
}


int main (int argc, char *argv[])
{
  double simulationTime = 10;
  uint32_t seed = 1;
  uint32_t  payloadSize = 100;
  uint32_t Nsta =1;
  uint32_t NRawSta = 1;
  uint32_t SlotFormat=1;
  uint32_t  NRawSlotCount = 829;
  uint32_t  NRawSlotNum = 1;
  uint32_t NGroup = 1;
  uint32_t BeaconInterval = 102400;
  bool OutputPosition = true;
  string DataMode = "OfdmRate2_4MbpsBW1MHz";
  double datarate = 2.4;
  double bandWidth = 1;
  string UdpInterval="0.2";
  string rho="250.0";
  string folder="./scratch/";
  string file="./scratch/mac-sta.txt";
  string pcapfile="./scratch/mac-s1g-slots";

  CommandLine cmd;
  cmd.AddValue ("seed", "random seed", seed);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime); 
  cmd.AddValue ("payloadSize", "Size of payload", payloadSize);
  cmd.AddValue ("Nsta", "number of total stations", Nsta);
  cmd.AddValue ("NRawSta", "number of stations supporting RAW", NRawSta);
  cmd.AddValue ("SlotFormat", "format of NRawSlotCount", SlotFormat);
  cmd.AddValue ("NRawSlotCount", "number of stations supporting RAW", NRawSlotCount);
  cmd.AddValue ("NRawSlotNum", "number of stations supporting RAW", NRawSlotNum);
  cmd.AddValue ("NGroup", "number of RAW group", NGroup);
  cmd.AddValue ("BeaconInterval", "Beacon interval time in us", BeaconInterval);
  cmd.AddValue ("DataMode", "Date mode", DataMode);
  cmd.AddValue ("datarate", "data rate in Mbps", datarate);
  cmd.AddValue ("bandWidth", "bandwidth in MHz", bandWidth);
  cmd.AddValue ("UdpInterval", "traffic mode", UdpInterval);
  cmd.AddValue ("rho", "maximal distance between AP and stations", rho);
  cmd.AddValue ("folder", "folder where result files are placed", folder);
  cmd.AddValue ("file", "files containing reslut information", file);
  cmd.AddValue ("pcapfile", "files containing reslut information", pcapfile);
  cmd.Parse (argc,argv);
  
  RngSeedManager::SetSeed (seed);

  NodeContainer wifiStaNode;
  wifiStaNode.Create (Nsta);
  NodeContainer wifiApNode;
  wifiApNode.Create (1);

  YansWifiChannelHelper channel = YansWifiChannelHelper ();
  channel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel","Exponent", DoubleValue(3.76) ,"ReferenceLoss", DoubleValue(8.0), "ReferenceDistance", DoubleValue(1.0));
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
        
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetErrorRateModel ("ns3::YansErrorRateModel");
  phy.SetChannel (channel.Create ());
  phy.Set ("ShortGuardEnabled", BooleanValue (false));
  phy.Set ("ChannelWidth", UintegerValue (bandWidth));
  phy.Set ("EnergyDetectionThreshold", DoubleValue (-116.0));
  phy.Set ("CcaMode1Threshold", DoubleValue (-119.0));
  phy.Set ("TxGain", DoubleValue (0.0));
  phy.Set ("RxGain", DoubleValue (0.0));
  phy.Set ("TxPowerLevels", UintegerValue (1));
  phy.Set ("TxPowerEnd", DoubleValue (0.0));
  phy.Set ("TxPowerStart", DoubleValue (0.0));
  phy.Set ("RxNoiseFigure", DoubleValue (3.0));
  phy.Set ("LdpcEnabled", BooleanValue (true));

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211ah);
  S1gWifiMacHelper mac = S1gWifiMacHelper::Default ();

  Ssid ssid = Ssid ("ns380211ah");
  StringValue DataRate;
  DataRate = StringValue (DataMode);
  
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", DataRate,
                                    "ControlMode", DataRate);
  mac.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid),
                "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevice;
  staDevice = wifi.Install (phy, mac, wifiStaNode);
  
  uint32_t NGroupStas = NRawSta/NGroup;
  mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid),
                 "BeaconInterval", TimeValue (MicroSeconds(BeaconInterval)),
                 "NRawGroupStas", UintegerValue (NGroupStas),
                 "NRawStations", UintegerValue (NRawSta),
                 "SlotFormat", UintegerValue (SlotFormat),
                 "SlotCrossBoundary", UintegerValue (1),
                 "SlotDurationCount", UintegerValue (NRawSlotCount),
                 "SlotNum", UintegerValue (NRawSlotNum));
 
  NetDeviceContainer apDevice;
  phy.Set ("TxGain", DoubleValue (3.0));
  phy.Set ("RxGain", DoubleValue (3.0));
  phy.Set ("TxPowerLevels", UintegerValue (1));
  phy.Set ("TxPowerEnd", DoubleValue (30.0));
  phy.Set ("TxPowerStart", DoubleValue (30.0));
  phy.Set ("RxNoiseFigure", DoubleValue (5));
  apDevice = wifi.Install (phy, mac, wifiApNode);

  Config::Set ("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_EdcaTxopN/Queue/MaxPacketNumber", UintegerValue(60000));
  Config::Set ("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/BE_EdcaTxopN/Queue/MaxDelay", TimeValue (NanoSeconds (6000000000000)));

  // mobility.
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
                                       "X", StringValue ("1000.0"),
                                       "Y", StringValue ("1000.0"),
                                       "rho", StringValue (rho));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(wifiStaNode);

  MobilityHelper mobilityAp;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (1000.0, 1000.0, 0.0));
  mobilityAp.SetPositionAllocator (positionAlloc);
  mobilityAp.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobilityAp.Install(wifiApNode);

 
   /* Internet stack*/
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNode);

  Ipv4AddressHelper address;

  address.SetBase ("192.168.0.0", "255.255.0.0");
  Ipv4InterfaceContainer staNodeInterface;
  Ipv4InterfaceContainer apNodeInterface;

  staNodeInterface = address.Assign (staDevice);
  apNodeInterface = address.Assign (apDevice);

  //trace association
  for (uint16_t kk=0; kk< Nsta; kk++)
    {
      std::ostringstream STA;
      STA << kk;
      std::string strSTA = STA.str();
    
      assoc_record *m_assocrecord=new assoc_record;
      m_assocrecord->setstaid (kk);
      Config::Connect ("/NodeList/"+strSTA+"/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/$ns3::StaWifiMac/Assoc", MakeCallback (&assoc_record::SetAssoc, m_assocrecord));
      Config::Connect ("/NodeList/"+strSTA+"/DeviceList/0/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/$ns3::StaWifiMac/DeAssoc", MakeCallback (&assoc_record::UnsetAssoc, m_assocrecord));
      assoc_vector.push_back (m_assocrecord);
    }
  
  // check BE_EdcaTxopN queue length of stations
  for (uint16_t i = 0; i < Nsta; i++)
    {
        Ptr<NetDevice> netDe;
        Ptr<WifiNetDevice> wifiDe;
        netDe = staDevice.Get(i);
        wifiDe = netDe->GetObject<WifiNetDevice>();
        PointerValue ptr1;
        wifiDe->GetMac()->GetAttribute("BE_EdcaTxopN", ptr1);
        Ptr<EdcaTxopN> BE_edca = ptr1.Get<EdcaTxopN>();
        PointerValue ptr2;
        BE_edca->GetAttribute("Queue", ptr2);
        Ptr<WifiMacQueue> BE_edca_queue = ptr2.Get<WifiMacQueue> ();
        m_queue.push_back (BE_edca_queue);
     }
    
    Simulator::ScheduleNow(&UdpTraffic, UdpInterval, Nsta,payloadSize);
    Simulator::Schedule(Seconds(1), &ReadQueue, m_queue);
    Simulator::Schedule(Seconds(1), &CheckAssoc, Nsta, simulationTime, wifiApNode, wifiStaNode,apNodeInterface);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    PopulateArpCache ();
    
      if  (OutputPosition)
        {
          int i =0;
          while (i < Nsta)
            {
               Ptr<MobilityModel> mobility = wifiStaNode.Get (i)->GetObject<MobilityModel>();
               Vector position = mobility->GetPosition();
               std::cout << "Sta node#" << i << ", " << "position = " << position << std::endl;
               i++;
            }
          Ptr<MobilityModel> mobility1 = wifiApNode.Get (0)->GetObject<MobilityModel>();
          Vector position = mobility1->GetPosition();
          std::cout << "AP node, position = " << position << std::endl;
        }
    
      phy.EnablePcap ("pcapfile", Nsta, 0);
      Simulator::Run ();
      Simulator::Destroy ();

      double throughput = 0;
      //UDP
      uint32_t totalPacketsThrough = DynamicCast<UdpServer> (serverApp.Get (0))->GetReceived ();
      throughput = totalPacketsThrough * payloadSize * 8 / ((ApStopTime - AppStartTime) * 1000000.0);
    

      std::cout << "DataRate" << "\t" << "NGroup" << "\t" << "NRawSlotNum" << "\t" << "Throughput" << '\n';
      std::cout << datarate << "\t" << NGroup << "\t" << NRawSlotNum << "\t" << throughput << " Mbit/s" << std::endl;
      ofstream myfile;
      myfile.open (file, ios::out | ios::app);
      myfile << NGroup << "\t" << NRawSlotNum << "\t" << throughput <<"\n";
      myfile.close();
    
return 0;
}












