#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"
#include <chrono>  
#include <thread>  

int main(int argc, char **argv, char**env){
    int i;
    int clk;
    
Verilated::commandArgs(argc, argv);
// init top verilog instance
Vcounter* top = new Vcounter;
// init trace dump
Verilated::traceEverOn(true);
VerilatedVcdC* tfp = new VerilatedVcdC;
top->trace (tfp, 99);
tfp->open ("counter.vcd");

// init Vbuddy
if (vbdOpen()!=1) return(-1);
vbdHeader("Lab 1: Counter");

// initialise simulation inputs
top->clk = 1;
top->rst = 1;
top->en = 0;

// run simulation for many clock cycles
for (i=0; i<1000; i++) {

    // dump varibles into VCD file and toggle clock inputs
    for (clk=0; clk<2; clk++){
        tfp->dump (2*i+clk);  // unit is in ps!
        top->clk = !top->clk;
        top->eval ();
    }

    // ++++ send count value to Vbuddy
    //
    vbdHex(4, (int(top->count) >> 16) & 0xF);
    vbdHex(3, (int(top->count) >> 8) & 0xF);
    vbdHex(2, (int(top->count) >> 4) & 0xF);
    vbdHex(1, int(top->count) & 0xF);
    vbdCycle(i+1);
    //
    // ---- end of Vbuddy output section

    //vbdPlot(int(top->count), 0, 255);


    // change input stimuli
    top->rst = (i<2) | (i == 15);
    if(vbdFlag() == 1){
        top->en = 1;
    }
    else if(vbdFlag() == 0){
        top->en = 0;
    }
    if (Verilated::gotFinish()) exit(0);

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100 ms delay
}

vbdClose();  // ++++
tfp->close();
exit(0);
}