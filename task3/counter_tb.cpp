#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"
#include <chrono>  
#include <thread>  

int main(int argc, char **argv, char**env){
    int i;
    int clk;
    int current_counter_val = 0;
    
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
top->v = 0;
vbdSetMode(1);

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

   

/* Step 1

    top->rst = (i<2);

    if(vbdFlag()){
        top->v = vbdValue();
        top->ld = 1;
    }
    else{
        top->ld = 0;
    }
*/

    // Step 2

    if(vbdFlag()){
        top->rst = 0;  
        top->inc = 1; 
    }
    else{
        top->inc = 0;
    }



    // change input stimuli
    if (Verilated::gotFinish()) exit(0);

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100 ms delay
}

vbdClose();  // ++++
tfp->close();
exit(0);
}