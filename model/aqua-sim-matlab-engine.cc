/*
 * aqua-sim-matlab-engine.cc
 *
 *  Created on: Dec 2, 2024
 *  Author: chenhao <haochen22@mails.jlu.edu.cn>
 */



#include "aqua-sim-matlab-engine.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "mclmcrrt.h"
#include "include/libtest.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AquaSimMatlabEngine");
NS_OBJECT_ENSURE_REGISTERED(AquaSimMatlabEngine);

AquaSimMatlabEngine::AquaSimMatlabEngine()
{
	mclInitializeApplication(NULL, 0);
    NS_LOG_INFO("Matlab engine start");
}

TypeId
AquaSimMatlabEngine::GetTypeId(void)
{
	static TypeId tid = TypeId("ns3::AquaSimMatlabEngine")
	.SetParent<Object>()
	.AddConstructor<AquaSimMatlabEngine>();
	return tid;
}

void AquaSimMatlabEngine::sum()
{
	libtestInitialize();
    mwArray a(1, 1, mxDOUBLE_CLASS);  // 1x1 double array
    mwArray b(1, 1, mxDOUBLE_CLASS);  // 1x1 double array

    a(1, 1)=3;
    b(1, 1)=5;

    mwArray result;

    test(1, result, a, b);  

    // 输出结果
    double res;
    res = result.Get(1, 1);  // get result
    std::cout << "Result: " << res << std::endl;
}
