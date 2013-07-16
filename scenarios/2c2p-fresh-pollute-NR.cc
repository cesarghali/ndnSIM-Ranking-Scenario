#include <iostream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#define TOTAL_DURATION 400
#define ATTACK_DURATION 50
#define TIMEOUT 100

#define NUM_OF_CONSUMERS 1

using namespace ns3;

int badContentReceivedCount = 0;
int goodContentReceivedCount = 0;
int contentCount = 0;
int badContentTransmittedCount = 0;

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
  contentCount++;
}

void
BadContentTransmitted (std::string context, Ptr<ns3::ndn::ContentObject const> content)
{
  badContentTransmittedCount++;
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

  // Creating nodes (0: router, 1: bad consumer, 2: bad producer, 3: good producer, rest are good consumers
  NodeContainer nodes;
  nodes.Create (4 + NUM_OF_CONSUMERS);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  p2p.Install (nodes.Get (1), nodes.Get (0));
  p2p.Install (nodes.Get (2), nodes.Get (0));
  p2p.Install (nodes.Get (3), nodes.Get (0));
  for (int i = 0; i < 4 + NUM_OF_CONSUMERS; i++)
    {
      p2p.Install (nodes.Get (i), nodes.Get (0));
    }

  // Install CCNx with cache on the router
  std::ostringstream exclusion_discard_timeout;
  exclusion_discard_timeout << TIMEOUT;
  std::ostringstream search_cache_after;
  search_cache_after << ATTACK_DURATION;
  std::ostringstream max_uint32_t;
  max_uint32_t << (sizeof(uint32_t) * 256) - 1;

  ndn::StackHelper ccnxHelperWithCache;
  ccnxHelperWithCache.SetDefaultRoutes (true);
  ccnxHelperWithCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0", "ExclusionDiscardedTimeout", exclusion_discard_timeout.str(), "DisableRanking", "true", "SearchCacheAfter", search_cache_after.str());
  ccnxHelperWithCache.Install (nodes.Get (0));
  // Install CCNx without cache on the other nodes
  ndn::StackHelper ccnxHelperNoCache;
  ccnxHelperNoCache.SetDefaultRoutes (true);
  ccnxHelperNoCache.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", max_uint32_t.str());
  for (int i = 1; i < 4 + NUM_OF_CONSUMERS; i++)
    {
      ccnxHelperNoCache.Install (nodes.Get (i));
    }

  // Install Applications
  ApplicationContainer apps;

  // Bad Consumer
  ndn::AppHelper badConsumerHelper ("ns3::ndn::ConsumerCbr");
  // Consumer will request /prefix/0, /prefix/1, ... up to /prefix/<MaxSeq>
  badConsumerHelper.SetPrefix ("/prefix");
  badConsumerHelper.SetAttribute ("Frequency", StringValue ("1"));
  badConsumerHelper.SetAttribute ("MaxSeq", StringValue ("0"));
  badConsumerHelper.SetAttribute ("Repeat", BooleanValue (true));
  badConsumerHelper.SetAttribute ("ExclusionRate", DoubleValue (0.0));
  badConsumerHelper.SetAttribute ("DisableExclusion", BooleanValue (true));
  apps = badConsumerHelper.Install (nodes.Get (1));
  apps.Start (Seconds (0));
  apps.Stop (Seconds (ATTACK_DURATION));

  // bad Producer
  ndn::AppHelper badProducerHelper ("ns3::ndn::Producer");
  // Producer will reply to all requests with bad content starting with /prefix
  badProducerHelper.SetPrefix ("/prefix");
  badProducerHelper.SetAttribute ("PayloadSize", StringValue("1024"));
  badProducerHelper.SetAttribute ("Freshness", TimeValue (Seconds (TIMEOUT)));
  badProducerHelper.SetAttribute ("BadContentRate", DoubleValue (1.0));
  apps = badProducerHelper.Install (nodes.Get (2));
  apps.Start (Seconds (0));
  apps.Stop (Seconds (ATTACK_DURATION));
  Config::Connect ("/NodeList/2/ApplicationList/0/BadContentTransmitted", MakeCallback (BadContentTransmitted));

  // Good Producer
  ndn::AppHelper goodProducerHelper ("ns3::ndn::Producer");
  // Producer will reply to all requests with good content starting with /prefix
  goodProducerHelper.SetPrefix ("/prefix");
  goodProducerHelper.SetAttribute ("PayloadSize", StringValue("1024"));
  goodProducerHelper.SetAttribute ("Freshness", TimeValue (Seconds(TIMEOUT)));
  goodProducerHelper.SetAttribute ("BadContentRate", DoubleValue (0.0));
  apps = goodProducerHelper.Install (nodes.Get (3));
  apps.Start (Seconds (ATTACK_DURATION));
  apps.Stop (Seconds (TOTAL_DURATION));

  // Good Consumer
  ndn::AppHelper goodConsumerHelper ("ns3::ndn::ConsumerCbr");
  // Consumer will request /prefix/0, /prefix/1, ... up to /prefix/<MaxSeq>
  goodConsumerHelper.SetPrefix ("/prefix");
  goodConsumerHelper.SetAttribute ("Frequency", StringValue ("1"));
  goodConsumerHelper.SetAttribute ("MaxSeq", StringValue ("0"));
  goodConsumerHelper.SetAttribute ("Repeat", BooleanValue (true));
  goodConsumerHelper.SetAttribute ("ExclusionRate", DoubleValue (0.0));
  badConsumerHelper.SetAttribute ("DisableExclusion", BooleanValue (false));
  for (int i = 4; i < 4 + NUM_OF_CONSUMERS; i++)
    {
      apps = goodConsumerHelper.Install (nodes.Get (i));
      apps.Start (Seconds (ATTACK_DURATION));
      apps.Stop (Seconds (TOTAL_DURATION));

      std::ostringstream node_id;
      node_id << i;
      Config::Connect ("/NodeList/" + node_id.str() + "/ApplicationList/0/BadContentReceived", MakeCallback (BadContentReceived));
      Config::Connect ("/NodeList/" + node_id.str() + "/ApplicationList/0/GoodContentReceived", MakeCallback (GoodContentReceived));
      Config::Connect ("/NodeList/" + node_id.str() + "/ApplicationList/0/ReceivedContentObjects", MakeCallback (ReceivedContentObjects));
    }


  // Reset counters
  badContentReceivedCount = 0;
  goodContentReceivedCount = 0;
  contentCount = 0;
  badContentTransmittedCount = 0;

  // Run simulation
  Simulator::Stop (Seconds (TOTAL_DURATION));
  Simulator::Run ();

  // Print Results
  std::cout << "Bad Content Transmitted: " << badContentTransmittedCount / 2 << std::endl;
  std::cout << "Content Received: " << contentCount << std::endl;
  std::cout << "Bad Content Received: " << badContentReceivedCount << std::endl;
  std::cout << "Good Content Received: " << goodContentReceivedCount << std::endl;

  Simulator::Destroy ();

  return 0;
}
