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
    class Southbound : public cSimpleModule
    {
    public:
        Southbound();
    private:
        cMessage*  TRIGGER;
        cMessage*  TRANSMIT;

        std::string rkWL = "WL";
        std::string rkRP = "RP";
        std::string rkWV = "WV";

        int pld; //generated payload
        double lambda;  // Rate parameter for the exponential distribution
        std::default_random_engine generator;
        std::exponential_distribution<double> distribution;
        std::vector<std::string> extractSubstrings(const std::string& inputString);
        void transmit(int pld,const std::string& inputString);
        void transmitWF(int pld);
        void get();
   protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

    };

    // The module class needs to be registered with OMNeT++
    Define_Module(Southbound);


    Southbound::Southbound() {
        lambda = 0.5;
    }

    void Southbound::initialize()
    {
        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);
    }
    void Southbound::handleMessage(cMessage *msg)
    {
        EV << "Received " << msg->getName()  << endl;
        std::string s = msg->getName();

        bool isFound = s.find("TRANSMIT") != std::string::npos;

        if(isFound){

            bool sensorWV = s.find("WV") != std::string::npos;
            bool sensorWL = s.find("WL") != std::string::npos;
            bool sensorRP = s.find("RP") != std::string::npos;

            std::vector<std::string> extractedSubstrings = extractSubstrings( msg->getName());
            std::string PLD = extractedSubstrings[2];
            int pld = std::stoi(PLD);
            if(sensorWV){
                transmit(pld,rkWV);
            }else{
                if(sensorWL){
                    transmit(pld,rkWL);
                }
                    else{
                        if(sensorRP){
                            transmit(pld,rkRP);
                        }else{
                           // attacker
                        }
                    }

                }
        }else{
            bool isFound = s.find("WF") != std::string::npos;

            if(isFound){
                std::vector<std::string> extractedSubstrings = extractSubstrings( msg->getName());
                std::string PLD = extractedSubstrings[1];
                int pld = std::stoi(PLD);
                transmitWF(pld);
            }else{
                get();
            }

        }





        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);

    }

    void Southbound::transmit(int pld, const std::string& routingkey)
    {
        std::string result = std::string("TRANSMIT/") + routingkey + "/"+std::string(std::to_string(pld));
        TRANSMIT = new cMessage(result.c_str());
        send(TRANSMIT, "data"); // send out the message
    }

    void Southbound::get()
    {
        TRANSMIT = new cMessage("SOUTHBOUNDWF");
        send(TRANSMIT, "data"); // send out the message
    }
    void Southbound::transmitWF(int pld)
    {
        std::string result = std::string("WF/") + std::string(std::to_string(pld));
        TRANSMIT = new cMessage(result.c_str());
        send(TRANSMIT, "dataWaterFlow"); // send out the message
    }

    std::vector<std::string> Southbound::extractSubstrings(const std::string& inputString) {
        std::vector<std::string> substrings;
        std::istringstream iss(inputString);
        std::string substring;

        while (std::getline(iss, substring, '/')) {
            substrings.push_back(substring);
        }

        return substrings;
    }
