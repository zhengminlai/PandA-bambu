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
 *              Copyright (C) 2015-2020 Politecnico di Milano
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
 * @file generate_taste_hdl_architecture.cpp
 * @brief Implementation of the class to generate the top HDL taste architecture
 *
 * @author Marco Lattuada <marco.lattuada@polimi.it>
 *
 */

/// Header include
#include "generate_taste_hdl_architecture.hpp"

/// Autoheader include
#include "config_PACKAGE_BUGREPORT.hpp"
#include "config_PACKAGE_NAME.hpp"
#include "config_PACKAGE_VERSION.hpp"

///. include
#include "Parameter.hpp"

/// behavior includes
#include "call_graph_manager.hpp"
#include "function_behavior.hpp"

/// circuit include
#include "structural_manager.hpp"

/// design_flows/backend/ToHDL
#include "language_writer.hpp"

/// HLS includes
#include "hls.hpp"
#include "hls_manager.hpp"
#include "hls_target.hpp"

/// intermediate_representation/aadl include
#include "aadl_information.hpp"

/// technology/target_device include
#include "target_device.hpp"

/// tree includes
#include "behavioral_helper.hpp"
#include "tree_manager.hpp"

GenerateTasteHDLArchitecture::GenerateTasteHDLArchitecture(const ParameterConstRef _parameters, const HLS_managerRef _HLSMgr, const DesignFlowManagerConstRef _design_flow_manager)
    : HLS_step(_parameters, _HLSMgr, _design_flow_manager, HLSFlowStep_Type::GENERATE_TASTE_HDL_ARCHITECTURE)
{
}

