/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *   Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   Author: Marco Miozzo <marco.miozzo@cttc.es>
 *           Nicola Baldo  <nbaldo@cttc.es>
 *
 *   Modified by: Marco Mezzavilla < mezzavilla@nyu.edu>
 *                         Sourjya Dutta <sdutta@nyu.edu>
 *                         Russell Ford <russell.ford@nyu.edu>
 *                         Menglei Zhang <menglei@nyu.edu>
 */

#include "ns3/applications-module.h"
#include "ns3/command-line.h"
#include "ns3/config-store.h"
#include "ns3/internet-module.h"
#include "ns3/isotropic-antenna-model.h"
#include "ns3/mmwave-helper.h"
#include "ns3/mmwave-point-to-point-epc-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-module.h"
#include <ns3/buildings-helper.h>
#include <ns3/buildings-module.h>
#include <ns3/packet.h>
#include <ns3/queue-size.h>
#include <ns3/tag.h>
#include "ns3/traffic-control-helper.h"
#include "ns3/log.h"


using namespace ns3;
using namespace mmwave;

/**
 * A script to simulate the DOWNLINK TCP data over mmWave links
 * with the mmWave devices and the LTE EPC.
 */
NS_LOG_COMPONENT_DEFINE("mmWaveTCPExample");

// Global variables for throughput calculation
uint64_t totalBytesReceived_ul = 0;
uint64_t totalBytesReceived_dl = 0;
Time simulationStartTime = Seconds(0.0); // Record simulation start time
Ptr<OutputStreamWrapper> dlThroughputStream;
Ptr<OutputStreamWrapper> ulThroughputStream;


bool DLfirstCwnd = true;
bool DLfirstSshThr = true;
bool DLfirstRtt = true;

bool ULfirstCwnd = true;
bool ULfirstSshThr = true;
bool ULfirstRtt = true;
//dl stream
Ptr<OutputStreamWrapper> DLcWndStream;
Ptr<OutputStreamWrapper> DLssThreshStream;
Ptr<OutputStreamWrapper> DLrttStream;
Ptr<OutputStreamWrapper> DLackStream;
Ptr<OutputStreamWrapper> DLcongStateStream;
uint32_t DLcWndValue;
uint32_t DLssThreshValue;
//ul stream
Ptr<OutputStreamWrapper> ULcWndStream;
Ptr<OutputStreamWrapper> ULssThreshStream;
Ptr<OutputStreamWrapper> ULrttStream;
Ptr<OutputStreamWrapper> ULackStream;
Ptr<OutputStreamWrapper> ULcongStateStream;
uint32_t ULcWndValue;
uint32_t ULssThreshValue;


//DL tracing
static void
DLCwndTracer (uint32_t oldval, uint32_t newval)
{
  if (DLfirstCwnd)
    {
      *DLcWndStream->GetStream () << "0.0 " << oldval << std::endl;
      DLfirstCwnd = false;
    }
  *DLcWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  DLcWndValue = newval;

  if (!DLfirstSshThr)
    {
      *DLssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << DLssThreshValue << std::endl;
    }
}


