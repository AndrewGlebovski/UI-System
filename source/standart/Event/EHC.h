/**
 * @file EHC.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_EVENT_EHC_H
#define __PLUG_EVENT_EHC_H

#include "Plug/Math.h"

namespace plug {

/**
 * @brief Context of event handling
 */
struct EHC {
  TransformStack &stack; /*!< Current state of transform stack */

  bool stopped; /*!< Event is intercepted and should not be handled further */

  bool overlapped; /*!< Event position is contained within area managed by one
                      of the handlers */
};

} // namespace plug

#endif /* EHC.h */
