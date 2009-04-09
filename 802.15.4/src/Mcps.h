#ifndef __MCPS_H__
#define __MCPS_H__

#include <omnetpp.h>
#include "BasicModule.h"
#include "typedef.h"
#include "Messages_m.h"
#include "BBItems.h"
#include <string>
#include <sstream>
#include "MacPib.h"

class Mcps: public BasicModule {
public:
	virtual void initialize(int);
	virtual void handleMessage(cMessage *msg);
protected:
	/** @brief gate id*/
	/*@{*/
	int mcpsSapIn;
	int mcpsSapOut;
	int pdSapIn;
	int pdSapOut;
	int mlmeIn;
	int mlmeOut;
	/*@}*/
	/** additional variables */
	cMessage* lastUpperMsg;
	//cMessage* timer;
	PdMsg* lastBeacon;
	McpsEncapsulation encapsulation;
	McpsEncapsulation* nextEncapsulation;
	/** @brief Sets the level of comments to the EV output */
	CommentsLevel commentsLevel;
	virtual ~Mcps();
	void handleSelfMsg(cMessage *);
	void handlePdMsg(cMessage *);
	void handleMcpsMsg(cMessage *);
	void handleMlmeMsg(cMessage *);
	void sendPdDown(cMessage *);
	void sendMcpsUp(cMessage *);
	void sendMlme(cMessage *);
	PdMsg* encapsulateMcps(McpsMsg *);
	McpsMsg* decapsulatePd(PdMsg *);
	void comment(CommentsLevel level, std::string s) {
		/** @todo align logName substrings for routers and endDevices */
		if ((level & commentsLevel) > COMMENT_NOTHING) {
			std::cout << logName() << "." << getName() << "\t" << s << endl;
		}
	}
	void commentMsgSending(cMessage *msg) {
		std::stringstream commentStream;
		commentStream << "Sending " << msg->getName();
		comment(COMMENT_MESSAGE, commentStream.str());
	}
	void commentMsgReceived(cMessage *msg) {
		std::stringstream commentStream;
		commentStream << "Received " << msg->getName();
		comment(COMMENT_MESSAGE, commentStream.str());
	}
	void commentError(char* errorMessage) {
		std::stringstream commentStream;
		commentStream << "ERROR: " << errorMessage;
		comment(COMMENT_ERROR, commentStream.str());
	}
public:
	void setLastUpperMsg(cMessage* msg) {
		delete (this->lastUpperMsg);
		this->lastUpperMsg = msg;
	}
	cMessage* getLastUpperMsg() {
		return this->lastUpperMsg;
	}
	MacPib* getMacPib() {
			return ((MacPib *) (this->getParentModule()->getModuleByRelativePath(
					"macPib")));
		}
	void setLastBeacon(PdMsg* lastBeacon) {
		delete(this->lastBeacon);
		this->lastBeacon = lastBeacon;
	}
	PdMsg* getLastBeacon() {
		return this->lastBeacon;
	}
	void setNextEncapsulation(McpsEncapsulation nextEncapsulation) {
		this->nextEncapsulation->frameType = nextEncapsulation.framePending;
		this->nextEncapsulation->securityEnabled = nextEncapsulation.securityEnabled;
		this->nextEncapsulation->framePending = nextEncapsulation.framePending;
		this->nextEncapsulation->ackRequest = nextEncapsulation.ackRequest;
		this->nextEncapsulation->panIdCompression = nextEncapsulation.panIdCompression;
		this->nextEncapsulation->destinationAddressingMode = nextEncapsulation.destinationAddressingMode;
		this->nextEncapsulation->frameVersion = nextEncapsulation.frameVersion;
		this->nextEncapsulation->sourceAddressingMode = nextEncapsulation.sourceAddressingMode;
		this->nextEncapsulation->sequenceNumber = nextEncapsulation.sequenceNumber;
		this->nextEncapsulation->destinationPanIdentifier = nextEncapsulation.destinationPanIdentifier;
		this->nextEncapsulation->destinationAddress = nextEncapsulation.destinationAddress;
		this->nextEncapsulation->sourcePanIdentifier = nextEncapsulation.sourcePanIdentifier;
		this->nextEncapsulation->sourceAddress = nextEncapsulation.sourceAddress;
	}
	McpsEncapsulation* getNextEncapsulation() {
		return nextEncapsulation;
	}
};

#endif