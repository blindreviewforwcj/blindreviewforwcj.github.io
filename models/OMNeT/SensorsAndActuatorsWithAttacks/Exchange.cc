#include <string.h>
#include <string>
#include <omnetpp.h>
#include <sstream>
#include <random>
#include <map>
#include <queue>
    using namespace omnetpp;

    /**
     * Derive the Txc1 class from cSimpleModule. In the Tictoc1 network,
     * both the `tic' and `toc' modules are Txc1 objects, created by OMNeT++
     * at the beginning of the simulation.
     */
    class Exchange : public cSimpleModule
    {
    public:
        Exchange();
    private:
        cMessage*  TRIGGER;
        cMessage*  PUSH;
        cMessage*  TRANSMIT;
        /* Binding keys*/
        std::string bkWL = "WL";
        std::string bkRP = "RP";
        std::string bkWV = "WV";
        std::string bkWF = "WF";
        std::map<std::string, std::queue<int>> fifoMap;
        int pld; //generated payload
        double lambda;  // Rate parameter for the exponential distribution
        std::default_random_engine generator;
        std::exponential_distribution<double> distribution;
        std::vector<std::string> extractSubstrings(const std::string& inputString);
        void transmit(int pld);
        void transmitToFog(const std::string& inputString);
        void push(int pld, const std::string& bkey);
protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

    };

    // The module class needs to be registered with OMNeT++
    Define_Module(Exchange);


    Exchange::Exchange() {
        lambda = 0.5;
    }

    void Exchange::initialize()
    {
        // Create WL Queue
        std::queue<int> bkWLmyQueue;

        // Store the queue in the map with an identifying string binding key WL
        fifoMap[bkWL] = bkWLmyQueue;

        // Create RP queue
        std::queue<int> bkRPmyQueue;

        // Store the queue in the map with an identifying string binding key RP
        fifoMap[bkRP] = bkRPmyQueue;


        // Create WV queue
        std::queue<int> bkWVmyQueue;

        // Store the queue in the map with an identifying string binding key WV
        fifoMap[bkWV] = bkWVmyQueue;


        // Create WF queue
        std::queue<int> bkWFmyQueue;

        // Store the queue in the map with an identifying string binding key WF
        fifoMap[bkWF] = bkWFmyQueue;


        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);
    }
    void Exchange::handleMessage(cMessage *msg)
    {
        EV << "Received  " << msg->getName()  << endl;
        std::string s = msg->getName();

        bool isFound = s.find("TRANSMIT") != std::string::npos;

        if(isFound){
            std::vector<std::string> extractedSubstrings = extractSubstrings( msg->getName());
            std::string rk = extractedSubstrings[1];
            std::string PLD = extractedSubstrings[2];
            int pld = std::stoi(PLD);

            if(rk==bkWL){
                push(pld,bkWL);
            }else{
                if(rk==bkRP){
                    push(pld,bkRP);
                }else{
                    if(rk==bkWV){
                        push(pld,bkWV);
                    }
                }
            }
        }else{
            isFound = s.find("PUSH") != std::string::npos;
            if(isFound){
                std::vector<std::string> extractedSubstrings = extractSubstrings( msg->getName());
                std::string bk = extractedSubstrings[1];
                std::string PLD = extractedSubstrings[2];
                int pld = std::stoi(PLD);
                std::queue<int>& queue = fifoMap[bk];
                queue.push(pld);
            }else{
                isFound = s.find("GET") != std::string::npos;
                if(isFound){

                               std::queue<int>& queuebkWL = fifoMap[bkWL];
                               std::queue<int>& queuebkRP = fifoMap[bkRP];
                               std::queue<int>& queuebkWV = fifoMap[bkWV];

                               if(!queuebkWL.empty() && !queuebkRP.empty() && !queuebkWV.empty()){
                                   std::string result = std::string("FOG/") +
                                           bkWL + "/"+std::string(std::to_string(queuebkWL.front()))+"/"+
                                           bkRP +  "/"+std::string(std::to_string(queuebkRP.front()))+"/"+
                                           bkWV + "/"+std::string(std::to_string(queuebkWV.front())) ;
                                   queuebkWL.pop(); queuebkRP.pop(); queuebkWV.pop();
                                   transmitToFog(result);
                               }

                           }else{
                               isFound = s.find("SETWF") != std::string::npos;
                               if(isFound){
                                   std::vector<std::string> extractedSubstrings = extractSubstrings( msg->getName());
                                   std::string PLD = extractedSubstrings[1];
                                   push(std::stoi(PLD),bkWF);
                               }else{
                                   isFound = s.find("SOUTHBOUNDWF") != std::string::npos;
                                   if(isFound){
                                       std::queue<int>& queuebkWF = fifoMap[bkWF];

                                       if(!queuebkWF.empty()){
                                           int pld = queuebkWF.front();
                                           queuebkWF.pop();
                                           transmit(pld);
                                       }

                                   }
                               }
                           }
            }
        }



        TRIGGER = new cMessage();
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, TRIGGER);

    }


    /**
     * Send push messages
     */
    void Exchange::push(int pld, const std::string& bkey)
    {
        std::string result = std::string("PUSH/") + bkey + "/"+std::string(std::to_string(pld));
        PUSH = new cMessage(result.c_str());
        double interval = distribution(generator, std::exponential_distribution<double>::param_type(lambda));
        scheduleAt(simTime() + interval, PUSH);
    }

    /**
     * transmit WF payload to southbound
     */
    void Exchange::transmit(int pld)
    {
        std::string result = std::string("WF/") + std::string(std::to_string(pld));
        TRANSMIT = new cMessage(result.c_str());
        send(TRANSMIT, "decisiondataWaterFlow"); // send out the message
    }

    void Exchange::transmitToFog(const std::string& inputString)
    {
        TRANSMIT = new cMessage(inputString.c_str());
        send(TRANSMIT, "dataToFog"); // send out the message
    }

    std::vector<std::string> Exchange::extractSubstrings(const std::string& inputString) {
        std::vector<std::string> substrings;
        std::istringstream iss(inputString);
        std::string substring;
        while (std::getline(iss, substring, '/')) {
            substrings.push_back(substring);
        }
        return substrings;
    }
