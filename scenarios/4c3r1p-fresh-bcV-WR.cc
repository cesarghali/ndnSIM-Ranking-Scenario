#include <iostream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#define DISABLE_RANKING "false"

#define BAD_CONTENT_RATE_STEP 0.1
#define DURATION 360000
#define TIMEOUT 7200
#define NUM_OF_CONSUMERS 4
#define PRODUCER_INDEX 3

#define NUM_OF_CONSUMERS 4

using namespace ns3;

int badContentCount = 0;

void
BadContentReceived (std::string context, Ptr<ns3::ndn::ContentObject const> content)
{
  badContentCount++;
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

  std::cout << "---------------------------------------------" << std::endl;
  std::cout << "Bad Content Rate        Bad Content Reception" << std::endl;
  std::cout << "---------------------------------------------" << std::endl;
  for (int i = 0; i < (1 / BAD_CONTENT_RATE_STEP) + 1; i++)
    {
      float bad_content_rate = 0 + (BAD_CONTENT_RATE_STEP * i);
  
      // Reset counters
      badContentCount = 0;

      std::cout << std::fixed << std::setprecision(2);
      std::cout << std::setw(16) << bad_content_rate;

      // Creating nodes
      NodeContainer nodes;
      nodes.Create (4 + NUM_OF_CONSUMERS);

      // Connecting nodes using two links
      PointToPointHelper p2p;
      p2p.Install (nodes.Get (1), nodes.Get (0));    // R2 <--> R1
      p2p.Install (nodes.Get (2), nodes.Get (0));    // R3 <--> R1
      p2p.Install (nodes.Get (1), nodes.Get (2));    // R2 <--> R3
      p2p.Install (nodes.Get (3), nodes.Get (0));    // Producer <--> R1
      for (int i = 4; i < 4 + (NUM_OF_CONSUMERS / 2); i++)
	{
	  p2p.Install (nodes.Get (i), nodes.Get (2));
	}
      for (int i = 4 + (NUM_OF_CONSUMERS / 2); i < 4 + NUM_OF_CONSUMERS; i++)
	{
	  p2p.Install (nodes.Get (i), nodes.Get (3));
	}

      // Install CCNx with cache on R1
      std::ostringstream exclusion_discard_timeout;
      exclusion_discard_timeout << TIMEOUT;

      ndn::StackHelper ccnxHelperWithCache;
      ccnxHelperWithCache.SetDefaultRoutes (true);
      ccnxHelperWithCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0", "ExclusionDiscardedTimeout", exclusion_discard_timeout.str(), "DisableRanking", DISABLE_RANKING);
      ccnxHelperWithCache.Install (nodes.Get (0));
      ccnxHelperWithCache.Install (nodes.Get (1));
      ccnxHelperWithCache.Install (nodes.Get (2));

      // Install CCNx without cache on the other nodes
      std::ostringstream max_uint32_t;
      max_uint32_t << (sizeof(uint32_t) * 256) - 1;

      ndn::StackHelper ccnxHelperNoCache;
      ccnxHelperNoCache.SetDefaultRoutes (true);
      ccnxHelperNoCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", max_uint32_t.str());
      for (int i = 3; i < 4 + NUM_OF_CONSUMERS; i++)
	{
	  ccnxHelperNoCache.Install (nodes.Get (i));
	}

      // Install Applications

      ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
      // Consumer will request /prefix/0, /prefix/1, ... up to /prefix/<MaxSeq>
      consumerHelper.SetPrefix ("/prefix");
      consumerHelper.SetAttribute ("Frequency", StringValue ("1"));
      consumerHelper.SetAttribute ("MaxSeq", StringValue ("0"));
      consumerHelper.SetAttribute ("Repeat", BooleanValue (true));
      consumerHelper.SetAttribute ("ExclusionRate", DoubleValue (0.0));
      // Install the application on all consumers
      for (int i = 4; i < 4 + NUM_OF_CONSUMERS; i++)
	{
	  consumerHelper.Install (nodes.Get (i));
	}
      Config::Connect ("/NodeList/*/ApplicationList/*/BadContentReceived", MakeCallback (BadContentReceived));

      // Producer
      ndn::AppHelper producerHelper ("ns3::ndn::Producer");
      // Producer will reply to all requests starting with /prefix
      producerHelper.SetPrefix ("/prefix");
      producerHelper.SetAttribute ("PayloadSize", StringValue("1024"));
      producerHelper.SetAttribute ("Freshness", TimeValue (Seconds (TIMEOUT)));
      producerHelper.SetAttribute ("BadContentRate", DoubleValue (bad_content_rate));
      producerHelper.Install (nodes.Get (PRODUCER_INDEX)); // last node

      // Run simulation
      Simulator::Stop (Seconds (DURATION));
      Simulator::Run ();
      Simulator::Destroy ();

      // Print out results
      std::cout << std::fixed << std::setprecision(4);
      std::cout << std::setw(28) << ((float)badContentCount / (DURATION * NUM_OF_CONSUMERS)) * 100 << "%";
      std::cout << std::endl;
    }
  std::cout << "---------------------------------------------" << std::endl;

  return 0;
}
