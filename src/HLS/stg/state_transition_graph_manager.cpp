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
 * @file StateTransitionGraphManager.cpp
 * @brief File contanining the structures necessary to manage a graph that will represent a state transition graph
 *
 * This file contains the necessary data structures used to represent a state transition graph
 *
 * @author Christian Pilato <pilato@elet.polimi.it>
 * @author Fabrizio Ferrandi <fabrizio.ferrandi@polimi.it>
 * $Revision$
 * $Date$
 * Last modified by $Author$
 *
 */

///Header include
#include "state_transition_graph_manager.hpp"

///Behavior include
#include "behavioral_helper.hpp"
#include "op_graph.hpp"

///hls include
#include "hls.hpp"
#include "hls_manager.hpp"
#include "function_behavior.hpp"

///Parameter include
#include "Parameter.hpp"


///State transition graph include
#include "state_transition_graph.hpp"
#include "StateTransitionGraph_constructor.hpp"

///Tree include
#include "var_pp_functor.hpp"

///Utilty include
#include "simple_indent.hpp"
#include "boost/graph/topological_sort.hpp"
#include "dbgPrintHelper.hpp"               // for DEBUG_LEVEL_

StateTransitionGraphManager::StateTransitionGraphManager(const HLS_managerConstRef _HLSMgr, const hlsConstRef  _HLS, const ParameterConstRef _Param) :
   state_transition_graphs_collection(StateTransitionGraphsCollectionRef(new StateTransitionGraphsCollection(StateTransitionGraphInfoRef(new StateTransitionGraphInfo(_HLSMgr->CGetFunctionBehavior(_HLS->functionId)->CGetOpGraph(FunctionBehavior::CFG))), _Param))),
   ACYCLIC_STG_graph(StateTransitionGraphRef(new StateTransitionGraph(state_transition_graphs_collection, ST_EDGE_NORMAL_T))),
   STG_graph(StateTransitionGraphRef(new StateTransitionGraph(state_transition_graphs_collection, ST_EDGE_NORMAL_T | ST_EDGE_FEEDBACK_T))),
   op_function_graph(_HLSMgr->CGetFunctionBehavior(_HLS->functionId)->CGetOpGraph(FunctionBehavior::CFG)),
   Param(_Param),
   output_level(_Param->getOption<int>(OPT_output_level)),
   debug_level(_Param->getOption<int>(OPT_debug_level)),
   STG_builder(StateTransitionGraph_constructorRef(new StateTransitionGraph_constructor(state_transition_graphs_collection, _HLSMgr, _HLS->functionId)))
{
   STG_builder->create_entry_state();
   STG_builder->create_exit_state();
}

StateTransitionGraphManager::~StateTransitionGraphManager()
{
}

const StateTransitionGraphConstRef StateTransitionGraphManager::CGetAstg() const
{
   return ACYCLIC_STG_graph;
}

const StateTransitionGraphConstRef StateTransitionGraphManager::CGetStg() const
{
   return STG_graph;
}

std::set<std::pair<vertex, unsigned int> > StateTransitionGraphManager::get_conditions(const vertex& v) const
{
   std::set<std::pair<vertex, unsigned int> > Conditions;

   InEdgeIterator ie, iend;
   for (boost::tie(ie, iend) = boost::in_edges(v, *STG_graph); ie != iend; ie++)
   {
      std::set<std::pair<vertex, unsigned int> > EdgeConditions = GET_EDGE_INFO(state_transition_graphs_collection, TransitionInfo, *ie)->conditions;
      for (std::set<std::pair<vertex, unsigned int> >::const_iterator c = EdgeConditions.begin(); c != EdgeConditions.end(); ++c)
      {
         if (Conditions.find(*c) != Conditions.end()) continue;
         if (c->second == TransitionInfo::DONTCARE)
         {
            if (Conditions.find(std::make_pair(c->first, T_COND)) != Conditions.end())
               Conditions.erase(std::make_pair(c->first, T_COND));
            if (Conditions.find(std::make_pair(c->first, F_COND)) != Conditions.end())
               Conditions.erase(std::make_pair(c->first, F_COND));
            Conditions.insert(*c);
         }
         if (c->second == T_COND)
         {
            if (Conditions.find(std::make_pair(c->first, F_COND)) != Conditions.end())
            {
               Conditions.erase(std::make_pair(c->first, F_COND));
               Conditions.insert(std::make_pair(c->first, TransitionInfo::DONTCARE));
            }
            if (Conditions.find(std::make_pair(c->first, TransitionInfo::DONTCARE)) == Conditions.end())
               Conditions.insert(*c);
         }
         if (c->second == F_COND)
         {
            if (Conditions.find(std::make_pair(c->first, T_COND)) != Conditions.end())
            {
               Conditions.erase(std::make_pair(c->first, T_COND));
               Conditions.insert(std::make_pair(c->first, TransitionInfo::DONTCARE));
            }
            if (Conditions.find(std::make_pair(c->first, TransitionInfo::DONTCARE)) == Conditions.end())
               Conditions.insert(*c);
         }
         else
         {
            Conditions.insert(*c);
         }
      }
   }

   return Conditions;
}