static void
DLRttTracer (Time oldval, Time newval)
{
  if (DLfirstRtt)
    {
      *DLrttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      DLfirstRtt = false;
    }
  *DLrttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

// static void
// Rx(Ptr<OutputStreamWrapper> stream, Ptr<const Packet> packet, const Address& from)
// {
//     *stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << packet->GetSize() << std::endl;
// }


static void
DLAckTracer (SequenceNumber32 old, SequenceNumber32 newAck)
{
  *DLackStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newAck << std::endl;
}

static void
DLCongStateTracer (TcpSocketState::TcpCongState_t old, TcpSocketState::TcpCongState_t newState)
{
  *DLcongStateStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newState << std::endl;
}

static void
DLSsThreshTracer (uint32_t oldval, uint32_t newval)
{
  if (DLfirstSshThr)
    {
      *DLssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      DLfirstSshThr = false;
    }
  *DLssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  DLssThreshValue = newval;

  if (!DLfirstCwnd)
    {
      *DLcWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << DLcWndValue << std::endl;
    }
}

//UL tracing
static void
ULCwndTracer (uint32_t oldval, uint32_t newval)
{
  if (ULfirstCwnd)
    {
      *ULcWndStream->GetStream () << "0.0 " << oldval << std::endl;
      ULfirstCwnd = false;
    }
  *ULcWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ULcWndValue = newval;

  if (!ULfirstSshThr)
    {
      *ULssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ULssThreshValue << std::endl;
    }
}

static void
ULRttTracer (Time oldval, Time newval)
{
  if (ULfirstRtt)
    {
      *ULrttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      ULfirstRtt = false;
    }
  *ULrttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
ULAckTracer (SequenceNumber32 old, SequenceNumber32 newAck)
{
  *ULackStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newAck << std::endl;
}

static void
ULCongStateTracer (TcpSocketState::TcpCongState_t old, TcpSocketState::TcpCongState_t newState)
{
  *ULcongStateStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newState << std::endl;
}

static void
ULSsThreshTracer (uint32_t oldval, uint32_t newval)
{
  if (ULfirstSshThr)
    {
      *ULssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      ULfirstSshThr = false;
    }
  *ULssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ULssThreshValue = newval;

  if (!ULfirstCwnd)
    {
      *ULcWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ULcWndValue << std::endl;
    }
}

// DL trace
static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  DLcWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&DLCwndTracer));
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  DLssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&DLSsThreshTracer));
}

static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  DLrttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&DLRttTracer));
}

static void
TraceAck (std::string &ack_file_name)
{
  AsciiTraceHelper ascii;
  DLackStream = ascii.CreateFileStream (ack_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/HighestRxAck", MakeCallback (&DLAckTracer));
}

static void
TraceCongState (std::string &cong_state_file_name)
{
  AsciiTraceHelper ascii;
  DLcongStateStream = ascii.CreateFileStream (cong_state_file_name.c_str ());
	Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/CongState", MakeCallback (&DLCongStateTracer));
}

// UL trace
static void
ULTraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  ULcWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/4/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&ULCwndTracer));
}

static void
ULTraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ULssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/4/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&ULSsThreshTracer));
}

static void
ULTraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  ULrttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/4/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&ULRttTracer));
}

