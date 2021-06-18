## 1. Components

#### 1.1 Framework Codes

* gstream/* for creating a streaming graphs
* msforest/* for expansion list, ms-trees and concurrent design
* timing/* main functions conduct Insertion/Deletion over streaming graphs

#### 1.2 Implemented Applications

* network/* for the application over network flow dataset
  * netflow dataset is anonymous network traffic that can be downloaded from CAIDA.org
  * network/netdat/query/* contains queries that can be parsed by bool netquery::parseQuery() in network/netquery.cpp
    * lines starting with 'e' denote edges
    * lines starting with 'b' denote timing order constraints
    * lines starting with 't' denote TC-subqueries (i.e., TC-Decomposition)
* rdf/* for the application over synthetic rdf stream dataset 
  * rdf dataset is generated with lsBench generator 
    * https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/lsbench/sibStream0617.tar
  * rdf/rdfdat/query/* contains queries that can be parsed by bool rdfquery::parseQuery() in rdf/rdfquery.cpp (similar to those in network/netdat/query/*)
* You can develop a new application by implementing framework codes as those in network/* and rdf/*

## 2. Compile

#### 2.1 network Makefile

* network/Makefile
* Enable 'CFLAGS += -DNO_THREAD' to compile for running in single-thread
* Enable 'CFLAGS += -DALL_I' to compile for running in Incremental-Only mode (only conduct Insertion)
* you may need to create folders: network/bin and network/.objs

 #### 2.2 rdf Makefile

* rdf/Makefile
* Similar to those in network/Makefile

## 3. Running

#### 3.1 network/rdf

* |      | * argv0 : binary         |
  | ---- | ------------------------ |
  |      | * argv1 : dataset        |
  |      | * argv2 : query          |
  |      | * argv3 : winsz          |
  |      | * argv4 : Max_Thread_Num |

* Refer to network/timing_network.cpp (rdf/timing_rdf.cpp) for more detail
