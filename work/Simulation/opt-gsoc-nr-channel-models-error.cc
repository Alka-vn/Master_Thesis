// Copyright (c) 2024 LASSE / Universidade Federal do Pará (UFPA)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/command-line.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/isotropic-antenna-model.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/nr-helper.h"
#include "ns3/nr-module.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/parabolic-antenna-model.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/pointer.h"
#include "ns3/traffic-generator-helper.h"
#include "ns3/traffic-generator-ngmn-gaming.h"
#include "ns3/udp-client-server-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("GsocNrChannelModels");

/**
 * @ingroup examples
 * @file gsoc-nr-channel-models.cc
 * @brief A simple NR example demonstrating the simulation of different spectrum channels.
 * This example showcases configuration of the spectrum channel with antenna and propagation
 * models that explicitly model multiple antenna elements (phased arrays), and with antenna
 * and propagation models that abstract away the individual elements (called 'non-phased' herein).
 * The 3GPP propagation models require Uniform Planar Array and propagation models of the
 * PhasedArraySpectrumPropagationLossModel type. Other propagation models are designed
 * to use antenna models without explicit array configuration (e.g., ParabolicAntennaModel)
 * and with propagation models (e.g., TraceFadingLossModel) that do not take into account the
 * explicit array configuration. This comment is a reminder that propagation models are
 * also dependent on the antenna type.
 *
 * In this example, the user can simulate a phased-array channel or the Friis model
 * (non-phased array model). By default, the example uses the 3GPP channel model with the default
 * channel condition and Urban Macro scenario. When selecting to simulate using the Friis model, the
 * ParabolicAntennaModel will be used as the antenna type.
 *
 * The simulation generates multiple text files containing flow statistics and pathloss traces.
 * Each SpectrumChannel produces distinct pathloss traces, which may or may not affect the
 * statistical results.
 *
 *
 * [ADDED]
 *  * This script sets up a 5G New Radio (NR) simulation environment to evaluate the impact
 * of different wireless channel models on network performance. Key functionalities include:
 *
 * - **Scenario Setup:** Creates a configurable number of User Equipments (UEs) and
 *   gNodeBs (gNBs) arranged in a hexagonal grid layout. UEs are configured with
 *   constant velocity mobility.
 * - **Channel Model Selection:** Allows users to specify various channel models via
 *   command-line arguments, including 'ThreeGpp', 'NYU', 'TwoRay', and 'Friis'.
 *   This enables comparison between models that explicitly support phased antenna arrays
 *   (e.g., 3GPP with UPA antennas) and simpler models (e.g., Friis with Parabolic antennas).
 *   Specific channel conditions like Line-of-Sight (LOS) or Non-Line-of-Sight (NLOS)
 *   can also be selected for applicable models.
 * - **NR Stack Configuration:** Installs the NR protocol stack on UEs and gNBs,
 *   configures physical layer parameters (e.g., frequency, bandwidth, numerology, Tx power),
 *   and sets up antenna configurations based on the chosen channel model.
 * - **Core Network Connectivity:** Utilizes a simplified Evolved Packet Core (EPC) helper
 *   to provide IP connectivity for UEs and connects the gNBs to a simulated remote host
 *   via a point-to-point link, representing an external server.
 * - **Traffic Generation:** Implements UDP downlink traffic from the remote host to each UE.
 *   Each UE runs a UDP server, and the remote host runs UDP clients sending a stream of
 *   packets at a fixed interval.
 * - **Performance Monitoring & Tracing:**
 *     - Employs the FlowMonitor module to collect and report key performance indicators (KPIs)
 *       such as throughput, packet loss, delay, and jitter for each data flow. These
 *       statistics are written to "channels-example-flows.txt".
 *     - Enables detailed NR trace generation, including pathloss (`Pathloss.txt`),
 *       uplink SINR (`UlCtrlSinr.txt`, `UlDataSinr.txt`), downlink SINR (`DlCtrlSinr.txt`,
 *       `DlDataSinr.txt`), and CQI reports (`DlCqi.txt`, `UlCqi.txt`).
 *       *(Note: The `nrHelper->EnableTraces()` call in the current code primarily enables
 *       UL SINR traces; specific DL SINR/CQI traces would require `EnableDlSinrTraces()`
 *       and `EnableDlCqiTraces()` respectively.)*
 * - **Output & Logging:** Provides console output indicating simulation progress and
 *   parameter settings. Supports optional detailed ns-3 logging.
 *
 *
 * @note This example was produced during the Google Summer of Code 2024 program. The main author is
 * João Albuquerque, under the supervision of Biljana Bojovic, Amir Ashtari, Gabriel Ferreira, in
 * project: 5G NR Module Benchmark and Analysis for Distinct Channel Models
 *
 * <joao.barbosa.albuquerque@itec.ufpa.br>
 */

