#include <iostream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#define DISABLE_RANKING "false"

#define BAD_CONTENT_RATE_STEP 0.1
#define TOTAL_DURATION 400
#define ITERATIONS 1000
#define GOOD_CONTENT_TIMEOUT 100
// L1 Routers
#define L1_BAD_CONTENT_FRESHNESS 100
#define L1_BAD_CONTENT_COUNT 100
// L2 Routers
#define L2_BAD_CONTENT_FRESHNESS 300
#define L2_BAD_CONTENT_COUNT 100

#define NUM_OF_CONSUMERS 4

using namespace ns3;

int badContentReceivedCount = 0;
int goodContentReceivedCount = 0;
int contentReceivedCount = 0;

void
BadContentReceived (std::string context, Ptr<ns3::ndn::ContentObject const> content)
{
  badContentReceivedCount++;
}

void
GoodContentReceived (std::string context, Ptr<ns3::ndn::ContentObject const> content)
{
  goodContentReceivedCount++;
}

void
ReceivedContentObjects (std::string context, Ptr<ns3::ndn::ContentObject const> content, Ptr<Packet const> packet, Ptr<ns3::ndn::App> app, Ptr<ns3::ndn::Face> face)
{
  contentReceivedCount++;
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

  std::cout << "-------------------------------------------------------------------------------------------------" << std::endl;
  std::cout << "Bad Content Rate        Content Received        Bad Content Received        Good Content Received" << std::endl;
  std::cout << "-------------------------------------------------------------------------------------------------" << std::endl;
  for (int r = 0; r < (1 / BAD_CONTENT_RATE_STEP) + 1; r++)
    {
      // Reset counters
      badContentReceivedCount = 0;
      goodContentReceivedCount = 0;
      contentReceivedCount = 0;

      for (int it = 0; it < ITERATIONS; it++)
	{
	  // Creating nodes (0: router, 1: bad consumer, 2: bad producer, 3: good producer, rest are good consumers
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
	  exclusion_discard_timeout << GOOD_CONTENT_TIMEOUT;
	  std::ostringstream bad_content_rate;
	  bad_content_rate << 0 + (BAD_CONTENT_RATE_STEP * r);

	  std::ostringstream l2_bad_content_freshness;
	  l2_bad_content_freshness << L2_BAD_CONTENT_FRESHNESS;
	  std::ostringstream l2_bad_content_count;
	  l2_bad_content_count << L2_BAD_CONTENT_COUNT;

	  ndn::StackHelper l2_ccnxHelperWithCache;
	  l2_ccnxHelperWithCache.SetDefaultRoutes (true);
	  l2_ccnxHelperWithCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0", "ExclusionDiscardedTimeout", exclusion_discard_timeout.str(), "DisableRanking", DISABLE_RANKING);
	  l2_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentName", "/prefix/0");
	  l2_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentFreshness", l2_bad_content_freshness.str());
	  l2_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentCount", l2_bad_content_count.str());
	  l2_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentPayloadSize", "1024");
	  l2_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentRate", bad_content_rate.str());
	  l2_ccnxHelperWithCache.Install (nodes.Get (0));
	  nodes.Get (0)->GetObject<ns3::ndn::ContentStore> ()->Populate ();

	  // Install CCNx with cache on R2 and R3
	  std::ostringstream l1_bad_content_freshness;
	  l1_bad_content_freshness << L1_BAD_CONTENT_FRESHNESS;
	  std::ostringstream l1_bad_content_count;
	  l1_bad_content_count << L1_BAD_CONTENT_COUNT;

	  ndn::StackHelper l1_ccnxHelperWithCache;
	  l1_ccnxHelperWithCache.SetDefaultRoutes (true);
	  l1_ccnxHelperWithCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0", "ExclusionDiscardedTimeout", exclusion_discard_timeout.str(), "DisableRanking", DISABLE_RANKING);
	  l1_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentName", "/prefix/0");
	  l1_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentFreshness", l1_bad_content_freshness.str());
	  l1_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentCount", l1_bad_content_count.str());
	  l1_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentPayloadSize", "1024");
	  l1_ccnxHelperWithCache.SetContentStoreAttribute ("BadContentRate", bad_content_rate.str());
	  l1_ccnxHelperWithCache.Install (nodes.Get (1));
	  l1_ccnxHelperWithCache.Install (nodes.Get (2));
	  nodes.Get (1)->GetObject<ns3::ndn::ContentStore> ()->Populate ();
 	  nodes.Get (2)->GetObject<ns3::ndn::ContentStore> ()->Populate ();

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

	  // Good Producer
	  ndn::AppHelper goodProducerHelper ("ns3::ndn::Producer");
	  // Producer will reply to all requests with good content starting with /prefix
	  goodProducerHelper.SetPrefix ("/prefix");
	  goodProducerHelper.SetAttribute ("PayloadSize", StringValue("1024"));
	  goodProducerHelper.SetAttribute ("Freshness", TimeValue (Seconds(GOOD_CONTENT_TIMEOUT)));
	  goodProducerHelper.SetAttribute ("BadContentRate", DoubleValue (0.0));
	  goodProducerHelper.Install (nodes.Get (3));

	  // Good Consumer
	  ndn::AppHelper goodConsumerHelper ("ns3::ndn::ConsumerCbr");
	  // Consumer will request /prefix/0, /prefix/1, ... up to /prefix/<MaxSeq>
	  goodConsumerHelper.SetPrefix ("/prefix");
	  goodConsumerHelper.SetAttribute ("Frequency", StringValue ("1"));
	  goodConsumerHelper.SetAttribute ("MaxSeq", StringValue ("0"));
	  goodConsumerHelper.SetAttribute ("Repeat", BooleanValue (true));
	  goodConsumerHelper.SetAttribute ("ExclusionRate", DoubleValue (0.0));
	  goodConsumerHelper.SetAttribute ("DisableExclusion", BooleanValue (false));
	  for (int i = 4; i < 4 + NUM_OF_CONSUMERS; i++)
	    {
	      goodConsumerHelper.Install (nodes.Get (i));

	      std::ostringstream node_id;
	      node_id << i;
	      Config::Connect ("/NodeList/" + node_id.str() + "/ApplicationList/0/BadContentReceived", MakeCallback (BadContentReceived));
	      Config::Connect ("/NodeList/" + node_id.str() + "/ApplicationList/0/GoodContentReceived", MakeCallback (GoodContentReceived));
	      Config::Connect ("/NodeList/" + node_id.str() + "/ApplicationList/0/ReceivedContentObjects", MakeCallback (ReceivedContentObjects));
	    }

	  // Run simulation
	  Simulator::Stop (Seconds (TOTAL_DURATION));
	  Simulator::Run ();
	  Simulator::Destroy ();
	}

      // Print Results
      std::cout << std::fixed << std::setprecision(2);
      std::cout << std::setw(15) << 0 + (BAD_CONTENT_RATE_STEP * r) << "%";
      std::cout << std::setw(24) << (double)contentReceivedCount / (ITERATIONS * NUM_OF_CONSUMERS);

      std::ostringstream bad_received_str;
      bad_received_str << std::fixed << std::setprecision(2) << (double)badContentReceivedCount / (ITERATIONS * NUM_OF_CONSUMERS) << " (" << ((double)badContentReceivedCount / contentReceivedCount) * 100 << "%)";
      std::cout << std::setw(28) << bad_received_str.str();
      
      std::ostringstream good_received_str;
      good_received_str << std::fixed << std::setprecision(2) << (double)goodContentReceivedCount / (ITERATIONS * NUM_OF_CONSUMERS) << " (" << ((double)goodContentReceivedCount / contentReceivedCount) * 100 << "%)";
      std::cout << std::setw(29) << good_received_str.str();
      std::cout << std::endl;
    }
  std::cout << "-------------------------------------------------------------------------------------------------" << std::endl;

  return 0;
}
