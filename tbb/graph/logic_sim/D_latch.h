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

#ifndef __TBBexample_graph_logicsim_dlatch_H
#define __TBBexample_graph_logicsim_dlatch_H 1

#include "basics.h"

class D_latch : public composite_node< tuple< signal_t, signal_t >, tuple< signal_t, signal_t > > {
    broadcast_node<signal_t> D_port;
    broadcast_node<signal_t> E_port;
    not_gate a_not;
    and_gate<2> first_and;
    and_gate<2> second_and;
    nor_gate<2> first_nor;
    nor_gate<2> second_nor;
    graph& my_graph;
    typedef composite_node< tuple< signal_t, signal_t >, tuple< signal_t, signal_t > > base_type;

 public:
    D_latch(graph& g) : base_type(g), my_graph(g), D_port(g), E_port(g), a_not(g), first_and(g), second_and(g), 
                        first_nor(g), second_nor(g) 
    {
        make_edge(D_port, input_port<0>(a_not));
        make_edge(D_port, input_port<1>(second_and));
        make_edge(E_port, input_port<1>(first_and));
        make_edge(E_port, input_port<0>(second_and));
        make_edge(a_not, input_port<0>(first_and));
        make_edge(first_and, input_port<0>(first_nor));
        make_edge(second_and, input_port<1>(second_nor));
        make_edge(first_nor, input_port<0>(second_nor));
        make_edge(second_nor, input_port<1>(first_nor));
 
        base_type::input_ports_type input_tuple(D_port, E_port);
        base_type::output_ports_type output_tuple(output_port<0>(first_nor), output_port<0>(second_nor)); 

        base_type::set_external_ports(input_tuple, output_tuple); 
        base_type::add_visible_nodes(D_port, E_port, a_not, first_and, second_and, first_nor, second_nor);
    }
    ~D_latch() {}
};

#endif /* __TBBexample_graph_logicsim_dlatch_H */
