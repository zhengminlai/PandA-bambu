/*
 *
 *                   _/_/_/    _/_/   _/    _/ _/_/_/    _/_/
 *                  _/   _/ _/    _/ _/_/  _/ _/   _/ _/    _/
 *                 _/_/_/  _/_/_/_/ _/  _/_/ _/   _/ _/_/_/_/
 *                _/      _/    _/ _/    _/ _/   _/ _/    _/
 *               _/      _/    _/ _/    _/ _/_/_/  _/    _/
 *
 *             ***********************************************
 *                              PandA Project
 *                     URL: http://panda.dei.polimi.it
 *                       Politecnico di Milano - DEIB
 *                        System Architectures Group
 *             ***********************************************
 *              Copyright (c) 2004-2018 Politecnico di Milano
 *
 *   This file is part of the PandA framework.
 *
 *   The PandA framework is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
/**
 * @file complete_call_graph.hpp
 * @brief This class models the ending of execution of all functions which can add a function to call graph
 *
 * @author Marco Lattuada <marco.lattuada@polimi.it>
 *
*/

#ifndef COMPLETE_CALL_GRAPH_HPP
#define COMPLETE_CALL_GRAPH_HPP

#include <unordered_set>                       // for unordered_set
#include <utility>                             // for pair
#include "application_frontend_flow_step.hpp"  // for ApplicationFrontendFlo...
#include "design_flow_step.hpp"                // for DesignFlowManagerConstRef
#include "frontend_flow_step.hpp"              // for FrontendFlowStep::Func...

class CompleteCallGraph: public ApplicationFrontendFlowStep
{
   private:
      /**
       * Return the set of analyses in relationship with this design step
       * @param relationship_type is the type of relationship to be considered
       */
      const std::unordered_set<std::pair<FrontendFlowStepType, FunctionRelationship> > ComputeFrontendRelationships(const DesignFlowStep::RelationshipType relationship_type) const;

   public:
      /**
       * Constructor
       * @param AppM is the application manager
       * @param design_flow_manager is the design flow manager
       * @param _Param is the set of the parameters
       */
      CompleteCallGraph(const application_managerRef AppM, const DesignFlowManagerConstRef design_flow_manager, const ParameterConstRef parameters);

      /**
       * Destructor
       */
      ~CompleteCallGraph();

      /**
       * Execute this step
       * @return the exit status of this step
       */
      DesignFlowStep_Status Exec();

      /**
       * Check if this step has actually to be executed
       * @return true if the step has to be executed
       */
      virtual bool HasToBeExecuted() const;
};
#endif

