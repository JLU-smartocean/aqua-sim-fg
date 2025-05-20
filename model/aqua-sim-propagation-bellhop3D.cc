/*
 * aqua-sim-propagation-bellhop3D.cc
 *
 *  Created on: Sep 23, 2022
 *  Author: chenhao <haochen22@mails.jlu.edu.cn>
 */

// add bellhop delay and modify tl on Nov 15, 2021

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "ns3/random-variable-stream.h"
#include "ns3/packet.h"
#include "aqua-sim-propagation-bellhop3D.h"
#include "aqua-sim-header.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <cstring>
#include <math.h>
#include <algorithm>
#define _USE_MATH_DEFINES

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

namespace ns3
{

	NS_LOG_COMPONENT_DEFINE("AquaSimBellhop3DPropagation");
	NS_OBJECT_ENSURE_REGISTERED(AquaSimBellhop3DPropagation);

	TypeId
	AquaSimBellhop3DPropagation::GetTypeId()
	{
		static TypeId tid = TypeId("ns3::AquaSimBellhop3DPropagation")
								.SetParent<AquaSimPropagation>()
								.AddConstructor<AquaSimBellhop3DPropagation>();
		return tid;
	}

	AquaSimBellhop3DPropagation::AquaSimBellhop3DPropagation() : AquaSimPropagation(), basePath("src/model/aqua-sim-fg/config/")
	{
	}

	AquaSimBellhop3DPropagation::~AquaSimBellhop3DPropagation()
	{
	}

	std::vector<PktRecvUnit> *
	AquaSimBellhop3DPropagation::ReceivedCopies(Ptr<AquaSimNetDevice> s,
												Ptr<Packet> p, std::vector<Ptr<AquaSimNetDevice>> dList)
	{
		NS_LOG_FUNCTION(this);
		NS_ASSERT(dList.size());
		std::vector<PktRecvUnit> *res = new std::vector<PktRecvUnit>;
		// find all nodes which will receive a copy
		PktRecvUnit pru;
		double dist = 0;

		AquaSimPacketStamp pstamp;
		p->PeekHeader(pstamp);

		Ptr<Object> sObject = s->GetNode();
		Ptr<MobilityModel> senderModel = sObject->GetObject<MobilityModel>();
		Vector sendPos = sObject->GetObject<MobilityModel>()->GetPosition();

		int senderID = AquaSimAddress::ConvertFrom(s->GetAddress()).GetAsInt();

		unsigned i = 0;
		std::vector<Ptr<AquaSimNetDevice>>::iterator it = dList.begin();
		for (; it != dList.end(); it++, i++)
		{
			Ptr<Object> rObject = dList[i]->GetNode();
			Ptr<MobilityModel> recvModel = rObject->GetObject<MobilityModel>();

			if (s == dList[i]) {
				continue;
			}
			if ((dist = senderModel->GetDistanceFrom(recvModel)) > pstamp.GetTxRange())
				continue;

			updateEnvFile(senderID, sendPos, recvModel->GetPosition(), pstamp.GetFreq());
			bellhop(senderID);

			pru.recver = dList[i];
			// pru.pDelay = readArr(std::to_string(senderID));
			pru.pDelay = Seconds(senderModel->GetDistanceFrom(recvModel) / 1500.0);
			double tl = readShd(std::to_string(senderID));
			pru.pR = pstamp.GetPt() - tl;
			res->push_back(pru);

			NS_LOG_DEBUG("BELLHOP3D-" << senderID << " to "
									  << AquaSimAddress::ConvertFrom(dList[i]->GetAddress()).GetAsInt()
									  << " tl:" << tl << " pr:" << pru.pR << " delay:" << pru.pDelay);
		}
		return res;
	}

	double AquaSimBellhop3DPropagation::getRange(Vector sendPos, Vector recvPos)
	{

		double dx = sendPos.x - recvPos.x;
		double dy = sendPos.y - recvPos.y;
		return sqrt(dx * dx + dy * dy);
	}

	void AquaSimBellhop3DPropagation::bellhop(int senderID)
	{

		string sender = std::to_string(senderID);
		system((basePath + "bellhop3d.exe " + basePath + sender).c_str());
		// system((basePath + "bellhop.exe " + basePath + sender + "arr").c_str());
	}

	Time AquaSimBellhop3DPropagation::readArr(std::string sender)
	{
		string fileName = basePath + sender + "arr.arr";
		ifstream infile(fileName, std::ios::in);
		double delay = 1e5;
		string line;

		for (int i = 0; i < 7; i++)
		{
			getline(infile, line);
		}
		while (getline(infile, line))
		{
			char str[512];
			strcpy(str, line.c_str());

			char *subStr = strtok(str, " ");
			for (int i = 0; i < 2; i++)
			{
				subStr = strtok(NULL, " ");
			}
			delay = std::min(delay, atof(subStr));
		}

		infile.close();
		return Seconds(delay);
	}

