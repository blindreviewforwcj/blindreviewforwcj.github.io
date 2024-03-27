#include <string.h>
#include <string>
#include <omnetpp.h>
#include <sstream>
#include <random>
    using namespace omnetpp;

    /**
     * Derive the Txc1 class from cSimpleModule. In the Tictoc1 network,
     * both the `tic' and `toc' modules are Txc1 objects, created by OMNeT++
     * at the beginning of the simulation.
     */
    class WaterVolume : public cSimpleModule
    {
    public:
        WaterVolume();
    private:
        cMessage*  TRIGGER;
        cMessage*  TRANSMIT;
        int pld; //generated payload
        double lambda;  // Rate parameter for the exponential distribution
        std::default_random_engine generator;
        std::exponential_distribution<double> distribution;
        std::vector<std::string> extractSubstrings(const std::string& inputString);
        void transmit(int pld);
protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

    };

    // The module class needs to be registered with OMNeT++
    Define_Module(WaterVolume);


    WaterVolume::WaterVolume() {
        lambda = 0.5;
    }

    void WaterVolume::initialize()
    {
        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);
    }
    void WaterVolume::handleMessage(cMessage *msg)
    {
        pld=intuniform(0, 100);
        transmit(pld);

        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);

    }

    void WaterVolume::transmit(int pld)
    {
        std::string result = std::string("TRANSMIT/WV/") + std::string(std::to_string(pld));
        TRANSMIT = new cMessage(result.c_str());
        send(TRANSMIT, "data"); // send out the message
    }