void StateTransitionGraphManager::compute_min_max()
{
   StateTransitionGraphInfoRef info = STG_graph->GetStateTransitionGraphInfo();
   if(!info->is_a_dag) return;
   std::list<vertex> sorted_vertices;
   ACYCLIC_STG_graph->TopologicalSort(sorted_vertices);
   std::unordered_map<vertex, unsigned int> CSteps_min;
   std::unordered_map<vertex, unsigned int> CSteps_max;
   const std::list<vertex>::iterator it_sv_end = sorted_vertices.end();
   for(std::list<vertex>::iterator it_sv = sorted_vertices.begin(); it_sv != it_sv_end; ++it_sv)
   {
      CSteps_min[*it_sv] = 0;
      CSteps_max[*it_sv] = 0;
      InEdgeIterator ie, ie_first,iend;
      boost::tie(ie, iend) = boost::in_edges(*it_sv, *ACYCLIC_STG_graph);
      ie_first = ie;
      for (; ie != iend; ie++)
      {
         vertex src = boost::source(*ie, *ACYCLIC_STG_graph);
         CSteps_max[*it_sv] = std::max(CSteps_max[*it_sv], 1+CSteps_max[src]);
         if(ie == ie_first)
            CSteps_min[*it_sv] = 1+CSteps_min[src];
         else
            CSteps_min[*it_sv] = std::min(CSteps_min[*it_sv], 1+CSteps_max[src]);
      }

   }
   THROW_ASSERT(CSteps_min.find(info->exit_node) != CSteps_min.end(), "Exit node not reachable");
   THROW_ASSERT(CSteps_max.find(info->exit_node) != CSteps_max.end(), "Exit node not reachable");
   info->min_cycles = CSteps_min.find(info->exit_node)->second-1;
   info->max_cycles = CSteps_max.find(info->exit_node)->second-1;
}

vertex StateTransitionGraphManager::get_entry_state() const
{
   return STG_graph->CGetStateTransitionGraphInfo()->entry_node;
}

std::set<vertex> StateTransitionGraphManager::get_states(const vertex& op, StateTypes statetypes) const
{
   std::set<vertex> vertex_set; 
   VertexIterator v, vend;
   for (boost::tie(v, vend) = boost::vertices(*STG_graph); v != vend; v++)
   {
      const std::list<vertex>* operations;
      switch (statetypes)
      {
      case EXECUTING:
         operations = &(STG_graph->CGetStateInfo(*v)->executing_operations);
         break;
      case STARTING:
         operations = &(STG_graph->CGetStateInfo(*v)->starting_operations);
         break;
      case ENDING:
         operations = &(STG_graph->CGetStateInfo(*v)->ending_operations);
         break;
      default:
         THROW_UNREACHABLE("Unknown state type. Which one are you lookig for?");
      }
      std::list<vertex>::const_iterator op_it_end = operations->end();
      bool stop_flag;
      stop_flag = false;
      for(std::list<vertex>::const_iterator op_it = operations->begin(); op_it !=  op_it_end && !stop_flag; ++op_it)
      {
         if(*op_it == op)
         {
            stop_flag = true;
            vertex_set.insert(*v);
         }
      }
   }
   THROW_ASSERT(vertex_set.size() > 0, "Something wrong! Operation " + GET_NAME(op_function_graph, op) + " is executed " + "into no states");
   return vertex_set;
}

std::set<vertex> StateTransitionGraphManager::get_ending_states(const vertex& op) const
{
   return get_states(op, StateTransitionGraphManager::StateTypes::ENDING);
}

std::set<vertex> StateTransitionGraphManager::get_starting_states(const vertex& op) const
{
   return get_states(op, StateTransitionGraphManager::StateTypes::STARTING);
}

std::set<vertex> StateTransitionGraphManager::get_execution_states(const vertex& op) const
{
   return get_states(op, StateTypes::EXECUTING);
}

vertex StateTransitionGraphManager::get_exit_state() const
{
   return STG_graph->CGetStateTransitionGraphInfo()->exit_node;
}

std::string StateTransitionGraphManager::get_state_name(vertex state) const
{
   return STG_graph->CGetStateInfo(state)->name;
}

StateTransitionGraphRef StateTransitionGraphManager::GetAstg()
{
   return ACYCLIC_STG_graph;
}

StateTransitionGraphRef StateTransitionGraphManager::GetStg()
{
   return STG_graph;
}

unsigned int StateTransitionGraphManager::get_number_of_states() const
{
   return static_cast<unsigned int>(boost::num_vertices(*state_transition_graphs_collection) - 2);
}

void StateTransitionGraphManager::print_statistics() const
{
   INDENT_OUT_MEX(OUTPUT_LEVEL_MINIMUM, output_level, "---Number of states: " + STR(get_number_of_states()));
   INDENT_OUT_MEX(OUTPUT_LEVEL_VERY_PEDANTIC, output_level, "---" + std::string("Is a DAG: ") + (STG_graph->CGetStateTransitionGraphInfo()->is_a_dag ? "T" : "F"));
   if(STG_graph->CGetStateTransitionGraphInfo()->min_cycles != 0 && STG_graph->CGetStateTransitionGraphInfo()->max_cycles != 0)
   {
      INDENT_OUT_MEX(OUTPUT_LEVEL_MINIMUM, output_level, "---Minimum number of cycles: " + STR(STG_graph->CGetStateTransitionGraphInfo()->min_cycles));
      INDENT_OUT_MEX(OUTPUT_LEVEL_MINIMUM, output_level, "---Maximum number of cycles " + STR(STG_graph->CGetStateTransitionGraphInfo()->max_cycles));
   }
}