static void
ULTraceAck (std::string &ack_file_name)
{
  AsciiTraceHelper ascii;
  ULackStream = ascii.CreateFileStream (ack_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/4/$ns3::TcpL4Protocol/SocketList/0/HighestRxAck", MakeCallback (&ULAckTracer));
}

static void
ULTraceCongState (std::string &cong_state_file_name)
{
  AsciiTraceHelper ascii;
  ULcongStateStream = ascii.CreateFileStream (cong_state_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/4/$ns3::TcpL4Protocol/SocketList/0/CongState", MakeCallback (&ULCongStateTracer));
}


// Callback function to handle packet reception
void ULReceivePacket(Ptr<const Packet> ULpacket, const Address &)
{
    totalBytesReceived_ul += ULpacket->GetSize();
}

void DLReceivePacket(Ptr<const Packet> DLpacket, const Address &)
{
    totalBytesReceived_dl += DLpacket->GetSize();
}


void CalculateDLThroughput(std::string &DL_tp_file_name)
{
    Time now = Simulator::Now();
    double elapsedTime = now.GetSeconds() - simulationStartTime.GetSeconds();

    double throughputDlMbps = (totalBytesReceived_dl * 8.0) / (elapsedTime * 1024 * 1024);
    double averageThroughputDlMbps = (totalBytesReceived_dl * 8.0) / (now.GetSeconds() * 1024 * 1024);

    // Write to console
    std::cout << "Time: " << now.GetSeconds() << "s"
              << "\tDownlink Throughput: " << throughputDlMbps << " Mbps"
              << "\tAverage Throughput: " << averageThroughputDlMbps << " Mbps" << std::endl;

    AsciiTraceHelper asciiTraceHelper;
    dlThroughputStream = asciiTraceHelper.CreateFileStream(DL_tp_file_name.c_str ());

    // Write to file
    *dlThroughputStream->GetStream() << now.GetSeconds() << "\t" << throughputDlMbps << "\t" 
                                     << averageThroughputDlMbps << std::endl;

    Simulator::Schedule(Seconds(1.0), &CalculateDLThroughput, DL_tp_file_name);
}


void CalculateULThroughput(std::string &UL_tp_file_name)
{
    Time now = Simulator::Now();
    double elapsedTime = now.GetSeconds() - simulationStartTime.GetSeconds();

    double throughputUlMbps = (totalBytesReceived_ul * 8.0) / (elapsedTime * 1024 * 1024);
    double averageThroughputUlMbps = (totalBytesReceived_ul * 8.0) / (now.GetSeconds() * 1024 * 1024);

    // Write to console
    std::cout << "Time: " << now.GetSeconds() << "s"
              << "\tUplink Throughput: " << throughputUlMbps << " Mbps"
              << "\tAverage Throughput: " << averageThroughputUlMbps << " Mbps" << std::endl;

    AsciiTraceHelper asciiTraceHelper;
    ulThroughputStream = asciiTraceHelper.CreateFileStream(UL_tp_file_name.c_str ());
    // Write to file
    *ulThroughputStream->GetStream() << now.GetSeconds() << "\t" << throughputUlMbps << "\t" 
                                     << averageThroughputUlMbps << std::endl;
    // Schedule the next throughput calculation
    Simulator::Schedule(Seconds(1.0), &CalculateULThroughput, UL_tp_file_name);
}



class MyAppTag : public Tag
{
  public:
    MyAppTag()
    {
    }

    MyAppTag(Time sendTs)
        : m_sendTs(sendTs)
    {
    }

    static TypeId GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::MyAppTag").SetParent<Tag>().AddConstructor<MyAppTag>();
        return tid;
    }

    virtual TypeId GetInstanceTypeId(void) const
    {
        return GetTypeId();
    }

    virtual void Serialize(TagBuffer i) const
    {
        i.WriteU64(m_sendTs.GetNanoSeconds());
    }

    virtual void Deserialize(TagBuffer i)
    {
        m_sendTs = NanoSeconds(i.ReadU64());
    }

    virtual uint32_t GetSerializedSize() const
    {
        return sizeof(m_sendTs);
    }

    virtual void Print(std::ostream& os) const
    {
        std::cout << m_sendTs;
    }

    Time m_sendTs;
};

class MyApp : public Application
{
  public:
    MyApp();
    virtual ~MyApp();
    void ChangeDataRate(DataRate rate);
    void Setup(Ptr<Socket> socket,
               Address address,
               uint32_t packetSize,
               uint32_t nPackets,
               DataRate dataRate);

  private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);

    void ScheduleTx(void);
    void SendPacket(void);

    Ptr<Socket> m_socket;
    Address m_peer;
    uint32_t m_packetSize;
    uint32_t m_nPackets;
    DataRate m_dataRate;
    EventId m_sendEvent;
    bool m_running;
    uint32_t m_packetsSent;
};

MyApp::MyApp()
    : m_socket(0),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0)
{
}

MyApp::~MyApp()
{
    m_socket = 0;
}

void
MyApp::Setup(Ptr<Socket> socket,
             Address address,
             uint32_t packetSize,
             uint32_t nPackets,
             DataRate dataRate)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
}

void
MyApp::ChangeDataRate(DataRate rate)
{
    m_dataRate = rate;
}

