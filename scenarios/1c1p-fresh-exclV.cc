#include <iostream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#define EXCLUSION_RATE_STEP 0.1
#define DURATION 3600
#define ITERATIONS 100

using namespace ns3;

int cacheMissCount = 0;
int cacheHitCount = 0;

void
CacheMiss (std::string context, Ptr<ns3::ndn::Interest const> interest)
{
  cacheMissCount++;
}

void
CacheHit (std::string context, Ptr<ns3::ndn::Interest const> interest, Ptr<ns3::ndn::ContentObject const> content)
{
  cacheHitCount++;
}

int 
main (int argc, char *argv[])
{
  // setting default parameters for PointToPoint links and channels
  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("1Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"));
  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("20"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse (argc, argv);

  // Creating nodes
  NodeContainer nodes;
  nodes.Create (3);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  p2p.Install (nodes.Get (0), nodes.Get (1));
  p2p.Install (nodes.Get (1), nodes.Get (2));

  // Install CCNx stack on all nodes
  ndn::StackHelper ccnxHelper;
  ccnxHelper.SetDefaultRoutes (true);
  ccnxHelper.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0", "ExclusionDiscardedTimeout", "20");
  ccnxHelper.InstallAll ();
  Config::Connect ("/NodeList/0/$ns3::ndn::cs::Freshness::Lru/CacheMisses", MakeCallback (CacheMiss));
  Config::Connect ("/NodeList/0/$ns3::ndn::cs::Freshness::Lru/CacheHits", MakeCallback (CacheHit));

  // Install Applications

  ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
  // Consumer will request /prefix/0, /prefix/1, ... up to /prefix/<MaxSeq>
  consumerHelper.SetPrefix ("/prefix");
  consumerHelper.SetAttribute ("Frequency", StringValue ("1"));
  consumerHelper.SetAttribute ("MaxSeq", StringValue ("0"));
  consumerHelper.SetAttribute ("Repeat", BooleanValue (true));
  consumerHelper.Install (nodes.Get (0)); // first node

  // Producer
  ndn::AppHelper producerHelper ("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  producerHelper.SetPrefix ("/prefix");
  producerHelper.SetAttribute ("PayloadSize", StringValue("1024"));
  producerHelper.SetAttribute ("Freshness", TimeValue (Seconds (20)));
  producerHelper.Install (nodes.Get (2)); // last node


  std::cout << "----------------------------------------------------" << std::endl;
  std::cout << "Exclusion Rate        Cache Misses        Cache Hits" << std::endl;
  std::cout << "----------------------------------------------------" << std::endl;
  for (int i = 0; i < (1 / EXCLUSION_RATE_STEP) + 1; i++)
    {
      float exclusion_rate = 0 + (EXCLUSION_RATE_STEP * i);
      nodes.Get (0)->GetApplication (0)->SetAttribute ("ExclusionRate", DoubleValue (exclusion_rate));

      // Reset counters
      cacheMissCount = 0;
      cacheHitCount = 0;

      for (int i = 0; i < ITERATIONS; i++)
	{
	  // Run simulation
	  Simulator::Stop (Seconds (DURATION));
	  Simulator::Run ();
	}

      // Print out results
      std::cout << std::fixed << std::setprecision(2);
      std::cout << std::setw(14) << exclusion_rate;
      std::cout << std::fixed << std::setprecision(4);
      std::cout << std::setw(19) << ((float)cacheMissCount / (DURATION * ITERATIONS)) * 100 << "%";
      std::cout << std::setw(17) << ((float)cacheHitCount / (DURATION * ITERATIONS)) * 100 << "%";
      std::cout << std::endl;

      sleep(5);
    }
  std::cout << "----------------------------------------------------" << std::endl;

  Simulator::Destroy ();

  return 0;
}
