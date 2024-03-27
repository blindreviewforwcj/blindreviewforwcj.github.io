#include <string.h>
#include <string>
#include <omnetpp.h>
#include <sstream>
#include <random>
    #include <omnetpp.h>

    using namespace omnetpp;

    using namespace std;

    class RainPrecipitation : public cSimpleModule
    {
    public:
        RainPrecipitation();
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


    Define_Module(RainPrecipitation);


    RainPrecipitation::RainPrecipitation() {
        lambda = 0.5;
    }

    void RainPrecipitation::initialize()
    {
        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);
    }
    void RainPrecipitation::handleMessage(cMessage *msg)
    {
        double threshold = 0.5; // Set the threshold for probabilistic true

        if (uniform(0, 1) < threshold) {
            pld=0;

            // Set the minimum and maximum frequency range
            double minFrequency = par("minFrequency");
            double maxFrequency = par("maxFrequency");

            // Generate a random frequency within the specified range
            double randomFrequency = uniform(minFrequency, maxFrequency);

            transmit(pld);
            TRIGGER = new cMessage();
            scheduleAt(simTime() + randomFrequency, TRIGGER);

        }else{
            pld=intuniform(1, 100);

            transmit(pld);
            TRIGGER = new cMessage();
            double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
            scheduleAt(simTime() + interval, TRIGGER);
        }


    }

    void RainPrecipitation::transmit(int pld)
    {
        std::string result = std::string("TRANSMIT/RP/") + std::string(std::to_string(pld));
        TRANSMIT = new cMessage(result.c_str());
        send(TRANSMIT, "data"); // send out the message
    }
