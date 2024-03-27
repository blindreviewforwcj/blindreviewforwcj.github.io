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
    class Fog : public cSimpleModule
    {
    public:
        Fog();
    private:
        cMessage*  TRIGGER;
        cMessage*  PUSH;
        cMessage*  TRANSMIT;

        double lambda;  // Rate parameter for the exponential distribution
        std::default_random_engine generator;
        std::exponential_distribution<double> distribution;
        std::vector<std::string> extractSubstrings(const std::string& inputString);
        void get();
        void transmit(int pld);
   protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

    };

    // The module class needs to be registered with OMNeT++
    Define_Module(Fog);


    Fog::Fog() {
        lambda = 0.5;
    }

    void Fog::initialize()
    {
        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);
    }

    void Fog::handleMessage(cMessage *msg)
    {
        EV << "Received  " << msg->getName()  << endl;
        std::string s = msg->getName();

        bool isFound = s.find("FOG") != std::string::npos;
        if(isFound){
            std::vector<std::string> extractedSubstrings = extractSubstrings( msg->getName() );

            std::string PLD = extractedSubstrings[2];
            int WL=std::stoi(PLD);

            PLD = extractedSubstrings[4];
            int RP=std::stoi(PLD);

            PLD = extractedSubstrings[6];
            int WV=std::stoi(PLD);


            int WF= (WL+RP+WV)/3;

            if(WL==-1 || RP==-1 || WV==-1){
                WF=-1;
            }
            transmit(WF);
        }else{
            get();
        }




        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);
    }

    void Fog::transmit(int pld)
    {
        std::string result = std::string("SETWF/") + std::string(std::to_string(pld));
        TRANSMIT = new cMessage(result.c_str());
        send(TRANSMIT, "fogToData"); // send out the message
    }

    void Fog::get()
    {
        TRANSMIT = new cMessage("GET");
        send(TRANSMIT, "fogToData"); // send out the message
    }

    std::vector<std::string> Fog::extractSubstrings(const std::string& inputString) {
        std::vector<std::string> substrings;
        std::istringstream iss(inputString);
        std::string substring;
        while (std::getline(iss, substring, '/')) {
            substrings.push_back(substring);
        }
        return substrings;
    }
