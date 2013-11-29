#include <iostream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#define DISABLE_RANKING "false"

#define TOTAL_DURATION 400
#define ITERATIONS 10
#define GOOD_CONTENT_TIMEOUT 400
#define BAD_CONSUMER_RATE 3/50.0
// Bad Content in Routers
#define POPULATED_CONTENT_FRESHNESS 400
#define POPULATED_CONTENT_COUNT 1000
#define GOOD_CONTENT_COUNT 1

#define NUM_OF_CONSUMERS 50
#define NUM_OF_ROUTERS 5

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
      // Connecting each 10 consumers to a router
      PointToPointHelper p2p;
      for (int j = 0; j < NUM_OF_ROUTERS; j++)
	{
	  for (int i = 0; i < NUM_OF_CONSUMERS / NUM_OF_ROUTERS; i++)
	    {
	      p2p.Install (nodes.Get (j), nodes.Get (NUM_OF_ROUTERS + (j * (NUM_OF_CONSUMERS / NUM_OF_ROUTERS)) + i));
	    }
	}

      // Install CCNx with cache on router
      std::ostringstream exclusion_discard_timeout;
      exclusion_discard_timeout << GOOD_CONTENT_TIMEOUT;

      std::ostringstream populated_content_freshness;
      populated_content_freshness << POPULATED_CONTENT_FRESHNESS;

      ndn::StackHelper ccnxHelperWithCache;
      ccnxHelperWithCache.SetDefaultRoutes (true);
      ccnxHelperWithCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0", "ExclusionDiscardedTimeout", exclusion_discard_timeout.str(), "DisableRanking", DISABLE_RANKING);
      ccnxHelperWithCache.SetContentStoreAttribute ("BadContentName", "/prefix/0");
      ccnxHelperWithCache.SetContentStoreAttribute ("BadContentFreshness", populated_content_freshness.str());
      ccnxHelperWithCache.SetContentStoreAttribute ("BadContentPayloadSize", "1024");
      for (int i = 0; i < NUM_OF_ROUTERS; i++)
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
	  ccnxHelperNoCache.Install (nodes.Get (NUM_OF_ROUTERS + i));
	}

      // Install Applications

      // Consumer
      for (int i = 0; i < NUM_OF_CONSUMERS; i++)
	{
	  ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
	  // Consumer will request /prefix/0, /prefix/1, ... up to /prefix/<MaxSeq>
	  consumerHelper.SetPrefix ("/prefix");
	  consumerHelper.SetAttribute ("Frequency", StringValue ("5"));
	  consumerHelper.SetAttribute ("Randomize", StringValue ("uniform"));
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
	  consumerHelper.Install (nodes.Get (NUM_OF_ROUTERS + i));

	  if (BAD_CONSUMER_RATE == 0 || r > BAD_CONSUMER_RATE)
	    {
	      std::ostringstream node_id;
	      node_id << NUM_OF_ROUTERS + i;
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
