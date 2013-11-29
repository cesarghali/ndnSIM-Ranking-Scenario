#include <iostream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#define DISABLE_RANKING "true"

#define TOTAL_DURATION 400
#define ITERATIONS 50
#define GOOD_CONTENT_TIMEOUT 400
#define BAD_CONSUMER_RATE 5/160.0
// Bad Content in Routers
#define POPULATED_CONTENT_FRESHNESS 400
#define POPULATED_CONTENT_COUNT 100
#define GOOD_CONTENT_COUNT 1

#define NUM_OF_CONSUMERS 16 * 10
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
      // Connecting 10 consumers to each edge routers
      p2p.Install (nodes.Get (0), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (1), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (2), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (3), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (4), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (5), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (6), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (7), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (8), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
      p2p.Install (nodes.Get (9), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0

      p2p.Install (nodes.Get (10), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (11), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (12), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (13), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (14), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (15), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (16), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (17), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (18), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1
      p2p.Install (nodes.Get (19), nodes.Get (36 + NUM_OF_CONSUMERS));      // C1 <--> R1

      p2p.Install (nodes.Get (20), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (21), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (22), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (23), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (24), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (25), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (26), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (27), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (28), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3
      p2p.Install (nodes.Get (29), nodes.Get (37 + NUM_OF_CONSUMERS));      // C2 <--> R3

      p2p.Install (nodes.Get (30), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (31), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (32), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (33), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (34), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (35), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (36), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (37), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (38), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5
      p2p.Install (nodes.Get (39), nodes.Get (9 + NUM_OF_CONSUMERS));      // C3 <--> R5

      p2p.Install (nodes.Get (40), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (41), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (42), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (43), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (44), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (45), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (46), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (47), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (48), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6
      p2p.Install (nodes.Get (49), nodes.Get (38 + NUM_OF_CONSUMERS));      // C4 <--> R6

      p2p.Install (nodes.Get (50), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (51), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (52), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (53), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (54), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (55), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (56), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (57), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (58), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10
      p2p.Install (nodes.Get (59), nodes.Get (13 + NUM_OF_CONSUMERS));     // C5 <--> R10

      p2p.Install (nodes.Get (60), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (61), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (62), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (63), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (64), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (65), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (66), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (67), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (68), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8
      p2p.Install (nodes.Get (69), nodes.Get (16 + NUM_OF_CONSUMERS));      // C6 <--> R8

      p2p.Install (nodes.Get (70), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (71), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (72), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (73), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (74), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (75), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (76), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (77), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (78), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11
      p2p.Install (nodes.Get (79), nodes.Get (20 + NUM_OF_CONSUMERS));     // C7 <--> R11

      p2p.Install (nodes.Get (80), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (81), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (82), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (83), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (84), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (85), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (86), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (87), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (88), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12
      p2p.Install (nodes.Get (89), nodes.Get (18 + NUM_OF_CONSUMERS));     // C8 <--> R12

      p2p.Install (nodes.Get (90), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (91), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (92), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (93), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (94), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (95), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (96), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (97), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (98), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18
      p2p.Install (nodes.Get (99), nodes.Get (28 + NUM_OF_CONSUMERS));     // C9 <--> R18

      p2p.Install (nodes.Get (100), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (101), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (102), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (103), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (104), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (105), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (106), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (107), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (108), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17
      p2p.Install (nodes.Get (109), nodes.Get (21 + NUM_OF_CONSUMERS));    // C10 <--> R17

      p2p.Install (nodes.Get (110), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (111), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (112), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (113), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (114), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (115), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (116), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (117), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (118), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20
      p2p.Install (nodes.Get (119), nodes.Get (24 + NUM_OF_CONSUMERS));    // C11 <--> R20

      p2p.Install (nodes.Get (120), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (121), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (122), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (123), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (124), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (125), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (126), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (127), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (128), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24
      p2p.Install (nodes.Get (129), nodes.Get (26 + NUM_OF_CONSUMERS));    // C12 <--> R24

      p2p.Install (nodes.Get (130), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (131), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (132), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (133), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (134), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (135), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (136), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (137), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (138), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29
      p2p.Install (nodes.Get (139), nodes.Get (35 + NUM_OF_CONSUMERS));    // C13 <--> R29

      p2p.Install (nodes.Get (140), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (141), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (142), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (143), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (144), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (145), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (146), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (147), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (148), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28
      p2p.Install (nodes.Get (149), nodes.Get (34 + NUM_OF_CONSUMERS));    // C14 <--> R28

      p2p.Install (nodes.Get (150), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (151), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (152), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (153), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (154), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (155), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (156), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (157), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (158), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
      p2p.Install (nodes.Get (159), nodes.Get (33 + NUM_OF_CONSUMERS));    // C15 <--> R21
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