	double AquaSimBellhop3DPropagation::readShd(std::string sender)
	{
		string fileName = basePath + sender + ".shd";
		FILE *fp = fopen(fileName.c_str(), "rb");

		int recl;
		fread(&recl, sizeof(int), 1, fp);
		char title[90];
		char plotType[20];
		fread(title, sizeof(char), 80, fp);
		fseek(fp, 4 * recl, 0);
		fread(plotType, sizeof(char), 10, fp);

		int nFreq, nTheta, nsx, nsy, nsz, nrz, nrr;
		int freq0, atten;

		fseek(fp, 2 * 4 * recl, 0);
		fread(&nFreq, sizeof(int), 1, fp);
		fread(&nTheta, sizeof(int), 1, fp);
		fread(&nsx, sizeof(int), 1, fp);
		fread(&nsy, sizeof(int), 1, fp);
		fread(&nsz, sizeof(int), 1, fp);
		fread(&nrz, sizeof(int), 1, fp);
		fread(&nrr, sizeof(int), 1, fp);
		fread(&freq0, sizeof(float), 1, fp);
		fread(&atten, sizeof(float), 1, fp);

		/*vector<double> freqVec;
		fseek(fp, 3 * 4 * recl, 0);
		for (int i = 0; i < nFreq; i++) {
			double temp;
			fread(&temp, sizeof(double), 1, fp);
			freqVec.push_back(temp);
		}


		vector<float> thetaV;
		fseek(fp, 4 * 4 * recl, 0);
		for (int i = 0; i < nTheta; i++) {
			float temp;
			fread(&temp, sizeof(float), 1, fp);
			thetaV.push_back(temp);
		}

		vector<float> sx;
		fseek(fp, 5 * 4 * recl, 0);
		for (int i = 0; i < nsx; i++) {
			float temp;
			fread(&temp, sizeof(float), 1, fp);
			sx.push_back(temp);
		}

		vector<float> sy;
		fseek(fp, 6 * 4 * recl, 0);
		for (int i = 0; i < nsy; i++) {
			float temp;
			fread(&temp, sizeof(float), 1, fp);
			sy.push_back(temp);
		}

		vector<float> sz;
		fseek(fp, 7 * 4 * recl, 0);
		for (int i = 0; i < nsz; i++) {
			float temp;
			fread(&temp, sizeof(float), 1, fp);
			sz.push_back(temp);
		}

		vector<float> rzV;
		fseek(fp, 8 * 4 * recl, 0);
		for (int i = 0; i < nrz; i++) {
			float temp;
			fread(&temp, sizeof(float), 1, fp);
			rzV.push_back(temp);
		}


		vector<float> rrV;
		fseek(fp, 9 * 4 * recl, 0);
		for (int i = 0; i < nrr; i++) {
			float temp;
			fread(&temp, sizeof(float), 1, fp);
			rrV.push_back(temp);
		}*/

		int Nrcvrs_per_range = nrz;
		vector<vector<float>> presure(nrz, vector<float>(nrr * 2, 0));
		int recnum = 9;
		for (int i = 0; i < Nrcvrs_per_range; i++)
		{
			recnum += 1;
			fseek(fp, recnum * 4 * recl, 0);
			vector<float> temp(2 * nrr, 1);
			for (int j = 0; j < nrr; j++)
			{
				fread(&(temp[2 * j]), sizeof(float), 1, fp);
				fread(&(temp[2 * j + 1]), sizeof(float), 1, fp);
			}
			presure[i] = temp;
		}

		fclose(fp);

		double tl;
		for (int i = 0; i < Nrcvrs_per_range; i++)
		{
			for (int j = 0; j < nrr; j++)
			{
				double temp = pow(presure[i][2 * j], 2) + pow(presure[i][2 * j + 1], 2);
				temp = sqrt(temp);
				if (temp < 1e-37)
					temp = 1e-37;
				tl = -20 * (log(temp) / log(10));
			}
		}
		return tl;
	}

	void AquaSimBellhop3DPropagation::updateEnvFile(int senderId, Vector sendPos,
													Vector recvPos, double freq)
	{

		string sender = std::to_string(senderId);
		std::vector<string> fileNames;
		fileNames.push_back(basePath + sender + ".env");
		// fileNames.push_back(basePath + sender + "arr.env");

		double sdepth = sendPos.z;
		double rdepth = recvPos.z;
		double range = getRange(sendPos, recvPos) / 1000.0;

		if (sdepth <= 0 || rdepth <= 0)
		{
			NS_LOG_WARN("BELLHOP-recver or sender depth less than zero");
		}

		double dx = recvPos.x - sendPos.x;
		double dy = recvPos.y - sendPos.y;
		double c = sqrt(dx * dx + dy * dy);
		double theta = asin(dy / c) * 180 / 3.1415926;
		if (dx < 0)
		{
			theta = 180 - theta;
		}

		for (size_t i = 0; i < fileNames.size(); i++)
		{
			string fileName = fileNames[i];
			string tempFileName = fileName + "tmp";
			FILE *infile = fopen(fileName.c_str(), "r");
			ofstream outfile(tempFileName, std::ios::out);
			string line;
			char buf[1000];

			while (!feof(infile))
			{
				fgets(buf, 1000, infile);
				if (feof(infile))
					continue;
				line = string(buf);
				if (line.find("freq") != string::npos)
				{
					outfile << freq << " !freq\n";
				}
				else if (line.find("sdepth") != string::npos)
				{
					outfile << sdepth << " / !sdepth\n";
				}
				else if (line.find("rdepth") != string::npos)
				{
					outfile << rdepth << " / !rdepth\n";
				}
				else if (line.find("range") != string::npos)
				{
					outfile << range << " / !range\n";
				}
				else if (line.find("posx") != string::npos)
				{
					outfile << sendPos.x / 1000.0 << " / !posx\n";
				}
				else if (line.find("posyy") != string::npos)
				{
					outfile << sendPos.y / 1000.0 << " / !posyy\n";
				}
				else if (line.find("luck") != string::npos)
				{
					outfile << theta << " / !luck\n";
				}
				else
				{
					outfile << line;
				}
				line.clear();
			}
			fclose(infile);
			outfile.close();

			system(("mv " + tempFileName + " " + fileName).c_str());
		}
	}

} // namespace ns3
