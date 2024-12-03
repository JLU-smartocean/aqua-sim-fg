/*
 * aqua-sim-matlab-engine.h
 *
 *  Created on: Dec 2, 2024
 *  Author: chenhao <haochen22@mails.jlu.edu.cn>
 */


#ifndef AQUA_SIM_MATLAB_ENGINE_H
#define AQUA_SIM_MATLAB_ENGINE_H

#include "ns3/object.h"

namespace ns3 {


/**
 * \ingroup aqua-sim-fg
 *
 * \brief matlab engine.
 *
 */
class AquaSimMatlabEngine : public Object {
public:
  AquaSimMatlabEngine();
  static TypeId GetTypeId(void);

  void sum();

}; //class AquaSimMatlabEngine

} // namespace ns3

#endif /* AQUA_SIM_MATLAB_ENGINE_H */
