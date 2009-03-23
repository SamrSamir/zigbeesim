#include "Nlme.h"
#include <string>

Define_Module( Nlme);

void Nlme::initialize(int stage) {
	BasicModule::initialize(stage);

	if (stage == 0) {
		mlmeSapIn = findGate("mlmeSapIn");
		mlmeSapOut = findGate("mlmeSapOut");
		nlmeSapIn = findGate("nlmeSapIn");
		nlmeSapOut = findGate("nlmeSapOut");
		nldeOut = findGate("nldeOut");
		nldeIn = findGate("nldeIn");
		nwkPibIn = findGate("nwkPibIn");
		nwkPibOut = findGate("nwkPibOut");

		commentsLevel = ALL;
	} else if (stage == 1) {
		lastUpperMsg = new cMessage();
	}
}

void Nlme::handleMessage(cMessage *msg) {
	if (!msg->isSelfMessage()) {
		commentMsgReceived(msg);
	}
	if (msg->getArrivalGateId() == mlmeSapIn) {
		handleMlmeMsg(msg);
	} else if (msg->getArrivalGateId() == nlmeSapIn) {
		handleNlmeMsg(msg);
	} else if (msg->getArrivalGateId() == nldeIn) {
		handleNlmeMsg(msg);
	} else if (msg->getArrivalGateId() == nwkPibIn) {
		handleNwkPibMsg(msg);
	} else {
		handleSelfMsg(msg);
	}
}

void Nlme::handleSelfMsg(cMessage *msg) {
	delete (msg);
}

void Nlme::handleMlmeMsg(cMessage *msg) {
	if (msg->getKind() == MLME_SCAN_CONFIRM) {
		MlmeScan_confirm* confirm = check_and_cast<MlmeScan_confirm *> (msg);
		if (getLastUpperMsg()->getKind() == NLME_NETWORK_FORMATION_REQUEST) {
			NlmeNetworkFormation_request* request = check_and_cast<
					NlmeNetworkFormation_request *> (getLastUpperMsg());
			if (confirm->getStatus() == MAC_SUCCESS) {
				if (confirm->getScanType() == ED_SCAN) {
					MlmeScan_request* scan = new MlmeScan_request();
					scan->setName("MLME-SCAN.request");
					scan->setKind(MLME_SCAN_REQUEST);
					scan->setScanType(ACTIVE_SCAN);
					scan->setScanChannels(request->getScanChannels());
					scan->setScanDuration(request->getScanDuration());
					scan->setChannelPage(request->getScanChannels() >> 28);
					sendMlmeDown(scan);
				} else if (confirm->getScanType() == ACTIVE_SCAN) {
					/** @comment we'll find not used PAN ID from the scan and save it for a future use */
					bool idOk = true;
					do {
						setPanId((unsigned short) (rand() % 0x00004000));
						for (int i = 0; i < confirm->getPanDescriptorListArraySize(); i++) {
							if (getPanId() == confirm->getPanDescriptorList(i).coordPANId) idOk = false;
						}
					} while (!idOk);
					MlmeSet_request* setRequest = new MlmeSet_request("MLME-SET.request", MLME_SET_REQUEST);
					setRequest->setPibAttribute(PHY_CURRENT_CHANNEL);
					setRequest->setPibAttributeValueArraySize(1);
					/** @todo here we should select a channel according to some better formula */
					unsigned char randomChannel;
					unsigned int goodChannels = confirm->getUnscannedChannels()^request->getScanChannels();
					do {
						randomChannel = (unsigned char) (rand() % 28);
					} while (((goodChannels>>randomChannel) & 0x00000001) != 0x00000001);
					setRequest->setPibAttributeValue(0, randomChannel);
					sendMlmeDown(setRequest);
				}
			}
		}
	} else if (msg->getKind() == MLME_SET_CONFIRM) {
		MlmeSet_confirm* confirm = check_and_cast<MlmeSet_confirm *> (msg);
		if (getLastUpperMsg()->getKind() == NLME_NETWORK_FORMATION_REQUEST) {
			if (confirm->getPibAttribute() == PHY_CURRENT_CHANNEL) {
				MlmeSet_request* setRequest = new MlmeSet_request("MLME-SET.request", MLME_SET_REQUEST);
				setRequest->setPibAttribute(MAC_PAN_ID);
				setRequest->setPibAttributeValueArraySize(1);
				setRequest->setPibAttributeValue(0, getPanId());
				sendMlmeDown(setRequest);
			}
		}
	}
	delete (msg);
}

void Nlme::handleNlmeMsg(cMessage *msg) {
	setLastUpperMsg(msg);
	if (msg->getKind() == NLME_NETWORK_FORMATION_REQUEST) {
		NlmeNetworkFormation_request* request = check_and_cast<
				NlmeNetworkFormation_request *> (msg);
		MlmeScan_request* scan = new MlmeScan_request("MLME-SCAN.request", MLME_SCAN_REQUEST);
		scan->setScanType(ED_SCAN);
		scan->setScanChannels(request->getScanChannels());
		scan->setScanDuration(request->getScanDuration());
		sendMlmeDown(scan);
	}
}

void Nlme::handleNwkPibMsg(cMessage *msg) {
	delete (msg);
}

void Nlme::handleNldeMsg(cMessage *msg) {
	delete (msg);
}

void Nlme::sendMlmeDown(cMessage *msg) {
	commentMsgSending(msg);
	sendDelayed(msg, 0.0, mlmeSapOut);
}

void Nlme::sendNlmeUp(cMessage *msg) {
	commentMsgSending(msg);
	sendDelayed(msg, 0.0, nlmeSapOut);
}

void Nlme::sendNlde(cMessage *msg) {
	commentMsgSending(msg);
	sendDelayed(msg, 0.0, nldeOut);
}

void Nlme::sendNwkPib(cMessage *msg) {
	commentMsgSending(msg);
	sendDelayed(msg, 0.0, nwkPibOut);
}