void
MyApp::StartApplication(void)
{
    m_running = true;
    m_packetsSent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void
MyApp::StopApplication(void)
{
    m_running = false;

    if (m_sendEvent.IsPending())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void
MyApp::SendPacket(void)
{
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    MyAppTag tag(Simulator::Now());

    m_socket->Send(packet);
    if (++m_packetsSent < m_nPackets)
    {
        ScheduleTx();
    }
}

void
MyApp::ScheduleTx(void)
{
    if (m_running)
    {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &MyApp::SendPacket, this);
    }
}

void
ChangeSpeed(Ptr<Node> n, Vector speed)
{
    n->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(speed);
}

int
main(int argc, char* argv[])
{
    /*
     * scenario 1: 1 building;
     * scenario 2: 3 building;
     * scenario 3: 6 random located small building, simulate tree and human blockage.
     * */
    int scenario = 2;
    double stopTime = 4.0;
    double simStopTime = 4.5;
    bool harqEnabled = true;
    // bool rlcAmEnabled = true;
    // bool harqEnabled = false;
    bool rlcAmEnabled = false;
    bool tcp = true;
    bool sack = true;
    bool enableUL = true;
    std::string transport_prot = "TcpYeah";
    std::string prefix_file_name = "TcpVariantsComparison";
    std::string queue_disc_type = "ns3::PfifoFastQueueDisc";
    std::string recovery = "ns3::TcpClassicRecovery";

    CommandLine cmd;
    cmd.AddValue("transport_prot",
                 "Transport protocol to use: TcpNewReno, TcpLinuxReno, "
                 "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                 "TcpBic, TcpYeah, TcpIllinois, TcpWestwoodPlus, TcpLedbat, "
                 "TcpLp, TcpDctcp, TcpCubic, TcpBbr",
                 transport_prot);
    cmd.AddValue("prefix_name", "Prefix of output trace file", prefix_file_name);
    cmd.AddValue("simTime", "Total duration of the simulation [s])", simStopTime);
    cmd.AddValue("harq", "Enable Hybrid ARQ", harqEnabled);
    cmd.AddValue("rlcAm", "Enable RLC-AM", rlcAmEnabled);
    cmd.AddValue("enableUL", "Enable UL", enableUL);
    cmd.Parse(argc, argv);

    transport_prot = std::string("ns3::") + transport_prot;
    // TCP settings
    // Select TCP variant
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(transport_prot, &tcpTid),
                        "TypeId " << transport_prot << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TypeId::LookupByName(transport_prot)));

    // TCP settings
    // Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpCubic::GetTypeId()));
    Config::SetDefault("ns3::TcpL4Protocol::RecoveryType",
                       TypeIdValue(TypeId::LookupByName(recovery)));
    Config::SetDefault("ns3::TcpSocketBase::Sack", BooleanValue(sack));
    Config::SetDefault("ns3::TcpSocketBase::MinRto", TimeValue(MilliSeconds(200)));
    Config::SetDefault("ns3::Ipv4L3Protocol::FragmentExpirationTimeout", TimeValue(Seconds(0.2)));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(2500));
    Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(1));
    // Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(12500000)); // 大约 12.5MB
    // Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(12500000));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(131072 * 50));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(131072 * 50));

    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(1024 * 1024));
    Config::SetDefault("ns3::LteRlcUmLowLat::MaxTxBufferSize", UintegerValue(1024 * 1024));
    Config::SetDefault("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue(1024 * 1024));
    Config::SetDefault("ns3::MmWaveHelper::RlcAmEnabled", BooleanValue(rlcAmEnabled));
    Config::SetDefault("ns3::MmWaveHelper::HarqEnabled", BooleanValue(harqEnabled));
    Config::SetDefault("ns3::MmWaveFlexTtiMacScheduler::HarqEnabled", BooleanValue(true));
    Config::SetDefault("ns3::MmWaveFlexTtiMaxWeightMacScheduler::HarqEnabled", BooleanValue(true));
    Config::SetDefault("ns3::MmWaveFlexTtiMacScheduler::HarqEnabled", BooleanValue(true));
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(100.0)));
    // Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(10.0)));
    Config::SetDefault("ns3::LteRlcAm::PollRetransmitTimer", TimeValue(MilliSeconds(4.0)));
    Config::SetDefault("ns3::LteRlcAm::ReorderingTimer", TimeValue(MilliSeconds(2.0)));
    Config::SetDefault("ns3::LteRlcAm::StatusProhibitTimer", TimeValue(MilliSeconds(1.0)));
    Config::SetDefault("ns3::LteRlcAm::ReportBufferStatusTimer", TimeValue(MilliSeconds(4.0)));
    Config::SetDefault("ns3::LteRlcAm::MaxTxBufferSize", UintegerValue(20 * 1024 * 1024));

    // by default, isotropic antennas are used. To use the 3GPP radiation pattern instead, use the
    // <ThreeGppAntennaArrayModel> beware: proper configuration of the bearing and downtilt angles
    // is needed
    Config::SetDefault("ns3::PhasedArrayModel::AntennaElement",
                       PointerValue(CreateObject<IsotropicAntennaModel>()));

    Ptr<MmWaveHelper> mmwaveHelper = CreateObject<MmWaveHelper>();

    mmwaveHelper->SetChannelConditionModelType("ns3::BuildingsChannelConditionModel");
    mmwaveHelper->Initialize();
    // enables Hybrid Automatic Repeat Request (HARQ)
    mmwaveHelper->SetHarqEnabled(true);
    //Evolved Packet Core (EPC) helper
    Ptr<MmWavePointToPointEpcHelper> epcHelper = CreateObject<MmWavePointToPointEpcHelper>();
    mmwaveHelper->SetEpcHelper(epcHelper);
    // Packet Gateway (PGW) node
    Ptr<Node> pgw = epcHelper->GetPgwNode();

    // Create a single RemoteHost nodeis 2
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // TrafficControlHelper tchPfifo;
    // tchPfifo.SetRootQueueDisc("ns3::PfifoFastQueueDisc");

    // TrafficControlHelper tchCoDel;
    // tchCoDel.SetRootQueueDisc("ns3::CoDelQueueDisc");

    // Create the Internet
    PointToPointHelper p2ph;
    // DL
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("10Gb/s"))); // 100Gb/s 10Gb/s 6Gb/s
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010))); // 0.010 0.05
    // p2ph.SetChannelAttribute("Delay", TimeValue(MilliSeconds(100)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    // Assign IP address to the devices
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    // interface 0 is localhost, 1 is the p2p device
    Ipv4Address remoteHostAddr;
    remoteHostAddr = internetIpIfaces.GetAddress(1);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    switch (scenario)
    {
    case 1: {
        Ptr<Building> building;
        building = Create<Building>();
        building->SetBoundaries(Box(40.0, 60.0, 0.0, 6, 0.0, 15.0));
        break;
    }
    case 2: {
        Ptr<Building> building1;
        building1 = Create<Building>();
        building1->SetBoundaries(Box(60.0, 64.0, 0.0, 2.0, 0.0, 1.5));
        // building1->SetBoundaries(Box(65.0, 70.0, -1.0, 1.0, 0.0, 10.0));


        Ptr<Building> building2;
        building2 = Create<Building>();
        building2->SetBoundaries(Box(60.0, 64.0, 6.0, 8.0, 0.0, 15.0));

        Ptr<Building> building3;
        building3 = Create<Building>();
        building3->SetBoundaries(Box(60.0, 64.0, 10.0, 11.0, 0.0, 15.0));
        break;
    }
    case 3: {
        Ptr<Building> building1;
        building1 = Create<Building>();
        building1->SetBoundaries(Box(69.5, 70.0, 4.5, 5.0, 0.0, 1.5));

        Ptr<Building> building2;
        building2 = Create<Building>();
        building2->SetBoundaries(Box(60.0, 60.5, 9.5, 10.0, 0.0, 1.5));

        Ptr<Building> building3;
        building3 = Create<Building>();
        building3->SetBoundaries(Box(54.0, 54.5, 5.5, 6.0, 0.0, 1.5));
        Ptr<Building> building4;
        building1 = Create<Building>();
        building1->SetBoundaries(Box(60.0, 60.5, 6.0, 6.5, 0.0, 1.5));

        Ptr<Building> building5;
        building2 = Create<Building>();
        building2->SetBoundaries(Box(70.0, 70.5, 0.0, 0.5, 0.0, 1.5));

        Ptr<Building> building6;
        building3 = Create<Building>();
        building3->SetBoundaries(Box(50.0, 50.5, 4.0, 4.5, 0.0, 1.5));
        break;
        break;
    }
    default: {
        NS_FATAL_ERROR("Invalid scenario");
    }
    }

    NodeContainer ueNodes;
    NodeContainer enbNodes;
    enbNodes.Create(1);
    ueNodes.Create(1);

    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();
    enbPositionAlloc->Add(Vector(0.0, 0.0, 25.0));
    MobilityHelper enbmobility;
    enbmobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbmobility.SetPositionAllocator(enbPositionAlloc);
    enbmobility.Install(enbNodes);
    BuildingsHelper::Install(enbNodes);
    MobilityHelper uemobility;
    uemobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    uemobility.Install(ueNodes);

    ueNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(70, -2.0, 1.8));
    ueNodes.Get(0)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(Vector(0, 1.0, 0));

    Simulator::Schedule(Seconds(0.5), &ChangeSpeed, ueNodes.Get(0), Vector(0, 1.5, 0));
    Simulator::Schedule(Seconds(1.0), &ChangeSpeed, ueNodes.Get(0), Vector(0, 0, 0));

    BuildingsHelper::Install(ueNodes);

    // Install LTE Devices to the nodes
    NetDeviceContainer enbDevs = mmwaveHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueDevs = mmwaveHelper->InstallUeDevice(ueNodes);

    // Install the IP stack on the UEs
    // Assign IP address to UEs, and install applications
    internet.Install(ueNodes);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueDevs));

    mmwaveHelper->AttachToClosestEnb(ueDevs, enbDevs);
    mmwaveHelper->EnableTraces();


    // Set the default gateway for the UE
    Ptr<Node> ueNode = ueNodes.Get(0);
    Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4>());
    ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

    if (tcp)
    {
        // Install and start applications on UEs and remote host
        uint16_t sinkPort = 20000;

        Address sinkAddress(InetSocketAddress(ueIpIface.GetAddress(0), sinkPort));
        PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
                                          InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
        ApplicationContainer sinkApps = packetSinkHelper.Install(ueNodes.Get(0));

        sinkApps.Start(Seconds(0.));
        sinkApps.Stop(Seconds(simStopTime));

        Ptr<Socket> ns3TcpSocket =
            Socket::CreateSocket(remoteHostContainer.Get(0), TcpSocketFactory::GetTypeId());
        NS_LOG_UNCOND("Created socket on Node " << remoteHostContainer.Get(0)->GetId());

        Ptr<MyApp> app = CreateObject<MyApp>();
        app->Setup(ns3TcpSocket, sinkAddress, 1400, 5000000, DataRate("500Mb/s")); // 500Mb/s 5Gb/s 10Gb/s

        remoteHostContainer.Get(0)->AddApplication(app);
        sinkApps.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&DLReceivePacket));

        // std::ofstream ascii;
        // Ptr<OutputStreamWrapper> DLascii_wrap;
        // ascii.open(prefix_file_name + "-dl-ascii");
        // DLascii_wrap = new OutputStreamWrapper(prefix_file_name + "-dl-ascii", std::ios::out);
        // internet.EnableAsciiIpv4All(DLascii_wrap);

        // Simulator::Schedule (Seconds (0.00001), &TraceCwnd, prefix_file_name + "-dl-cwnd.data");
        // Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, prefix_file_name + "-dl-ssth.data");
        // Simulator::Schedule (Seconds (0.00001), &TraceRtt, prefix_file_name + "-dl-rtt.data");
        // Simulator::Schedule (Seconds (0.00001), &TraceAck, prefix_file_name + "-dl-ack.data");
        // Simulator::Schedule (Seconds (0.00001), &TraceCongState, prefix_file_name + "-dl-cong-state.data");

        app->SetStartTime(Seconds(0.1));
        // Schedule throughput calculation every second
        simulationStartTime = Seconds(0.1); // Set simulation start time
        app->SetStopTime(Seconds(stopTime));

        Simulator::Schedule(Seconds(0.5), &CalculateDLThroughput, prefix_file_name + "-dl-tp.data");

        //Uplink application
        if (enableUL){
            uint16_t ulSinkPort = 30000;
            Address ulSinkAddress(InetSocketAddress(remoteHostAddr, ulSinkPort));
            PacketSinkHelper ulPacketSinkHelper("ns3::TcpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), ulSinkPort));
            ApplicationContainer ulSinkApps = ulPacketSinkHelper.Install(remoteHostContainer.Get(0));
            ulSinkApps.Start(Seconds(0.));
            ulSinkApps.Stop(Seconds(simStopTime));

            Ptr<Socket> ulSocket = Socket::CreateSocket(ueNodes.Get(0), TcpSocketFactory::GetTypeId());
            NS_LOG_UNCOND("Created UL socket on Node " << ueNodes.Get(0)->GetId());
            Ptr<MyApp> ulApp = CreateObject<MyApp>();
            ulApp->Setup(ulSocket, ulSinkAddress, 1400, 100000, DataRate("100Mb/s"));
            ueNodes.Get(0)->AddApplication(ulApp);
            ulSinkApps.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&ULReceivePacket));
            ulApp->SetStartTime(Seconds(0.1));
            ulApp->SetStopTime(Seconds(stopTime));

            // Tracing
            // Ptr<OutputStreamWrapper> ULascii_wrap;
            // ascii.open(prefix_file_name + "-ul-ascii");
            // ULascii_wrap = new OutputStreamWrapper(prefix_file_name + "-ul-ascii", std::ios::out);
            // internet.EnableAsciiIpv4All(ULascii_wrap);

            // Simulator::Schedule (Seconds (0.00001), &ULTraceCwnd, prefix_file_name + "-ul-cwnd.data");
            // Simulator::Schedule (Seconds (0.00001), &ULTraceSsThresh, prefix_file_name + "-ul-ssth.data");
            // Simulator::Schedule (Seconds (0.00001), &ULTraceRtt, prefix_file_name + "-ul-rtt.data");
            // Simulator::Schedule (Seconds (0.00001), &ULTraceAck, prefix_file_name + "-ul-ack.data");
            // Simulator::Schedule (Seconds (0.00001), &ULTraceCongState, prefix_file_name + "-ul-cong-state.data");

            Simulator::Schedule(Seconds(0.5), &CalculateULThroughput, prefix_file_name + "-ul-tp.data");
        }
    }
    // else
    // {
    //     // Install and start applications on UEs and remote host
    //     uint16_t sinkPort = 20000;

    //     Address sinkAddress(InetSocketAddress(ueIpIface.GetAddress(0), sinkPort));
    //     PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory",
    //                                       InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    //     ApplicationContainer sinkApps = packetSinkHelper.Install(ueNodes.Get(0));

    //     sinkApps.Start(Seconds(0.));
    //     sinkApps.Stop(Seconds(simStopTime));

    //     Ptr<Socket> ns3UdpSocket =
    //         Socket::CreateSocket(remoteHostContainer.Get(0), UdpSocketFactory::GetTypeId());
    //     Ptr<MyApp> app = CreateObject<MyApp>();
    //     app->Setup(ns3UdpSocket, sinkAddress, 1400, 5000000, DataRate("500Mb/s"));

    //     remoteHostContainer.Get(0)->AddApplication(app);
    //     AsciiTraceHelper asciiTraceHelper;
    //     Ptr<OutputStreamWrapper> stream2 =
    //         asciiTraceHelper.CreateFileStream("mmWave-udp-data-am.txt");
    //     sinkApps.Get(0)->TraceConnectWithoutContext("Rx", MakeBoundCallback(&Rx, stream2));

    //     app->SetStartTime(Seconds(0.1));
    //     app->SetStopTime(Seconds(stopTime));
    // }

    // p2ph.EnablePcapAll("mmwave-sgi-capture");
    // Config::Set("/NodeList/*/DeviceList/*/TxQueue/MaxSize", QueueSizeValue(QueueSize("1000p"))); //100000 packets 100p
    // data-udl2
    // Config::Set("/NodeList/*/DeviceList/*/TxQueue/MaxSize", QueueSizeValue(QueueSize("100000p"))); //100000 packets 100p

     
    // Configure downlink queue size (Node 0 to Node 1)
    Config::Set("/NodeList/2/DeviceList/*/TxQueue/MaxSize", QueueSizeValue(QueueSize("100000p"))); // Large DL queue

    // Configure uplink queue size (Node 1 to Node 0)
    Config::Set("/NodeList/4/DeviceList/*/TxQueue/MaxSize", QueueSizeValue(QueueSize("100p"))); // Small UL queue


    Simulator::Stop(Seconds(simStopTime));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