DesignFlowStep_Status GenerateTasteHDLArchitecture::Exec()
{
   const auto TreeM = HLSMgr->get_tree_manager();
   const technology_managerConstRef TM = HLSMgr->get_HLS_target()->get_target_device()->get_technology_manager();
   language_writerRef writer = language_writer::create_writer(HDLWriter_Language::VHDL, TM, parameters);
   writer->write_comment(std::string("File automatically generated by: ") + PACKAGE_NAME + " framework version=" + PACKAGE_VERSION + "\n");
   writer->write_comment(std::string("Send any bug to: ") + PACKAGE_BUGREPORT + "\n");
   writer->WriteLicense();

   writer->write_comment("Description : This file is based on LEON3 Demonstration design.\n");
   writer->write_comment("It implements the different components of the architecture :\n");
   writer->write_comment("Gailser reset generator, clock generator, clock pads and PCI pads.\n");
   writer->write_comment("Gailser simple target PCI\n");
   writer->write_comment("Gailser AHB controller\n");
   writer->write_comment("Gailser APB bridge\n");
   writer->write_comment("APB wrappers\n");
   writer->write("\n");
   writer->write("library ieee;\n");
   writer->write("use ieee.std_logic_1164.all;\n");
   writer->write("\n");
   writer->write("library grlib, techmap;\n");
   writer->write("use grlib.amba.all;\n");
   writer->write("use grlib.stdlib.all;\n");
   writer->write("use techmap.gencomp.all;\n");
   writer->write("\n");
   writer->write("library gaisler;\n");
   writer->write("use gaisler.misc.all;\n");
   writer->write("use gaisler.pci.all;\n");
   writer->write("\n");
   writer->write("use work.config.all;\n");
   writer->write("\n");
   writer->write("entity TASTE_hardware_architecture is\n");
   writer->write("  generic (\n");
   writer->write("    fabtech    : integer := CFG_FABTECH;\n");
   writer->write("    memtech    : integer := CFG_MEMTECH;\n");
   writer->write("    padtech    : integer := CFG_PADTECH;\n");
   writer->write("    clktech    : integer := CFG_CLKTECH\n");
   writer->write("  );\n");
   writer->write("  port (\n");
   writer->write("    resetn     : in  std_logic;\n");
   writer->write("    clk        : in  std_logic;\n");
   writer->write("    pllref     : in  std_logic;\n");
   writer->write("\n");
   writer->write("    pci_rst    : inout std_logic;\n");
   writer->write("    pci_clk    : in std_logic;\n");
   writer->write("    pci_gnt    : in std_logic;\n");
   writer->write("    pci_idsel  : in std_logic;\n");
   writer->write("    pci_lock   : inout std_logic;\n");
   writer->write("    pci_ad     : inout std_logic_vector(31 downto 0);\n");
   writer->write("    pci_cbe    : inout std_logic_vector(3 downto 0);\n");
   writer->write("    pci_frame  : inout std_logic;\n");
   writer->write("    pci_irdy   : inout std_logic;\n");
   writer->write("    pci_trdy   : inout std_logic;\n");
   writer->write("    pci_devsel : inout std_logic;\n");
   writer->write("    pci_stop   : inout std_logic;\n");
   writer->write("    pci_perr   : inout std_logic;\n");
   writer->write("    pci_par    : inout std_logic;\n");
   writer->write("    pci_req    : inout std_logic;\n");
   writer->write("    pci_serr   : inout std_logic;\n");
   writer->write("    pci_host   : in std_logic;\n");
   writer->write("    pci_66     : in std_logic\n");
   writer->write("   );\n");
   writer->write("end;\n");
   writer->write("\n");
   writer->write("architecture rtl of TASTE_hardware_architecture is\n");
   writer->write("\n");
   for(const auto& top_function : HLSMgr->aadl_information->top_functions_names)
   {
      const auto function_id = TreeM->function_index(top_function);
      const auto SM = HLSMgr->get_HLS(function_id)->top;
      const auto circuit = SM->get_circ();
      writer->write_component_declaration(circuit);
   }

   writer->write("signal apbi  : apb_slv_in_type;\n");
   writer->write("signal apbo  : apb_slv_out_vector := (others => apb_none);\n");
   writer->write("signal ahbsi : ahb_slv_in_type;\n");
   writer->write("signal ahbso : ahb_slv_out_vector := (others => ahbs_none);\n");
   writer->write("signal ahbmi : ahb_mst_in_type;\n");
   writer->write("signal ahbmo : ahb_mst_out_vector := (others => ahbm_none);\n");
   writer->write("\n");
   writer->write("signal clkm, rstn, rstraw, pciclk, lclk, pci_lclk : std_logic;\n");
   writer->write("signal cgi   : clkgen_in_type;\n");
   writer->write("signal cgo   : clkgen_out_type;\n");
   writer->write("\n");
   writer->write("signal pcii : pci_in_type;\n");
   writer->write("signal pcio : pci_out_type;\n");
   writer->write("\n");
   writer->write("constant BOARD_FREQ : integer := 50000;   -- Board frequency in KHz\n");
   writer->write("\n");
   size_t counter = 0;
   for(const auto& top_function : HLSMgr->aadl_information->top_functions_names)
   {
      writer->write_comment(top_function + "\n");
      writer->write("constant pconfig_" + STR(counter) + " : apb_config_type := (\n");
      writer->write("   0 => ahb_device_reg ( 16#ff#, 1, 0, 1, 0),\n");
      writer->write("   1 => apb_iobar(" + STR(counter) + ", " + STR(counter) + "));\n\n");
      counter++;
   }
   writer->write("\n");
   writer->write("begin\n");
   writer->write("\n");
   writer->write_comment("--------------------------------------------------------------------\n");
   writer->write_comment("-  Reset and Clock generation  -------------------------------------\n");
   writer->write_comment("--------------------------------------------------------------------\n");
   writer->write("\n");
   writer->write("   cgi.pllctrl <= \"00\"; cgi.pllrst <= rstraw;\n");
   writer->write("\n");
   writer->write("   pllref_pad : clkpad\n");
   writer->write("   generic map (tech => padtech)\n");
   writer->write("   port map (pllref, cgi.pllref);\n");
   writer->write("\n");
   writer->write("   clk_pad : clkpad\n");
   writer->write("   generic map (tech => padtech)\n");
   writer->write("   port map (clk, lclk);\n");
   writer->write("\n");
   writer->write("   pci_clk_pad : clkpad\n");
   writer->write("   generic map (tech => padtech, level => pci33)\n");
   writer->write("   port map (pci_clk, pci_lclk);\n");
   writer->write("\n");
   writer->write("   clkgen0 : clkgen\n");
   writer->write("    generic map (clktech, CFG_CLKMUL, CFG_CLKDIV, 0,\n");
   writer->write("            0, CFG_PCI, CFG_PCIDLL, CFG_PCISYSCLK, BOARD_FREQ)\n");
   writer->write("    port map (lclk, pci_lclk, clkm, open, open, open, pciclk, cgi, cgo);\n");
   writer->write("\n");
   writer->write("   rst0 : rstgen\n");
   writer->write("   port map (resetn, clkm, cgo.clklock, rstn, rstraw);\n");
   writer->write("\n");
   writer->write_comment("--------------------------------------------------------------------\n");
   writer->write_comment("-  AHB CONTROLLER --------------------------------------------------\n");
   writer->write_comment("--------------------------------------------------------------------\n");
   writer->write("\n");
   writer->write("   ahb0 : ahbctrl\n");
   writer->write("   generic map (defmast => CFG_DEFMST, split => CFG_SPLIT,\n");
   writer->write("            rrobin => CFG_RROBIN, ioaddr => CFG_AHBIO,\n");
   writer->write("            ioen => 1,nahbm => 2, nahbs => 1)\n");
   writer->write("   port map (rstn, clkm, ahbmi, ahbmo, ahbsi, ahbso);\n");
   writer->write("\n");
   writer->write_comment("---------------------------------------------------------------------\n");
   writer->write_comment("-  PCI Simple Target-------------------------------------------------\n");
   writer->write_comment("---------------------------------------------------------------------\n");
   writer->write("\n");
   writer->write("   pci0 : pci_target\n");
   writer->write("   generic map (hindex => 0,device_id => CFG_PCIDID, vendor_id => CFG_PCIVID)\n");
   writer->write("   port map (rstn, clkm, pciclk, pcii, pcio, ahbmi, ahbmo(0));\n");
   writer->write("\n");
   writer->write("\n");
   writer->write("   pcipads0 : pcipads\n");
   writer->write("   generic map (padtech => padtech)\n");
   writer->write("   port map ( pci_rst, pci_gnt, pci_idsel, pci_lock, pci_ad, pci_cbe,\n");
   writer->write("         pci_frame, pci_irdy, pci_trdy, pci_devsel, pci_stop, pci_perr,\n");
   writer->write("         pci_par, pci_req, pci_serr, pci_host, pci_66, pcii, pcio );\n");
   writer->write("\n");
   writer->write_comment("--------------------------------------------------------------------\n");
   writer->write_comment("-  APB Bridge  -----------------------------------------------------\n");
   writer->write_comment("--------------------------------------------------------------------\n");
   writer->write("\n");
   writer->write("   apb0 : apbctrl\n");
   writer->write("   generic map (hindex => 0, haddr => CFG_APBADDR)\n");
   writer->write("   port map (rstn, clkm, ahbsi, ahbso(0), apbi, apbo );\n");
   writer->write("\n");
   writer->write_comment("---------------------------------------------------------------------\n");
   writer->write_comment("-  Custom IP wrapper ------------------------------------------------\n");
   writer->write_comment("---------------------------------------------------------------------\n");
   writer->write("\n");
   writer->write("\n");
   counter = 0;
   for(const auto& top_function : HLSMgr->aadl_information->top_functions_names)
   {
      writer->write(top_function + "_instance : " + top_function + "_taste_interface generic map(pindex => " + STR(counter) + ", paddr => " + STR(counter) + ") port map (clock => clkm, reset => rstn, apbi_psel => apbi.psel(" + STR(counter) +
                    "), apbi_penable => apbi.penable, apbi_pwrite => apbi.pwrite, apbi_pwdata => apbi.pwdata, apbi_paddr => apbi.paddr, apbo_prdata => apbo(" + STR(counter) + ").prdata);\n\n");
      writer->write("apbo(" + STR(counter) + ").pconfig <= pconfig_" + STR(counter) + ";\n\n");
      counter++;
   }
   writer->write("end;\n");
   writer->WriteFile("architecture_top.vhd");
   HLSMgr->hdl_files.push_back("top.vhd");
   HLSMgr->hdl_files.push_back("architecture_top.vhd");

   /// Write configuration file
   language_writerRef config_writer = language_writer::create_writer(HDLWriter_Language::VHDL, TM, parameters);
   config_writer->write_comment(std::string("File automatically generated by: ") + PACKAGE_NAME + " framework version=" + PACKAGE_VERSION + "\n");
   config_writer->write_comment(std::string("Send any bug to: ") + PACKAGE_BUGREPORT + "\n");
   config_writer->WriteLicense();

   config_writer->write("library techmap;\n");
   config_writer->write("use techmap.gencomp.all;\n");
   config_writer->write("\n");
   config_writer->write("library ieee;\n");
   config_writer->write("use ieee.std_logic_1164.all;\n");
   config_writer->write("\n");
   config_writer->write("package config is\n");
   config_writer->write("   -- Technology and synthesis options\n");
   config_writer->write("   constant CFG_FABTECH : integer := virtex4;\n");
   config_writer->write("   constant CFG_MEMTECH : integer := virtex4;\n");
   config_writer->write("   constant CFG_PADTECH : integer := virtex4;\n");
   config_writer->write("   -- Clock generator\n");
   config_writer->write("   constant CFG_CLKTECH : integer := virtex4;\n");
   config_writer->write("\n");
   config_writer->write("   -- TASTE specific:\n");
   config_writer->write("   -- If your Synthesis reports that you have to lessen the clock,\n");
   config_writer->write("   -- (i.e. less than 100MHz) , you have to modify these two:\n");
   config_writer->write("   -- (default value: 2x50/1 = 100MHz\n");
   config_writer->write("   constant CFG_CLKMUL : integer := (2);\n");
   config_writer->write("   constant CFG_CLKDIV : integer := (1);\n");
   config_writer->write("\n");
   config_writer->write("\n");
   config_writer->write("   constant CFG_PCIDLL : integer := 0;\n");
   config_writer->write("   constant CFG_PCISYSCLK: integer := 0;\n");
   config_writer->write("   -- AMBA settings\n");
   config_writer->write("   constant CFG_DEFMST : integer := (0);\n");
   config_writer->write("   constant CFG_RROBIN : integer := 1;\n");
   config_writer->write("   constant CFG_SPLIT : integer := 0;\n");
   config_writer->write("   constant CFG_AHBIO : integer := 16#FFF#;\n");
   config_writer->write("   constant CFG_APBADDR : integer := 16#800#;\n");
   config_writer->write("   -- PCI interface\n");
   config_writer->write("   constant CFG_PCI : integer := 1;\n");
   config_writer->write("   constant CFG_PCIVID : integer := 16#1AC8#;\n");
   config_writer->write("   constant CFG_PCIDID : integer := 16#0054#;\n");
   config_writer->write("\n");
   config_writer->write("   -- TASTE specific:\n");
   config_writer->write("   -- types for byte arrays\n");
   config_writer->write("   type octStr_16 is array (0 to 15) of std_logic_vector(7 downto 0);\n");
   config_writer->write("   type octStr_24 is array (0 to 23) of std_logic_vector(7 downto 0);\n");
   config_writer->write("end;\n");

   config_writer->WriteFile("architecture_config.vhd");
   HLSMgr->hdl_files.push_back("architecture_config.vhd");

   return DesignFlowStep_Status::SUCCESS;
}

bool GenerateTasteHDLArchitecture::HasToBeExecuted() const
{
   return true;
}

const CustomUnorderedSet<std::tuple<HLSFlowStep_Type, HLSFlowStepSpecializationConstRef, HLSFlowStep_Relationship>> GenerateTasteHDLArchitecture::ComputeHLSRelationships(const DesignFlowStep::RelationshipType relationship_type) const
{
   CustomUnorderedSet<std::tuple<HLSFlowStep_Type, HLSFlowStepSpecializationConstRef, HLSFlowStep_Relationship>> ret;
   switch(relationship_type)
   {
      case DEPENDENCE_RELATIONSHIP:
      {
         ret.insert(std::make_tuple(parameters->getOption<HLSFlowStep_Type>(OPT_hls_flow), HLSFlowStepSpecializationConstRef(), HLSFlowStep_Relationship::ALL_FUNCTIONS));
         break;
      }
      case INVALIDATION_RELATIONSHIP:
      {
         break;
      }
      case PRECEDENCE_RELATIONSHIP:
      {
         break;
      }
      default:
         THROW_UNREACHABLE("");
   }
   return ret;
}