int
main(int argc, char* argv[])
{
    int64_t randomStream = 1;
    uint32_t rngSeed = 1;
    uint32_t rngRun = 1;

    double centralFrequency = 30.5e9;               // 30.5 GHz
    double bandwidth = 100e6;                       // 100 MHz
    Time simTime = Seconds(10.0);                   // 1 second simulation time
    Time udpTime = MilliSeconds(0);                 // 0 ms
    Time maxDelay = MilliSeconds(100);              // 100 ms
    std::string scenario = "UMa";                   // Urban Macro
    std::string channelModel = "ThreeGpp";          // 3GPP channel model
    uint32_t numUes = 4;                            // Number of UEs
    uint32_t numGnbs = 1;                           // Number of gNBs
    bool logging = true;                            // Enable logging
    uint16_t numerology = 1;                        // Numerology
    std::string errorModelType = "ns3::NrEesmCcT1"; // Default error model
    std::string amcSelectionModel = "ErrorModel";   // "ErrorModel" or "ShannonModel"
    printf("Starting GSoC NR Channel Models Example\n");
    /**
     * Default channel condition model: This model varies based on the selected scenario.
     * For instance, in the Urban Macro scenario, the default channel condition model is
     * the ThreeGppUMaChannelConditionModel.
     */
    std::string channelConditionModel = "Default";
    // Output file with the statistics
    CommandLine cmd(__FILE__);
    // cmd.Usage(""); Leave it empty until we decide the final example
    cmd.AddValue("seed", "RNG seed value (default=1)", rngSeed);
    cmd.AddValue("run", "RNG run number (default=1)", rngRun);

    cmd.AddValue("channelModel",
                 "The channel model for the simulation, which can be 'NYU', "
                 "'ThreeGpp', 'TwoRay', 'Friis'. ",
                 channelModel);
    cmd.AddValue("channelConditionModel",
                 "The channel condition model for the simulation. Choose among 'Default', 'LOS',"
                 "'NLOS', 'Buildings'.",
                 channelConditionModel);
    cmd.AddValue("ueNum", "Number of UEs in the simulation.", numUes);
    cmd.AddValue("gNbNum", "Number of gNBs in the simulation.", numGnbs);
    cmd.AddValue("frequency", "The central carrier frequency in Hz.", centralFrequency);
    cmd.AddValue("logging", "Enable logging", logging);
    cmd.Parse(argc, argv);
    printf("Channel model: %s\n", channelModel.c_str());
    printf("Channel condition model: %s\n", channelConditionModel.c_str());
    printf("Number of UEs: %u\n", numUes);
    printf("Number of gNBs: %u\n", numGnbs);
    printf("Central frequency: %.2f GHz\n", centralFrequency / 1e9);
    cmd.AddValue("errorModelType",
                 "NR Error Model Type (e.g., ns3::NrEesmCcT1, ns3::NrLteMiErrorModel)",
                 errorModelType);
    cmd.AddValue("amcSelectionModel",
                 "AMC selection logic: ErrorModel or ShannonModel",
                 amcSelectionModel);
    if (logging)
    {
        LogComponentEnable("GsocNrChannelModels", LOG_LEVEL_INFO);
    }

    // Create the simulated scenario
    HexagonalGridScenarioHelper hexGrid;
    /**
     * Set the scenario parameters for the simulation, considering the UMa scenario.
     * Following the TR 38.901 specification - Table 7.4.1-1 pathloss models.
     * hBS = 25m for UMa scenario.
     * hUT = 1.5m for UMa scenario.
     */
    hexGrid.SetUtHeight(1.5);    // Height of the UE in meters
    hexGrid.SetBsHeight(25);     // Height of the gNB in meters
    hexGrid.SetSectorization(1); // Number of sectors
    hexGrid.m_isd = 200;         // Inter-site distance in meters
    uint32_t ueTxPower = 23;     // UE transmission power in dBm
    uint32_t bsTxPower = 41;     // gNB transmission power in dBm
    double ueSpeed = 30;         // in m/s (3 km/h)
    // Antenna parameters
    uint32_t ueNumRows = 1;  // Number of rows for the UE antenna
    uint32_t ueNumCols = 1;  // Number of columns for the UE antenna
    uint32_t gnbNumRows = 4; // Number of rows for the gNB antenna
    uint32_t gnbNumCols = 8; // Number of columns for the gNB antenna
    // Set the number of UEs and gNBs nodes in the scenario
    hexGrid.SetUtNumber(numUes);  // Number of UEs
    hexGrid.SetBsNumber(numGnbs); // Number of gNBs
    // Create a scenario with mobility
    hexGrid.CreateScenarioWithMobility(Vector(ueSpeed, 0.0, 0.0),
                                       0); // move UE with 3 km/h in x-axis

    auto ueNodes = hexGrid.GetUserTerminals();
    auto gNbNodes = hexGrid.GetBaseStations();

    NS_LOG_INFO("Number of UEs: " << ueNodes.GetN() << ", Number of gNBs: " << gNbNodes.GetN());
    for (size_t ueIndex = 0; ueIndex < ueNodes.GetN(); ueIndex++)
    {
        Vector3D position(10.0, 20.0, 1.5);
        NS_LOG_INFO("UE [" << ueNodes.Get(ueIndex) << "] at "
                           << ueNodes.Get(ueIndex)->GetObject<MobilityModel>()->GetPosition());

        if (ueIndex > 0)
        {
            position.x = 50.0 * ueIndex;
            position.y = 30.0 * ((ueIndex % 2 == 0) ? 1 : -1);
        }
        Ptr<MobilityModel> mob = ueNodes.Get(ueIndex)->GetObject<MobilityModel>();
        mob->SetPosition(position);
        printf("UE [%zu] position set to (%.2f, %.2f, %.2f)\n",
               ueIndex,
               position.x,
               position.y,
               position.z);
    }
    printf("hex grid setup completed\n");
    for (size_t ueIndex = 0; ueIndex < ueNodes.GetN(); ueIndex++)
    {
        Ptr<ConstantVelocityMobilityModel> mob =
            ueNodes.Get(ueIndex)->GetObject<ConstantVelocityMobilityModel>();

        double speed = 1.0 + ueIndex * 3.0; // 1 m/s, 4 m/s, 7 m/s, etc.
        mob->SetVelocity(Vector(speed, (ueIndex % 2 == 0 ? 1 : -1) * speed, 0)); // zigzag movement
    }
    /*
     * Setup the NR module:
     * - NrHelper, which takes care of creating and connecting the various
     * part of the NR stack
     * - NrChannelHelper, which takes care of the spectrum channel
     */

    Config::SetDefault("ns3::NrAmc::ErrorModelType",
                       TypeIdValue(TypeId::LookupByName(errorModelType)));
    if (amcSelectionModel == "ErrorModel")
    {
        Config::SetDefault("ns3::NrAmc::AmcModel", EnumValue(NrAmc::ErrorModel));
    }
    else if (amcSelectionModel == "ShannonModel")
    {
        Config::SetDefault("ns3::NrAmc::AmcModel", EnumValue(NrAmc::ShannonModel));
    }
    else
    {
        NS_FATAL_ERROR("Invalid amcSelectionModel: " << amcSelectionModel);
    }
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999)); // Good to have

    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    nrHelper->SetEpcHelper(epcHelper);

    uint8_t numCc = 1; // Number of component carriers
    CcBwpCreator ccBwpCreator;
    auto band = ccBwpCreator.CreateOperationBandContiguousCc({centralFrequency, bandwidth, numCc});

    if (channelModel == "ThreeGpp" || channelModel == "NYU" || channelModel == "TwoRay")
    {
        // Create the ideal beamforming helper in case of a non-phased array model
        Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
        nrHelper->SetBeamformingHelper(idealBeamformingHelper);
        // First configure the channel helper object factories
        channelHelper->ConfigureFactories(scenario, channelConditionModel, channelModel);
        // Enable slow fading (shadowing)
        channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(true));
        // Optional: exaggerate it
        // channelHelper->SetPathlossAttribute("ShadowSigma", DoubleValue(10.0));
        // Set channel condition attributes
        if (channelConditionModel == "Default" || channelConditionModel == "Buildings")
        {
            channelHelper->SetChannelConditionModelAttribute("UpdatePeriod",
                                                             TimeValue(MilliSeconds(100)));
        }
        // Beamforming method
        idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                             TypeIdValue(DirectPathBeamforming::GetTypeId()));

        // Antennas for all the UEs
        nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(ueNumRows));
        nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(ueNumCols));
        nrHelper->SetUeAntennaAttribute("AntennaElement",
                                        PointerValue(CreateObject<IsotropicAntennaModel>()));

        // Antennas for all the gNbs
        nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(gnbNumRows));
        nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(gnbNumCols));
        nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                         PointerValue(CreateObject<IsotropicAntennaModel>()));
    }
    else if (channelModel == "Friis")
    {
        // Override the default antenna model with ParabolicAntennaModel
        nrHelper->SetUeAntennaTypeId(ParabolicAntennaModel::GetTypeId().GetName());
        nrHelper->SetGnbAntennaTypeId(ParabolicAntennaModel::GetTypeId().GetName());
        // Configure Friis propagation loss model before assign it to band
        channelHelper->ConfigurePropagationFactory(FriisPropagationLossModel::GetTypeId());
    }
    else
    {
        NS_FATAL_ERROR("Invalid channel model: "
                       << channelModel << ". Choose among 'ThreeGpp', 'NYU', 'TwoRay', 'Friis'.");
    }

    // After configuring the factories, create and assign the spectrum channels to the bands
    channelHelper->AssignChannelsToBands({band});
    printf("Spectrum channel created and assigned to the band\n");

    // Get all the BWPs
    auto allBwps = CcBwpCreator::GetAllBwps({band});
    // Set the numerology and transmission powers attributes to all the gNBs and UEs
    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(bsTxPower));
    nrHelper->SetGnbPhyAttribute("Numerology", UintegerValue(numerology));
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(ueTxPower));
    printf("Attributes set for gNBs and UEs\n");
    // Scheduler: Ensure AMC is active, not fixed MCS
    nrHelper->SetSchedulerAttribute("FixedMcsDl", BooleanValue(false));
    nrHelper->SetSchedulerAttribute("FixedMcsUl", BooleanValue(false));

    // Error Model: Apply to UEs and gNBs
    nrHelper->SetUlErrorModel(errorModelType);
    nrHelper->SetDlErrorModel(errorModelType);

    // AMC Model: Ensure gNB uses the chosen AMC logic for DL and UL scheduling
    if (amcSelectionModel == "ErrorModel")
    {
        nrHelper->SetGnbDlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));
        nrHelper->SetGnbUlAmcAttribute("AmcModel", EnumValue(NrAmc::ErrorModel));
        // UEs will also use this for CQI reporting by default matching NrAmc global default
    }
    else if (amcSelectionModel == "ShannonModel")
    {
        nrHelper->SetGnbDlAmcAttribute("AmcModel", EnumValue(NrAmc::ShannonModel));
        nrHelper->SetGnbUlAmcAttribute("AmcModel", EnumValue(NrAmc::ShannonModel));
    }
    // Install and get the pointers to the NetDevices
    NetDeviceContainer gNbNetDev = nrHelper->InstallGnbDevice(gNbNodes, allBwps);
    NetDeviceContainer ueNetDev = nrHelper->InstallUeDevice(ueNodes, allBwps);

    randomStream += nrHelper->AssignStreams(gNbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);
    printf("NetDevices installed and streams assigned\n");
    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    Ptr<Node> remoteHost = CreateObject<Node>();
    InternetStackHelper internet;
    internet.Install(remoteHost);
    printf("Internet stack installed on remote host\n");
    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);

    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;

    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(ueNodes);

    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));
    printf("IPv4 addresses assigned to UEs\n");
    // assign IP address to UEs, and install UDP downlink applications
    uint16_t dlPort = 1234;
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
    for (size_t i = 0; i < ueNodes.GetN(); i++)
    {
        TrafficGeneratorHelper trafficHelper("ns3::UdpSocketFactory", // or "ns3::TcpSocketFactory"
                                             InetSocketAddress(ueIpIface.GetAddress(i), dlPort),
                                             TrafficGeneratorNgmnGaming::GetTypeId());

        clientApps.Add(trafficHelper.Install(remoteHost));
    }
    // attach UEs to the closest eNB
    nrHelper->AttachToClosestGnb(ueNetDev, gNbNetDev);
    // start UDP server and client apps
    serverApps.Start(udpTime);
    clientApps.Start(udpTime);
    serverApps.Stop(simTime);
    clientApps.Stop(simTime);
    printf("Gaming applications started\n");
    // Check pathloss traces
    nrHelper->EnableDlDataPhyTraces();
    nrHelper->EnableDlMacSchedTraces();
    nrHelper->EnableGnbMacCtrlMsgsTraces();
    nrHelper->EnablePathlossTraces();

    Simulator::Stop(simTime);
    RngSeedManager::SetSeed(rngSeed); // Changes the base seed
    RngSeedManager::SetRun(rngRun);   // Changes the run number

    // Measure simulation runtime
    auto simStart = std::chrono::high_resolution_clock::now();
    Simulator::Run();
    auto simEnd = std::chrono::high_resolution_clock::now();
    auto simDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(simEnd - simStart).count();

    std::cout << "\n🕒 Simulation runtime: " << simDuration << " ms (" << simDuration / 1000.0
              << " seconds)" << std::endl;

    Simulator::Destroy();
    printf("Simulation completed\n");

    return 0;
}
