#include <iostream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#define DISABLE_RANKING "true"

#define TOTAL_DURATION 400
#define ITERATIONS 100
#define GOOD_CONTENT_TIMEOUT 400
#define BAD_CONSUMER_RATE 3/16
// Bad Content in Routers
#define POPULATED_CONTENT_FRESHNESS 400
#define POPULATED_CONTENT_COUNT 100
#define GOOD_CONTENT_COUNT 1

#define NUM_OF_CONSUMERS 16
#define NUM_OF_ROUTERS 42

using namespace ns3;

int goodConsumerCount;
int64_t stoppingMicroSeconds[NUM_OF_CONSUMERS] = { 0 };
int stoppedConsumerCount;

void
StoppedOnGoodContent (std::string context, Ptr<ns3::ndn::ContentObject const> content, ns3::Time stoppingTime)
{
  stoppingMicroSeconds[stoppedConsumerCount] += stoppingTime.GetMicroSeconds();
  stoppedConsumerCount++;
}

int 
main (int argc, char *argv[])
{
  goodConsumerCount = 0;

  // setting default parameters for PointToPoint links and channels
  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("1Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"));
  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("20"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse (argc, argv);

  for (int it = 0; it < ITERATIONS; it++)
    {
      stoppedConsumerCount = 0;

      // Creating nodes
      NodeContainer nodes;
      nodes.Create (NUM_OF_CONSUMERS + NUM_OF_ROUTERS);

      // Connecting nodes using two links
      PointToPointHelper p2p;
      // Connecting consumers to edge routers
      p2p.Install (nodes.Get (0), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (1), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (2), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (3), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (4), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (5), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (6), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (7), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (8), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (9), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (10), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (11), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (12), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (13), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (14), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (15), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      // Connecting routers
      p2p.Install (nodes.Get (0 + NUM_OF_CONSUMERS), nodes.Get (1 + NUM_OF_CONSUMERS));      // R0 <--> R9
      p2p.Install (nodes.Get (1 + NUM_OF_CONSUMERS), nodes.Get (2 + NUM_OF_CONSUMERS));     // R1 <--> R15
      p2p.Install (nodes.Get (1 + NUM_OF_CONSUMERS), nodes.Get (17 + NUM_OF_CONSUMERS));     // R1 <--> R15
      p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (3 + NUM_OF_CONSUMERS));      // R2 <--> R9
      p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (4 + NUM_OF_CONSUMERS));      // R2 <--> R9
      p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (6 + NUM_OF_CONSUMERS));      // R2 <--> R9
      p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (36 + NUM_OF_CONSUMERS));      // R2 <--> R9
      p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (8 + NUM_OF_CONSUMERS));      // R2 <--> R9
      p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (17 + NUM_OF_CONSUMERS));      // R2 <--> R9
      p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (19 + NUM_OF_CONSUMERS));      // R2 <--> R9
      p2p.Install (nodes.Get (3 + NUM_OF_CONSUMERS), nodes.Get (5 + NUM_OF_CONSUMERS));      // R3 <--> R4
      p2p.Install (nodes.Get (4 + NUM_OF_CONSUMERS), nodes.Get (16 + NUM_OF_CONSUMERS));      // R4 <--> R7
      p2p.Install (nodes.Get (5 + NUM_OF_CONSUMERS), nodes.Get (8 + NUM_OF_CONSUMERS));     // R5 <--> R13
      p2p.Install (nodes.Get (6 + NUM_OF_CONSUMERS), nodes.Get (7 + NUM_OF_CONSUMERS));      // R6 <--> R7
      p2p.Install (nodes.Get (6 + NUM_OF_CONSUMERS), nodes.Get (37 + NUM_OF_CONSUMERS));      // R6 <--> R7
      p2p.Install (nodes.Get (7 + NUM_OF_CONSUMERS), nodes.Get (10 + NUM_OF_CONSUMERS));      // R7 <--> R9
      p2p.Install (nodes.Get (8 + NUM_OF_CONSUMERS), nodes.Get (9 + NUM_OF_CONSUMERS));      // R8 <--> R9
      p2p.Install (nodes.Get (8 + NUM_OF_CONSUMERS), nodes.Get (11 + NUM_OF_CONSUMERS));      // R8 <--> R9
      p2p.Install (nodes.Get (8 + NUM_OF_CONSUMERS), nodes.Get (17 + NUM_OF_CONSUMERS));      // R8 <--> R9
      // 9 done
      p2p.Install (nodes.Get (10 + NUM_OF_CONSUMERS), nodes.Get (11 + NUM_OF_CONSUMERS));    // R10 <--> R14
      p2p.Install (nodes.Get (11 + NUM_OF_CONSUMERS), nodes.Get (16 + NUM_OF_CONSUMERS));    // R11 <--> R13
      p2p.Install (nodes.Get (11 + NUM_OF_CONSUMERS), nodes.Get (38 + NUM_OF_CONSUMERS));    // R11 <--> R13
      p2p.Install (nodes.Get (11 + NUM_OF_CONSUMERS), nodes.Get (12 + NUM_OF_CONSUMERS));    // R11 <--> R13
      p2p.Install (nodes.Get (11 + NUM_OF_CONSUMERS), nodes.Get (13 + NUM_OF_CONSUMERS));    // R11 <--> R13
      p2p.Install (nodes.Get (11 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));    // R11 <--> R13
      p2p.Install (nodes.Get (12 + NUM_OF_CONSUMERS), nodes.Get (13 + NUM_OF_CONSUMERS));    // R12 <--> R13
      p2p.Install (nodes.Get (13 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));    // R13 <--> R14
      p2p.Install (nodes.Get (14 + NUM_OF_CONSUMERS), nodes.Get (16 + NUM_OF_CONSUMERS));    // R14 <--> R15
      p2p.Install (nodes.Get (14 + NUM_OF_CONSUMERS), nodes.Get (15 + NUM_OF_CONSUMERS));    // R14 <--> R18
      p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (39 + NUM_OF_CONSUMERS));    // R15 <--> R16
      p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (17 + NUM_OF_CONSUMERS));    // R15 <--> R19
      p2p.Install (nodes.Get (16 + NUM_OF_CONSUMERS), nodes.Get (17 + NUM_OF_CONSUMERS));    // R16 <--> R23
      p2p.Install (nodes.Get (16 + NUM_OF_CONSUMERS), nodes.Get (19 + NUM_OF_CONSUMERS));    // R16 <--> R27
      p2p.Install (nodes.Get (17 + NUM_OF_CONSUMERS), nodes.Get (18 + NUM_OF_CONSUMERS));    // R17 <--> R23
      p2p.Install (nodes.Get (17 + NUM_OF_CONSUMERS), nodes.Get (19 + NUM_OF_CONSUMERS));    // R17 <--> R23
      p2p.Install (nodes.Get (17 + NUM_OF_CONSUMERS), nodes.Get (29 + NUM_OF_CONSUMERS));    // R17 <--> R23
      p2p.Install (nodes.Get (17 + NUM_OF_CONSUMERS), nodes.Get (32 + NUM_OF_CONSUMERS));    // R17 <--> R23
      p2p.Install (nodes.Get (17 + NUM_OF_CONSUMERS), nodes.Get (31 + NUM_OF_CONSUMERS));    // R17 <--> R23
      p2p.Install (nodes.Get (17 + NUM_OF_CONSUMERS), nodes.Get (39 + NUM_OF_CONSUMERS));    // R17 <--> R23
      // 18 done
      p2p.Install (nodes.Get (19 + NUM_OF_CONSUMERS), nodes.Get (20 + NUM_OF_CONSUMERS));    // R19 <--> R22
      p2p.Install (nodes.Get (19 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R19 <--> R22
      // 20 done
      p2p.Install (nodes.Get (21 + NUM_OF_CONSUMERS), nodes.Get (22 + NUM_OF_CONSUMERS));    // R21 <--> R22
      p2p.Install (nodes.Get (22 + NUM_OF_CONSUMERS), nodes.Get (40 + NUM_OF_CONSUMERS));    // R22 <--> R23
      p2p.Install (nodes.Get (22 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));    // R22 <--> R28
      p2p.Install (nodes.Get (22 + NUM_OF_CONSUMERS), nodes.Get (25 + NUM_OF_CONSUMERS));    // R22 <--> R29
      p2p.Install (nodes.Get (23 + NUM_OF_CONSUMERS), nodes.Get (24 + NUM_OF_CONSUMERS));    // R23 <--> R24
      // 24 done
      p2p.Install (nodes.Get (25 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (26 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (27 + NUM_OF_CONSUMERS), nodes.Get (40 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (27 + NUM_OF_CONSUMERS), nodes.Get (30 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (27 + NUM_OF_CONSUMERS), nodes.Get (31 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (28 + NUM_OF_CONSUMERS), nodes.Get (29 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (29 + NUM_OF_CONSUMERS), nodes.Get (30 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (30 + NUM_OF_CONSUMERS), nodes.Get (31 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (30 + NUM_OF_CONSUMERS), nodes.Get (41 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (30 + NUM_OF_CONSUMERS), nodes.Get (35 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (31 + NUM_OF_CONSUMERS), nodes.Get (41 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (31 + NUM_OF_CONSUMERS), nodes.Get (32 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (31 + NUM_OF_CONSUMERS), nodes.Get (34 + NUM_OF_CONSUMERS));    // R26 <--> R27
      p2p.Install (nodes.Get (32 + NUM_OF_CONSUMERS), nodes.Get (33 + NUM_OF_CONSUMERS));    // R26 <--> R27
      // 33 done
      // 34 done
      // 35 done
      // 36 done
      // 37 done
      // 38 done
      // 39 done 
      // 40 done
      // 41 done
      
      // Install CCNx with cache on routers
      std::ostringstream exclusion_discard_timeout;
      exclusion_discard_timeout << GOOD_CONTENT_TIMEOUT;

      std::ostringstream populated_content_freshness;
      populated_content_freshness << POPULATED_CONTENT_FRESHNESS;
      // std::ostringstream populated_content_count;
      // populated_content_count << POPULATED_CONTENT_COUNT;

      ndn::StackHelper ccnxHelperWithCache;
      ccnxHelperWithCache.SetDefaultRoutes (true);
      ccnxHelperWithCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0", "ExclusionDiscardedTimeout", exclusion_discard_timeout.str(), "DisableRanking", DISABLE_RANKING);
      ccnxHelperWithCache.SetContentStoreAttribute ("BadContentName", "/prefix/0");
      ccnxHelperWithCache.SetContentStoreAttribute ("BadContentFreshness", populated_content_freshness.str());
      // ccnxHelperWithCache.SetContentStoreAttribute ("BadContentCount", populated_content_count.str());
      ccnxHelperWithCache.SetContentStoreAttribute ("BadContentPayloadSize", "1024");
      // ccnxHelperWithCache.SetContentStoreAttribute ("BadContentRate", bad_content_rate.str());
      for (int i = NUM_OF_CONSUMERS; i < NUM_OF_CONSUMERS + NUM_OF_ROUTERS; i++)
	{
	  ccnxHelperWithCache.Install (nodes.Get (i));
	  nodes.Get (i)->GetObject<ns3::ndn::ContentStore> ()->Populate (POPULATED_CONTENT_COUNT, GOOD_CONTENT_COUNT);
	}

      // Install CCNx without cache on consumers
      std::ostringstream max_uint32_t;
      max_uint32_t << (sizeof(uint32_t) * 256) - 1;

      ndn::StackHelper ccnxHelperNoCache;
      ccnxHelperNoCache.SetDefaultRoutes (true);
      ccnxHelperNoCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", max_uint32_t.str());
      for (int i = 0; i < NUM_OF_CONSUMERS; i++)
	{
	  ccnxHelperNoCache.Install (nodes.Get (i));
	}

      // Install Applications

      // Consumer
      for (int i = 0; i < NUM_OF_CONSUMERS; i++)
	{
	  ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
	  // Consumer will request /prefix/0, /prefix/1, ... up to /prefix/<MaxSeq>
	  consumerHelper.SetPrefix ("/prefix");
	  consumerHelper.SetAttribute ("Frequency", StringValue ("1"));
	  consumerHelper.SetAttribute ("MaxSeq", StringValue ("0"));
	  consumerHelper.SetAttribute ("Repeat", BooleanValue (true));
	  consumerHelper.SetAttribute ("ExclusionRate", DoubleValue (0.0));
	  consumerHelper.SetAttribute ("DisableExclusion", BooleanValue (false));
	  double r = (double)rand() / RAND_MAX;
	  if (BAD_CONSUMER_RATE != 0 && r <= BAD_CONSUMER_RATE)
	    {
	      consumerHelper.SetAttribute ("Malicious", BooleanValue (true));
	    }
	  else
	    {
	      goodConsumerCount++;
	      consumerHelper.SetAttribute ("Malicious", BooleanValue (false));
	      consumerHelper.SetAttribute ("StopOnGoodContent", BooleanValue (true));
	    }
	  consumerHelper.Install (nodes.Get (i));

	  if (BAD_CONSUMER_RATE == 0 || r > BAD_CONSUMER_RATE)
	    {
	      std::ostringstream node_id;
	      node_id << i;
	      Config::Connect ("/NodeList/" + node_id.str() + "/ApplicationList/0/StoppedOnGoodContent", MakeCallback (StoppedOnGoodContent));
	    }
	}

      // Run simulation
      Simulator::Stop (Seconds (TOTAL_DURATION));
      Simulator::Run ();
      Simulator::Destroy ();
    }

  std::cout << "Stopping microseconds: ";
  for (int j = 0; j < goodConsumerCount / ITERATIONS; j++)
    {
      std::cout << ((double)(stoppingMicroSeconds[j])) / ITERATIONS;
      if (j < (goodConsumerCount / ITERATIONS) - 1)
	{
	  std::cout << ", ";
	}
    }
  std::cout << std::endl;

  return 0;
}
