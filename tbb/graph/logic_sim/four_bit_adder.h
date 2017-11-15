/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright (C) Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#ifndef __TBBexample_graph_logicsim_fba_H
#define __TBBexample_graph_logicsim_fba_H 1

#include "one_bit_adder.h"

typedef composite_node< tuple< signal_t, signal_t, signal_t, signal_t, signal_t, signal_t, signal_t, signal_t, signal_t >,
                      tuple< signal_t, signal_t, signal_t, signal_t, signal_t > > fba_base_type;

class four_bit_adder : public fba_base_type {
    graph& my_graph;
    std::vector<one_bit_adder> four_adders; 

public:
    four_bit_adder(graph& g) : fba_base_type(g), my_graph(g), four_adders(4, one_bit_adder(g)) {
        make_connections();
        set_up_composite();
    }
    four_bit_adder(const four_bit_adder& src) : 
        fba_base_type(src.my_graph), my_graph(src.my_graph), four_adders(4, one_bit_adder(src.my_graph)) 
    {
        make_connections();
        set_up_composite();
    }
    ~four_bit_adder() {}

private:
    void make_connections() {
        make_edge(output_port<1>(four_adders[0]), input_port<0>(four_adders[1]));
        make_edge(output_port<1>(four_adders[1]), input_port<0>(four_adders[2]));
        make_edge(output_port<1>(four_adders[2]), input_port<0>(four_adders[3]));
    }
    void set_up_composite() {

        fba_base_type::input_ports_type input_tuple(input_port<0>(four_adders[0]/*CI*/), input_port<1>(four_adders[0]), input_port<2>(four_adders[0]), input_port<1>(four_adders[1]), input_port<2>(four_adders[1]), input_port<1>(four_adders[2]), input_port<2>(four_adders[2]), input_port<1>(four_adders[3]), input_port<2>(four_adders[3])); 

       fba_base_type::output_ports_type output_tuple(output_port<0>(four_adders[0]), output_port<0>(four_adders[1]), output_port<0>(four_adders[2]), output_port<0>(four_adders[3]),output_port<1>(four_adders[3]/*CO*/));

        fba_base_type::set_external_ports(input_tuple, output_tuple);
    }
};

#endif /* __TBBexample_graph_logicsim_fba_H */
