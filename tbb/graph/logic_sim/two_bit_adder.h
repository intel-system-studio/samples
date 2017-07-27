/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright 2016 Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#ifndef __TBBexample_graph_logicsim_tba_H
#define __TBBexample_graph_logicsim_tba_H 1

#include "one_bit_adder.h"

class two_bit_adder : public composite_node< tuple< signal_t, signal_t, signal_t, signal_t, signal_t >, 
                                            tuple< signal_t, signal_t, signal_t > > {
    graph& my_graph;
    std::vector<one_bit_adder> two_adders; 
    typedef composite_node< tuple< signal_t, signal_t, signal_t, signal_t, signal_t >, 
                          tuple< signal_t, signal_t, signal_t > > base_type;
 public:
    two_bit_adder(graph& g) : base_type(g), my_graph(g), two_adders(2, one_bit_adder(g)) {
        make_connections();
        set_up_composite();
    }
    two_bit_adder(const two_bit_adder& src) : 
        base_type(src.my_graph), my_graph(src.my_graph), two_adders(2, one_bit_adder(src.my_graph)) 
    {
        make_connections();
        set_up_composite();
    }
    ~two_bit_adder() {}

private:
    void make_connections() {
        make_edge(output_port<1>(two_adders[0]), input_port<0>(two_adders[1]));
    }
    void set_up_composite() {

        base_type::input_ports_type input_tuple(input_port<0>(two_adders[0]/*CI*/), input_port<1>(two_adders[0]), input_port<2>(two_adders[0]), input_port<1>(two_adders[1]), input_port<2>(two_adders[1]));

       base_type::output_ports_type output_tuple(output_port<0>(two_adders[0]), output_port<0>(two_adders[1]),output_port<1>(two_adders[1]/*CO*/));
       base_type::set_external_ports(input_tuple, output_tuple);
    }
};

#endif /* __TBBexample_graph_logicsim_tba_H */

